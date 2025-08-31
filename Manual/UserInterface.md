# User Interface

Table of Contents


Back to [Overview](../README.md)

## Main Dialog

The main dialog of Control.exe is shown below. It contains the following elements:  
![Main dialog](Images/MainDialog.png)

Manual operation: enables manual control of outputs

Parameter menus, top part: initialization parameters of outputs. The outputs will be set to those values when Control.exe is started, when the button "Reset System" is pressed, or when ResetSystem is executed at the end of an experimental run.

Parameter menus, bottom part: code blocks of the experimental sequence

Utilities: utilities to e.g. run the AQuRA clock (i.e. stabilize the frequency comb on the Sr clock transition), or test the IO system.


## Manual Operation Dialog

The manual operation dialog is shown below.

![Manual operation dialog](Images/ManualOperationDialog.png)

The outputs can be changed by typing new values into the dialog boxes or checking the check boxes. The new values are sent to the output ports by clicking into the grey area of the control panel or clicking "Apply" or "Ok". The changes are not applied and the menu is left by clicking "Cancel" or closing the dialog box.
The C++ internal names of the outputs are displayed when clicking or hovering the mouse over the yellow areas.


## Initial Parameters Dialog

The initial parameters dialog is shown below. 

![Initial parameters dialog](Images/InitialParametersDialog.png)


This menu looks similar to the manual control menu. It contains the initialization parameters. These values are stored on disk when the program quits and loaded again when it starts. These values are used to initialize the outputs when the "Reset System" button is clicked or ResetSystem() is called.


## Useful Utilities 

![Measure FPGA ethernet bandwidth](Images/MeasureFPGAEthernetBandwidth.png)

The measured ethernet bandwidth should be around 300MBit/s.

![Test Vision](Images/TestVisionUtility.png)

Test Vision Command sends the command in  the edit box to Vision. "VisionGridOnOff" switches a grid over the result picture on or off, which can be used to quickly verify that Control and Vision communicate correctly.

"Test Vision" runs an image taking sequence as specified by the parameters of the main experimental sequence. If "Use external trigger" is clicked, the cameras' trigger inputs need to be connected to the trigger outputs of the control system.

"Trigger cameras" triggers all cameras. It's useful when the camera servers are stuck, waiting for triggers to arrive.

![Test IO](Images/TestIOUtility.png)

Tests analog or digital outputs and analog inputs by recording an output on an analog input. The user should connect a signal of interest, e.g. a digital output port or an analog output port or any other voltage signal (-10..10V), to an analog input port. The standard SPI port settings are shown here. The recoded data is saved in the "Data" folder.

![Test Multi IO bus](Images/TestMultiIOBusUtility.png)

Utilities that blink the bits of the data and address bus. This can be used together with the bus LED debug board to quickly check if all bus signals reach the rackslot of interest correctly.

## System configuration parameters

These parameters are identical to the ones in the file 
C:\...\OpticsFoundry_Control_AQuRA\ConfigParams\ControlParam_SystemParamList.txt 
Especially the file names can be more easily changed by editing that text file.
The debug options create debug files in the DebugFiles folder. These options should be off when not debugging.


![System configuration Dialog 1](Images/SystemConfigurationDialog1.png)

![System configuration Dialog 2](Images/SystemConfigurationDialog2.png)
