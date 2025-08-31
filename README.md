# Overview

OpticsFoudry's control system is suitable for many ultracold atom experiments and supports digial and analog IO and direct digital synthesizers (AD9858, AD9854, AD9958). It is a modernized version of the one presented on [StrontiumBEC.com](https://www.strontiumbec.com/) -> Control. The electronics can be accessed through three types of software, listed below. This repository contains ControlAPI and Control.exe. 

1. **ControlLightAPI**    
    A simple API giving the most direct access to the hardware. Tested with Python, Visual Studio C++ and Qt Creator (with MinGW).  
    ControlLightAPI can be configured through commands or through a json file. See repository [ControlLightAPI](https://github.com/opticsfoundry/OpticsFoundry_ControlLight) contains the ControlLightAPI.

2. **ControlAPI**  
    A fully featured API, having functions such as complex sequence assembly with "GoBackInTime", "Wavefunctions", and sequence cycling with individual command updates in the background. This is for example used for the [AQuRA clock](https://www.aquraclock.eu/).  
    This API's DLL has been tested with Visual Studio C++ and Qt Creator (with MinGW, see repository [OpticsFoundry_Control_Qt](https://github.com/opticsfoundry/OpticsFoundry_Control_Qt)). The API can also be accessed through TCP/IP, and we provide a Python and a Qt Creator example for that.  
    (If you need this API directly from Python, then tell us and we can create such a version within a few days.)  
    ControlAPI can be configured through ASCII files, a simple directory setting txt file, a hardware device configuration json file and a user output definition json file.  

3. **Control.exe**  
    This is a fully featured experiment control system, based on ControlAPI. Control.exe can be configured through code and/or through configuration files (the latter in the same manner as ControlAPI, on which Control.exe is based). A somewhat outdated introduction and manual is [ControlManual](http://www.strontiumbec.com/Schreck/downloads/ControlManual.pdf).

Control uses Microsoft Foundation Classes (MFC, or also called Afx). When installing Visual Studio C++, you need to select it as an extra option, otherwise it won't be installed.



&nbsp;

## Table of content

1. [Installation and Configuration](Manual/InstallationAndConfiguration.md)  
2. [User interface](Manual/UserInterface.md)  
3. [Programing experimental sequences](Manual/ProgrammingSequences.md)  
