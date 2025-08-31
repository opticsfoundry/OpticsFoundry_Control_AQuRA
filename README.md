# Overview

OpticsFoudry's control system is suitable for many ultracold atom experiments and supports digital and analog IO and direct digital synthesizers (AD9858, AD9854, AD9958). It is a modernized version of the one presented on [StrontiumBEC.com](https://www.strontiumbec.com/) -> Control. The electronics can be accessed through three types of software, listed below. This repository contains ControlAPI and Control.exe. 

1. **Control.exe**  
    A fully featured experiment control system, allowing efficient and convenient operation of ultracold atom (and similar) experiments. 

2. **ControlAPI**  
    A fully featured API, providing access to most functions of Control.exe. This API is for example used for the [AQuRA clock](https://www.aquraclock.eu/). It can be used with Visual Studio C++ and Qt Creator (with MinGW, see repository [OpticsFoundry_Control_Qt](https://github.com/opticsfoundry/OpticsFoundry_Control_Qt)). The API can also be accessed through TCP/IP, and we provide a Python and a Qt Creator example for that.  

3. **ControlLightAPI**    
    A simple API giving the most direct access to the hardware. Tested with Python, Visual Studio C++ and Qt Creator (with MinGW), see repository [ControlLightAPI](https://github.com/opticsfoundry/OpticsFoundry_ControlLight).

&nbsp;

## Table of content

1. Control.exe 
   1. [Installation and Configuration](Manual/InstallationAndConfigurationOfControlEXE.md)  
   2. [User interface and operation](Manual/UserInterface.md)  
   3. [Programing experimental sequences](Manual/ProgrammingSequences.md)  
2. ControlAPI
   1. [Installation and Configuration](Manual/InstallationAndConfigurationOfControlAPI.md)  
