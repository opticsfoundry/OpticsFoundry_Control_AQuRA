// DialogElement.cpp: implementation of the CDialogElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "DialogElement.h"
#include "MenuObListElement.h"
#include "Param.h"
#include "SimpleColorButton.h"
#include "color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC( CDialogElement, CObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogElement::CDialogElement(CMenuObListElement* aMyMenuObListElement,const COLORREF aColor)
{
	ID=0;
	HelpID=0;
	if (aMyMenuObListElement) Color=aMyMenuObListElement->Color;	
	else Color=aColor;
	MyDialog=NULL;
	ButtonColor=RGB(1,1,1);
	ShowHelpButton=false;
	MyMenuObListElement=aMyMenuObListElement;	
	ButtonColor=RGB(1,1,1);
	OldButtonColor=-100;	
}

CDialogElement::~CDialogElement()
{	
}

const unsigned int MaxLines=38;

const unsigned int TopBorder=10;
const unsigned int HorizontalDistance=4;
const unsigned int Height=21;
const unsigned int CheckBoxWidth=16;
const unsigned int HelpButtonPosition=6;
const unsigned int HelpButtonWidth=6;
const unsigned int LeftCheckBox=18;
const unsigned int GroupBox=15;
const unsigned int LeftBorder=LeftCheckBox+CheckBoxWidth+HorizontalDistance;
const unsigned int VerticalDistance=3;
const unsigned int LineDistance=Height+VerticalDistance;
const unsigned int QuestionWidth=310;
const unsigned int EditPosition=LeftBorder+QuestionWidth+HorizontalDistance;
const unsigned int EditWidth=70;
const unsigned int UnitsPosition=EditPosition+EditWidth+HorizontalDistance;
const unsigned int UnitsWidth=80;
const unsigned int StringEditWidth=EditWidth+HorizontalDistance+UnitsWidth;
const unsigned int ComboBoxWidth=4*StringEditWidth;
const unsigned int CheckBoxEditWidth=Height;
const unsigned int GroupBoxWidth=155;
const unsigned int GroupBoxHeight=26;
const unsigned int ColumnDistance=10;
const unsigned int SecondColumnPos=EditPosition+StringEditWidth+ColumnDistance;
const unsigned int ComboBoxHeight=500;

RECT CDialogElement::GetQuestionRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+LeftBorder,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+LeftBorder+QuestionWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height};
	return rec;
}

RECT CDialogElement::GetStaticRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+LeftBorder,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+LeftBorder+QuestionWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height};		
	return rec;
}

RECT CDialogElement::GetHelpButtonRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+HelpButtonPosition,
		TopBorder+(Line%MaxLines)*LineDistance-3,
		(SecondColumnPos*(int)(Line/MaxLines))+HelpButtonPosition+HelpButtonWidth,
		TopBorder+(Line%MaxLines)*LineDistance+LineDistance-3};
	return rec;
}

RECT CDialogElement::GetEditRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+EditPosition,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+EditPosition+EditWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height};
	return rec;
}

RECT CDialogElement::GetUnitsRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+UnitsPosition,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+UnitsPosition+UnitsWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height};
	return rec;
}

RECT CDialogElement::GetStringEditRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+EditPosition,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+EditPosition+StringEditWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height};
	return rec;
}

RECT CDialogElement::GetStringComboBoxEditRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+EditPosition,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+EditPosition+ComboBoxWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height+ComboBoxHeight};
	return rec;
}

RECT CDialogElement::GetCheckBoxRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+EditPosition,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+EditPosition+CheckBoxEditWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height};
	return rec;
}

RECT CDialogElement::GetLeftCheckBoxRect(unsigned int Line)
{
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+LeftCheckBox,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+LeftCheckBox+CheckBoxEditWidth,
		TopBorder+(Line%MaxLines)*LineDistance+Height};
	return rec;
}

RECT CDialogElement::GetButtonRect(unsigned int Line, unsigned int NumberLines)
{
	if (NumberLines<1) NumberLines=1;
	if (NumberLines>10) NumberLines=10;
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+LeftBorder,
		TopBorder+(Line%MaxLines)*LineDistance,
		(SecondColumnPos*(int)(Line/MaxLines))+LeftBorder+QuestionWidth,
		TopBorder+((NumberLines-1)+Line%MaxLines)*LineDistance+Height};
	return rec;
}

