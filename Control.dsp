# Microsoft Developer Studio Project File - Name="Control" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Control - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Control.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Control.mak" CFG="Control - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Control - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Control - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Control - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Control - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Control - Win32 Release"
# Name "Control - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AD9852.cpp
# End Source File
# Begin Source File

SOURCE=.\Agilent33250A.cpp
# End Source File
# Begin Source File

SOURCE=.\Agilent33250ASerial.cpp
# End Source File
# Begin Source File

SOURCE=.\AnaIn.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalogOut.cpp
# End Source File
# Begin Source File

SOURCE=.\AnaOut.cpp
# End Source File
# Begin Source File

SOURCE=.\AOMCalibration.cpp
# End Source File
# Begin Source File

SOURCE=.\Calibration.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSocketUser.cpp
# End Source File
# Begin Source File

SOURCE=.\clntsock.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsecutiveCalibration.cpp
# End Source File
# Begin Source File

SOURCE=.\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\Control.rc
# End Source File
# Begin Source File

SOURCE=.\DelayedWaveform.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogElement.cpp
# End Source File
# Begin Source File

SOURCE=.\DigIn.cpp
# End Source File
# Begin Source File

SOURCE=.\DigitalOut.cpp
# End Source File
# Begin Source File

SOURCE=.\DigOut.cpp
# End Source File
# Begin Source File

SOURCE=.\DoublePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\EasyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EIP548A.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementAnalogOut.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementBool.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementDigitalOut.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementInt.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementString.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementStringComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\EvaporationSweep.cpp
# End Source File
# Begin Source File

SOURCE=.\ExecuteMeasurementDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExecuteMeasurementDlgContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\FiberLaser.cpp
# End Source File
# Begin Source File

SOURCE=.\GPIB.cpp
# End Source File
# Begin Source File

SOURCE=.\GPIBDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\HP5334A.cpp
# End Source File
# Begin Source File

SOURCE=.\IO.cpp
# End Source File
# Begin Source File

SOURCE=.\IOList.cpp
# End Source File
# Begin Source File

SOURCE=.\IORegister.cpp
# End Source File
# Begin Source File

SOURCE=.\Linear.cpp
# End Source File
# Begin Source File

SOURCE=.\lstnsock.cpp
# End Source File
# Begin Source File

SOURCE=.\MainDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Measurement.cpp
# End Source File
# Begin Source File

SOURCE=.\MeasurementList.cpp
# End Source File
# Begin Source File

SOURCE=.\MeasurementPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\MeasurementQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuButton.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuObList.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuObListElement.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuTitle.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageReceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiIO.cpp
# End Source File
# Begin Source File

SOURCE=.\NetIO.cpp
# End Source File
# Begin Source File

SOURCE=.\netsock.cpp
# End Source File
# Begin Source File

SOURCE=.\Network.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkClient.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkServer.cpp
# End Source File
# Begin Source File

SOURCE=.\NI6024E.cpp
# End Source File
# Begin Source File

SOURCE=.\NI653x.cpp
# End Source File
# Begin Source File

SOURCE=.\NI67x3.cpp
# End Source File
# Begin Source File

SOURCE=.\OmegaiSeries.cpp
# End Source File
# Begin Source File

SOURCE=.\Output.cpp
# End Source File
# Begin Source File

SOURCE=.\ParabolicRamp.cpp
# End Source File
# Begin Source File

SOURCE=.\Param.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamBool.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\ParameterDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamInt.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamList.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamRegister.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamString.cpp
# End Source File
# Begin Source File

SOURCE=.\ParamStringComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Polynom.cpp
# End Source File
# Begin Source File

SOURCE=.\Puls.cpp
# End Source File
# Begin Source File

SOURCE=.\Ramp.cpp
# End Source File
# Begin Source File

SOURCE=.\RampdBLinear.cpp
# End Source File
# Begin Source File

SOURCE=.\RangeCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\Rectangle.cpp
# End Source File
# Begin Source File

SOURCE=.\RunProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Sequence.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceLib.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceListAnalogOut.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceListDigitalOut.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceListPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\SequenceListWaveform.cpp
# End Source File
# Begin Source File

SOURCE=.\Serial.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\Sin.cpp
# End Source File
# Begin Source File

SOURCE=.\SineRamp.cpp
# End Source File
# Begin Source File

SOURCE=.\SingleWaveform.cpp
# End Source File
# Begin Source File

SOURCE=.\SlaveIO.cpp
# End Source File
# Begin Source File

SOURCE=.\SlaveIOList.cpp
# End Source File
# Begin Source File

SOURCE=.\SlaveMainDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SlaveServer.cpp
# End Source File
# Begin Source File

SOURCE=.\Square.cpp
# End Source File
# Begin Source File

SOURCE=.\SRS345.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StepwiseLinearRamp.cpp
# End Source File
# Begin Source File

SOURCE=.\SyncCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemParamList.cpp
# End Source File
# Begin Source File

