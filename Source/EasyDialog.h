#if !defined(AFX_EASYDIALOG_H__552B3A5D_C4B0_435F_AD19_EEE3488AD02C__INCLUDED_)
#define AFX_EASYDIALOG_H__552B3A5D_C4B0_435F_AD19_EEE3488AD02C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EasyDialog.h : header file
//
#include "stdafx.h"
//#include "ElementStatic.h"
#include "Resource.h"
#include "FlatToolTipCtrl.h"


//#define DebugEasyDialog

class CMessageReceiver;
class CDialogElement;

/////////////////////////////////////////////////////////////////////////////
// CEasyDialog dialog

class CEasyDialog : public CDialog
{
public:
	CStatic StatusBar;
	CToolTipCtrlEx* ToolTip;
private:
	unsigned int AktID;
	unsigned int AktButtonID;
	unsigned int AktHelpID;
	unsigned int Message;
	bool MainWindow;
	int MainDialogStartX;
	int MainDialogStartY;
	CMessageReceiver* MessageReceiver;
// Construction
public:		
	BOOL PreTranslateMessage(MSG* pMsg);
	void OnDestroy();
	bool ContainsEnabledFlowDecidingCheckBox();
	bool UpDownButtonEnabled;
	void UpDownStopHere();
	void UpDownStartHere();
	unsigned int MaxUpDownMenuNr;
	unsigned int MinUpDownMenuNr;
	bool UpDownEnabled;
	void DeleteElements();
	virtual void ReCreate();
	bool Refresh;
	void SetCaption(CString aCaption);
	void OnButton(unsigned int Nr);
	void OnHelpButton(unsigned int Nr);
	void AddElement(CDialogElement *Element);
	void NewColumn();
	CEasyDialog(CString Caption="", bool MainWindow = false, int _MainDialogStartX=0, int _MainDialogStartY=0, unsigned int aMessage=0,CMessageReceiver* aMessageReceiver=NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEasyDialog();
#ifdef DebugEasyDialog
	void DebugProtocol(CString message);
#endif
	virtual void OnOK( );
	void OnUp(int mode);
	void OnDown(int mode);

// Dialog Data
	//{{AFX_DATA(CEasyDialog)
	enum { IDD = IDD_EASYDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyDialog)
	public:
	//virtual int DoModal();
	virtual INT_PTR DoModal();  //x64
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEasyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnApply();
	afx_msg void OnButton0() {OnButton(0);}
	afx_msg void OnButton1() {OnButton(1);}
	afx_msg void OnButton2() {OnButton(2);}
	afx_msg void OnButton3() {OnButton(3);}
	afx_msg void OnButton4() {OnButton(4);}
	afx_msg void OnButton5() {OnButton(5);}
	afx_msg void OnButton6() {OnButton(6);}
	afx_msg void OnButton7() {OnButton(7);}
	afx_msg void OnButton8() {OnButton(8);}
	afx_msg void OnButton9() {OnButton(9);}
	afx_msg void OnButton10() {OnButton(10);}
	afx_msg void OnButton11() {OnButton(11);}
	afx_msg void OnButton12() {OnButton(12);}
	afx_msg void OnButton13() {OnButton(13);}
	afx_msg void OnButton14() {OnButton(14);}
	afx_msg void OnButton15() {OnButton(15);}
	afx_msg void OnButton16() {OnButton(16);}
	afx_msg void OnButton17() {OnButton(17);}
	afx_msg void OnButton18() {OnButton(18);}
	afx_msg void OnButton19() {OnButton(19);}
	afx_msg void OnButton20() {OnButton(20);}
	afx_msg void OnButton21() {OnButton(21);}
	afx_msg void OnButton22() {OnButton(22);}
	afx_msg void OnButton23() {OnButton(23);}
	afx_msg void OnButton24() {OnButton(24);}
	afx_msg void OnButton25() {OnButton(25);}
	afx_msg void OnButton26() {OnButton(26);}
	afx_msg void OnButton27() {OnButton(27);}
	afx_msg void OnButton28() {OnButton(28);}
	afx_msg void OnButton29() {OnButton(29);}
	afx_msg void OnButton30() {OnButton(30);}
	afx_msg void OnButton31() {OnButton(31);}
	afx_msg void OnButton32() {OnButton(32);}
	afx_msg void OnButton33() {OnButton(33);}
	afx_msg void OnButton34() {OnButton(34);}
	afx_msg void OnButton35() {OnButton(35);}
	afx_msg void OnButton36() {OnButton(36);}
	afx_msg void OnButton37() {OnButton(37);}
	afx_msg void OnButton38() {OnButton(38);}
	afx_msg void OnButton39() {OnButton(39);}	
	afx_msg void OnButton40() {OnButton(40);}	
	afx_msg void OnButton41() {OnButton(41);}	
	afx_msg void OnButton42() {OnButton(42);}	
	afx_msg void OnButton43() {OnButton(43);}	
	afx_msg void OnButton44() {OnButton(44);}	
	afx_msg void OnButton45() {OnButton(45);}	
	afx_msg void OnButton46() {OnButton(46);}	
	afx_msg void OnButton47() {OnButton(47);}	
	afx_msg void OnButton48() {OnButton(48);}	
	afx_msg void OnButton49() {OnButton(49);}	
	afx_msg void OnButton50() {OnButton(50);}	
	afx_msg void OnButton51() {OnButton(51);}	
	afx_msg void OnButton52() {OnButton(52);}	
	afx_msg void OnButton53() {OnButton(53);}	
	afx_msg void OnButton54() {OnButton(54);}	
	afx_msg void OnButton55() {OnButton(55);}	
	afx_msg void OnButton56() {OnButton(56);}	
	afx_msg void OnButton57() {OnButton(57);}	
	afx_msg void OnButton58() {OnButton(58);}	
	afx_msg void OnButton59() {OnButton(59);}	
	afx_msg void OnButton60() {OnButton(60);}

	afx_msg void OnButton61() {OnButton(61);}	
	afx_msg void OnButton62() {OnButton(62);}	
	afx_msg void OnButton63() {OnButton(63);}	
	afx_msg void OnButton64() {OnButton(64);}	
	afx_msg void OnButton65() {OnButton(65);}	
	afx_msg void OnButton66() {OnButton(66);}	
	afx_msg void OnButton67() {OnButton(67);}	
	afx_msg void OnButton68() {OnButton(68);}	
	afx_msg void OnButton69() {OnButton(69);}

	afx_msg void OnButton70() {OnButton(70);}
	afx_msg void OnButton71() {OnButton(71);}	
	afx_msg void OnButton72() {OnButton(72);}	
	afx_msg void OnButton73() {OnButton(73);}	
	afx_msg void OnButton74() {OnButton(74);}	
	afx_msg void OnButton75() {OnButton(75);}	
	afx_msg void OnButton76() {OnButton(76);}	
	afx_msg void OnButton77() {OnButton(77);}	
	afx_msg void OnButton78() {OnButton(78);}	
	afx_msg void OnButton79() {OnButton(79);}

	afx_msg void OnButton80() {OnButton(80);}
	afx_msg void OnButton81() {OnButton(81);}	
	afx_msg void OnButton82() {OnButton(82);}	
	afx_msg void OnButton83() {OnButton(83);}	
	afx_msg void OnButton84() {OnButton(84);}	
	afx_msg void OnButton85() {OnButton(85);}	
	afx_msg void OnButton86() {OnButton(86);}	
	afx_msg void OnButton87() {OnButton(87);}	
	afx_msg void OnButton88() {OnButton(88);}	
	afx_msg void OnButton89() {OnButton(89);}	
	
	afx_msg void OnButton90() {OnButton(90);}
	afx_msg void OnButton91() {OnButton(91);}	
	afx_msg void OnButton92() {OnButton(92);}	
	afx_msg void OnButton93() {OnButton(93);}	
	afx_msg void OnButton94() {OnButton(94);}	
	afx_msg void OnButton95() {OnButton(95);}	
	afx_msg void OnButton96() {OnButton(96);}	
	afx_msg void OnButton97() {OnButton(97);}	
	afx_msg void OnButton98() {OnButton(98);}	
	afx_msg void OnButton99() {OnButton(99);}	
	
	afx_msg void OnButton100() {OnButton(100);}
	afx_msg void OnButton101() {OnButton(101);}	
	afx_msg void OnButton102() {OnButton(102);}	
	afx_msg void OnButton103() {OnButton(103);}	
	afx_msg void OnButton104() {OnButton(104);}	
	afx_msg void OnButton105() {OnButton(105);}	
	afx_msg void OnButton106() {OnButton(106);}	
	afx_msg void OnButton107() {OnButton(107);}	
	afx_msg void OnButton108() {OnButton(108);}	
	afx_msg void OnButton109() {OnButton(109);}	

	afx_msg void OnButton110() {OnButton(110);}
	afx_msg void OnButton111() {OnButton(111);}	
	afx_msg void OnButton112() {OnButton(112);}	
	afx_msg void OnButton113() {OnButton(113);}	
	afx_msg void OnButton114() {OnButton(114);}	
	afx_msg void OnButton115() {OnButton(115);}	
	afx_msg void OnButton116() {OnButton(116);}	
	afx_msg void OnButton117() {OnButton(117);}	
	afx_msg void OnButton118() {OnButton(118);}	
	afx_msg void OnButton119() {OnButton(119);}	

	afx_msg void OnButton120() {OnButton(120);}
	afx_msg void OnButton121() {OnButton(121);}	
	afx_msg void OnButton122() {OnButton(122);}	
	afx_msg void OnButton123() {OnButton(123);}	
	afx_msg void OnButton124() {OnButton(124);}	
	afx_msg void OnButton125() {OnButton(125);}	
	afx_msg void OnButton126() {OnButton(126);}	
	afx_msg void OnButton127() {OnButton(127);}	
	afx_msg void OnButton128() {OnButton(128);}	
	afx_msg void OnButton129() {OnButton(129);}	

	afx_msg void OnButton130() {OnButton(130);}
	afx_msg void OnButton131() {OnButton(131);}	
	afx_msg void OnButton132() {OnButton(132);}	
	afx_msg void OnButton133() {OnButton(133);}	
	afx_msg void OnButton134() {OnButton(134);}	
	afx_msg void OnButton135() {OnButton(135);}	
	afx_msg void OnButton136() {OnButton(136);}	
	afx_msg void OnButton137() {OnButton(137);}	
	afx_msg void OnButton138() {OnButton(138);}	
	afx_msg void OnButton139() {OnButton(139);}	

	afx_msg void OnButton140() {OnButton(140);}
	afx_msg void OnButton141() {OnButton(141);}	
	afx_msg void OnButton142() {OnButton(142);}	
	afx_msg void OnButton143() {OnButton(143);}	
	afx_msg void OnButton144() {OnButton(144);}	
	afx_msg void OnButton145() {OnButton(145);}	
	afx_msg void OnButton146() {OnButton(146);}	
	afx_msg void OnButton147() {OnButton(147);}	
	afx_msg void OnButton148() {OnButton(148);}	
	afx_msg void OnButton149() {OnButton(149);}	

	afx_msg void OnButton150() {OnButton(150);}
	afx_msg void OnButton151() {OnButton(151);}	
	afx_msg void OnButton152() {OnButton(152);}	
	afx_msg void OnButton153() {OnButton(153);}	
	afx_msg void OnButton154() {OnButton(154);}	
	afx_msg void OnButton155() {OnButton(155);}	
	afx_msg void OnButton156() {OnButton(156);}	
	afx_msg void OnButton157() {OnButton(157);}	
	afx_msg void OnButton158() {OnButton(158);}	
	afx_msg void OnButton159() {OnButton(159);}	

	afx_msg void OnButton160() {OnButton(160);}
	afx_msg void OnButton161() {OnButton(161);}	
	afx_msg void OnButton162() {OnButton(162);}	
	afx_msg void OnButton163() {OnButton(163);}	
	afx_msg void OnButton164() {OnButton(164);}	
	afx_msg void OnButton165() {OnButton(165);}	
	afx_msg void OnButton166() {OnButton(166);}	
	afx_msg void OnButton167() {OnButton(167);}	
	afx_msg void OnButton168() {OnButton(168);}	
	afx_msg void OnButton169() {OnButton(169);}	

	afx_msg void OnButton170() {OnButton(170);}
	afx_msg void OnButton171() {OnButton(171);}	
	afx_msg void OnButton172() {OnButton(172);}	
	afx_msg void OnButton173() {OnButton(173);}	
	afx_msg void OnButton174() {OnButton(174);}	
	afx_msg void OnButton175() {OnButton(175);}	
	afx_msg void OnButton176() {OnButton(176);}	
	afx_msg void OnButton177() {OnButton(177);}	
	afx_msg void OnButton178() {OnButton(178);}	
	afx_msg void OnButton179() {OnButton(179);}	
	
	//HelpButtons
	afx_msg void OnHelpButton0() {OnHelpButton(0);}
	afx_msg void OnHelpButton1() {OnHelpButton(1);}
	afx_msg void OnHelpButton2() {OnHelpButton(2);}
	afx_msg void OnHelpButton3() {OnHelpButton(3);}
	afx_msg void OnHelpButton4() {OnHelpButton(4);}
	afx_msg void OnHelpButton5() {OnHelpButton(5);}
	afx_msg void OnHelpButton6() {OnHelpButton(6);}
	afx_msg void OnHelpButton7() {OnHelpButton(7);}
	afx_msg void OnHelpButton8() {OnHelpButton(8);}
	afx_msg void OnHelpButton9() {OnHelpButton(9);}
	afx_msg void OnHelpButton10() {OnHelpButton(10);}
	afx_msg void OnHelpButton11() {OnHelpButton(11);}
	afx_msg void OnHelpButton12() {OnHelpButton(12);}
	afx_msg void OnHelpButton13() {OnHelpButton(13);}
	afx_msg void OnHelpButton14() {OnHelpButton(14);}
	afx_msg void OnHelpButton15() {OnHelpButton(15);}
	afx_msg void OnHelpButton16() {OnHelpButton(16);}
	afx_msg void OnHelpButton17() {OnHelpButton(17);}
	afx_msg void OnHelpButton18() {OnHelpButton(18);}
	afx_msg void OnHelpButton19() {OnHelpButton(19);}
	afx_msg void OnHelpButton20() {OnHelpButton(20);}
	afx_msg void OnHelpButton21() {OnHelpButton(21);}
	afx_msg void OnHelpButton22() {OnHelpButton(22);}
	afx_msg void OnHelpButton23() {OnHelpButton(23);}
	afx_msg void OnHelpButton24() {OnHelpButton(24);}
	afx_msg void OnHelpButton25() {OnHelpButton(25);}
	afx_msg void OnHelpButton26() {OnHelpButton(26);}
	afx_msg void OnHelpButton27() {OnHelpButton(27);}
	afx_msg void OnHelpButton28() {OnHelpButton(28);}
	afx_msg void OnHelpButton29() {OnHelpButton(29);}
	afx_msg void OnHelpButton30() {OnHelpButton(30);}
	afx_msg void OnHelpButton31() {OnHelpButton(31);}
	afx_msg void OnHelpButton32() {OnHelpButton(32);}
	afx_msg void OnHelpButton33() {OnHelpButton(33);}
	afx_msg void OnHelpButton34() {OnHelpButton(34);}
	afx_msg void OnHelpButton35() {OnHelpButton(35);}
	afx_msg void OnHelpButton36() {OnHelpButton(36);}
	afx_msg void OnHelpButton37() {OnHelpButton(37);}
	afx_msg void OnHelpButton38() {OnHelpButton(38);}
	afx_msg void OnHelpButton39() {OnHelpButton(39);}	
	afx_msg void OnHelpButton40() {OnHelpButton(40);}	
	afx_msg void OnHelpButton41() {OnHelpButton(41);}	
	afx_msg void OnHelpButton42() {OnHelpButton(42);}	
	afx_msg void OnHelpButton43() {OnHelpButton(43);}	
	afx_msg void OnHelpButton44() {OnHelpButton(44);}	
	afx_msg void OnHelpButton45() {OnHelpButton(45);}	
	afx_msg void OnHelpButton46() {OnHelpButton(46);}	
	afx_msg void OnHelpButton47() {OnHelpButton(47);}	
	afx_msg void OnHelpButton48() {OnHelpButton(48);}	
	afx_msg void OnHelpButton49() {OnHelpButton(49);}	
	afx_msg void OnHelpButton50() {OnHelpButton(50);}	
	afx_msg void OnHelpButton51() {OnHelpButton(51);}	
	afx_msg void OnHelpButton52() {OnHelpButton(52);}	
	afx_msg void OnHelpButton53() {OnHelpButton(53);}	
	afx_msg void OnHelpButton54() {OnHelpButton(54);}	
	afx_msg void OnHelpButton55() {OnHelpButton(55);}	
	afx_msg void OnHelpButton56() {OnHelpButton(56);}	
	afx_msg void OnHelpButton57() {OnHelpButton(57);}	
	afx_msg void OnHelpButton58() {OnHelpButton(58);}	
	afx_msg void OnHelpButton59() {OnHelpButton(59);}	
	afx_msg void OnHelpButton60() {OnHelpButton(60);}

	afx_msg void OnHelpButton61() {OnHelpButton(61);}	
	afx_msg void OnHelpButton62() {OnHelpButton(62);}	
	afx_msg void OnHelpButton63() {OnHelpButton(63);}	
	afx_msg void OnHelpButton64() {OnHelpButton(64);}	
	afx_msg void OnHelpButton65() {OnHelpButton(65);}	
	afx_msg void OnHelpButton66() {OnHelpButton(66);}	
	afx_msg void OnHelpButton67() {OnHelpButton(67);}	
	afx_msg void OnHelpButton68() {OnHelpButton(68);}	
	afx_msg void OnHelpButton69() {OnHelpButton(69);}

	afx_msg void OnHelpButton70() {OnHelpButton(70);}
	afx_msg void OnHelpButton71() {OnHelpButton(71);}	
	afx_msg void OnHelpButton72() {OnHelpButton(72);}	
	afx_msg void OnHelpButton73() {OnHelpButton(73);}	
	afx_msg void OnHelpButton74() {OnHelpButton(74);}	
	afx_msg void OnHelpButton75() {OnHelpButton(75);}	
	afx_msg void OnHelpButton76() {OnHelpButton(76);}	
	afx_msg void OnHelpButton77() {OnHelpButton(77);}	
	afx_msg void OnHelpButton78() {OnHelpButton(78);}	
	afx_msg void OnHelpButton79() {OnHelpButton(79);}

	afx_msg void OnHelpButton80() {OnHelpButton(80);}
	afx_msg void OnHelpButton81() {OnHelpButton(81);}	
	afx_msg void OnHelpButton82() {OnHelpButton(82);}	
	afx_msg void OnHelpButton83() {OnHelpButton(83);}	
	afx_msg void OnHelpButton84() {OnHelpButton(84);}	
	afx_msg void OnHelpButton85() {OnHelpButton(85);}	
	afx_msg void OnHelpButton86() {OnHelpButton(86);}	
	afx_msg void OnHelpButton87() {OnHelpButton(87);}	
	afx_msg void OnHelpButton88() {OnHelpButton(88);}	
	afx_msg void OnHelpButton89() {OnHelpButton(89);}	
	
	afx_msg void OnHelpButton90() {OnHelpButton(90);}
	afx_msg void OnHelpButton91() {OnHelpButton(91);}	
	afx_msg void OnHelpButton92() {OnHelpButton(92);}	
	afx_msg void OnHelpButton93() {OnHelpButton(93);}	
	afx_msg void OnHelpButton94() {OnHelpButton(94);}	
	afx_msg void OnHelpButton95() {OnHelpButton(95);}	
	afx_msg void OnHelpButton96() {OnHelpButton(96);}	
	afx_msg void OnHelpButton97() {OnHelpButton(97);}	
	afx_msg void OnHelpButton98() {OnHelpButton(98);}	
	afx_msg void OnHelpButton99() {OnHelpButton(99);}	
	
	afx_msg void OnHelpButton100() {OnHelpButton(100);}
	afx_msg void OnHelpButton101() {OnHelpButton(101);}	
	afx_msg void OnHelpButton102() {OnHelpButton(102);}	
	afx_msg void OnHelpButton103() {OnHelpButton(103);}	
	afx_msg void OnHelpButton104() {OnHelpButton(104);}	
	afx_msg void OnHelpButton105() {OnHelpButton(105);}	
	afx_msg void OnHelpButton106() {OnHelpButton(106);}	
	afx_msg void OnHelpButton107() {OnHelpButton(107);}	
	afx_msg void OnHelpButton108() {OnHelpButton(108);}	
	afx_msg void OnHelpButton109() {OnHelpButton(109);}	

	afx_msg void OnHelpButton110() {OnHelpButton(110);}
	afx_msg void OnHelpButton111() {OnHelpButton(111);}	
	afx_msg void OnHelpButton112() {OnHelpButton(112);}	
	afx_msg void OnHelpButton113() {OnHelpButton(113);}	
	afx_msg void OnHelpButton114() {OnHelpButton(114);}	
	afx_msg void OnHelpButton115() {OnHelpButton(115);}	
	afx_msg void OnHelpButton116() {OnHelpButton(116);}	
	afx_msg void OnHelpButton117() {OnHelpButton(117);}	
	afx_msg void OnHelpButton118() {OnHelpButton(118);}	
	afx_msg void OnHelpButton119() {OnHelpButton(119);}	

	afx_msg void OnHelpButton120() {OnHelpButton(120);}
	afx_msg void OnHelpButton121() {OnHelpButton(121);}	
	afx_msg void OnHelpButton122() {OnHelpButton(122);}	
	afx_msg void OnHelpButton123() {OnHelpButton(123);}	
	afx_msg void OnHelpButton124() {OnHelpButton(124);}	
	afx_msg void OnHelpButton125() {OnHelpButton(125);}	
	afx_msg void OnHelpButton126() {OnHelpButton(126);}	
	afx_msg void OnHelpButton127() {OnHelpButton(127);}	
	afx_msg void OnHelpButton128() {OnHelpButton(128);}	
	afx_msg void OnHelpButton129() {OnHelpButton(129);}	

	afx_msg void OnHelpButton130() {OnHelpButton(130);}
	afx_msg void OnHelpButton131() {OnHelpButton(131);}	
	afx_msg void OnHelpButton132() {OnHelpButton(132);}	
	afx_msg void OnHelpButton133() {OnHelpButton(133);}	
	afx_msg void OnHelpButton134() {OnHelpButton(134);}	
	afx_msg void OnHelpButton135() {OnHelpButton(135);}	
	afx_msg void OnHelpButton136() {OnHelpButton(136);}	
	afx_msg void OnHelpButton137() {OnHelpButton(137);}	
	afx_msg void OnHelpButton138() {OnHelpButton(138);}	
	afx_msg void OnHelpButton139() {OnHelpButton(139);}	

	afx_msg void OnHelpButton140() {OnHelpButton(140);}
	afx_msg void OnHelpButton141() {OnHelpButton(141);}	
	afx_msg void OnHelpButton142() {OnHelpButton(142);}	
	afx_msg void OnHelpButton143() {OnHelpButton(143);}	
	afx_msg void OnHelpButton144() {OnHelpButton(144);}	
	afx_msg void OnHelpButton145() {OnHelpButton(145);}	
	afx_msg void OnHelpButton146() {OnHelpButton(146);}	
	afx_msg void OnHelpButton147() {OnHelpButton(147);}	
	afx_msg void OnHelpButton148() {OnHelpButton(148);}	
	afx_msg void OnHelpButton149() {OnHelpButton(149);}	

	afx_msg void OnHelpButton150() {OnHelpButton(150);}
	afx_msg void OnHelpButton151() {OnHelpButton(151);}	
	afx_msg void OnHelpButton152() {OnHelpButton(152);}	
	afx_msg void OnHelpButton153() {OnHelpButton(153);}	
	afx_msg void OnHelpButton154() {OnHelpButton(154);}	
	afx_msg void OnHelpButton155() {OnHelpButton(155);}	
	afx_msg void OnHelpButton156() {OnHelpButton(156);}	
	afx_msg void OnHelpButton157() {OnHelpButton(157);}	
	afx_msg void OnHelpButton158() {OnHelpButton(158);}	
	afx_msg void OnHelpButton159() {OnHelpButton(159);}	

	afx_msg void OnHelpButton160() {OnHelpButton(160);}
	afx_msg void OnHelpButton161() {OnHelpButton(161);}	
	afx_msg void OnHelpButton162() {OnHelpButton(162);}	
	afx_msg void OnHelpButton163() {OnHelpButton(163);}	
	afx_msg void OnHelpButton164() {OnHelpButton(164);}	
	afx_msg void OnHelpButton165() {OnHelpButton(165);}	
	afx_msg void OnHelpButton166() {OnHelpButton(166);}	
	afx_msg void OnHelpButton167() {OnHelpButton(167);}	
	afx_msg void OnHelpButton168() {OnHelpButton(168);}	
	afx_msg void OnHelpButton169() {OnHelpButton(169);}	

	afx_msg void OnHelpButton170() {OnHelpButton(170);}
	afx_msg void OnHelpButton171() {OnHelpButton(171);}	
	afx_msg void OnHelpButton172() {OnHelpButton(172);}	
	afx_msg void OnHelpButton173() {OnHelpButton(173);}	
	afx_msg void OnHelpButton174() {OnHelpButton(174);}	
	afx_msg void OnHelpButton175() {OnHelpButton(175);}	
	afx_msg void OnHelpButton176() {OnHelpButton(176);}	
	afx_msg void OnHelpButton177() {OnHelpButton(177);}	
	afx_msg void OnHelpButton178() {OnHelpButton(178);}	
	afx_msg void OnHelpButton179() {OnHelpButton(179);}	



	afx_msg void OnUp();
	afx_msg void OnDown();	
	afx_msg BOOL OnMouseWheel( UINT, short, CPoint );
	afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
private:
	CString Caption;
	CObList ElementList;
	void (*UpdateFunction)(unsigned int);  
	int UpdateFunctionParameter;
	bool AutoUpdate;
	bool m_bDialogClosed;
public:
	afx_msg void OnBnClickedRunExperiment();
	afx_msg void OnBnClickedResetsystem();
	afx_msg void OnBnClickedEmergencyShutoff();
};

extern CWnd* CallingDialog;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EASYDIALOG_H__552B3A5D_C4B0_435F_AD19_EEE3488AD02C__INCLUDED_)