RECT CDialogElement::GetGroupBoxRect(unsigned int Line, unsigned int NumberLines)
{
	if (NumberLines<1) NumberLines=1;
	if (NumberLines>10) NumberLines=10;
	RECT rec={(SecondColumnPos*(int)(Line/MaxLines))+GroupBox,
		TopBorder+(Line%MaxLines)*LineDistance,		
		(SecondColumnPos*(int)(Line/MaxLines))+EditPosition+GroupBoxWidth,
		TopBorder+((NumberLines-2)+Line%MaxLines)*LineDistance+GroupBoxHeight};
	return rec;
}

void CDialogElement::SetMyDialog(CEasyDialog *aMyDialog)
{
	MyDialog=aMyDialog;
}	

void CDialogElement::Create(unsigned int Line, CEasyDialog* &Dialog, CWnd* aHelpWnd)
{
	HelpWnd=aHelpWnd;
	MyDialog=Dialog;
	if (!ShowHelpButton) ButtonColor=-1;
	OldButtonColor=ButtonColor+1;
	HelpButton.Create(NULL, WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, GetHelpButtonRect(Line), Dialog, HelpID);
	CString ToolTipText="";
	if (MyMenuObListElement) {
		if (MyMenuObListElement->Help!="") {
			ToolTipText=MyMenuObListElement->Help;						
		}		
	}
	if (Dialog->ToolTip) {
		if (HelpWnd) Dialog->ToolTip->AddTool(HelpWnd, ToolTipText);
		Dialog->ToolTip->AddTool(&HelpButton, ToolTipText);
		Dialog->ToolTip->Activate(TRUE);
	}
	UpdateHelpButtonColor(false);		
}

void CDialogElement::UpdateHelpButtonColor(bool Redraw) {	
	if (MyMenuObListElement) {
		CParam* help=(CParam*)MyMenuObListElement;
		if  (help->IsKindOf( RUNTIME_CLASS( CParam) )) {		
			ButtonColor=(help->SameAsReference()) ? 0 : 1;
		}
		if (ButtonColor!=1) {			
			if (MyMenuObListElement->Help!="") ButtonColor=3;
		} else {
			if (MyMenuObListElement->Help!="") ButtonColor=7;
		}
	}
	if (ButtonColor!=OldButtonColor) {
		CString buf=GetHelp();
		if (MyDialog->ToolTip) {
			MyDialog->ToolTip->UpdateTipText(buf, &HelpButton);
			if (HelpWnd) MyDialog->ToolTip->UpdateTipText(buf, HelpWnd);
		}
		switch (ButtonColor) {
			case 0:HelpButton.SetColor(ColorGreen); break;
			case 1:HelpButton.SetColor(ColorRed); break;
			case 2:HelpButton.SetColor(ColorBlue); break;
			case 3:HelpButton.SetColor(ColorYellow); break;
			case 4:HelpButton.SetColor(ColorWhite); break;			
			case 5:HelpButton.SetColor(ColorGray);break;
			case 6:HelpButton.SetColor(ColorBlack);break;
			case 7:HelpButton.SetColor(ColorOrange);break;
			default:HelpButton.SetColor(ColorLightGray);
		}
		OldButtonColor=ButtonColor;
		if (Redraw) HelpButton.RedrawWindow();
	}
}

CString CDialogElement::GetHelp() {
	CString buf="";
	if (MyMenuObListElement) {			
		if (MyMenuObListElement->Help!="") {			
			buf=MyMenuObListElement->Help;
		}			
		CParam* help=(CParam*)MyMenuObListElement;
		if  (help->IsKindOf( RUNTIME_CLASS( CParam) )) {				
			if (!help->SameAsReference()) {
				if (buf!="") buf=buf+"\n\n";
				buf=buf+help->Name+"="+help->GetParamComparisonAsString();
			} else {
				if (buf!="") buf=buf+"\n\n";
				buf=buf+help->Name;
			}
		}
	}			
	return buf;
}


void CDialogElement::DisplayHelp() {
	CString buf=GetHelp();
	if (buf!="") ControlMessageBox(buf,MB_ICONINFORMATION);
}
