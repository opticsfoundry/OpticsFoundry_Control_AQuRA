// EasyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Control.h"
#include "EasyDialog.h"
#include "MainDialog.h"
#include "ElementButton.h"
#include "Param.h"
#include "Sequence.h"
#include "afxpriv.h"
#include "ElementBool.h"
#include "ElementStatic.h"
#include "CTimestamp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CWnd* CallingDialog;

/////////////////////////////////////////////////////////////////////////////
// CEasyDialog dialog


CEasyDialog::CEasyDialog(CString aCaption, bool aMainWindow,  int _MainDialogStartX,  int _MainDialogStartY, unsigned int aMessage,CMessageReceiver* aMessageReceiver, CWnd* pParent /*=NULL*/)
	: CDialog(CEasyDialog::IDD, pParent)
{
	m_bDialogClosed = false;
	MainWindow = aMainWindow;
	MainDialogStartX = _MainDialogStartX;
	MainDialogStartY = _MainDialogStartY;
	CallingDialog=NULL;
	MessageReceiver=aMessageReceiver;
	AktID=IDS_EDIT0;
	AktButtonID=IDS_BUTTON0;
	AktHelpID=IDS_HELPBUTTON0;
	
	Message=aMessage;	
	Caption=aCaption;
	MaxUpDownMenuNr=0;
	MinUpDownMenuNr=0;
	UpDownEnabled=false;
	UpDownButtonEnabled=false;
	ToolTip=nullptr;

	//{{AFX_DATA_INIT(CEasyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


CEasyDialog::~CEasyDialog()
{
	m_bDialogClosed = true;
	if (ToolTip) {
		if (::IsWindow(ToolTip->GetSafeHwnd())) {
			ToolTip->DestroyWindow(); // Safely destroys the HWND
		}
		delete ToolTip;
		ToolTip = nullptr;
	}
	DeleteElements();	
}

#ifdef DebugEasyDialog
void CEasyDialog::DebugProtocol(CString message) {
	//open file to append to it
	std::fstream file(*DebugFilePath + "EasyDialog_log.dat", std::ios::out | std::ios::app);
	if (!file.is_open()) {
		ControlMessageBox("CEasyDialog::Debug : Couldn't open log file for writing");
		return;
	}
	file << Caption << ": " << message.GetString() << std::endl;
	file.close();
}
#endif

void CEasyDialog::DeleteElements()
{
	POSITION pos=ElementList.GetHeadPosition();
	while (pos!=NULL) {
		CParam* help=((CParam*)ElementList.GetAt(pos));			
		ElementList.RemoveAt(pos);			
		if (help) delete help;		
		pos=ElementList.GetHeadPosition();
	}
	ElementList.RemoveAll();
}

void CEasyDialog::OnOK()
{
	((CControlApp*)AfxGetApp())->OnWakeUp();
	UpdateData(TRUE);
	UpdateData(FALSE);
	if (Message) {
		CallingDialog=this;
		Sequence->MessageMap(Message,this);
	}
	CDialog::OnOK();	
}

BOOL CEasyDialog::OnMouseWheel( UINT nFlags,short zDelta, CPoint pt) {
	if (zDelta<0) OnDown(1);
	else OnUp(1);
	return true;
}

void CEasyDialog::OnRButtonUp( UINT nFlags, CPoint point ) {
	OnOK();
}

void CEasyDialog::OnLButtonUp( UINT nFlags, CPoint point ) {
	OnApply();
}

void CEasyDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {

}

void CEasyDialog::OnUp() 
{
	OnUp(0);
}

void CEasyDialog::OnUp(int mode) {
	OnApply();
	if (UpDownButtonEnabled) {
		if (!UpdateData(TRUE))
		{
			TRACE0("UpdateData failed during dialog termination.\n");
			// the UpdateData routine will set focus to correct item
			return;
		}
		UpdateData(FALSE);
		if (Message) Sequence->MessageMap(Message,this);
		EndDialog((mode) ? IDUPMOUSE : IDUP);	
	}
}

void CEasyDialog::OnDown() 
{
	OnDown(0);
}

void CEasyDialog::OnDown(int mode) {
	OnApply();	
	if (UpDownButtonEnabled) {
		if (!UpdateData(TRUE))
		{
			TRACE0("UpdateData failed during dialog termination.\n");
			// the UpdateData routine will set focus to correct item
			return;
		}
		UpdateData(FALSE);
		CallingDialog=this;
		if (Message) {
			CallingDialog=this;
			Sequence->MessageMap(Message,this);
		}
		EndDialog((mode) ? IDDOWNMOUSE : IDDOWN);	
	}
}

void CEasyDialog::OnDestroy()
{
	m_bDialogClosed = true;
	if (ToolTip) {	
		if (::IsWindow(ToolTip->GetSafeHwnd())) {
			ToolTip->DestroyWindow(); // Safely destroys the HWND
		}
		delete ToolTip;
		ToolTip = nullptr;
	}
	CDialog::OnDestroy();
}

void CEasyDialog::DoDataExchange(CDataExchange* pDX)
{
#ifdef DebugEasyDialog
	DebugProtocol("DoDataExchange");
#endif
	//Timestamp.Mark("CEasyDialog::DoDataExchange 0");
	CDialog::DoDataExchange(pDX);	
//	bool DirectOutMode=false;
	if (pDX->m_bSaveAndValidate!=0) {		
	//	if (Output->IsInDirectOutputMode()) {
//			DirectOutMode=true;
		//this might be called again without the sequence having been executed in case the user makes a mistake and enters out of range data
			Sequence->SetAssembleSequenceListMode();			
			Output->StartSequence();
			Output->StartMenuUpdateMode();
			Output->SwitchForceWritingMode(On);
	//	}
	}	
	//Timestamp.Mark("CEasyDialog::DoDataExchange 1");
	POSITION pos=ElementList.GetHeadPosition();
	while (pos!=NULL) {
		CDialogElement* Element=(CDialogElement*)ElementList.GetNext(pos);
		// if the user enters out of range data, this is detected during the DoDataExchange procedure
		// and CEasyDialog::DoDataExchange is called again
		if (Element) Element->DoDataExchange(pDX);			
	}
	//Timestamp.Mark("CEasyDialog::DoDataExchange 2");
	if (pDX->m_bSaveAndValidate!=0) {
//		if (DirectOutMode) {
			Sequence->StopSequence();
			Sequence->SetWaveformGenerationMode();			
			//Timestamp.Mark("CEasyDialog::DoDataExchange 3");
			if (Sequence->ExecuteSequenceList(/*ShowRunProgressDialog*/false)) Sequence->EmptyNIcardFIFO();
			if (DebugSequenceListOn) Sequence->DebugSequenceList(*DebugFilePath + "SequenceListDoDataExchange.dat",0);
			//Timestamp.Mark("CEasyDialog::DoDataExchange 4");
			Sequence->SetDirectOutputMode();				
			//Timestamp.Mark("CEasyDialog::DoDataExchange 5");
			Output->StopMenuUpdateMode();
			//Timestamp.Mark("CEasyDialog::DoDataExchange 6");
//		}
	}
	Output->SwitchForceWritingMode(Off);
	//Timestamp.Mark("CEasyDialog::DoDataExchange 7");
}

BEGIN_MESSAGE_MAP(CEasyDialog, CDialog)
	//{{AFX_MSG_MAP(CEasyDialog)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDS_BUTTON0, OnButton0)
	ON_BN_CLICKED(IDS_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDS_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDS_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDS_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDS_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDS_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDS_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDS_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDS_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDS_BUTTON10, OnButton10)
	ON_BN_CLICKED(IDS_BUTTON11, OnButton11)
	ON_BN_CLICKED(IDS_BUTTON12, OnButton12)
	ON_BN_CLICKED(IDS_BUTTON13, OnButton13)
	ON_BN_CLICKED(IDS_BUTTON14, OnButton14)
	ON_BN_CLICKED(IDS_BUTTON15, OnButton15)
	ON_BN_CLICKED(IDS_BUTTON16, OnButton16)
	ON_BN_CLICKED(IDS_BUTTON17, OnButton17)
	ON_BN_CLICKED(IDS_BUTTON18, OnButton18)
	ON_BN_CLICKED(IDS_BUTTON19, OnButton19)
	ON_BN_CLICKED(IDS_BUTTON20, OnButton20)
	ON_BN_CLICKED(IDS_BUTTON21, OnButton21)
	ON_BN_CLICKED(IDS_BUTTON22, OnButton22)
	ON_BN_CLICKED(IDS_BUTTON23, OnButton23)
	ON_BN_CLICKED(IDS_BUTTON24, OnButton24)
	ON_BN_CLICKED(IDS_BUTTON25, OnButton25)
	ON_BN_CLICKED(IDS_BUTTON26, OnButton26)
	ON_BN_CLICKED(IDS_BUTTON27, OnButton27)
	ON_BN_CLICKED(IDS_BUTTON28, OnButton28)
	ON_BN_CLICKED(IDS_BUTTON29, OnButton29)
	ON_BN_CLICKED(IDS_BUTTON30, OnButton30)
	ON_BN_CLICKED(IDS_BUTTON31, OnButton31)
	ON_BN_CLICKED(IDS_BUTTON32, OnButton32)
	ON_BN_CLICKED(IDS_BUTTON33, OnButton33)
	ON_BN_CLICKED(IDS_BUTTON34, OnButton34)
	ON_BN_CLICKED(IDS_BUTTON35, OnButton35)
	ON_BN_CLICKED(IDS_BUTTON36, OnButton36)
	ON_BN_CLICKED(IDS_BUTTON37, OnButton37)
	ON_BN_CLICKED(IDS_BUTTON38, OnButton38)
	ON_BN_CLICKED(IDS_BUTTON39, OnButton39)	
	ON_BN_CLICKED(IDS_BUTTON40, OnButton40)
	ON_BN_CLICKED(IDS_BUTTON41, OnButton41)
	ON_BN_CLICKED(IDS_BUTTON42, OnButton42)
	ON_BN_CLICKED(IDS_BUTTON43, OnButton43)
	ON_BN_CLICKED(IDS_BUTTON44, OnButton44)
	ON_BN_CLICKED(IDS_BUTTON45, OnButton45)
	ON_BN_CLICKED(IDS_BUTTON46, OnButton46)
	ON_BN_CLICKED(IDS_BUTTON47, OnButton47)
	ON_BN_CLICKED(IDS_BUTTON48, OnButton48)
	ON_BN_CLICKED(IDS_BUTTON49, OnButton49)	
	ON_BN_CLICKED(IDS_BUTTON50, OnButton50)
	ON_BN_CLICKED(IDS_BUTTON51, OnButton51)
	ON_BN_CLICKED(IDS_BUTTON52, OnButton52)
	ON_BN_CLICKED(IDS_BUTTON53, OnButton53)
	ON_BN_CLICKED(IDS_BUTTON54, OnButton54)
	ON_BN_CLICKED(IDS_BUTTON55, OnButton55)
	ON_BN_CLICKED(IDS_BUTTON56, OnButton56)
	ON_BN_CLICKED(IDS_BUTTON57, OnButton57)
	ON_BN_CLICKED(IDS_BUTTON58, OnButton58)
	ON_BN_CLICKED(IDS_BUTTON59, OnButton59)	
	ON_BN_CLICKED(IDS_BUTTON60, OnButton60)	
	ON_BN_CLICKED(IDS_BUTTON61, OnButton61)	
	ON_BN_CLICKED(IDS_BUTTON62, OnButton62)	
	ON_BN_CLICKED(IDS_BUTTON63, OnButton63)	
	ON_BN_CLICKED(IDS_BUTTON64, OnButton64)	
	ON_BN_CLICKED(IDS_BUTTON65, OnButton65)	
	ON_BN_CLICKED(IDS_BUTTON66, OnButton66)	
	ON_BN_CLICKED(IDS_BUTTON67, OnButton67)	
	ON_BN_CLICKED(IDS_BUTTON68, OnButton68)	
	ON_BN_CLICKED(IDS_BUTTON69, OnButton69)	
	ON_BN_CLICKED(IDS_BUTTON70, OnButton70)
	ON_BN_CLICKED(IDS_BUTTON71, OnButton71)
	ON_BN_CLICKED(IDS_BUTTON72, OnButton72)
	ON_BN_CLICKED(IDS_BUTTON73, OnButton73)
	ON_BN_CLICKED(IDS_BUTTON74, OnButton74)
	ON_BN_CLICKED(IDS_BUTTON75, OnButton75)
	ON_BN_CLICKED(IDS_BUTTON76, OnButton76)
	ON_BN_CLICKED(IDS_BUTTON77, OnButton77)
	ON_BN_CLICKED(IDS_BUTTON78, OnButton78)
	ON_BN_CLICKED(IDS_BUTTON79, OnButton79)
	ON_BN_CLICKED(IDS_BUTTON80, OnButton80)
	ON_BN_CLICKED(IDS_BUTTON81, OnButton81)
	ON_BN_CLICKED(IDS_BUTTON82, OnButton82)
	ON_BN_CLICKED(IDS_BUTTON83, OnButton83)
	ON_BN_CLICKED(IDS_BUTTON84, OnButton84)
	ON_BN_CLICKED(IDS_BUTTON85, OnButton85)
	ON_BN_CLICKED(IDS_BUTTON86, OnButton86)
	ON_BN_CLICKED(IDS_BUTTON87, OnButton87)
	ON_BN_CLICKED(IDS_BUTTON88, OnButton88)
	ON_BN_CLICKED(IDS_BUTTON89, OnButton89)
	ON_BN_CLICKED(IDS_BUTTON90, OnButton90)
	ON_BN_CLICKED(IDS_BUTTON91, OnButton91)
	ON_BN_CLICKED(IDS_BUTTON92, OnButton92)
	ON_BN_CLICKED(IDS_BUTTON93, OnButton93)
	ON_BN_CLICKED(IDS_BUTTON94, OnButton94)
	ON_BN_CLICKED(IDS_BUTTON95, OnButton95)
	ON_BN_CLICKED(IDS_BUTTON96, OnButton96)
	ON_BN_CLICKED(IDS_BUTTON97, OnButton97)
	ON_BN_CLICKED(IDS_BUTTON98, OnButton98)
	ON_BN_CLICKED(IDS_BUTTON99, OnButton99)
	ON_BN_CLICKED(IDS_BUTTON100, OnButton100)
	ON_BN_CLICKED(IDS_BUTTON101, OnButton101)
	ON_BN_CLICKED(IDS_BUTTON102, OnButton102)
	ON_BN_CLICKED(IDS_BUTTON103, OnButton103)
	ON_BN_CLICKED(IDS_BUTTON104, OnButton104)
	ON_BN_CLICKED(IDS_BUTTON105, OnButton105)
	ON_BN_CLICKED(IDS_BUTTON106, OnButton106)
	ON_BN_CLICKED(IDS_BUTTON107, OnButton107)
	ON_BN_CLICKED(IDS_BUTTON108, OnButton108)
	ON_BN_CLICKED(IDS_BUTTON109, OnButton109)
	
	ON_BN_CLICKED(IDS_BUTTON110, OnButton110)
	ON_BN_CLICKED(IDS_BUTTON111, OnButton111)
	ON_BN_CLICKED(IDS_BUTTON112, OnButton112)
	ON_BN_CLICKED(IDS_BUTTON113, OnButton113)
	ON_BN_CLICKED(IDS_BUTTON114, OnButton114)
	ON_BN_CLICKED(IDS_BUTTON115, OnButton115)
	ON_BN_CLICKED(IDS_BUTTON116, OnButton116)
	ON_BN_CLICKED(IDS_BUTTON117, OnButton117)
	ON_BN_CLICKED(IDS_BUTTON118, OnButton118)
	ON_BN_CLICKED(IDS_BUTTON119, OnButton119)

	ON_BN_CLICKED(IDS_BUTTON120, OnButton120)
	ON_BN_CLICKED(IDS_BUTTON121, OnButton121)
	ON_BN_CLICKED(IDS_BUTTON122, OnButton122)
	ON_BN_CLICKED(IDS_BUTTON123, OnButton123)
	ON_BN_CLICKED(IDS_BUTTON124, OnButton124)
	ON_BN_CLICKED(IDS_BUTTON125, OnButton125)
	ON_BN_CLICKED(IDS_BUTTON126, OnButton126)
	ON_BN_CLICKED(IDS_BUTTON127, OnButton127)
	ON_BN_CLICKED(IDS_BUTTON128, OnButton128)
	ON_BN_CLICKED(IDS_BUTTON129, OnButton129)

	ON_BN_CLICKED(IDS_BUTTON130, OnButton130)
	ON_BN_CLICKED(IDS_BUTTON131, OnButton131)
	ON_BN_CLICKED(IDS_BUTTON132, OnButton132)
	ON_BN_CLICKED(IDS_BUTTON133, OnButton133)
	ON_BN_CLICKED(IDS_BUTTON134, OnButton134)
	ON_BN_CLICKED(IDS_BUTTON135, OnButton135)
	ON_BN_CLICKED(IDS_BUTTON136, OnButton136)
	ON_BN_CLICKED(IDS_BUTTON137, OnButton137)
	ON_BN_CLICKED(IDS_BUTTON138, OnButton138)
	ON_BN_CLICKED(IDS_BUTTON139, OnButton139)
	
	ON_BN_CLICKED(IDS_BUTTON140, OnButton140)
	ON_BN_CLICKED(IDS_BUTTON141, OnButton141)
	ON_BN_CLICKED(IDS_BUTTON142, OnButton142)
	ON_BN_CLICKED(IDS_BUTTON143, OnButton143)
	ON_BN_CLICKED(IDS_BUTTON144, OnButton144)
	ON_BN_CLICKED(IDS_BUTTON145, OnButton145)
	ON_BN_CLICKED(IDS_BUTTON146, OnButton146)
	ON_BN_CLICKED(IDS_BUTTON147, OnButton147)
	ON_BN_CLICKED(IDS_BUTTON148, OnButton148)
	ON_BN_CLICKED(IDS_BUTTON149, OnButton149)

	ON_BN_CLICKED(IDS_BUTTON150, OnButton150)
	ON_BN_CLICKED(IDS_BUTTON151, OnButton151)
	ON_BN_CLICKED(IDS_BUTTON152, OnButton152)
	ON_BN_CLICKED(IDS_BUTTON153, OnButton153)
	ON_BN_CLICKED(IDS_BUTTON154, OnButton154)
	ON_BN_CLICKED(IDS_BUTTON155, OnButton155)
	ON_BN_CLICKED(IDS_BUTTON156, OnButton156)
	ON_BN_CLICKED(IDS_BUTTON157, OnButton157)
	ON_BN_CLICKED(IDS_BUTTON158, OnButton158)
	ON_BN_CLICKED(IDS_BUTTON159, OnButton159)

	ON_BN_CLICKED(IDS_BUTTON160, OnButton160)
	ON_BN_CLICKED(IDS_BUTTON161, OnButton161)
	ON_BN_CLICKED(IDS_BUTTON162, OnButton162)
	ON_BN_CLICKED(IDS_BUTTON163, OnButton163)
	ON_BN_CLICKED(IDS_BUTTON164, OnButton164)
	ON_BN_CLICKED(IDS_BUTTON165, OnButton165)
	ON_BN_CLICKED(IDS_BUTTON166, OnButton166)
	ON_BN_CLICKED(IDS_BUTTON167, OnButton167)
	ON_BN_CLICKED(IDS_BUTTON168, OnButton168)
	ON_BN_CLICKED(IDS_BUTTON169, OnButton169)
	
	ON_BN_CLICKED(IDS_BUTTON170, OnButton170)
	ON_BN_CLICKED(IDS_BUTTON171, OnButton171)
	ON_BN_CLICKED(IDS_BUTTON172, OnButton172)
	ON_BN_CLICKED(IDS_BUTTON173, OnButton173)
	ON_BN_CLICKED(IDS_BUTTON174, OnButton174)
	ON_BN_CLICKED(IDS_BUTTON175, OnButton175)
	ON_BN_CLICKED(IDS_BUTTON176, OnButton176)
	ON_BN_CLICKED(IDS_BUTTON177, OnButton177)
	ON_BN_CLICKED(IDS_BUTTON178, OnButton178)
	ON_BN_CLICKED(IDS_BUTTON179, OnButton179)
	
	//HelpButtons
	ON_BN_CLICKED(IDS_HELPBUTTON0, OnHelpButton0)
	ON_BN_CLICKED(IDS_HELPBUTTON1, OnHelpButton1)
	ON_BN_CLICKED(IDS_HELPBUTTON2, OnHelpButton2)
	ON_BN_CLICKED(IDS_HELPBUTTON3, OnHelpButton3)
	ON_BN_CLICKED(IDS_HELPBUTTON4, OnHelpButton4)
	ON_BN_CLICKED(IDS_HELPBUTTON5, OnHelpButton5)
	ON_BN_CLICKED(IDS_HELPBUTTON6, OnHelpButton6)
	ON_BN_CLICKED(IDS_HELPBUTTON7, OnHelpButton7)
	ON_BN_CLICKED(IDS_HELPBUTTON8, OnHelpButton8)
	ON_BN_CLICKED(IDS_HELPBUTTON9, OnHelpButton9)
	ON_BN_CLICKED(IDS_HELPBUTTON10, OnHelpButton10)
	ON_BN_CLICKED(IDS_HELPBUTTON11, OnHelpButton11)
	ON_BN_CLICKED(IDS_HELPBUTTON12, OnHelpButton12)
	ON_BN_CLICKED(IDS_HELPBUTTON13, OnHelpButton13)
	ON_BN_CLICKED(IDS_HELPBUTTON14, OnHelpButton14)
	ON_BN_CLICKED(IDS_HELPBUTTON15, OnHelpButton15)
	ON_BN_CLICKED(IDS_HELPBUTTON16, OnHelpButton16)
	ON_BN_CLICKED(IDS_HELPBUTTON17, OnHelpButton17)
	ON_BN_CLICKED(IDS_HELPBUTTON18, OnHelpButton18)
	ON_BN_CLICKED(IDS_HELPBUTTON19, OnHelpButton19)
	ON_BN_CLICKED(IDS_HELPBUTTON20, OnHelpButton20)
	ON_BN_CLICKED(IDS_HELPBUTTON21, OnHelpButton21)
	ON_BN_CLICKED(IDS_HELPBUTTON22, OnHelpButton22)
	ON_BN_CLICKED(IDS_HELPBUTTON23, OnHelpButton23)
	ON_BN_CLICKED(IDS_HELPBUTTON24, OnHelpButton24)
	ON_BN_CLICKED(IDS_HELPBUTTON25, OnHelpButton25)
	ON_BN_CLICKED(IDS_HELPBUTTON26, OnHelpButton26)
	ON_BN_CLICKED(IDS_HELPBUTTON27, OnHelpButton27)
	ON_BN_CLICKED(IDS_HELPBUTTON28, OnHelpButton28)
	ON_BN_CLICKED(IDS_HELPBUTTON29, OnHelpButton29)
	ON_BN_CLICKED(IDS_HELPBUTTON30, OnHelpButton30)
	ON_BN_CLICKED(IDS_HELPBUTTON31, OnHelpButton31)
	ON_BN_CLICKED(IDS_HELPBUTTON32, OnHelpButton32)
	ON_BN_CLICKED(IDS_HELPBUTTON33, OnHelpButton33)
	ON_BN_CLICKED(IDS_HELPBUTTON34, OnHelpButton34)
	ON_BN_CLICKED(IDS_HELPBUTTON35, OnHelpButton35)
	ON_BN_CLICKED(IDS_HELPBUTTON36, OnHelpButton36)
	ON_BN_CLICKED(IDS_HELPBUTTON37, OnHelpButton37)
	ON_BN_CLICKED(IDS_HELPBUTTON38, OnHelpButton38)
	ON_BN_CLICKED(IDS_HELPBUTTON39, OnHelpButton39)	
	ON_BN_CLICKED(IDS_HELPBUTTON40, OnHelpButton40)
	ON_BN_CLICKED(IDS_HELPBUTTON41, OnHelpButton41)
	ON_BN_CLICKED(IDS_HELPBUTTON42, OnHelpButton42)
	ON_BN_CLICKED(IDS_HELPBUTTON43, OnHelpButton43)
	ON_BN_CLICKED(IDS_HELPBUTTON44, OnHelpButton44)
	ON_BN_CLICKED(IDS_HELPBUTTON45, OnHelpButton45)
	ON_BN_CLICKED(IDS_HELPBUTTON46, OnHelpButton46)
	ON_BN_CLICKED(IDS_HELPBUTTON47, OnHelpButton47)
	ON_BN_CLICKED(IDS_HELPBUTTON48, OnHelpButton48)
	ON_BN_CLICKED(IDS_HELPBUTTON49, OnHelpButton49)	
	ON_BN_CLICKED(IDS_HELPBUTTON50, OnHelpButton50)
	ON_BN_CLICKED(IDS_HELPBUTTON51, OnHelpButton51)
	ON_BN_CLICKED(IDS_HELPBUTTON52, OnHelpButton52)
	ON_BN_CLICKED(IDS_HELPBUTTON53, OnHelpButton53)
	ON_BN_CLICKED(IDS_HELPBUTTON54, OnHelpButton54)
	ON_BN_CLICKED(IDS_HELPBUTTON55, OnHelpButton55)
	ON_BN_CLICKED(IDS_HELPBUTTON56, OnHelpButton56)
	ON_BN_CLICKED(IDS_HELPBUTTON57, OnHelpButton57)
	ON_BN_CLICKED(IDS_HELPBUTTON58, OnHelpButton58)
	ON_BN_CLICKED(IDS_HELPBUTTON59, OnHelpButton59)	
	ON_BN_CLICKED(IDS_HELPBUTTON60, OnHelpButton60)	
	ON_BN_CLICKED(IDS_HELPBUTTON61, OnHelpButton61)	
	ON_BN_CLICKED(IDS_HELPBUTTON62, OnHelpButton62)	
	ON_BN_CLICKED(IDS_HELPBUTTON63, OnHelpButton63)	
	ON_BN_CLICKED(IDS_HELPBUTTON64, OnHelpButton64)	
	ON_BN_CLICKED(IDS_HELPBUTTON65, OnHelpButton65)	
	ON_BN_CLICKED(IDS_HELPBUTTON66, OnHelpButton66)	
	ON_BN_CLICKED(IDS_HELPBUTTON67, OnHelpButton67)	
	ON_BN_CLICKED(IDS_HELPBUTTON68, OnHelpButton68)	
	ON_BN_CLICKED(IDS_HELPBUTTON69, OnHelpButton69)	
	ON_BN_CLICKED(IDS_HELPBUTTON70, OnHelpButton70)
	ON_BN_CLICKED(IDS_HELPBUTTON71, OnHelpButton71)
	ON_BN_CLICKED(IDS_HELPBUTTON72, OnHelpButton72)
	ON_BN_CLICKED(IDS_HELPBUTTON73, OnHelpButton73)
	ON_BN_CLICKED(IDS_HELPBUTTON74, OnHelpButton74)
	ON_BN_CLICKED(IDS_HELPBUTTON75, OnHelpButton75)
	ON_BN_CLICKED(IDS_HELPBUTTON76, OnHelpButton76)
	ON_BN_CLICKED(IDS_HELPBUTTON77, OnHelpButton77)
	ON_BN_CLICKED(IDS_HELPBUTTON78, OnHelpButton78)
	ON_BN_CLICKED(IDS_HELPBUTTON79, OnHelpButton79)
	ON_BN_CLICKED(IDS_HELPBUTTON80, OnHelpButton80)
	ON_BN_CLICKED(IDS_HELPBUTTON81, OnHelpButton81)
	ON_BN_CLICKED(IDS_HELPBUTTON82, OnHelpButton82)
	ON_BN_CLICKED(IDS_HELPBUTTON83, OnHelpButton83)
	ON_BN_CLICKED(IDS_HELPBUTTON84, OnHelpButton84)
	ON_BN_CLICKED(IDS_HELPBUTTON85, OnHelpButton85)
	ON_BN_CLICKED(IDS_HELPBUTTON86, OnHelpButton86)
	ON_BN_CLICKED(IDS_HELPBUTTON87, OnHelpButton87)
	ON_BN_CLICKED(IDS_HELPBUTTON88, OnHelpButton88)
	ON_BN_CLICKED(IDS_HELPBUTTON89, OnHelpButton89)
	ON_BN_CLICKED(IDS_HELPBUTTON90, OnHelpButton90)
	ON_BN_CLICKED(IDS_HELPBUTTON91, OnHelpButton91)
	ON_BN_CLICKED(IDS_HELPBUTTON92, OnHelpButton92)
	ON_BN_CLICKED(IDS_HELPBUTTON93, OnHelpButton93)
	ON_BN_CLICKED(IDS_HELPBUTTON94, OnHelpButton94)
	ON_BN_CLICKED(IDS_HELPBUTTON95, OnHelpButton95)
	ON_BN_CLICKED(IDS_HELPBUTTON96, OnHelpButton96)
	ON_BN_CLICKED(IDS_HELPBUTTON97, OnHelpButton97)
	ON_BN_CLICKED(IDS_HELPBUTTON98, OnHelpButton98)
	ON_BN_CLICKED(IDS_HELPBUTTON99, OnHelpButton99)
	ON_BN_CLICKED(IDS_HELPBUTTON100, OnHelpButton100)
	ON_BN_CLICKED(IDS_HELPBUTTON101, OnHelpButton101)
	ON_BN_CLICKED(IDS_HELPBUTTON102, OnHelpButton102)
	ON_BN_CLICKED(IDS_HELPBUTTON103, OnHelpButton103)
	ON_BN_CLICKED(IDS_HELPBUTTON104, OnHelpButton104)
	ON_BN_CLICKED(IDS_HELPBUTTON105, OnHelpButton105)
	ON_BN_CLICKED(IDS_HELPBUTTON106, OnHelpButton106)
	ON_BN_CLICKED(IDS_HELPBUTTON107, OnHelpButton107)
	ON_BN_CLICKED(IDS_HELPBUTTON108, OnHelpButton108)
	ON_BN_CLICKED(IDS_HELPBUTTON109, OnHelpButton109)
	
	ON_BN_CLICKED(IDS_HELPBUTTON110, OnHelpButton110)
	ON_BN_CLICKED(IDS_HELPBUTTON111, OnHelpButton111)
	ON_BN_CLICKED(IDS_HELPBUTTON112, OnHelpButton112)
	ON_BN_CLICKED(IDS_HELPBUTTON113, OnHelpButton113)
	ON_BN_CLICKED(IDS_HELPBUTTON114, OnHelpButton114)
	ON_BN_CLICKED(IDS_HELPBUTTON115, OnHelpButton115)
	ON_BN_CLICKED(IDS_HELPBUTTON116, OnHelpButton116)
	ON_BN_CLICKED(IDS_HELPBUTTON117, OnHelpButton117)
	ON_BN_CLICKED(IDS_HELPBUTTON118, OnHelpButton118)
	ON_BN_CLICKED(IDS_HELPBUTTON119, OnHelpButton119)

	ON_BN_CLICKED(IDS_HELPBUTTON120, OnHelpButton120)
	ON_BN_CLICKED(IDS_HELPBUTTON121, OnHelpButton121)
	ON_BN_CLICKED(IDS_HELPBUTTON122, OnHelpButton122)
	ON_BN_CLICKED(IDS_HELPBUTTON123, OnHelpButton123)
	ON_BN_CLICKED(IDS_HELPBUTTON124, OnHelpButton124)
	ON_BN_CLICKED(IDS_HELPBUTTON125, OnHelpButton125)
	ON_BN_CLICKED(IDS_HELPBUTTON126, OnHelpButton126)
	ON_BN_CLICKED(IDS_HELPBUTTON127, OnHelpButton127)
	ON_BN_CLICKED(IDS_HELPBUTTON128, OnHelpButton128)
	ON_BN_CLICKED(IDS_HELPBUTTON129, OnHelpButton129)

	ON_BN_CLICKED(IDS_HELPBUTTON130, OnHelpButton130)
	ON_BN_CLICKED(IDS_HELPBUTTON131, OnHelpButton131)
	ON_BN_CLICKED(IDS_HELPBUTTON132, OnHelpButton132)
	ON_BN_CLICKED(IDS_HELPBUTTON133, OnHelpButton133)
	ON_BN_CLICKED(IDS_HELPBUTTON134, OnHelpButton134)
	ON_BN_CLICKED(IDS_HELPBUTTON135, OnHelpButton135)
	ON_BN_CLICKED(IDS_HELPBUTTON136, OnHelpButton136)
	ON_BN_CLICKED(IDS_HELPBUTTON137, OnHelpButton137)
	ON_BN_CLICKED(IDS_HELPBUTTON138, OnHelpButton138)
	ON_BN_CLICKED(IDS_HELPBUTTON139, OnHelpButton139)
	
	ON_BN_CLICKED(IDS_HELPBUTTON140, OnHelpButton140)
	ON_BN_CLICKED(IDS_HELPBUTTON141, OnHelpButton141)
	ON_BN_CLICKED(IDS_HELPBUTTON142, OnHelpButton142)
	ON_BN_CLICKED(IDS_HELPBUTTON143, OnHelpButton143)
	ON_BN_CLICKED(IDS_HELPBUTTON144, OnHelpButton144)
	ON_BN_CLICKED(IDS_HELPBUTTON145, OnHelpButton145)
	ON_BN_CLICKED(IDS_HELPBUTTON146, OnHelpButton146)
	ON_BN_CLICKED(IDS_HELPBUTTON147, OnHelpButton147)
	ON_BN_CLICKED(IDS_HELPBUTTON148, OnHelpButton148)
	ON_BN_CLICKED(IDS_HELPBUTTON149, OnHelpButton149)

	ON_BN_CLICKED(IDS_HELPBUTTON150, OnHelpButton150)
	ON_BN_CLICKED(IDS_HELPBUTTON151, OnHelpButton151)
	ON_BN_CLICKED(IDS_HELPBUTTON152, OnHelpButton152)
	ON_BN_CLICKED(IDS_HELPBUTTON153, OnHelpButton153)
	ON_BN_CLICKED(IDS_HELPBUTTON154, OnHelpButton154)
	ON_BN_CLICKED(IDS_HELPBUTTON155, OnHelpButton155)
	ON_BN_CLICKED(IDS_HELPBUTTON156, OnHelpButton156)
	ON_BN_CLICKED(IDS_HELPBUTTON157, OnHelpButton157)
	ON_BN_CLICKED(IDS_HELPBUTTON158, OnHelpButton158)
	ON_BN_CLICKED(IDS_HELPBUTTON159, OnHelpButton159)

	ON_BN_CLICKED(IDS_HELPBUTTON160, OnHelpButton160)
	ON_BN_CLICKED(IDS_HELPBUTTON161, OnHelpButton161)
	ON_BN_CLICKED(IDS_HELPBUTTON162, OnHelpButton162)
	ON_BN_CLICKED(IDS_HELPBUTTON163, OnHelpButton163)
	ON_BN_CLICKED(IDS_HELPBUTTON164, OnHelpButton164)
	ON_BN_CLICKED(IDS_HELPBUTTON165, OnHelpButton165)
	ON_BN_CLICKED(IDS_HELPBUTTON166, OnHelpButton166)
	ON_BN_CLICKED(IDS_HELPBUTTON167, OnHelpButton167)
	ON_BN_CLICKED(IDS_HELPBUTTON168, OnHelpButton168)
	ON_BN_CLICKED(IDS_HELPBUTTON169, OnHelpButton169)
	
	ON_BN_CLICKED(IDS_HELPBUTTON170, OnHelpButton170)
	ON_BN_CLICKED(IDS_HELPBUTTON171, OnHelpButton171)
	ON_BN_CLICKED(IDS_HELPBUTTON172, OnHelpButton172)
	ON_BN_CLICKED(IDS_HELPBUTTON173, OnHelpButton173)
	ON_BN_CLICKED(IDS_HELPBUTTON174, OnHelpButton174)
	ON_BN_CLICKED(IDS_HELPBUTTON175, OnHelpButton175)
	ON_BN_CLICKED(IDS_HELPBUTTON176, OnHelpButton176)
	ON_BN_CLICKED(IDS_HELPBUTTON177, OnHelpButton177)
	ON_BN_CLICKED(IDS_HELPBUTTON178, OnHelpButton178)
	ON_BN_CLICKED(IDS_HELPBUTTON179, OnHelpButton179)
	
	ON_BN_CLICKED(IDC_UP, OnUp)	
	ON_BN_CLICKED(IDC_DOWN, OnDown)	
	ON_WM_MOUSEWHEEL( )
	ON_WM_RBUTTONUP( )
	ON_WM_LBUTTONUP( )
	ON_WM_KEYDOWN( )
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_BN_CLICKED(IDC_RUN_EXPERIMENT, OnBnClickedRunExperiment)
	ON_BN_CLICKED(IDC_RESETSYSTEM, OnBnClickedResetsystem)
	ON_BN_CLICKED(IDC_EMERGENCY_SHUTOFF, OnBnClickedEmergencyShutoff)
		ON_WM_DESTROY()
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyDialog message handlers

void CEasyDialog::AddElement(CDialogElement *Element)
{
	if (Element) {		
		Element->SetID(AktID,AktButtonID,AktHelpID);		
		ElementList.AddTail(Element);
	}
}

void CEasyDialog::NewColumn() 
{
	int LinesSinceLastMenu=ElementList.GetCount();
	int AddLines=(LinesSinceLastMenu/MaxLines)+1;
	AddLines=AddLines*MaxLines;
	AddLines=AddLines-LinesSinceLastMenu;
	for (int i=0;i<AddLines;i++) AddElement(new CElementStatic(""));
}

void CEasyDialog::UpDownStartHere()
{	
	MinUpDownMenuNr=AktButtonID-IDS_BUTTON0;
}

void CEasyDialog::UpDownStopHere()
{
	MaxUpDownMenuNr=AktButtonID-IDS_BUTTON0;
	if (MaxUpDownMenuNr>0) MaxUpDownMenuNr--;
}

HICON m_hIconSmall;
HICON m_hIconLarge;

BOOL CEasyDialog::OnInitDialog() 
{	
	((CControlApp*)AfxGetApp())->OnWakeUp();
	//RECT rec={10,600,1000,620};
	//StatusBar.Create("Status", SS_SIMPLE + WS_CHILD + WS_VISIBLE, rec, this);
	unsigned int Line=0;
	POSITION pos=ElementList.GetHeadPosition();
	AktID=IDS_EDIT0;
	AktHelpID=IDS_HELPBUTTON0;
	AktButtonID=IDS_BUTTON0;
	CWnd *AktDialog=this;
	EnableToolTips();	
	if (ToolTip) delete ToolTip;
	ToolTip=new CToolTipCtrlEx();
	ToolTip->Create(this);
	while (pos!=NULL) {
		CDialogElement* Element=(CDialogElement*)ElementList.GetNext(pos);
		if (Element) {
			Element->SetID(AktID,AktButtonID,AktHelpID);
			Element->Create(Line,(CEasyDialog*&)AktDialog,NULL);
			Line++;
		}
	}
	ToolTip->Activate(TRUE);
	CDialog::SetWindowText(Caption);
	RECT rect;
	if ((!ActiveDialog) || (MainWindow)) {
		rect.left = MainDialogStartX;
		rect.top = MainDialogStartY;
	}
	else {
		ActiveDialog->GetWindowRect(&rect);
		rect.top += 30;
	}
	CDialog::SetWindowPos(&CWnd::wndTop, rect.left, rect.top, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
	CDialog::OnInitDialog();

	if (MainWindow) {

		// Load your icon
		m_hIconSmall = AfxGetApp()->LoadIcon(IDI_ICON1);
		m_hIconLarge = (HICON)::LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_ICON1),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXICON),
			GetSystemMetrics(SM_CYICON),
			0);

		SetIcon(m_hIconLarge, TRUE);   // Set big icon (e.g., Alt+Tab, title bar)
		SetIcon(m_hIconSmall, FALSE);  // Set small icon (e.g., taskbar button)

		//SetClassLongPtr(m_pMainWnd->m_hWnd, GCLP_HICON, (LONG_PTR)m_hIconLarge);
		//SetClassLongPtr(m_pMainWnd->m_hWnd, GCLP_HICONSM, (LONG_PTR)m_hIconSmall);

		// Remove owner to allow taskbar button
		::SetWindowLongPtr(this->GetSafeHwnd(), GWLP_HWNDPARENT, 0);

		// Add WS_EX_APPWINDOW to force taskbar button
		LONG_PTR exStyle = ::GetWindowLongPtr(this->GetSafeHwnd(), GWL_EXSTYLE);
		exStyle |= WS_EX_APPWINDOW;
		::SetWindowLongPtr(this->GetSafeHwnd(), GWL_EXSTYLE, exStyle);

		// Optional: ensure the dialog is considered the main window
		AfxGetApp()->m_pMainWnd = this;
	}

	// Modify style to include minimize button
	LONG style = ::GetWindowLong(this->GetSafeHwnd(), GWL_STYLE);
	style |= WS_MINIMIZEBOX | WS_SYSMENU;
	::SetWindowLong(this->GetSafeHwnd(), GWL_STYLE, style);

	// Redraw non-client area to apply new style
	::SetWindowPos(this->GetSafeHwnd(), nullptr, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEasyDialog::OnApply() 
{
#ifdef DebugEasyDialog
	DebugProtocol("OnApply");
#endif
	//Timestamp.Mark("CEasyDialog::OnApply 0");
	((CControlApp*)AfxGetApp())->OnWakeUp();
	//Timestamp.Mark("CEasyDialog::OnApply 1");
	UpdateData(TRUE);
	//Timestamp.Mark("CEasyDialog::OnApply 2");
	UpdateData(FALSE);
	//Timestamp.Mark("CEasyDialog::OnApply 3");
	if ((Message)) {
		CallingDialog=this;
		Sequence->MessageMap(Message,this);
	}
	//Timestamp.Mark("CEasyDialog::OnApply 4");
}

void CEasyDialog::OnButton(unsigned int Nr)
{	
	((CControlApp*)AfxGetApp())->OnWakeUp();
	POSITION pos=ElementList.GetHeadPosition();
	while (pos!=NULL) {
		CDialogElement* Element=(CDialogElement*)ElementList.GetNext(pos);
		if (Element) {
			if (Element->IsKindOf(RUNTIME_CLASS(CElementButton))) {
				if (Element->ID==IDS_BUTTON0+Nr) {
#ifdef DebugEasyDialog
					DebugProtocol("OnButton->Action");
#endif
					int ReturnValue=((CElementButton*)Element)->Action(this);
					switch (ReturnValue) {
						case IDUP:if ((UpDownEnabled) && (Nr>MinUpDownMenuNr)) OnButton(Nr-1);break;
						case IDDOWN:if ((UpDownEnabled) && (Nr<MaxUpDownMenuNr)) OnButton(Nr+1);break;							
						case IDUPMOUSE:if ((UpDownEnabled) && (Nr>MinUpDownMenuNr)) OnButton(Nr-1); else OnButton(Nr); break;
						case IDDOWNMOUSE:if ((UpDownEnabled) && (Nr<MaxUpDownMenuNr)) OnButton(Nr+1); else OnButton(Nr); break;							
					}
					return;
				}
			}
		}
	}	
}

void CEasyDialog::OnHelpButton(unsigned int Nr)
{	
	((CControlApp*)AfxGetApp())->OnWakeUp();
	POSITION pos=ElementList.GetHeadPosition();
	while (pos!=NULL) {
		CDialogElement* Element=(CDialogElement*)ElementList.GetNext(pos);
		if (Element) {
			if (Element->HelpID==IDS_HELPBUTTON0+Nr) Element->DisplayHelp();			
		}
	}	
}

bool CEasyDialog::ContainsEnabledFlowDecidingCheckBox()
{	
	POSITION pos=ElementList.GetHeadPosition();
	while (pos!=NULL) {
		CDialogElement* Element=(CDialogElement*)ElementList.GetNext(pos);
		if (Element) {
			if (Element->IsKindOf(RUNTIME_CLASS(CElementBool))) {
				if (((CElementBool*)(Element))->IsEnabledFlowDecidingCheckBox()) return true;
			}
		}
	}	
	return false;
}

void CEasyDialog::SetCaption(CString aCaption)
{
	Caption=aCaption;
}

//int CEasyDialog::DoModal() 
INT_PTR CEasyDialog::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class
	ReCreate(); 
	return CDialog::DoModal();
}

