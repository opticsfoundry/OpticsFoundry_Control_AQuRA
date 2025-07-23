; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CRunProgressDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "control.h"
LastPage=0

ClassCount=10
Class1=CChildFrame
Class2=CControlApp
Class3=CAboutDlg
Class4=CControlDoc
Class5=CControlView
Class6=CEasyDialog
Class7=CExecuteMeasurementDlg
Class8=CMainFrame
Class9=CRunProgressDlg
Class10=CSimpleDialog

ResourceCount=7
Resource1=IDR_CONTROTYPE
Resource2=IDD_EASYDIALOG
Resource3=IDD_RUN_PROGRESS_DIALOG
Resource4=IDD_EXECUTE_MEASUREMENT_DIALOG
Resource5=IDD_ABOUTBOX
Resource6=IDR_MAINFRAME
Resource7=IDD_DIALOG1

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp

[CLS:CControlApp]
Type=0
BaseClass=CWinApp
HeaderFile=Control.h
ImplementationFile=Control.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Control.cpp
ImplementationFile=Control.cpp

[CLS:CControlDoc]
Type=0
BaseClass=CDocument
HeaderFile=ControlDoc.h
ImplementationFile=ControlDoc.cpp

[CLS:CControlView]
Type=0
BaseClass=CView
HeaderFile=ControlView.h
ImplementationFile=ControlView.cpp

[CLS:CEasyDialog]
Type=0
BaseClass=CDialog
HeaderFile=EasyDialog.h
ImplementationFile=EasyDialog.cpp

[CLS:CExecuteMeasurementDlg]
Type=0
BaseClass=CDialog
HeaderFile=ExecuteMeasurementDlg.h
ImplementationFile=ExecuteMeasurementDlg.cpp
LastObject=CExecuteMeasurementDlg
Filter=D
VirtualFilter=dWC

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CRunProgressDlg]
Type=0
BaseClass=CDialog
HeaderFile=RunProgressDlg.h
ImplementationFile=RunProgressDlg.cpp
LastObject=CRunProgressDlg

[CLS:CSimpleDialog]
Type=0
BaseClass=CDialog
HeaderFile=SimpleDialog.h
ImplementationFile=SimpleDialog.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDOK,button,1342373889

[DLG:IDD_EASYDIALOG]
Type=1
Class=CEasyDialog
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_APPLY,button,1342242816
Control4=IDC_UP,button,1342242816
Control5=IDC_DOWN,button,1342242816

[DLG:IDD_RUN_PROGRESS_DIALOG]
Type=1
Class=CExecuteMeasurementDlg
ControlCount=7
Control1=IDC_PROGRESS_BAR1,msctls_progress32,1350565889
Control2=IDC_PROGRESS_STATIC,static,1342308352
Control3=IDC_PROGRESS_BAR3,msctls_progress32,1350565889
Control4=IDC_PROGRESS_BAR2,msctls_progress32,1350565889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352

[MNU:IDR_CONTROTYPE]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
Command22=ID_SEQUENCE_GO
CommandCount=22

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
Command9=ID_SEQUENCE_GO
CommandCount=9

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_MENU
CommandCount=9

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_EXECUTE_MEASUREMENT_DIALOG]
Type=1
Class=CExecuteMeasurementDlg
ControlCount=4
Control1=IDC_PROGRESS_BAR,msctls_progress32,1350565889
Control2=IDC_PROGRESS_STATIC,static,1342308352
Control3=IDCANCEL,button,1342242816
Control4=IDC_TEST_INJECTIONS_DURING_MEASUREMENT,button,1342242816

[DLG:IDD_DIALOG1]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

