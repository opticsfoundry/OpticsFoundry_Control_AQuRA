# Installation and Configuration of ControlAPI

This file describes how to install and configure Control.exe and ControlAPI. It is assumed that you have installed Visual Studio C++ 2022 with MFC (Microsoft Foundation Classes, or Afx) support.

*Table of content*  

1. [ControlAPI](#controlapi)
   - [Recompiling Control.dll](#recompiling-controldll)
 
Back to [Overview](../README.md).

&nbsp;


## ControlAPI

The large version of the ControlAPI interface is a DLL that can be used by Visual Studio 2022 C++ and Qt Creator (MinGW). If you need it for Python, please tell us as it would be very easy to support it.

The best way to get to know this API is to study the demo code in [Control_Firefly_AQuRA_Qt](https://github.com/opticsfoundry/OpticsFoundry_Control_Qt).


The ControlAPI is basically Control.exe, just running without showing the GUI, as a DLL. This means it is configured like Control.exe. One difference is how one points the DLL to the folder in which the configuration files are. For ControlAPI, you specify this folder when you call "ConnectToLowLevelSoftware", see the definition of ParamFileDirectory in TestSequence.cpp. For example  
_const char\* ParamFileDirectory = "D:\\\\Florian\\\\Firefly\\\\FireflyControl\\\\Control_Firefly_AQuRA\\\\ConfigParams\\\\ControlParam";_  
points to the configuration files  
_ControlParam_ParamList.txt_  
_ControlParam_SystemParamList.txt_  
_ControlParam_UniMessList.dat_  
_ControlParam_UtilityDialog.txt_  
in the folder  
_D:\\Florian\\Firefly\\FireflyControl\\Control_Firefly_AQuRA\\ConfigParams\\_  
Note that ParamFileDirectory includes the folder name and the beginning of the configuration file names.

At the same location, you should also specify a folder in which debug files will be stored, e.g.  
_const QString DebugFileDirectory = "D:\\\\Florian\\\\Firefly\\\\FireflyControl\\\\DebugFiles";_  

You can switch debug on or off in this codeline:  
_if (!CA.ConnectToLowLevelSoftware(atelnet, ParamFileDirectory, /\*IP\*/ "192.168.0.103", /\*Debug\*/ true, DebugFileDirectory)) {_  


You can use the API in TCPIP mode or in DLL mode. The TCPIP mode is useful for debugging, but otherwise the DLL mode is preferred. You can switch between TCPIP mode and DLL mode in  
_ControlAPI.h_  
By defining or not defining  
_#define USE_CA_DLL_  
If you use the TCPIP mode, search for "192.168." and adjust the IP adress to the one of the computer on which Control.exe is running. Make sure that Control.exe starts the ControlAPI sever, by enabling that option in the system parameter menus.  

In either mode, you need to select the IP address of the FPGA. You do this using a Python hardware configuration script, which is described in section [Configuration of Control.exe](InstallationAndConfigurationOfControlEXE.md#configuration-of-controlexe).  

### Recompiling Control.dll

To recompile Control.dll, install [Visual Studio 2022 with MFC](InstallationOfVisualStudio.md) and open Control.sln in Visual Studio 2022. 

Usually, select _Debug_ and _x64_ in the toolbar. 

![Build version selector](Images/VisualStudio2022BuildVersionSelection.png)

Note: pay attention to always **use the same x64/Win32 Debug/Release setting** for the DLL compiler and for the compiler that compiles the program that uses the DLL.  
- **_x64_ or _Win32_** (i.e. 32bit or 64bit AMD/Intel code). For most situations x64 is better. The one situation in which Win32 is better is when editing the Control.exe software, as the compilation of Control.exe is vastly faster in Win32 than x64 mode.  
- **_Debug_ or _Release_**: Release is quite a bit faster than Debug, both when compiling and when executing the program. Debug enables one to see Visual Studio debug information (even when using the DLL in Qt with MinGW).  
- **_DLL or EXE:_** If not automatically changed to DLL when selecting _x64_, in the Solution Explorer (in the Visual Studio GUI) you right click on "Control" (not " Solution Control" ) and select DLL under Configuration Properties -> General-> Configuration Type.  


To select "DLL", right-click on "Control" in the Solution explorer (not "Solution Control"). Select Properties. Select DLL under "General Properties -> Configuration Type".  

Build the solution. Look at the linker output. It tells you in which folder the DLL was created, e.g.  
_D:\\Florian\\Firefly\\FireflyControl\\Control_Firefly_AQuRA\\x64\\Template_  

Copy _Control.dll_ to the folder in which Qt creates your executable, e.g.  
_D:\\Florian\\Firefly\\FireflyControl\\Control_Firefly_Qt\\_  

If you want to change the hardware configuration, do it by modifying the Configuration Python scripts and running them to create configuration json files, see [Specifying hardware configuration](#specifying-hardware-configuration).  

If you need a second name for an output, do that in the file _ConfigFiles\AlternativeCommandNames.txt_, which contains alternative names for user IO outputs and their calibration.  
_MOTCurrent = SetMOTCoilCurrent * 2 + 0_  
_ChillerSetpoint = SetChillerSetpoint * 1 + 0_  
_LatticePowerSetpoint = SetLatticePowerSetpoint * 1 + 0_  
(This option was needed for the AQuRA clock project, as the higher level software by LTE accessed the lower level software using command names that just contained the number of an output. Using those names would have led to barely readable code in Control.exe, the code that was used to commission the clock.)