void CEasyDialog::ReCreate()
{

}

LRESULT CEasyDialog::OnKickIdle(WPARAM, LPARAM lCount) {
	 AfxGetApp()->OnIdle(lCount);
	 return true;
}

void CEasyDialog::OnBnClickedRunExperiment()
{
	((CControlApp*)AfxGetApp())->OnWakeUp();
	UpdateData(TRUE);
	if (Message) {
		CallingDialog=this;
		Sequence->MessageMap(Message,this);
	}
	Sequence->MessageMap(IDM_RUN_EXPERIMENT,this);
	UpdateData(FALSE);
}

void CEasyDialog::OnBnClickedResetsystem()
{
	((CControlApp*)AfxGetApp())->OnWakeUp();
	UpdateData(TRUE);
	if (Message) {
		CallingDialog=this;
		Sequence->MessageMap(Message,this);
	}
	Sequence->MessageMap(IDM_RESET_SYSTEM,this);
	UpdateData(FALSE);
}

void CEasyDialog::OnBnClickedEmergencyShutoff()
{
	((CControlApp*)AfxGetApp())->OnWakeUp();
	UpdateData(TRUE);
	if (Message) {
		CallingDialog=this;
		Sequence->MessageMap(Message,this);
	}
	Sequence->MessageMap(IDM_EMERGENCY_SHUTOFF,this);
	UpdateData(FALSE);
}