SOURCE=.\TektronixAWG5105.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeStretch.cpp
# End Source File
# Begin Source File

SOURCE=.\TweezerRamp.cpp
# End Source File
# Begin Source File

SOURCE=.\UniMess.cpp
# End Source File
# Begin Source File

SOURCE=.\UniMessList.cpp
# End Source File
# Begin Source File

SOURCE=.\USBMultiIO.cpp
# End Source File
# Begin Source File

SOURCE=.\UtilityDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VCOCalibration.cpp
# End Source File
# Begin Source File

SOURCE=.\VerdiLaser.cpp
# End Source File
# Begin Source File

SOURCE=.\Vision.cpp
# End Source File
# Begin Source File

SOURCE=.\Waveform.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveformGenerator.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AD9852.h
# End Source File
# Begin Source File

SOURCE=.\Agilent33250A.h
# End Source File
# Begin Source File

SOURCE=.\Agilent33250ASerial.h
# End Source File
# Begin Source File

SOURCE=.\AnaIn.h
# End Source File
# Begin Source File

SOURCE=.\AnalogOut.h
# End Source File
# Begin Source File

SOURCE=.\AnaOut.h
# End Source File
# Begin Source File

SOURCE=.\AOMCalibration.h
# End Source File
# Begin Source File

SOURCE=.\Calibration.h
# End Source File
# Begin Source File

SOURCE=.\ClientSocketUser.h
# End Source File
# Begin Source File

SOURCE=.\clntsock.h
# End Source File
# Begin Source File

SOURCE=.\ConsecutiveCalibration.h
# End Source File
# Begin Source File

SOURCE=.\Control.h
# End Source File
# Begin Source File

SOURCE=.\DelayedWaveform.h
# End Source File
# Begin Source File

SOURCE=.\DialogElement.h
# End Source File
# Begin Source File

SOURCE=.\DigIn.h
# End Source File
# Begin Source File

SOURCE=.\DigitalOut.h
# End Source File
# Begin Source File

SOURCE=.\DigOut.h
# End Source File
# Begin Source File

SOURCE=.\DoublePoint.h
# End Source File
# Begin Source File

SOURCE=.\EasyDialog.h
# End Source File
# Begin Source File

SOURCE=.\EIP548A.h
# End Source File
# Begin Source File

SOURCE=.\ElementAnalogOut.h
# End Source File
# Begin Source File

SOURCE=.\ElementBool.h
# End Source File
# Begin Source File

SOURCE=.\ElementButton.h
# End Source File
# Begin Source File

SOURCE=.\ElementDigitalOut.h
# End Source File
# Begin Source File

SOURCE=.\ElementDouble.h
# End Source File
# Begin Source File

SOURCE=.\ElementInt.h
# End Source File
# Begin Source File

SOURCE=.\ElementStatic.h
# End Source File
# Begin Source File

SOURCE=.\ElementString.h
# End Source File
# Begin Source File

SOURCE=.\ElementStringComboBox.h
# End Source File
# Begin Source File

SOURCE=.\EvaporationSweep.h
# End Source File
# Begin Source File

SOURCE=.\ExecuteMeasurementDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExecuteMeasurementDlgContainer.h
# End Source File
# Begin Source File

SOURCE=.\FiberLaser.h
# End Source File
# Begin Source File

SOURCE=.\FTD2XX.H
# End Source File
# Begin Source File

SOURCE=.\GPIB.h
# End Source File
# Begin Source File

SOURCE=.\GPIBDevice.h
# End Source File
# Begin Source File

SOURCE=.\HP5334A.h
# End Source File
# Begin Source File

SOURCE=.\IO.h
# End Source File
# Begin Source File

SOURCE=.\IOList.h
# End Source File
# Begin Source File

SOURCE=.\IORegister.h
# End Source File
# Begin Source File

SOURCE=.\Linear.h
# End Source File
# Begin Source File

SOURCE=.\lstnsock.h
# End Source File
# Begin Source File

SOURCE=.\MainDialog.h
# End Source File
# Begin Source File

SOURCE=.\Measurement.h
# End Source File
# Begin Source File

SOURCE=.\MeasurementList.h
# End Source File
# Begin Source File

SOURCE=.\MeasurementPoint.h
# End Source File
# Begin Source File

SOURCE=.\MeasurementQueue.h
# End Source File
# Begin Source File

SOURCE=.\MenuButton.h
# End Source File
# Begin Source File

SOURCE=.\MenuObList.h
# End Source File
# Begin Source File

SOURCE=.\MenuObListElement.h
# End Source File
# Begin Source File

SOURCE=.\MenuStatic.h
# End Source File
# Begin Source File

SOURCE=.\MenuTitle.h
# End Source File
# Begin Source File

SOURCE=.\MessageReceiver.h
# End Source File
# Begin Source File

SOURCE=.\MultiIO.h
# End Source File
# Begin Source File

