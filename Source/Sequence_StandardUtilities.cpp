
#include "stdafx.h"
#include "sequence.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif




// MainUtilities arranges functions module block on User interface
//If you want to add more utilities, follow this style.
bool CSequence::SequenceStandardUtilities(unsigned int Message, CWnd* parent) {
	bool Received = false;
	
	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewMenu("Sequence Standard Utilities");
		UtilityDialog->AddStatic("Sequence Standard utilities", "", ColorGreen);
		//		UtilityDialog->AddStatic("");
	}
	Received |= MeasureFPGAEthernetBandwidth(Message, parent);
	Received |= TestAD9958Programming(Message, parent);
	Received |= TestDDS(Message, parent);
	Received |= ToggleDDSFrequency(Message, parent);
	Received |= BlinkDDSIntensity(Message, parent);
	Received |= TestServo(Message, parent);
	Received |= TestVision(Message, parent);
	Received |= TestVisionCommand(Message, parent);

	if (AssemblingUtilityDialog())
	{
		UtilityDialog->NewMenu("IO system test utilities");
		UtilityDialog->AddStatic("IO system test", "", ColorGreen);
		UtilityDialog->AddStatic("");
	}
	Received |= TestDigitalOut(Message, parent);
	Received |= TestAnalogOutBoard(Message, parent);
	Received |= TestAnalogOut(Message, parent);
	Received |= TestAnalogInput(Message, parent);
	Received |= AnalogInRecorder(Message, parent);
	Received |= TestDigitalInput(Message, parent);
	Received |= DigitalInRecorder(Message, parent);

	if (AssemblingUtilityDialog()) {
		//UtilityDialog->NewColumn();
		UtilityDialog->NewMenu("MultiIO utilities");
		UtilityDialog->AddStatic("MultiIO utilities", "", ColorGreen);
	}
	Received |= TestSubbusDecoder(Message, parent);
	Received |= MultiIOShuffleTest(Message, parent);
	Received |= TestMultiIOBus(Message, parent);

	return Received;
}