BOOL CEasyDialog::PreTranslateMessage(MSG* pMsg)
{
	if (m_bDialogClosed)
		return FALSE; // Don't process messages if dialog is closed

#ifdef DebugEasyDialog
	CString buf;
	buf.Format("PreTranslateMessage(msg=%u)", pMsg->message);
	DebugProtocol(buf);
#endif
	//We had a bug somewhere here, with the debugger not pointing out the problem.
	//Now we check the validity of all relevant pointers.
	//Later it was found that when ToolTip was deleted when a dialog was closed, it was not set to nullptr.
	//Even if its strange that PreTranslateMessage is called after the dialog is closed, maybe it was the bug that we had.
	//Now we set ToolTip to nullptr after deleting it, so if that was the origin, the bug shouldn't happen and all the ASSERT below are not needed.
	//removing the ASSERTs below leads to the error reappearing. Strangely, the ASSERTs below are not triggered when the bug happens.
	//ASSERT(pMsg != nullptr);
	//ASSERT(this != nullptr);
	//ASSERT(AfxIsValidAddress(this, sizeof(CEasyDialog), FALSE)); // FALSE = not writable check
	//ASSERT(AfxIsValidAddress(pMsg, sizeof(MSG), FALSE));
/*	if (ToolTip != nullptr)
		ASSERT(AfxIsValidAddress(ToolTip, sizeof(*ToolTip), FALSE));
	if (!AfxIsValidAddress(pMsg, sizeof(MSG), FALSE)) {
		TRACE("Invalid pMsg\n");
		ControlMessageBox("CEasyDialog::PreTranslateMessage: error : pMsg not valid");
	}*/
	//Here we code the equivalent of the ASSERTs, so that they are executed even in Release mode.
	/*
	if (this == nullptr) ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : this == nullptr");
	if (!AfxIsValidAddress(this, sizeof(CEasyDialog), FALSE)) ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : this is not valid");
	if (pMsg == nullptr) ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : pMsg == nullptr");
	if (!AfxIsValidAddress(pMsg, sizeof(MSG), FALSE)) ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : pMsg is not valid");
	
	if (ToolTip && ::IsWindow(ToolTip->GetSafeHwnd())) {
		//The following check is likely not necessary. 
		//ASSERT(AfxIsValidAddress(ToolTip, sizeof(*ToolTip), FALSE));
		if (AfxIsValidAddress(ToolTip, sizeof(*ToolTip), FALSE)) {
			if (ToolTip->m_hWnd != nullptr && ::IsWindow(ToolTip->m_hWnd)) {
				if (AfxIsValidAddress(ToolTip->m_hWnd, sizeof(*ToolTip->m_hWnd), FALSE))
					if (ToolTip->m_hWnd != INVALID_HANDLE_VALUE)
						//ToolTip->RelayEvent(pMsg);
						;
					else
						ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : ToolTip->m_hWnd contains INVALID_HANDLE_VALUE");
				else
					ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : ToolTip->m_hWnd not valid");
			}
			else
				ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : ToolTip->m_hWnd contains INVALID_HANDLE_VALUE");
		} 
		else
			ControlMessageBox("CEasyDialog::PreTranslateMessage: strange error : ToolTip not valid");
	}
	*/
    return CDialog::PreTranslateMessage(pMsg);
}