SOURCE=.\NetIO.h
# End Source File
# Begin Source File

SOURCE=.\NetSock.h
# End Source File
# Begin Source File

SOURCE=.\network.h
# End Source File
# Begin Source File

SOURCE=.\NetworkClient.h
# End Source File
# Begin Source File

SOURCE=.\NetworkServer.h
# End Source File
# Begin Source File

SOURCE=.\ni488.h
# End Source File
# Begin Source File

SOURCE=.\NI6024E.h
# End Source File
# Begin Source File

SOURCE=.\NI653x.h
# End Source File
# Begin Source File

SOURCE=.\NI67x3.h
# End Source File
# Begin Source File

SOURCE=.\nidaq.h
# End Source File
# Begin Source File

SOURCE=.\OmegaiSeries.h
# End Source File
# Begin Source File

SOURCE=.\Output.h
# End Source File
# Begin Source File

SOURCE=.\ParabolicRamp.h
# End Source File
# Begin Source File

SOURCE=.\Param.h
# End Source File
# Begin Source File

SOURCE=.\ParamBool.h
# End Source File
# Begin Source File

SOURCE=.\ParamDouble.h
# End Source File
# Begin Source File

SOURCE=.\ParameterDialog.h
# End Source File
# Begin Source File

SOURCE=.\Parameters.h
# End Source File
# Begin Source File

SOURCE=.\ParamInt.h
# End Source File
# Begin Source File

SOURCE=.\ParamList.h
# End Source File
# Begin Source File

SOURCE=.\ParamRegister.h
# End Source File
# Begin Source File

SOURCE=.\ParamString.h
# End Source File
# Begin Source File

SOURCE=.\ParamStringComboBox.h
# End Source File
# Begin Source File

SOURCE=.\Polynom.h
# End Source File
# Begin Source File

SOURCE=.\Puls.h
# End Source File
# Begin Source File

SOURCE=.\Ramp.h
# End Source File
# Begin Source File

SOURCE=.\RampdBLinear.h
# End Source File
# Begin Source File

SOURCE=.\RangeCheck.h
# End Source File
# Begin Source File

SOURCE=.\Rectangle.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RunProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\Sequence.h
# End Source File
# Begin Source File

SOURCE=.\SequenceLib.h
# End Source File
# Begin Source File

SOURCE=.\SequenceListAnalogOut.h
# End Source File
# Begin Source File

SOURCE=.\SequenceListDigitalOut.h
# End Source File
# Begin Source File

SOURCE=.\SequenceListPoint.h
# End Source File
# Begin Source File

SOURCE=.\SequenceListWaveform.h
# End Source File
# Begin Source File

SOURCE=.\Serial.h
# End Source File
# Begin Source File

SOURCE=.\SerialDevice.h
# End Source File
# Begin Source File

SOURCE=.\Sin.h
# End Source File
# Begin Source File

SOURCE=.\SineRamp.h
# End Source File
# Begin Source File

SOURCE=.\SingleWaveform.h
# End Source File
# Begin Source File

SOURCE=.\SlaveIO.h
# End Source File
# Begin Source File

SOURCE=.\SlaveIOList.h
# End Source File
# Begin Source File

SOURCE=.\SlaveMainDialog.h
# End Source File
# Begin Source File

SOURCE=.\SlaveServer.h
# End Source File
# Begin Source File

SOURCE=.\spline.h
# End Source File
# Begin Source File

SOURCE=.\Square.h
# End Source File
# Begin Source File

SOURCE=.\SRS345.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StepwiseLinearRamp.h
# End Source File
# Begin Source File

SOURCE=.\SyncCommand.h
# End Source File
# Begin Source File

SOURCE=.\SystemParamList.h
# End Source File
# Begin Source File

SOURCE=.\TektronixAWG5105.h
# End Source File
# Begin Source File

SOURCE=.\TimeStretch.h
# End Source File
# Begin Source File

SOURCE=.\TweezerRamp.h
# End Source File
# Begin Source File

SOURCE=.\UniMess.h
# End Source File
# Begin Source File

SOURCE=.\UniMessList.h
# End Source File
# Begin Source File

SOURCE=.\USBMultiIO.h
# End Source File
# Begin Source File

SOURCE=.\UtilityDialog.h
# End Source File
# Begin Source File

SOURCE=.\VCOCalibration.h
# End Source File
# Begin Source File

SOURCE=.\VerdiLaser.h
# End Source File
# Begin Source File

SOURCE=.\Vision.h
# End Source File
# Begin Source File

SOURCE=.\Waveform.h
# End Source File
# Begin Source File

SOURCE=.\WaveformGenerator.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Control.ico
# End Source File
# Begin Source File

SOURCE=.\res\Control.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ControlDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter "*.lib"
# Begin Source File

SOURCE=".\gpib-32.obj"
# End Source File
# Begin Source File

SOURCE=.\nidaq32.lib
# End Source File
# Begin Source File

SOURCE=.\nidex32.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