bool CSequence::MeasureFPGAEthernetBandwidth(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		if (Message != IDM_MEASURE_FPGA_ETHERNET_BANDWIDTH) return false;
		double Bandwidth = Sequencer0->MeasureEthernetBandwidth(1024 * 1024);
		if (Bandwidth > 100) Bandwidth = Sequencer0->MeasureEthernetBandwidth(8 * 1024 * 1024);
		CString message;
		message.Format("Ethernet Bandwidth = %.1f MBit/s", Bandwidth);
		ControlMessageBox(message);
	}
	else {
		UtilityDialog->AddStatic("");
		UtilityDialog->AddButton(IDM_MEASURE_FPGA_ETHERNET_BANDWIDTH, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

//Utility TestAD9958 programming

bool CSequence::TestAD9958Programming(unsigned int Message, CWnd* parent)
{
	static long AD9958Number;
	static double AD9958DDS0Frequency0;
	static double AD9958DDS0Amplitude0;
	static double AD9958DDS0Phase0;
	static double AD9958DDS0Frequency1;
	static double AD9958DDS0Amplitude1;
	static double AD9958DDS0Phase1;
	if (!AssemblingUtilityDialog()) {
		if (Message != IDM_TEST_AD9985_PROGRAMMING) return false;
		SetAssembleSequenceListMode();
		StartSequence();
		AD9958[AD9958Number]->Initialise();
		Wait(10);
		AD9958[AD9958Number]->SyncIO();
		Wait(0.01); //Is this wait needed?
		AD9958[AD9958Number]->SetFrequency(E_AD9958_CHANNEL_0, AD9958DDS0Frequency0);//in MHz
		AD9958[AD9958Number]->SetAmplitude(E_AD9958_CHANNEL_0, AD9958DDS0Amplitude0); //0...1023
		AD9958[AD9958Number]->SetPhaseOffset(E_AD9958_CHANNEL_0, AD9958DDS0Phase0); //in degree 
		AD9958[AD9958Number]->SetFrequency(E_AD9958_CHANNEL_1, AD9958DDS0Frequency1);//in MHz
		AD9958[AD9958Number]->SetAmplitude(E_AD9958_CHANNEL_1, AD9958DDS0Amplitude1); //0...1023
		AD9958[AD9958Number]->SetPhaseOffset(E_AD9958_CHANNEL_1, AD9958DDS0Phase1); //in degree */
		StopSequence();
		SetWaveformGenerationMode();
		ExecuteSequenceList();
		if (DebugSequenceListOn) DebugSequenceList(*DebugFilePath + "TestAD9958SequenceListAfterExecution.dat", 0);
	}
	else {
		UtilityDialog->RegisterLong(&AD9958Number, "AD9958Number", 0, 225, "AD9958 DDS Number", "");
		UtilityDialog->RegisterDouble(&AD9958DDS0Frequency0, "AD9958DDS0Frequency0", 0, 225, "AD9958 DDS0 Frequency 0", "MHz");
		UtilityDialog->RegisterDouble(&AD9958DDS0Amplitude0, "AD9958DDS0Amplitude0", 0, 100, "AD9958 DDS0 Amplitude 0", "%");
		UtilityDialog->RegisterDouble(&AD9958DDS0Phase0, "AD9958DDS0Phase0", 0, 360, "AD9958 DDS0 Phase 0", "deg");
		UtilityDialog->RegisterDouble(&AD9958DDS0Frequency1, "AD9958DDS0Frequency1", 0, 225, "AD9958 DDS0 Frequency 1", "MHz");
		UtilityDialog->RegisterDouble(&AD9958DDS0Amplitude1, "AD9958DDS0Amplitude1", 0, 100, "AD9958 DDS0 Amplitude 1", "%");
		UtilityDialog->RegisterDouble(&AD9958DDS0Phase1, "AD9958DDS0Phase1", 0, 360, "AD9958 DDS0 Phase 1", "deg");
		UtilityDialog->AddStatic("");
		UtilityDialog->AddButton(IDM_TEST_AD9985_PROGRAMMING, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}


//Utility TestAnalogOut
long TestAnalogOutChannel;
double TestAnalogOutMinVoltage;
double TestAnalogOutMaxVoltage;
double TestAnalogOutTime;
double TestAnalogOutTimeStep;
long TestAnalogOutRepetitions;

double TestAnalogOutFunctionLastValue = 0;
double TestAnalogOutFunction(double Value, bool GetValue) {
	if (GetValue) return TestAnalogOutFunctionLastValue;
	else {
		Output->AnalogOutScaled(TestAnalogOutChannel, Value, Value);
		TestAnalogOutFunctionLastValue = Value;
	}
	return 0;
}

bool CSequence::TestAnalogOut(unsigned int Message, CWnd* parent) {
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDC_TEST_ANALOG_OUT:
			SetAssembleSequenceListMode();
			StartSequence(NULL, NULL, false);
			for (int n = 0; n < TestAnalogOutRepetitions; n++) {
				Waveform(new CRamp(&TestAnalogOutFunction, TestAnalogOutMinVoltage, TestAnalogOutMaxVoltage, TestAnalogOutTime, TestAnalogOutTimeStep));
				//TestAnalogOutFunction(2,false);
				Wait(TestAnalogOutTime);
				Waveform(new CRamp(&TestAnalogOutFunction, TestAnalogOutMaxVoltage, TestAnalogOutMinVoltage, TestAnalogOutTime, TestAnalogOutTimeStep));
				//TestAnalogOutFunction(0,false);
				Wait(TestAnalogOutTime);
			}
			Wait(10);
			TestAnalogOutFunction(0, false);
			Wait(10);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();
			EmptyNIcardFIFO();
			break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&TestAnalogOutChannel, "TestAnalogOutChannel", 0, 10000, "TestAnalogOutChannel");
		UtilityDialog->RegisterDouble(&TestAnalogOutMinVoltage, "TestAnalogOutMinVoltage", -10, 10, "Min Voltage", "V");
		UtilityDialog->RegisterDouble(&TestAnalogOutMaxVoltage, "TestAnalogOutMaxVoltage", -10, 10, "Max Voltage", "V");
		UtilityDialog->RegisterDouble(&TestAnalogOutTime, "TestAnalogOutTime", 0, 60000, "Time", "ms");
		UtilityDialog->RegisterDouble(&TestAnalogOutTimeStep, "TestAnalogOutTimeStep", 0, 1000, "Time step", "ms");


		UtilityDialog->RegisterLong(&TestAnalogOutRepetitions, "TestAnalogOutRepetitions", 0, 10000, "Repetitions");


		UtilityDialog->AddButton(IDC_TEST_ANALOG_OUT, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}


//Utility TestMultiIOBus
long TestMultiIOBusAddress;
long TestMultiIOBusBlinkBitTestBitNr;
long TestMultiIOBusBlinkBitTestPeriodBusCycles;
long TestMultiIOBusBlinkBitTestNrPeriods;
double TestMultiIOBusFrequency;
bool CSequence::TestMultiIOBus(unsigned int Message, CWnd* parent) {
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDC_TEST_MULTIIO_BUS:
			Output->TestMultiIOBus((unsigned short)TestMultiIOBusAddress, TestMultiIOBusFrequency);
			break;
		case IDC_TEST_MULTIIO_BUS_BLINK_EACH_BIT:
			Output->TestMultiIOBusBlinkEachBit();
			break;
		case IDC_TEST_MULTIIO_BUS_BLINK_ALL_BITS:
			Output->TestMultiIOBusBlinkAllBits();
			break;
		case IDC_TEST_MULTIIO_BUS_BLINK_BIT:
			Output->TestMultiIOBusBlinkBit(TestMultiIOBusBlinkBitTestBitNr, TestMultiIOBusBlinkBitTestPeriodBusCycles, TestMultiIOBusBlinkBitTestNrPeriods);
			break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&TestMultiIOBusAddress, "TestMultiIOBusAddress", 0, 16, "MultiIO SubBus Address");
		UtilityDialog->RegisterDouble(&TestMultiIOBusFrequency, "TestMultiIOBusFrequency", 0, 2000000, "Bus Frequency", "Hz");
		UtilityDialog->AddButton(IDC_TEST_MULTIIO_BUS, Sequence);
		UtilityDialog->AddButton(IDC_TEST_MULTIIO_BUS_BLINK_EACH_BIT, Sequence);
		UtilityDialog->AddButton(IDC_TEST_MULTIIO_BUS_BLINK_ALL_BITS, Sequence);
		UtilityDialog->AddStatic("");
		UtilityDialog->RegisterLong(&TestMultiIOBusBlinkBitTestPeriodBusCycles, "TestMultiIOBusBlinkBitTestPeriodBusCycles", 1, 1024 * 1024, "Period", "bus cycles");
		UtilityDialog->RegisterLong(&TestMultiIOBusBlinkBitTestNrPeriods, "TestMultiIOBusBlinkBitTestNrPeriods", 1, 1024, "Nr periods");
		UtilityDialog->RegisterLong(&TestMultiIOBusBlinkBitTestBitNr, "TestMultiIOBusBlinkBitTestBitNr", 0, 31, "Test Bit Nr (0..31)");
		UtilityDialog->AddButton(IDC_TEST_MULTIIO_BUS_BLINK_BIT, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

//Utility MultiIOShuffleTest
long MultiIOShuffleTestExcludedAddress;
long MultiIOShuffleTestExcludedBus;
CString* MultiIOShuffleTestAddressMask;
CString* MultiIOShuffleTestDefaultPattern;
CString* MultiIOShuffleTestDataMask;
bool MultiIOShuffleTestMaskDataIfAddressMatched;
CString* MultiIOShuffleTestDataDefaultPattern;
bool MultiIOShuffleTestStrobeBox;
bool MultiIOShuffleTestAvoidAddressOnAllSubbusses;
bool CSequence::MultiIOShuffleTest(unsigned int Message, CWnd* parent) {
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDC_MULTIIO_SHUFFLE_TEST:
			if (ControlMessageBox("Did you switch off all RF amplifiers, power supplies and similar dangerous computer controlled devices ?", MB_YESNO) == IDYES) {
				unsigned char help[1];
				help[0] = (unsigned char)MultiIOShuffleTestExcludedAddress;
				Output->MultiIOShuffleTest(MultiIOShuffleTestExcludedBus, 1, help, MultiIOShuffleTestAddressMask, MultiIOShuffleTestDefaultPattern, MultiIOShuffleTestMaskDataIfAddressMatched, MultiIOShuffleTestDataMask, MultiIOShuffleTestDataDefaultPattern, MultiIOShuffleTestAvoidAddressOnAllSubbusses, MultiIOShuffleTestStrobeBox);
				HardResetSystem();
			}
			break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&MultiIOShuffleTestExcludedBus, "MultiIOShuffleTestExcludedBus", 0, 15, "Multi IO Shuffle Test Excluded Bus");
		UtilityDialog->RegisterBool(&MultiIOShuffleTestStrobeBox, "MultiIOShuffleTestStrobeBox", "Test strobe box ?");
		UtilityDialog->RegisterLong(&MultiIOShuffleTestExcludedAddress, "MultiIOShuffleTestExcludedAddress", 0, 255, "Multi IO Shuffle Test Excluded Address");
		UtilityDialog->RegisterBool(&MultiIOShuffleTestAvoidAddressOnAllSubbusses, "MultiIOShuffleTestAvoidAddressOnAllSubbusses", "Avoid address on all subbusses?");
		UtilityDialog->RegisterString(MultiIOShuffleTestAddressMask, "MultiIOShuffleTestAddressMask", "0111111111111111", 20, "Multi IO Shuffle Test Address Mask");
		UtilityDialog->RegisterString(MultiIOShuffleTestDefaultPattern, "MultiIOShuffleTestDefaultPattern", "0111111111111111", 20, "Multi IO Shuffle Test Default Pattern");
		UtilityDialog->RegisterBool(&MultiIOShuffleTestMaskDataIfAddressMatched, "MultiIOShuffleTestMaskDataIfAddressMatched", "Mask Data If Address Matched ?");
		UtilityDialog->RegisterString(MultiIOShuffleTestDataMask, "MultiIOShuffleTestDataMask", "11111111", 20, "Multi IO Shuffle Test Data Mask");
		UtilityDialog->RegisterString(MultiIOShuffleTestDataDefaultPattern, "MultiIOShuffleTestDataDefaultPattern", "11111111", 20, "Multi IO Shuffle Test Data Pattern");
		UtilityDialog->AddButton(IDC_MULTIIO_SHUFFLE_TEST, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}




bool CSequence::TestAnalogInput(unsigned int Message, CWnd* parent) {
	static long SequencerNr = 0;
	static long SPI_port = 1;
	static long SPI_CS = 0;
	static long AnalogInChannelNumber = 0;
	static long NrDataPoints = 1000;
	static double timestep = 0.1;
	static long RunNumber = 0;
	static bool DoRampsOnAnalogOutput = true;
	static CString* AnalogOutputName = NULL;
	static double AnalogOutMinVoltage = -10;
	static double AnalogOutMaxVoltage = 10;
	static long NrRamps = 10;
	static bool DoBlinkDigitalOutput = true;
	static CString* DigitalOutputName = NULL;

	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDC_TEST_ANALOG_IN: {
			if (SequencerNr >= Output->MaxSequencer) {
				SequencerNr = 0;
			}
			//take data
			SetAssembleSequenceListMode();
			StartSequence(NULL, NULL, false);
			StartAnalogInAcquisition(SequencerNr, SPI_port, SPI_CS, AnalogInChannelNumber, NrDataPoints, timestep);
			if (DoRampsOnAnalogOutput) {
				double TotalTime = timestep * NrDataPoints;
				constexpr int NrRamps = 10;
				double RampTime = TotalTime / (2 * NrRamps);
				double RampStepTime = RampTime / 256;
				for (unsigned int i = 0; i < NrRamps; i++) {
					StartNewWaveformGroup();
					Waveform(new CRamp(*AnalogOutputName, LastValue, AnalogOutMinVoltage, RampTime, RampStepTime));
					WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
					StartNewWaveformGroup();
					Waveform(new CRamp(*AnalogOutputName, LastValue, AnalogOutMaxVoltage, RampTime, RampStepTime));
					WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
				}
				StartNewWaveformGroup();
				Waveform(new CRamp(*AnalogOutputName, LastValue, 0, RampTime, RampStepTime));
				WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
			}
			else if (DoBlinkDigitalOutput) {
				double TotalTime = timestep * NrDataPoints;
				double RampTime = TotalTime / (2 * NrRamps);
				for (unsigned int i = 0; i < NrRamps; i++) {
					IOList->DigitalOut(*DigitalOutputName, On);
					Wait(RampTime);
					IOList->DigitalOut(*DigitalOutputName, Off);
					Wait(RampTime);
				}
			}
			else Wait(timestep * NrDataPoints + 1);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();

			//read out data
			unsigned char* buffer = NULL;
			unsigned long buffer_length = 0;
			DWORD EndTimeOfCycle = 0;
			double timeout_in_s = 10;
			bool success = Output->SequencerList[SequencerNr]->WaitTillEndOfSequenceThenGetInputData(buffer, buffer_length, EndTimeOfCycle, timeout_in_s);
			if (!success) {
				ControlMessageBox("CSequence::TestAnalogInput : GetCycleData: no data");
				return true;
			}
			if (buffer == NULL) {
				ControlMessageBox("CSequence::SaveInputDataToFile : no input data received 2");
				return true;
			}

			//Store data
			CString filename;
			filename.Format("%sAnalogInputTest%04u.dat", *DataFilePath, RunNumber);
			CT2A ascii_filename(filename);
			std::ofstream file(ascii_filename);
			if (!file.is_open()) {
				ControlMessageBox("CSequence::SaveInputDataToFile : Couldn't open file for writing");
				return true;
			}
			for (unsigned long i = 0; i < buffer_length / 4; i++)
			{
				char out_buf[100];
				sprintf_s(out_buf, sizeof(out_buf), "%u %u\n", i, ((unsigned int*)buffer)[i]);
				// Or this line instead:
				// sprintf_s(out_buf, sizeof(out_buf), "%u %u 0x%x\n", i, ((unsigned int*)buffer)[i], ((unsigned int*)buffer)[i]);
				file << out_buf;
			}
			file.close();
			RunNumber++;
		}
							   break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&SequencerNr, "SequencerNr", 0, 100, "Sequencer Nr", "");
		UtilityDialog->RegisterLong(&SPI_port, "SPI_port", 0, 1, "SPI port", "0, 1");
		UtilityDialog->RegisterLong(&SPI_CS, "SPI_CS", 0, 3, "SPI Chip Select", "0..3");
		UtilityDialog->RegisterLong(&AnalogInChannelNumber, "AnalogInChannelNumber", 0, 7, "Analog In Channel Number", "0..7");
		UtilityDialog->RegisterLong(&NrDataPoints, "NrDataPoints", 0, 1000000, "Nr Data Points", "");
		UtilityDialog->RegisterDouble(&timestep, "timestep", 0, 10, "time between data points", "ms");

		UtilityDialog->RegisterBool(&DoRampsOnAnalogOutput, "DoRampsOnAnalogOutput", "Do Ramps On Analog Output?");
		UtilityDialog->RegisterString(AnalogOutputName, "AnalogOutputName", "SetMOTCoilCurrent", 200, "Analog Output Name");
		UtilityDialog->RegisterDouble(&AnalogOutMinVoltage, "AnalogOutMinVoltage", -10, 10, "Min Voltage", "V");
		UtilityDialog->RegisterDouble(&AnalogOutMaxVoltage, "AnalogOutMaxVoltage", -10, 10, "Max Voltage", "V");
		UtilityDialog->RegisterLong(&NrRamps, "NrRamps", 0, 1000, "Nr ramps");




		UtilityDialog->RegisterBool(&DoBlinkDigitalOutput, "DoBlinkDigitalOutput", "Do Blink Digital Output?");
		UtilityDialog->RegisterString(DigitalOutputName, "DigitalOutputName", "SwitchMOTShutter", 200, "Digital Output Name");

		UtilityDialog->AddButton(IDC_TEST_ANALOG_IN, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}


bool CSequence::TestVision(unsigned int Message, CWnd* parent) {
	static bool UseExternalTrigger = true;
	if (AssemblingUtilityDialog()) {
		UtilityDialog->RegisterBool(&UseExternalTrigger, "UseExternalTrigger", "Use external trigger?");
		UtilityDialog->AddButton(IDC_TEST_VISION, Sequence);
		UtilityDialog->AddButton(IDM_TRIGGER_CAMERAS, Sequence);
		UtilityDialog->AddStatic("");
	}
	else {
		switch (Message) {
		case IDM_TRIGGER_CAMERAS: {
			TriggerCameras();
		}
								break;
		case IDC_TEST_VISION: {
			if (!Vision->CheckReady()) {
				ControlMessageBox("CSequence::TestVision : Vision not ready");
				return true;
			}
			for (int i = 0; i < NrCameras; i++) {
				if (CameraFKSDataImages[i] < CameraFKSReferenceImages[i]) CameraFKSDataImages[i] = CameraFKSReferenceImages[i];
				double ExposureTime = CameraExposureTimeFluo;
				if (DoTakeAbsorptionPicture) ExposureTime = CameraExposureTimeAbs;
				else if (DoTakeFKSAbsorptionPicture) {
					//				if ((ExpansionTimeOfPicture[i]+FKSSecondFlashWait)>FKSExpansionTime) {
					if ((InitialExpansionTime + CameraExposureTimeFKS + FKSSecondFlashWait) > FKSExpansionTime) {
						ExposureTime = CameraExposureTimeFKS;
					}
					else {
						ExposureTime = FKSExpansionTime - InitialExpansionTime - FKSSecondFlashWait;
					}
				}
				//			else if (DoTakeFKSAbsorptionPicture) ExposureTime=FKSExpansionTime-InitialExpansionTime-FKSSecondFlashWait;
				//			else if (DoTakeFKSAbsorptionPicture) ExposureTime=CameraExposureTimeFKS;
				Vision->SetNetCameraParameters(
					i, *CameraDirection[i], CameraUsed[i], CameraDisplayedPictureNumber[i], CameraMainCamera[i],
					(DoTakeFKSAbsorptionPicture) ? CameraStartXFKS[i] : CameraStartX[i],
					(DoTakeFKSAbsorptionPicture) ? CameraStartYFKS[i] : CameraStartY[i],
					(DoTakeFKSAbsorptionPicture) ? CameraWidthFKS[i] : CameraWidth[i],
					(DoTakeFKSAbsorptionPicture) ? CameraHeightFKS[i] : CameraHeight[i],
					CameraCalX[i], CameraCalY[i], ExposureTime,
					CameraTemperature[i], CameraBinningX[i], CameraBinningY[i], CamerahSpeed[i], DoTakeFKSAbsorptionPicture, CameraFKSDataImages[i], CameraFKSReferenceImages[i], CameraFKSHeight[i],
					CameraFKSvSpeed[i], CameraFKSDirtyLines[i], CameraNumberPictures[i], CameraPrePicture[i], CameraFlushWait[i], CameraInternalTriggerAfterExternalTrigger[i], CameraSoftTriggerDelay[i],
					CameraAtomicMass[i], CameraAtomicWavelength[i], CameraAtomicClebschGordon[i], CameraAtomicLinewidth[i],
					CameraAtomicMagneticMoment[i], *CameraAtomicName[i], CameraProbeDetuning[i], /*ExpansionTime*/20,
					DoTakeFluorescencePicture, CameraMaxFluorescence[i], CameraCalibration[i], *LastAtomPath, *LastFinalTrap,
					/*ExternalTrigger*/UseExternalTrigger, /*NumberOfAdditionalReferenceImages*/0, /*DepthOfReferenceImageLibrary*/0);
			}
			VisionTriggered = false;
			if (!TriggerVision(/*LastChance*/true)) {
				ControlMessageBox("CSequence::TestVision : could not trigger Vision");
				return true;
			}
			//wait for the software trigger to be processed, then trigger cameras sufficiently often for all pictures
			Wait(200);
			TriggerCameras();
			Wait(500);
			TriggerCameras();
			Wait(500);
			TriggerCameras();
			Wait(500);
			TriggerCameras();
			const unsigned int LastDataArrayPoints = 50;
			double LastDataArray[LastDataArrayPoints];
			CString LastDataArrayName[LastDataArrayPoints];
			for (int i = 0; i < LastDataArrayPoints; i++) {
				LastDataArrayName[i] = "Data" + itos(i);
				LastDataArray[i] = 0;
			}
			if ((DoTakeAbsorptionPicture || DoTakeFKSAbsorptionPicture || DoTakeFluorescencePicture)) {
				Vision->Ready();
				Vision->SendPictureData(0/*FluoEndLevel*/, 0/*RecaptureFluorescence*/,
					/*FinalQuadrupoleCurrent*/0,/*FinalIoffeCurrent*/0,/*FinalOffsetCurrent*/0,/*FinalBias*/0,/*FinalGradiant*/0,
					/*FinalCurvature*/0, LastLoadingTime, 0, LastDataArray, LastDataArrayName, LastDataArrayPoints);
				//not very usefull, just consumes time: Vision->SendDataFile(*DebugFilePath + "SequenceListAfterExecution.dat","Sequence");

				if (SendSupplementaryDataToVisionAfterRun) {
					ofstream DataNames(*DataFilePath + "DataNames.txt");
					for (int i = 0; i < LastDataArrayPoints; i++) DataNames << "Data" << i << " = " << LastDataArrayName[i] << " = " << LastDataArray[i] << endl;
					DataNames.close();
					Vision->SendDataFile(*DataFilePath + "DataNames.txt", "DataNames");
				}
			}
			Vision->RunFinished();
		}
							break;
		default: return false;
		}
	}

	return true;
}






//Utility TestDigitalOut
long TestDigitalOutChannel;
bool TestDigitalOutFunctionLastValue = 0;
bool TestDigitalOutFunction(bool Value, bool GetValue) {
	if (GetValue) return TestDigitalOutFunctionLastValue;
	else {
		Output->DigitalOutScaled(TestDigitalOutChannel, Value, Value);
		TestDigitalOutFunctionLastValue = Value;
	}
	return true;
}

long TestSubbusDecoderSubbus;
long TestSubbusDecoderDigitalOutChannel;
CString* TestSubbusDecoderFileName;
long TestSubbusDecoderAnalogInBox;
bool CSequence::TestSubbusDecoder(unsigned int Message, CWnd* parent) {
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDC_TEST_SUBBUS_DECODER: {
			if (parent) {
				MeasurementDialog = new CExecuteMeasurementDlg(parent, Sequence);
				MeasurementDialog->Create();
			}
			ofstream out;
			CString buf;
			buf.Format("%sAddressing.dat", *TestSubbusDecoderFileName);
			out.open(buf);
			out << "Address Address Active" << endl;
			double ypos = 11;
			bool ErrorAddressingTest = false;
			int EST = -1;
			unsigned char MyAddress;
			unsigned int HardwareAddress;
			unsigned int MyBus;
			unsigned char MyDigOutDeviceNr;
			if (Output->GetDigitalOutMultiIOAddressIfPossible(TestSubbusDecoderDigitalOutChannel, MyDigOutDeviceNr, HardwareAddress)) {
				MyAddress = HardwareAddress & 0xFF;
				MyBus = (HardwareAddress & 0xF00) >> 8;
			}
			else {
				ControlMessageBox("CSequence::TestAnalogOutBoard : nothing configured on specified address");
				return true;
			}
			Output->SwitchForceWritingMode(On);
			for (unsigned char Subbus = 0; Subbus < 8; Subbus++) {
				Output->WriteMultiIO(MyDigOutDeviceNr, MyBus, MyAddress, 0);
				Output->WriteMultiIO(MyDigOutDeviceNr, Subbus, MyAddress, 0xFFFF);
				double A;
				A = GetComparatorAnalogIn(TestSubbusDecoderAnalogInBox, 0);
				if (A > 1) EST = Subbus;
				if (A > 1) ErrorAddressingTest = true;

				CString binary;
				binary = "";
				unsigned short helpData = Subbus;
				for (int k = 0; k < 4; k++) {
					binary = binary + ((helpData & 1) > 0 ? '1' : '0');
					helpData = helpData >> 1;
				}

				CString buf;
				buf.Format("%u %s %s", Subbus, binary, (A < 2.5) ? "." : "X");
				out << buf << endl;
				out.flush();

				buf.Format("Analog output addressing test\n\n%u: %s\n\nActive address = %i\n",
					Subbus, binary, EST);
				if (MeasurementDialog) MeasurementDialog->SetData(buf, Subbus * 10, 8 * 10);
				else {
					out.close();
					Output->SwitchForceWritingMode(Off);
					HardResetSystem();
					return true;
				}
			}
			Output->SwitchForceWritingMode(Off);
			if (MeasurementDialog) {
				MeasurementDialog->DestroyWindow();
				MeasurementDialog = NULL;
			}
			buf.Format("Last activated Address: %i", EST);
			out << buf << endl;
			out.flush();
			out.close();
			HardResetSystem();
			PlaySound(*SourceFilePath + "Sound\\short-and-bubbly-120528.wav", NULL, SND_FILENAME);
		} break;
		default: return false;
		}
	}
	else {

		UtilityDialog->RegisterLong(&TestSubbusDecoderSubbus, "TestSubbusDecoderSubbus", 0, 16, "Subbus");
		UtilityDialog->RegisterLong(&TestSubbusDecoderDigitalOutChannel, "TestSubbusDecoderDigitalOutChannel", 0, 10000, "Subbus test digital out channel");
		UtilityDialog->RegisterLong(&TestSubbusDecoderAnalogInBox, "TestSubbusDecoderAnalogInBox", 0, 4, "Analog In Box nb", "");
		UtilityDialog->RegisterString(TestSubbusDecoderFileName, "TestSubbusDecoderFileName", "TestSubbus", 200, "Test subbus decoder filename");

		UtilityDialog->AddButton(IDC_TEST_SUBBUS_DECODER, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

CString* TestDigitalOutBoardFileName;
long TestDigitalOutBoardAnalogInBox;
bool CSequence::TestDigitalOut(unsigned int Message, CWnd* parent) {
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDC_TEST_DIGITAL_OUT:
			SetAssembleSequenceListMode();
			StartSequence(NULL, NULL, false);
			for (int n = 0; n < 2; n++) {
				Output->DigitalOutScaled(TestDigitalOutChannel, true, true);
				Wait(1000);
				Output->DigitalOutScaled(TestDigitalOutChannel, false, false);
				Wait(1000);
			}
			Wait(10, 1);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();
			EmptyNIcardFIFO();
			break;
		case IDC_TEST_DIGITAL_OUT16BIT:
			SetAssembleSequenceListMode();
			StartSequence(NULL, NULL, false);
			for (int n = 0; n < 16; n++) {
				Output->DigitalOutScaled(TestDigitalOutChannel + n, false, false);
			}
			for (int n = 0; n < 16; n++) {
				Output->DigitalOutScaled(TestDigitalOutChannel + n, true, true);
				Wait(100);
			}
			for (int n = 0; n < 16; n++) {
				Output->DigitalOutScaled(TestDigitalOutChannel + n, false, false);
				Wait(100);
			}
			for (int n = 0; n < 16; n++) {
				Output->DigitalOutScaled(TestDigitalOutChannel + n, true, true);
				Wait(100);
			}
			Wait(10, 1);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();
			EmptyNIcardFIFO();
			break;
		case IDC_TEST_DIGITAL_ADDRESSING: {
			if (parent) {
				MeasurementDialog = new CExecuteMeasurementDlg(parent, Sequence);
				MeasurementDialog->Create();
			}
			ofstream out;
			CString buf;
			buf.Format("%sAddressing.dat", *TestDigitalOutBoardFileName);
			out.open(buf);
			out << "Address Address A0 A1 A2 A3 A4 A5 A6 A7" << endl;
			double ypos = 11;
			bool ErrorAddressingTest = false;
			int EST = -1;
			unsigned char MyAddress;
			unsigned int HardwareAddress;
			unsigned int MyBus;
			unsigned char MyDigOutDeviceNr;
			if (Output->GetDigitalOutMultiIOAddressIfPossible(TestDigitalOutChannel, MyDigOutDeviceNr, HardwareAddress)) {
				MyAddress = HardwareAddress & 0xFF;
				MyBus = (HardwareAddress & 0xF00) >> 8;
			}
			else {
				ControlMessageBox("CSequence::TestAnalogOutBoard : nothing configured on specified address");
				return true;
			}
			Output->SwitchForceWritingMode(On);
			for (int Address = 0; Address < 256; Address++) {
				Output->WriteMultiIO(MyDigOutDeviceNr, MyBus, MyAddress, 0);
				Output->WriteMultiIO(MyDigOutDeviceNr, MyBus, Address, 0xFFFF);
				double A;
				A = GetComparatorAnalogIn(TestDigitalOutBoardAnalogInBox, 0);
				if (A > 1) EST = Address;
				if (A > 1) ErrorAddressingTest = true;

				CString binary;
				binary = "";
				unsigned short helpData = Address;
				for (int k = 0; k < 8; k++) {
					binary = binary + ((helpData & 1) > 0 ? '1' : '0');
					helpData = helpData >> 1;
				}

				CString buf;
				buf.Format("%u %s %s", Address, binary, (A < 2.5) ? "." : "X");
				out << buf << endl;
				out.flush();

				buf.Format("Analog output addressing test\n\n%u: %s\n\nActive address = %i\n",
					Address, binary, EST);
				if (MeasurementDialog) MeasurementDialog->SetData(buf, Address * 10, 256 * 10);
				else {
					out.close();
					Output->SwitchForceWritingMode(Off);
					HardResetSystem();
					return true;
				}

			}
			Output->SwitchForceWritingMode(Off);
			if (MeasurementDialog) {
				MeasurementDialog->DestroyWindow();
				MeasurementDialog = NULL;
			}
			buf.Format("Last activated Address: %i", EST);
			out << buf << endl;
			out.flush();
			out.close();
			HardResetSystem();
			PlaySound(*SourceFilePath + "Sound\\short-and-bubbly-120528.wav", NULL, SND_FILENAME);
		} break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&TestDigitalOutChannel, "TestDigitalOutChannel", 0, 10000, "TestDigitalOutChannel");
		UtilityDialog->RegisterLong(&TestDigitalOutBoardAnalogInBox, "TestDigitalOutBoardAnalogInBox", 0, 4, "Analog In Box nb", "");
		UtilityDialog->RegisterString(TestDigitalOutBoardFileName, "TestDigitalOutBoardFileName", "TestDigital", 200, "Test digital out board filename");

		UtilityDialog->AddButton(IDC_TEST_DIGITAL_OUT, Sequence);
		UtilityDialog->AddButton(IDC_TEST_DIGITAL_OUT16BIT, Sequence);

		UtilityDialog->AddButton(IDC_TEST_DIGITAL_ADDRESSING, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

//Utility TestDigitalInput
long TestDigitalInputChannel;
bool CSequence::TestDigitalInput(unsigned int Message, CWnd* parent) {
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDC_TEST_DIGITAL_INPUT: {
			CString buf;
			unsigned short in = Output->DigitalIn(0);
			buf = "LPT1 input=MSB ";
			for (int i = 15; i >= 0; i--) buf = buf + (((in & (1 << i)) > 0) ? "1" : "0");
			buf = buf + " LSB\n\n";
			CString buf2;
			buf2.Format("LPT1 input=%x", Output->DigitalIn(0, (unsigned short)TestDigitalInputChannel));
			buf = buf + buf2;
			ControlMessageBox(buf);
		} break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&TestDigitalInputChannel, "TestDigitalInputChannel", 0, 16, "Test Digital Input Channel", "");
		UtilityDialog->AddButton(IDC_TEST_DIGITAL_INPUT, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

//Utility TestServo
long TestServoDigitalOutputNr;
double TestServoOnPulseDuration;
double TestServoOffPulseDuration;
double TestServoPulseSequenceDuration;
double TestServoPulseSeparation;
bool TestServoSmoothly;

bool CSequence::TestServo(unsigned int Message, CWnd* parent) {
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDM_TEST_SERVO_ON: TestPositionServo(TestServoDigitalOutputNr, On, TestServoOnPulseDuration, TestServoOffPulseDuration, TestServoPulseSequenceDuration, TestServoSmoothly); break;
		case IDM_TEST_SERVO_OFF: TestPositionServo(TestServoDigitalOutputNr, Off, TestServoOnPulseDuration, TestServoOffPulseDuration, TestServoPulseSequenceDuration, TestServoSmoothly); break;
		case IDM_TEST_SERVO_BLINK: TestPositionServoBlink(parent); break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&TestServoDigitalOutputNr, "TestServoDigitalOutputNr", 0, 200, "Test Servo Digital Output Nr");
		UtilityDialog->RegisterDouble(&TestServoOnPulseDuration, "TestServoOnPulseDuration", 0.01, 2.15, "On Pulse Duration", "ms");
		UtilityDialog->RegisterDouble(&TestServoOffPulseDuration, "TestServoOffPulseDuration", 0.01, 2.15, "Off Pulse Duration", "ms");
		UtilityDialog->RegisterDouble(&TestServoPulseSeparation, "TestServoPulseSeparation", 0, 100, "TestServoPulseSeparation", "ms");

		UtilityDialog->RegisterDouble(&TestServoPulseSequenceDuration, "TestServoPulseSequenceDuration", 0, 5000, "TestServoPulseSequenceDuration", "ms");
		UtilityDialog->RegisterBool(&TestServoSmoothly, "TestServoSmoothly", "TestServoSmoothly");
		UtilityDialog->AddButton(IDM_TEST_SERVO_ON, Sequence);
		UtilityDialog->AddButton(IDM_TEST_SERVO_OFF, Sequence);
		UtilityDialog->AddButton(IDM_TEST_SERVO_BLINK, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

void CSequence::TestPositionServo(long TestServoDigitalOutputNr, bool OnOff, double TestServoOnPulseDuration, double TestServoOffPulseDuration, double TestServoPulseSequenceDuration, bool smoothly) {
	SetAssembleSequenceListMode();
	StartSequence(NULL, NULL, false);
	double PositionServoTime = PositionServo(TestServoDigitalOutputNr, OnOff, TestServoOnPulseDuration, TestServoOffPulseDuration, TestServoPulseSequenceDuration, smoothly);
	Wait(PositionServoTime + 10);
	StopSequence();
	SetWaveformGenerationMode();
	ExecuteSequenceList();
	EmptyNIcardFIFO();
}



void CSequence::TestPositionServoBlink(CWnd* parent) {
	if ((CancelLoopDialog == NULL) && (parent)) {
		CancelLoopDialog = new CExecuteMeasurementDlg(parent, Sequence);
		CancelLoopDialog->Create();
		CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
	}
	while (CancelLoopDialog) {
		if (CancelLoopDialog) CancelLoopDialog->SetData("Servo On", 0, 1, true);
		else return;
		TestPositionServo(TestServoDigitalOutputNr, On, TestServoOnPulseDuration, TestServoOffPulseDuration, TestServoPulseSequenceDuration, TestServoSmoothly);
		double TimeOver = 0;
		while ((CancelLoopDialog) && (TimeOver < 1000)) {
			CancelLoopDialog->PumpMessages();
			Wait(50);
			TimeOver = TimeOver + 50;
		}
		if (CancelLoopDialog) CancelLoopDialog->SetData("Servo Off", 1, 1, true);
		TestPositionServo(TestServoDigitalOutputNr, Off, TestServoOnPulseDuration, TestServoOffPulseDuration, TestServoPulseSequenceDuration, TestServoSmoothly);
		TimeOver = 0;
		while ((CancelLoopDialog) && (TimeOver < 1000)) {
			CancelLoopDialog->PumpMessages();
			Wait(50);
			TimeOver = TimeOver + 50;
		}
	}
}


//Utility TestDDS
long TestDDSNr;
double TestDDSFrequency;
bool TestDDSAttenuationControl;
long TestDDSAddressingStartAddress;
long TestDDSAddressingStopAddress;
long TestDDSAddressingExcludedAddress;
long TestDDSAddressingBus;
bool CSequence::TestDDS(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDM_TEST_DDS:
			//DDS emiting frequenies outside the AOM bandwith can destroy the RF amplifier since the AOM is not a 50 Ohm load 
			//outside its bandwidth
			if (ControlMessageBox("Are you sure this DDS is not hooked up to an AOM ?", MB_YESNO) == IDNO) return true;
			if (((unsigned int)(TestDDSNr) >= NrAD9852) || (TestDDSNr < 0)) {
				ControlMessageBox("CSequence::TestDDS : DDSNr out of range");
				return true;
			}
			Wait(1);
			AD9852[TestDDSNr]->MasterReset();
			Wait(1);
			AD9852[TestDDSNr]->SetFrequencyWritePrecision(DDSFrequencyWritePrecision);
			AD9852[TestDDSNr]->SetOutputShapeKeyMult((unsigned short)(0x0FC0));
			AD9852[TestDDSNr]->SetFrequencyTuningWord1(50);
			AD9852[TestDDSNr]->SetComparatorPowerDown(true);
			AD9852[TestDDSNr]->SetBypassInverseSinc(true);
			AD9852[TestDDSNr]->SetControlDACPowerDown(false);
			AD9852[TestDDSNr]->InitPLL();
			Wait(1);
			Output->DisableRangeCheck();
			SetAssembleSequenceListMode();
			StartSequence();
			//for (int i=0;i<100;i++) {
			/*	for (unsigned int k=0;k<1000;k++) {
					AD9852[TestDDSNr]->SetControlDAC(k*10);
					Wait(0.1);
				}
				for (k=1000;k>0;k--) {
					AD9852[TestDDSNr]->SetControlDAC(k*10);
					Wait(0.1);
				}*/
				//}
			for (double f = 0; f < 135 * AD9852[TestDDSNr]->FrequencyMultiplier; f = f + 0.001 * (AD9852[TestDDSNr]->FrequencyMultiplier)) {
				SetFrequencyDDSAD9852(TestDDSNr, f);
				Wait(0.1);
			}
			for (int i = 2000; i > 0; i--) {
				if (TestDDSAttenuationControl) {
					SetAttenuationDDSAD9852(TestDDSNr, -42.0 * (2000 - i) / 2000);
				}
				else {
					SetIntensityDDSAD9852(TestDDSNr, 0.05 * i);
				}
				Wait(1);
			}
			for (int i = 0; i < 2000; i++) {
				if (TestDDSAttenuationControl) {
					SetAttenuationDDSAD9852(TestDDSNr, -42.0 * (2000 - i) / 2000);
				}
				else {
					SetIntensityDDSAD9852(TestDDSNr, 0.05 * i);
				}
				Wait(1);
			}
			if (TestDDSAttenuationControl) {
				SetAttenuationDDSAD9852(TestDDSNr, -42.0);
			}
			else {
				SetIntensityDDSAD9852(TestDDSNr, 0);
			}
			Wait(10);
			//Output->DebugWriteOutMultiIOBuffer(*DebugFilePath + "MultiIODebug.dat",110,0);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();
			//DebugSequenceList(*DebugFilePath + "TestSequenceListAfterExecution.dat",0);
			//if (SetMemoryReadoutMode()) ExecuteSequenceList();				
			EmptyNIcardFIFO();
			Output->EnableRangeCheck();
			break;
		case IDM_TEST_1GHZ_DDS:
			//DDS emiting frequenies outside the AOM bandwith can destroy the RF amplifier since the AOM is not a 50 Ohm load 
			//outside its bandwidth
			if (ControlMessageBox("Are you sure this DDS is not hooked up to an AOM ?", MB_YESNO) == IDNO) return true;
			if (((unsigned int)(TestDDSNr) >= NrAD9858) || (TestDDSNr < 0)) {
				ControlMessageBox("CSequence::TestDDS : DDSNr out of range");
				return true;
			}
			Output->DisableRangeCheck();
			SetAssembleSequenceListMode();
			StartSequence();
			Wait(1);
			AD9858[TestDDSNr]->MasterReset();
			Wait(10);
			AD9858[TestDDSNr]->Set2GHzDividerDisable(On);
			Wait(1);
			//for (int i=0;i<100;i++) {
			/*	for (unsigned int k=0;k<1000;k++) {
					AD9858[TestDDSNr]->SetControlDAC(k*10);
					Wait(0.1);
				}
				for (k=1000;k>0;k--) {
					AD9858[TestDDSNr]->SetControlDAC(k*10);
					Wait(0.1);
				}*/
				//}
			for (double f = 0; f < 450 * AD9858[TestDDSNr]->FrequencyMultiplier; f = f + 2 * AD9858[TestDDSNr]->FrequencyMultiplier) {
				AD9858[TestDDSNr]->SetFrequencyTuningWord0(f);
				Wait(50);
			}
			AD9858[TestDDSNr]->SetFrequencyTuningWord0(100);
			Wait(10);
			for (int i = 0; i >= -(4 * 32); i--) {
				AD9858[TestDDSNr]->SetAttenuation(0.25 * i);
				Wait(50);
			}
			for (int i = -(4 * 32); i <= 0; i++) {
				AD9858[TestDDSNr]->SetAttenuation(0.25 * i);
				Wait(50);
			}
			//Output->DebugWriteOutMultiIOBuffer(*DebugFilePath + "MultiIODebug.dat",110,0);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();
			//DebugSequenceList(*DebugFilePath + "TestSequenceListAfterExecution.dat",0);
			//if (SetMemoryReadoutMode()) ExecuteSequenceList();				
			EmptyNIcardFIFO();
			Output->EnableRangeCheck();
			break;
		case IDM_TEST_DDS_MODULATION: {
			//DDS emiting frequenies outside the AOM bandwith can destroy the RF amplifier since the AOM is not a 50 Ohm load 
			//outside its bandwidth
			if (ControlMessageBox("Are you sure this DDS is not hooked up to an AOM ?", MB_YESNO) == IDNO) return true;
			if (((unsigned int)(TestDDSNr) >= NrAD9852) || (TestDDSNr < 0)) {
				ControlMessageBox("CSequence::TestDDS : DDSNr out of range");
				return true;
			}
			SetAssembleSequenceListMode();
			StartSequence();
			//SetStartFrequencyRFAntenna0(79);
			//SetStopFrequencyRFAntenna0(81);
			//SetRampTimeRFAntenna0(100);
			//SetRampRateClockRFAntenna0(1);
			//SetIntensityRFAntenna0(100);	
			StartNewWaveformGroup();
			double RampTime = 5000;
			//Waveform(new CRamp("SetStartFrequencyRFAntenna0",LastValue,81.5,RampTime,0.1));
			//Waveform(new CRamp("SetStopFrequencyRFAntenna0",LastValue,81.5,RampTime,0.1));
			//Waveform(new CRamp("SetModulationFrequencyRFAntenna0",LastValue,0.001/0.1,RampTime,0.1));	
			//Waveform(new CRamp("SetIntensityRFAntenna0",LastValue,10,RampTime,0.1));
			WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
			Wait(100);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();
			EmptyNIcardFIFO();

		} break;
		case IDM_TEST_DDS_BUS: {
			////DDS emiting frequenies outside the AOM bandwith can destroy the RF amplifier since the AOM is not a 50 Ohm load 
			////outside its bandwidth
			//if (ControlMessageBox("Are you sure this DDS is not hooked up to an AOM ?",MB_YESNO) == IDNO) return true;	
			//if (((unsigned int)(TestDDSNr)>=NrAD9852) || (TestDDSNr<0)) {
			//	ControlMessageBox("CSequence::TestDDS : DDSNr out of range");
			//	return true;
			//}
			Output->DisableRangeCheck();
			SetAssembleSequenceListMode();
			SwitchForceWritingMode(On);
			StartSequence();
			//SwitchTestDigitalOut0(On);
			SetFrequencyDDSAD9852(TestDDSNr, TestDDSFrequency);
			WaitTillBusBufferEmpty();
			for (long i = 0; i < 20000; i++) {
				if (TestDDSAttenuationControl) {
					SetAttenuationDDSAD9852(TestDDSNr, -42.0 * (20000 - i) / 20000);
				}
				else {
					SetIntensityDDSAD9852(TestDDSNr, 0.0005 * i);
				}
				Wait(0.0005);
			}
			Wait(20);
			if (TestDDSAttenuationControl) {
				SetAttenuationDDSAD9852(TestDDSNr, -42.0);
			}
			else {
				SetIntensityDDSAD9852(TestDDSNr, 0);
			}
			//SwitchTestDigitalOut0(Off);
			Wait(10);
			StopSequence();
			SetWaveformGenerationMode();
			ExecuteSequenceList();
			SwitchForceWritingMode(Off);
			EmptyNIcardFIFO();
			Output->EnableRangeCheck();
		} break;
		case IDM_TEST_DDS_ADDRESSING:
		{
			//SetAttenuationTestDDSAD9852(0.);
			//SetFrequencyTestDDSAD9852(100.);
			//SetAttenuationTestDDSAD9858(0.);
			//SetFrequencyTestDDSAD9858(200.);
			if (TestDDSAddressingStartAddress < 0) TestDDSAddressingStartAddress = 0;
			if (TestDDSAddressingStopAddress > 255) TestDDSAddressingStopAddress = 255;
			for (int Address = TestDDSAddressingStartAddress; Address < TestDDSAddressingStopAddress; Address++) {
				if (Address != TestDDSAddressingExcludedAddress) {
					Output->WriteMultiIO(/*MultiIONr*/0, TestDDSAddressingBus, Address, 0x21); //to switch AD9852 off
					Output->WriteMultiIO(/*MultiIONr*/0, TestDDSAddressingBus, Address, 0x28); //to switch AD9858 off
				}
			}
		}
		break;
		default: return false;
		}
	}
	else {
		UtilityDialog->RegisterLong(&TestDDSNr, "TestDDSNr", 0, 100000, "Test DDS Nr", "");
		UtilityDialog->RegisterDouble(&TestDDSFrequency, "TestDDSFrequency", 0, 500, "Test frequency", "MHz");
		UtilityDialog->RegisterBool(&TestDDSAttenuationControl, "TestDDSAttenuationControl", "Attenuation control");
		UtilityDialog->AddButton(IDM_TEST_DDS, Sequence);
		UtilityDialog->AddButton(IDM_TEST_1GHZ_DDS, Sequence);
		UtilityDialog->AddButton(IDM_TEST_DDS_BUS, Sequence);
		UtilityDialog->AddButton(IDM_TEST_DDS_MODULATION, Sequence);
		UtilityDialog->AddStatic("");
		UtilityDialog->AddStatic("Test DDS addressing");
		UtilityDialog->RegisterLong(&TestDDSAddressingStartAddress, "TestDDSAddressingStartAddress", 0, 255, "Start Address", "");
		UtilityDialog->RegisterLong(&TestDDSAddressingStopAddress, "TestDDSAddressingStopAddress", 0, 255, "Stop Address", "");
		UtilityDialog->RegisterLong(&TestDDSAddressingExcludedAddress, "TestDDSAddressingExcludedAddress", 0, 255, "Excluded Address", "");
		UtilityDialog->RegisterLong(&TestDDSAddressingBus, "TestDDSAddressingBus", 0, 7, "Bus", "");
		UtilityDialog->AddButton(IDM_TEST_DDS_ADDRESSING, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

//Utility AnalogInRecorder
CString* AnalogInFileName;
bool AnalogInRecorderWithKeithleysAndOven;
double AnalogInPeriod;
bool CSequence::AnalogInRecorder(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		if (Message != IDM_ANALOG_IN_RECORDER) return false;

		if (!HardwareAccess) return true;
		if (MeasurementDialog) {
			MeasurementDialog->DestroyWindow();
			MeasurementDialog = NULL;
		}
		if (parent) {
			MeasurementDialog = new CExecuteMeasurementDlg(parent, Sequence);
			MeasurementDialog->Create();
		}

		ofstream out;
		out.open(*DataFilePath + *AnalogInFileName);
		out << "Time";
		for (int i = 0; i < NrAnalogInBoxes * 8; i++) out << " A" << i;
		if (AnalogInRecorderWithKeithleysAndOven) {
			out << " O0 O1 O2 O3 O4 O5 O6 O7 O8 O9 K0 K1 " << endl;
		}
		else out << endl;
		int OnOff = 0;
		DWORD Start = GetTickCount();
		long point = 1;
		DWORD Time = 0;
		double Voltage = -10;
		while (MeasurementDialog) {
			WakeUp();
			while (Time < (AnalogInPeriod * point)) Time = GetTickCount() - Start;
			point++;
			double A[8 * NrAnalogInBoxes];
			for (int b = 0; b < NrAnalogInBoxes; b++) {
				for (int y = 0; y < 8; y++) A[b * 8 + y] = GetComparatorAnalogInAveraged(b, y, 0);
			}
			CString buf;
			buf.Format("Analog input recorder\n\nA0 = %.4f V      A1 = %.4f V\nA2 = %.4f V      A3 = %.4f V\nA4 = %.4f V      A5 = %.4f V\nA6 = %.4f V      A7 = %.4f V\n",
				A[AnalogInBox * 8 + 0], A[AnalogInBox * 8 + 1], A[AnalogInBox * 8 + 2],
				A[AnalogInBox * 8 + 3], A[AnalogInBox * 8 + 4], A[AnalogInBox * 8 + 5],
				A[AnalogInBox * 8 + 6], A[AnalogInBox * 8 + 7]);
			double fTime = GetTickCount() - Start;
			if (MeasurementDialog) {
				MeasurementDialog->SetData(buf, (int)(fTime / 1000 + 10) * 5, 100, true);
			}
			buf.Format("%.3f", fTime / 1000.0);
			out << buf;
			for (int i = 0; i < NrAnalogInBoxes * 8; i++) {
				buf.Format(" %.4f", A[i]);
				out << buf;
			}
			if (AnalogInRecorderWithKeithleysAndOven) {
				CString buf2;
				double O[NrOvenTemperatures];
				OvenControl->GetTemperatures(O);
				buf2.Format(" %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f", O[0], O[1], O[2], O[3], O[4], O[5], O[6], O[7], O[8], O[9]);
				out << buf2;
				for (int i = 0; i < NrKeithleyMultimeter; i++) {
					KeithleyMultimeter[i]->GetData(KeithleyMultimeterData[i]);
					out << " " << KeithleyMultimeterData[i];
				}
			}
			out << endl;
			if (OnOff == 0) OnOff = 1; else OnOff = 0;
		}
		if (MeasurementDialog) {
			MeasurementDialog->DestroyWindow();
			MeasurementDialog = NULL;
		}
		out.close();
	}
	else {
		UtilityDialog->RegisterLong(&AnalogInBox, "AnalogInBox", 0, 4, "Analog In Box nb", "");
		UtilityDialog->RegisterString(AnalogInFileName, "AnalogInFileName", "AnalogIn.dat", 200, "Analog In recorder Filename");
		UtilityDialog->RegisterDouble(&AnalogInPeriod, "AnalogInPeriod", 0, 1000000, "Analog in period", "ms");
		UtilityDialog->RegisterBool(&AnalogInRecorderWithKeithleysAndOven, "AnalogInRecorderWithKeithleysAndOven", "With Keithleys and Oven ?");
		UtilityDialog->AddButton(IDM_ANALOG_IN_RECORDER, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

//Utility DigitalInRecorder
CString* DigitalInFileName;
double DigitalInPeriod;
bool CSequence::DigitalInRecorder(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		if (Message != IDM_DIGITAL_IN_RECORDER) return false;

		if (MeasurementDialog) {
			MeasurementDialog->DestroyWindow();
			MeasurementDialog = NULL;
		}
		if (parent) {
			MeasurementDialog = new CExecuteMeasurementDlg(parent, Sequence);
			MeasurementDialog->Create();
		}

		ofstream out;
		out.open(*DataFilePath + *DigitalInFileName);
		out << "Time A0 A1 A2 A3 A4 A5 A6 A7" << endl;
		int OnOff = 0;
		DWORD Start = GetTickCount();
		long point = 1;
		DWORD Time = 0;
		//double Voltage=-10;
		while (MeasurementDialog) {
			while (Time < (DigitalInPeriod * point)) Time = GetTickCount() - Start;
			point++;
			//for (int n=0;n<8;n++) Output->DigitalOut(56+n,Voltage);
			//Voltage+=0.01;
			//if (Voltage>10) Voltage=-10;
			int A0 = Output->DigitalIn(0, 0);
			int A1 = Output->DigitalIn(0, 1);
			int A2 = Output->DigitalIn(0, 2);
			int A3 = Output->DigitalIn(0, 3);
			int A4 = Output->DigitalIn(0, 4);
			int A5 = Output->DigitalIn(0, 5);
			int A6 = Output->DigitalIn(0, 6);
			int A7 = Output->DigitalIn(0, 7);
			CString buf;
			buf.Format("Digital input recorder\n\nA0 = %i      A1 = %i\nA2 = %i      A3 = %i\nA4 = %i      A5 = %i\nA6 = %i      A7 = %i\n",
				A0, A1, A2, A3, A4, A5, A6, A7);
			MeasurementDialog->SetData(buf, OnOff, 1);
			double fTime = Time;
			buf.Format("%.3f %i %i %i %i %i %i %i %i", fTime / 1000.0, A0, A1, A2, A3, A4, A5, A6, A7);
			out << buf << endl;
			if (OnOff == 0) OnOff = 1; else OnOff = 0;
		}
		out.close();
	}
	else {
		UtilityDialog->RegisterString(DigitalInFileName, "DigitalInFileName", "DigitalIn.dat", 200, "Digital In recorder Filename");
		UtilityDialog->RegisterDouble(&DigitalInPeriod, "DigitalInPeriod", 0, 1000000, "Digital in period", "ms");
		UtilityDialog->AddButton(IDM_DIGITAL_IN_RECORDER, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}


//Utility TestAnalogOutBoard
CString* TestAnalogOutBoardFileName;
long TestAnalogOutBoardStartAddress;
long TestAnalogOutBoardAnalogInBox;
bool CSequence::TestAnalogOutBoard(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		switch (Message) {
		case IDM_TEST_ANALOG_OUT_BOARD_SWEEP: {
			/*if (ControlMessageBox("Did you switch off all RF amplifiers, power supplies and similar dangerous computer controlled devices ?",MB_YESNO) == IDNO) return true;
			if (MeasurementDialog) {
				MeasurementDialog->DestroyWindow();
				MeasurementDialog=NULL;
			}
			*/
			if (parent) {
				MeasurementDialog = new CExecuteMeasurementDlg(parent, Sequence);
				MeasurementDialog->Create();
			}

			ofstream out;
			CString buf;
			buf.Format("%sSweep.dat", *TestAnalogOutBoardFileName);
			out.open(buf);
			out << "Voltage A0 A1 A2 A3 A4 A5 A6 A7" << endl;
			int OnOff = 0;

			long point = 1;
			double Voltage = -10;
			double MaxError = 0;
			double E[8];
			for (int i = 0; i < 8; i++) E[i] = 0;
			while ((MeasurementDialog) && (Voltage < 10)) {
				point++;
				double A[8];
				for (int n = 0; n < 8; n++) {
					double AktVoltage = Voltage + n;
					if (AktVoltage > 10) AktVoltage = AktVoltage - 20;
					Output->AnalogOut(TestAnalogOutBoardStartAddress + n, AktVoltage);

					A[n] = GetComparatorAnalogIn(TestAnalogOutBoardAnalogInBox, n);
					double Error = A[n] - AktVoltage;
					if (fabs(Error) > E[n]) E[n] = fabs(Error);
					if (fabs(Error) > MaxError) MaxError = fabs(Error);
				}

				CString buf;
				buf.Format("Analog output board sweep\n\nA0 = %.4f V      A1 = %.4f V\nA2 = %.4f V      A3 = %.4f V\nA4 = %.4f V      A5 = %.4f V\nA6 = %.4f V      A7 = %.4f V\n",
					A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7]);
				if (MeasurementDialog) {
					MeasurementDialog->SetData(buf, (int)((Voltage + 10) * 5), 100);
				}
				buf.Format("%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f", Voltage, A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7]);
				out << buf << endl;
				Voltage += 0.1;
				if (Voltage > 10) Voltage = -10;
				if (OnOff == 0) OnOff = 1; else OnOff = 0;
			}
			if (MeasurementDialog) {
				MeasurementDialog->DestroyWindow();
				MeasurementDialog = NULL;
			}
			buf.Format("MaxError %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f", E[0], E[1], E[2], E[3], E[4], E[5], E[6], E[7]);
			out << buf << endl;
			buf.Format("MaxError %.4f", MaxError);
			out << buf << endl;
			out.flush();
			out.close();
			HardResetSystem();
			PlaySound(*SourceFilePath + "Sound\\short-and-bubbly-120528.wav", NULL, SND_FILENAME);
		}
											break;
		case IDM_TEST_ANALOG_OUT_BOARD_ADDRESSING: {
			if (parent) {
				MeasurementDialog = new CExecuteMeasurementDlg(parent, Sequence);
				MeasurementDialog->Create();
			}
			ofstream out;
			CString buf;
			buf.Format("%sAddressing.dat", *TestAnalogOutBoardFileName);
			out.open(buf);
			out << "Address A0 A1 A2 A3 A4 A5 A6 A7" << endl;
			double ypos = 11;
			bool ErrorAddressingTest = false;
			int EST[8];
			int ENr[8];
			for (int i = 0; i < 8; i++) {
				EST[i] = -1;
				ENr[i] = 0;
			}
			unsigned char MyAddresse;
			unsigned int HardwareAddress;
			unsigned int MyBus;
			unsigned char MyAnaOutDeviceNr;
			if (Output->GetAnalogOutMultiIOAddressIfPossible(TestAnalogOutBoardStartAddress, MyAnaOutDeviceNr, HardwareAddress)) {
				MyAddresse = HardwareAddress & 0xFF;
				MyBus = (HardwareAddress & 0xF00) >> 8;
			}
			else {
				ControlMessageBox("CSequence::TestAnalogOutBoard : nothing configured on specified address");
				return true;
			}
			Output->SwitchForceWritingMode(On);
			for (int Address = 0; Address < 256; Address++) {

				for (int n = 0; n < 8; n++) Output->AnalogOut(TestAnalogOutBoardStartAddress + n, 0);

				Output->WriteMultiIO(MyAnaOutDeviceNr, MyBus, Address, 0xFFFF);

				bool A[8];
				for (int n = 0; n < 8; n++) {
					A[n] = GetComparatorAnalogIn(TestAnalogOutBoardAnalogInBox, n) > 1;
					if (A[n]) {
						EST[n] = Address;
						ENr[n]++;
					}
					if (A[n]) ErrorAddressingTest = true;
				}

				CString binary;
				binary = "";
				unsigned short helpData = Address;
				for (int k = 0; k < 8; k++) {
					binary = binary + ((helpData & 1) > 0 ? '1' : '0');
					helpData = helpData >> 1;
				}

				CString buf;
				buf.Format("%u %s %s %s %s %s %s %s %s %s", Address, binary, (A[0]) ? "X" : ".", (A[1]) ? "X" : ".", (A[2]) ? "X" : ".", (A[3]) ? "X" : ".", (A[4]) ? "X" : ".", (A[5]) ? "X" : ".", (A[6]) ? "X" : ".", (A[7]) ? "X" : ".");
				out << buf << endl;
				out.flush();

				buf.Format("Analog output addressing test\n\n%u: %s\n\nA0 = %i: %i      A1 = %i: %i\nA2 = %i: %i      A3 = %i: %i\nA4 = %i: %i      A5 = %i: %i\nA6 = %i: %i      A7 = %i: %i\n",
					Address, binary, ENr[0], EST[0], ENr[1], EST[1], ENr[2], EST[2], ENr[3], EST[3], ENr[4], EST[4], ENr[5], EST[5], ENr[6], EST[6], ENr[7], EST[7]);
				if (MeasurementDialog) MeasurementDialog->SetData(buf, Address * 10, 256 * 10);
				else {
					out.close();
					Output->SwitchForceWritingMode(Off);
					HardResetSystem();
					return true;
				}

			}
			Output->SwitchForceWritingMode(Off);
			if (MeasurementDialog) {
				MeasurementDialog->DestroyWindow();
				MeasurementDialog = NULL;
			}
			buf.Format("Last Error Addressing test %i:%i %i:%i %i:%i %i:%i %i:%i %i:%i %i:%i %i:%i", ENr[0], EST[0], ENr[1], EST[1], ENr[2], EST[2], ENr[3], EST[3], ENr[4], EST[4], ENr[5], EST[5], ENr[6], EST[6], ENr[7], EST[7]);
			out << buf << endl;
			out.flush();
			out.close();
			HardResetSystem();
			PlaySound(*SourceFilePath + "Sound\\short-and-bubbly-120528.wav", NULL, SND_FILENAME);
		}
												 break;
		default: return false;
		}
		return true;
	}
	else {
		UtilityDialog->RegisterLong(&TestAnalogOutBoardAnalogInBox, "TestAnalogOutBoardAnalogInBox", 0, 4, "Analog In Box nb", "");
		UtilityDialog->RegisterString(TestAnalogOutBoardFileName, "TestAnalogOutBoardFileName", "TestAnalogOut.dat", 200, "Test analog out board filename");
		UtilityDialog->RegisterLong(&TestAnalogOutBoardStartAddress, "TestAnalogOutBoardStartAddress", 0, 256, "Test Analog Out Board Start Address", "");
		UtilityDialog->AddButton(IDM_TEST_ANALOG_OUT_BOARD_SWEEP, Sequence);
		UtilityDialog->AddButton(IDM_TEST_ANALOG_OUT_BOARD_ADDRESSING, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}

//Utility TestVision
CString* VisionMessage;
bool CSequence::TestVisionCommand(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		if (Message != IDM_TEST_VISION_COMMAND) return false;
		Vision->Command(*VisionMessage);
	}
	else {
		UtilityDialog->RegisterString(VisionMessage, "VisionGridOnOff", "VisionGridOnOff", 200, "Vision Computer Message");
		UtilityDialog->AddButton(IDM_TEST_VISION_COMMAND, Sequence);
		UtilityDialog->AddStatic("");
	}
	return true;
}





long ToggleDDSNumber;
long RampRepetitions;
bool ToggleDDSType;
bool Ramptype;
double ToggleDDSFrequency1;
double ToggleDDSFrequency2;
double ToggleDDSWaitTime;

double TestDDSFreqOutFunctionLastValue = 0;
double TestDDSFreqOutFunction(double Value, bool GetValue) {
	if (GetValue) return TestDDSFreqOutFunctionLastValue;
	else {
		//		Output->AnalogOutScaled(TestDDSFreqOutChannel,Value,Value);
		if (!ToggleDDSType) { SetFrequencyDDSAD9852(ToggleDDSNumber, Value); }
		if (ToggleDDSType) { SetFrequencyDDSAD9858(ToggleDDSNumber, Value); }
		//		Wait(10);
		TestDDSFreqOutFunctionLastValue = Value;
	}
	return 0;
}

bool CSequence::ToggleDDSFrequency(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		if (Message != IDM_TOGGLE_AD9958_FREQUENCY) return false;
		if ((CancelLoopDialog == NULL) && (parent)) {
			CancelLoopDialog = new CExecuteMeasurementDlg(parent, Sequence);
			CancelLoopDialog->Create();
			CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
		}
		while (CancelLoopDialog) {
			if (Ramptype) {
				if (CancelLoopDialog) CancelLoopDialog->SetData("DDS Frequency 1", 0, 1);
				if (!ToggleDDSType) { SetFrequencyDDSAD9852(ToggleDDSNumber, ToggleDDSFrequency1); }
				if (ToggleDDSType) { SetFrequencyDDSAD9858(ToggleDDSNumber, ToggleDDSFrequency1); }
				Wait(ToggleDDSWaitTime);
				if (!CancelLoopDialog) return true;
				if (CancelLoopDialog) CancelLoopDialog->SetData("DDS Frequency 2", 1, 1);
				if (!ToggleDDSType) { SetFrequencyDDSAD9852(ToggleDDSNumber, ToggleDDSFrequency2); }
				if (ToggleDDSType) { SetFrequencyDDSAD9858(ToggleDDSNumber, ToggleDDSFrequency2); }
				Wait(ToggleDDSWaitTime);
			}
			else {
				/* this block (3 line code below) make sure you can close the dialog */
				if (CancelLoopDialog) CancelLoopDialog->SetData("Sweep mode !! ", 0, 002);
				Wait(10);
				if (!CancelLoopDialog) return true;
				/**/
				SetAssembleSequenceListMode();
				StartSequence(NULL, NULL, false);
				StartNewWaveformGroup();
				Waveform(new CRamp(&TestDDSFreqOutFunction, ToggleDDSFrequency1, ToggleDDSFrequency2, ToggleDDSWaitTime, 0.02));

				WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
				StopSequence();
				SetWaveformGenerationMode();
				ExecuteSequenceList();
				EmptyNIcardFIFO();
			}
		}
	}
	else {
		UtilityDialog->AddStatic("Toggle / Scan DDS frequency");
		UtilityDialog->AddButton(IDM_TOGGLE_AD9958_FREQUENCY, Sequence);
		UtilityDialog->RegisterBool(&ToggleDDSType, "ToggleDDSType", "DDS F:AD5852(135MHz)/T:AD5858(500MHz)");
		UtilityDialog->RegisterBool(&Ramptype, "Ramptype", "Toggle ? F: Scan");
		UtilityDialog->RegisterLong(&ToggleDDSNumber, "ToggleDDSNumber", 0, 100, "DDS Number", "");
		UtilityDialog->RegisterLong(&RampRepetitions, "RampRepetitions", 1, 200, " Frequency ramp repetitions", "");
		UtilityDialog->RegisterDouble(&ToggleDDSFrequency1, "ToggleDDSFrequency1", 25, 500, "DDS Frequency 1", " MHz");
		UtilityDialog->RegisterDouble(&ToggleDDSFrequency2, "ToggleDDSFrequency2", 25, 500, "DDS Frequency 2", " MHz");
		UtilityDialog->RegisterDouble(&ToggleDDSWaitTime, "ToggleDDSWaitTime", 1, 20000, "DDS Time Between Toggles", " ms");
		UtilityDialog->AddStatic("");
	}
	return true;
}


long BlinkDDSNumber;
bool BlinkDDSType;
double BlinkDDSIntensity1;
double BlinkDDSIntensity2;
double BlinkDDSWaitTime;
bool CSequence::BlinkDDSIntensity(unsigned int Message, CWnd* parent)
{
	if (!AssemblingUtilityDialog()) {
		if (Message != IDM_BLINK_DDS) return false;
		if ((CancelLoopDialog == NULL) && (parent)) {
			CancelLoopDialog = new CExecuteMeasurementDlg(parent, Sequence);
			CancelLoopDialog->Create();
			CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
		}
		while (CancelLoopDialog) {
			if (CancelLoopDialog) CancelLoopDialog->SetData("DDS Intensity 1", 0, 1);
			if (!BlinkDDSType) { SetIntensityDDSAD9852(BlinkDDSNumber, BlinkDDSIntensity1); }
			if (BlinkDDSType) { SetIntensityDDSAD9858(BlinkDDSNumber, BlinkDDSIntensity1); }
			Wait(BlinkDDSWaitTime);
			if (!CancelLoopDialog) return true;
			if (CancelLoopDialog) CancelLoopDialog->SetData("DDS Intensity 2", 1, 1);
			if (!BlinkDDSType) { SetIntensityDDSAD9852(BlinkDDSNumber, BlinkDDSIntensity2); }
			if (BlinkDDSType) { SetIntensityDDSAD9858(BlinkDDSNumber, BlinkDDSIntensity2); }
			Wait(BlinkDDSWaitTime);
		}
	}
	else {
		UtilityDialog->AddStatic("Blink DDS Intensity");
		UtilityDialog->AddButton(IDM_BLINK_DDS, Sequence);
		UtilityDialog->RegisterBool(&BlinkDDSType, "BlinkDDSType", "DDS F:AD5852(135MHz)/T:AD5858(500MHz)");
		UtilityDialog->RegisterLong(&BlinkDDSNumber, "BlinkDDSNumber", 0, 256, "DDS Number", "");
		UtilityDialog->RegisterDouble(&BlinkDDSIntensity1, "BlinkDDSIntensity1", 0, 100, "DDS Intensity 1", " %");
		UtilityDialog->RegisterDouble(&BlinkDDSIntensity2, "BlinkDDSIntensity2", 0, 100, "DDS Intensity 2", " %");
		UtilityDialog->RegisterDouble(&BlinkDDSWaitTime, "BlinkDDSWaitTime", 1, 20000, "DDS Time Between Blinks", " ms");
		UtilityDialog->AddStatic("");
	}
	return true;
}



