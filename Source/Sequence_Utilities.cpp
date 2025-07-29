
#include "stdafx.h"
#include "sequence.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif




// MainUtilities arranges functions module block on User interface
//If you want to add more utilities, follow this style.
bool CSequence::SequenceUtilities(unsigned int Message, CWnd* parent) {
	bool Received = false;
	
	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewMenu("User utilities");
		UtilityDialog->AddStatic("Run AQuRA clock");
	}
	Received |= UtilityRunClock(Message, parent);

	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewColumn();
		UtilityDialog->AddStatic("User utilities");
		//		UtilityDialog->AddStatic("");
	}
	Received |= UtilityTestSequence(Message, parent);
	Received |= UtilityTest448nmCavityAnalogOut(Message, parent);
	Received |= UtilityBlinkShutters(Message, parent);
	Received |= UtilityTorunCoilDrivers(Message, parent);
	


	/*
	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewMenu("High power laser utilities");
	}
	*/
	/*
	if (AssemblingUtilityDialog()) {
		UtilityDialog->NewMenu("Agilent frequency comb utilities");
	}
	Received|=FrequencyCombUtility(Message,parent);
	*/
	return Received;
}



///////////////////////////////////////////////////////////////////////////////////
// CSequence::MessageMap
//
// dispatches Messages from Dialog to corresponding subroutines
//
bool CSequence::MessageMap(unsigned int Message, CWnd* parent)
{
	if (!parent) {
		//this should not happen, but if it does, we use the main window as parent
		parent = ControlApp.m_pMainWnd;
	}
	bool Received = false;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	SaveParams();

	Received |= SequenceUtilities(Message, parent);
	Received |= SequenceStandardUtilities(Message, parent);

	//here comes an outdated style of adding utilities. Don't use it for new ulilities. Use the style used in in SequenceUtilities.

	if (!Received) {
		Received = true;
		switch (Message) {

			//case IDM_START_CYCLIC_OPERATION: StartCyclicOperation(parent); break;
			//case IDM_STOP_CYCLIC_OPERATION: StopCyclicOperation(); break;
			//case IDM_SWITCH_TO_BROAD_RED_MOT: SwitchToBroadRedSrMOT(); break;
			//case IDM_SWITCH_TO_NARROW_RED_MOT: SwitchToNarrowRedSrMOT(); break;
			
		case IDM_LINE_NOISE_COMPENSATION_STOP_PHASE_SHIFT: LineNoiseCompensationStopPhaseShift(); break;
		case IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_FAST_FORWARD: LineNoiseCompensationStartPhaseShiftFastForward(); break;
		case IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_FAST_BACKWARD: LineNoiseCompensationStartPhaseShiftFastBackward(); break;
		case IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_SLOW_FORWARD: LineNoiseCompensationStartPhaseShiftSlowForward(); break;
		case IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_SLOW_BACKWARD: LineNoiseCompensationStartPhaseShiftSlowBackward(); break;
		case IDM_LINE_NOISE_COMPENSATION_START_PHASE_SHIFT_BY_GIVEN_TIME: LineNoiseCompensationPhaseShiftByGivenTime(); break;
		case IDM_LINE_NOISE_COMPENSATION_APPLY_CONSTANT_VOLTAGE: LineNoiseCompensationApplyConstantVoltage(); break;
		case IDM_LINE_NOISE_COMPENSATION_APPLY_WAVEFORM: LineNoiseCompensationApplyWaveform(); break;


		case IDM_TEST_PULSE_SEQUENCE: TestPulseSequence(parent); break;
		case IDM_WRITE_PARAMLIST_TO_ASCII_FILE: WriteParamListToASCIIFile(); break;
		case IDM_TEST_INJECTIONS: PauseSystem(parent); break;
		case IDM_RESET_SYSTEM: ResetSystem(); break;
		case IDM_SET_IPG5WLaser1_LASER_Current: SetIPGLaserCurrent(1); break;
		case IDM_SET_IPG5WLaser1_LASER_POWER: SetIPGLaserPower(1); break;
		case IDM_GET_IPG5WLaser1_LASER_STATUS: GetIPGLaserStatus(1, true); break;
		case IDM_SET_IPG5WLaser2_LASER_Current: SetIPGLaserCurrent(2); break;
		case IDM_SET_IPG5WLaser2_LASER_POWER: SetIPGLaserPower(2); break;
		case IDM_GET_IPG5WLaser2_LASER_STATUS: GetIPGLaserStatus(2, true); break;
		case IDM_SET_IPG100WLaser_LASER_Current: SetIPGLaserCurrent(0); break;
		case IDM_SET_IPG100WLaser_LASER_Power: SetIPGLaserPower(0); break;
		case IDM_GET_IPG100WLaser_LASER_STATUS: GetIPGLaserStatus(0, true); break;
		case IDM_TEST_CAMERAS: TestCameras(parent); break;

		case IDM_HARD_RESET_SYSTEM: HardResetSystem(); break;
		case IDM_RED_HARD_RESET_SYSTEM: HardResetRedDDS(); break;
		case IDM_MENU_0: ParamList->AdaptMenu0RadioButtonBoxVariables(); break;
		case IDM_POWER_SUPPLIES_OFF: SwitchPowerSuppliesOff(); break;
		case IDM_POWER_SUPPLIES_ON: SwitchPowerSuppliesOn(); break;
		case IDM_EMERGENCY_SHUTOFF: EmergencyShutoff(); break;
			//case IDM_ZEEMAN_SLOWER_OFF: SwitchZeemanSlower(Off); break;
		case IDM_ALL_DDS_OFF: AllDDSOff(); break;
		case IDM_SWITCH_LASER_SECURITY_SIGN_ON: SwitchLaserSecuritySignOnManual(); break;
		case IDS_AQuRA_SWITCHAGILENTPULSEGENERATORON: SwitchAgilentPulseGeneratorPermanentlyOn(); break;
		case IDM_START_REMOTE_CONTROL_SERVER: RemoteControl.StartNetworkServer(ControlApp.m_pMainWnd); break;

		default: Received = false;
		}
	}
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (!Received) CSequenceLib::MessageMap(Message, parent);
	return Received;
}





void CSequence::WriteParamListToASCIIFile() {
	CFileDialog FileDialog( false, "txt", *UserParameterASCIIFileName);
	if (FileDialog.DoModal()==IDOK) {
		*UserParameterASCIIFileName=FileDialog.GetPathName();
		ofstream* out=new ofstream(*UserParameterASCIIFileName);
		ParamList->WriteToASCIIFile(out);
		UtilityDialog->WriteToASCIIFile(out);
		SystemParamList->WriteToASCIIFile(out);
		out->close();
	}
}


void CSequence::SetIPGLaserCurrent(int IPGLaserNumber)
{
	ReadIPGLaserStatus();
	//SwitchOpticalDipoleTrapAOMsOff();
	if (IPGLaserNumber == 0) {
		if (!AskLaserPowerIncrease(IPGLaserNumber)) return;
		if (IPG100WLaserCurrent > AktIPG100WLaserCurrent) {
			if (IPG100WLaserCurrent <= 500) PlaySound(*SourceFilePath + "Sound\\chime-alert-demo-309545.wav", NULL, SND_FILENAME);
			else {
				for (int i = 0; i < 3; i++) PlaySound(*SourceFilePath + "Sound\\siren-alert-96052.wav", NULL, SND_FILENAME);
			}
		} //else PlaySound(*SourceFilePath + "Sound\\lifeover.wav",NULL,SND_FILENAME);		
//		Set100WIRLaserCurrent(IPG100WLaserCurrent);		
		AktIPG100WLaserCurrent = IPG100WLaserCurrent;
		IPG100WLaserSaveMode = false;
	}
	else if (IPGLaserNumber == 1) {
		if (!AskLaserPowerIncrease(IPGLaserNumber)) return;
		if (IPG5WLaser1Current > 0) PlaySound(*SourceFilePath + "Sound\\chime-alert-demo-309545.wav", NULL, SND_FILENAME);
		IPGLaser[1]->SetOutputCurrent(IPG5WLaser1Current);
		AktIPG5WLaser1Current = IPG5WLaser1Current;
	}
	else if (IPGLaserNumber == 2) {
		if (!AskLaserPowerIncrease(IPGLaserNumber)) return;
		if (IPG5WLaser2Current > 0) PlaySound(*SourceFilePath + "Sound\\chime-alert-demo-309545.wav", NULL, SND_FILENAME);
		IPGLaser[2]->SetOutputCurrent(IPG5WLaser2Current);
		AktIPG5WLaser2Current = IPG5WLaser2Current;
	}
	Wait(5000, 2040);
	UpdateLaserSecuritySignSetting();
}



void CSequence::SetIPGLaserPower(int IPGLaserNumber)
{
	ReadIPGLaserStatus();
	//  SwitchOpticalDipoleTrapAOMsOff();
	if (IPGLaserNumber == 0) {
		if (!AskLaserPowerIncrease(IPGLaserNumber)) return;
		//	Wait(5000,2050);
		if (IPG100WLaserPower > AktIPG100WLaserPower) {
			if (IPG100WLaserPower <= 2) PlaySound(*SourceFilePath + "Sound\\chime-alert-demo-309545.wav", NULL, SND_FILENAME);
			else {
				for (int i = 0; i < 2; i++)
					PlaySound(*SourceFilePath + "Sound\\siren-alert-96052.wav", NULL, SND_FILENAME);
			}
		} //else PlaySound(*SourceFilePath + "Sound\\lifeover.wav",NULL,SND_FILENAME) */;				
		//Set100WIRLaserPower(IPG100WLaserPower);
		IPG100WLaserSaveMode = false;
		AktIPG100WLaserPower = IPG100WLaserPower;
		Wait(5000, 2050);
		UpdateLaserSecuritySignSetting();
	} if (IPGLaserNumber == 1) {
		if (!AskLaserPowerIncrease(IPGLaserNumber)) return;
		if (IPG5WLaser1Power > AktIPG5WLaser1Power) {
			if (IPG5WLaser1Power <= 2) PlaySound(*SourceFilePath + "Sound\\chime-alert-demo-309545.wav", NULL, SND_FILENAME);
			else {
				for (int i = 0; i < 2; i++)
					PlaySound(*SourceFilePath + "Sound\\siren-alert-96052.wav", NULL, SND_FILENAME);
			}
		} //else PlaySound(*SourceFilePath + "Sound\\lifeover.wav",NULL,SND_FILENAME) */;						
		IPGLaser[1]->SetOutputPower(IPG5WLaser1Power);
		AktIPG5WLaser1Power = IPG5WLaser1Power;
		Wait(5000, 2050);
		UpdateLaserSecuritySignSetting();
	} if (IPGLaserNumber == 2) {
		if (!AskLaserPowerIncrease(IPGLaserNumber)) return;
		if (IPG5WLaser2Power > AktIPG5WLaser2Power) {
			if (IPG5WLaser2Power <= 2) PlaySound(*SourceFilePath + "Sound\\chime-alert-demo-309545.wav", NULL, SND_FILENAME);
			else {
				for (int i = 0; i < 2; i++)
					PlaySound(*SourceFilePath + "Sound\\siren-alert-96052.wav", NULL, SND_FILENAME);
			}
		} //else PlaySound(*SourceFilePath + "Sound\\lifeover.wav",NULL,SND_FILENAME) */;						
		IPGLaser[2]->SetOutputPower(IPG5WLaser2Power);
		AktIPG5WLaser2Power = IPG5WLaser2Power;
		Wait(5000, 2050);
		UpdateLaserSecuritySignSetting();
	}
}

bool CSequence::AskLaserPowerIncrease(int IPGLaserNumber)
{
	if ((IPG5WLaser1Current > AktIPG5WLaser1Current) || (IPG5WLaser2Current > AktIPG5WLaser2Current)) {
		CString buf;
		buf.Format("Are you sure you want to change power of laser %i ?", IPGLaserNumber);
		if (ControlMessageBox(buf, MB_YESNO) == IDYES) return true;
		else {
			switch (IPGLaserNumber) {
			case 0:
				IPG100WLaserPower = (long)AktIPG100WLaserPower;
				IPG100WLaserCurrent = (long)AktIPG100WLaserCurrent;
				break;
			case 1:
				IPG5WLaser1Current = (long)AktIPG5WLaser1Current;
				break;
			case 2:
				IPG5WLaser2Current = (long)AktIPG5WLaser2Current;
				break;
			}
			return false;
		}
	}
	else return true;
}




void CSequence::PauseSystemDuringMeasurement(CWnd* parent)
{
	PauseSystem(parent);
}


void CSequence::TestCameras(CWnd* parent)
{
	if (!Vision->CheckReady()) {
		ControlMessageBox("CSequence::InitializeSequence : Vision not ready");
		return;
	}
	if ((CancelLoopDialog == NULL) && (parent)) {
		CancelLoopDialog = new CExecuteMeasurementDlg(parent, this);
		CancelLoopDialog->Create();
		CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
	}
	bool i = true;
	while (CancelLoopDialog) {
		CancelLoopDialog->SetData("Testing Cameras...", (i) ? 1 : 0, 1);
		i = !i;
		if (CameraFKSDataImages[i] < CameraFKSReferenceImages[i]) CameraFKSDataImages[i] = CameraFKSReferenceImages[i];
		double ExposureTime = CameraExposureTimeFluo;
		if (DoTakeAbsorptionPicture) ExposureTime = CameraExposureTimeAbs;
		else if (DoTakeFKSAbsorptionPicture) {
			if ((InitialExpansionTime + CameraExposureTimeFKS + FKSSecondFlashWait) > FKSExpansionTime) {
				ExposureTime = CameraExposureTimeFKS;
			}
			else {
				ExposureTime = FKSExpansionTime - InitialExpansionTime - FKSSecondFlashWait;
			}
		}
		//		else if (DoTakeFKSAbsorptionPicture) ExposureTime=CameraExposureTimeFKS;
		for (int i = 0; i < NrCameras; i++) Vision->SetNetCameraParameters(
			i, *CameraDirection[i], CameraUsed[i], CameraDisplayedPictureNumber[i], CameraMainCamera[i],
			(DoTakeFKSAbsorptionPicture) ? CameraStartXFKS[i] : CameraStartX[i],
			(DoTakeFKSAbsorptionPicture) ? CameraStartYFKS[i] : CameraStartY[i],
			(DoTakeFKSAbsorptionPicture) ? CameraWidthFKS[i] : CameraWidth[i],
			(DoTakeFKSAbsorptionPicture) ? CameraHeightFKS[i] : CameraHeight[i],
			CameraCalX[i], CameraCalY[i], ExposureTime,
			CameraTemperature[i], CameraBinningX[i], CameraBinningY[i], CamerahSpeed[i], DoTakeFKSAbsorptionPicture, CameraFKSDataImages[i], CameraFKSReferenceImages[i], CameraFKSHeight[i],
			CameraFKSvSpeed[i], CameraFKSDirtyLines[i], CameraNumberPictures[i], CameraPrePicture[i], CameraFlushWait[i], CameraInternalTriggerAfterExternalTrigger[i], CameraSoftTriggerDelay[i],
			CameraAtomicMass[i], CameraAtomicWavelength[i], CameraAtomicClebschGordon[i], CameraAtomicLinewidth[i],
			CameraAtomicMagneticMoment[i], *CameraAtomicName[i], CameraProbeDetuning[i], 0,
			DoTakeFluorescencePicture, CameraMaxFluorescence[i], CameraCalibration[i], *LastAtomPath, *LastFinalTrap,
			/*ExternalTrigger*/false,/*NumberOfAdditionalReferenceImages*/0,/*DepthOfReferenceImageLibrary*/0);
		Vision->TakeAbsorptionPicture(100);
		Vision->Ready();
		const unsigned int LastDataArrayPoints = 10;
		double LastDataArray[LastDataArrayPoints];
		CString LastDataArrayNames[LastDataArrayPoints];
		Vision->SendPictureData(/*MOTFluorescence*/0,/*RecapFluo*/0,
			/*BarCurrent*/0,/*TransferCurrent*/0,
			/*MOTCurrent*/0,/*Bias*/0,/*Gradiant*/0,/*Curvature*/0,
			/*LoadingTime*/0,/*TimingError*/0, LastDataArray, LastDataArrayNames, LastDataArrayPoints);
	}
}








void CSequence::LineNoiseCompensationApplyConstantVoltage() {
	LineNoiseCompensationArbitraryWaveformGenerator->SetDC();
	LineNoiseCompensationArbitraryWaveformGenerator->SetOffset(LineNoiseCompensationConstantVoltage);
}

void CSequence::LineNoiseCompensationApplyWaveform() {
	const int MaxArbitraryWaveformBufferLength=65536;
	double *Waveform=new double[MaxArbitraryWaveformBufferLength];
	double *WaveformSpline=new double[MaxArbitraryWaveformBufferLength];
	double *WaveformInterpolated=new double[MaxArbitraryWaveformBufferLength];
	short int *WaveformDigitized=new short int[MaxArbitraryWaveformBufferLength];
	for (int i=0;i<MaxArbitraryWaveformBufferLength;i++) {
		Waveform[i]=0;
		WaveformInterpolated[i]=0;
	}

	//load waveform
	ifstream in(*LineNoiseCompensationFile);
	if (!in.is_open()) {
		ControlMessageBox("CSequence::LineNoiseCompensationApplyWaveform : file "+(*LineNoiseCompensationFile)+" not found");
		return;
	}
	int ArbitraryWaveformBufferLength=0;
	double fMin=9999999999;
	double fMax=-9999999999;
	while ((!in.eof()) && (ArbitraryWaveformBufferLength<MaxArbitraryWaveformBufferLength)) {
		in>>Waveform[ArbitraryWaveformBufferLength];
		if (Waveform[ArbitraryWaveformBufferLength]>fMax) fMax=Waveform[ArbitraryWaveformBufferLength];
		if (Waveform[ArbitraryWaveformBufferLength]<fMin) fMin=Waveform[ArbitraryWaveformBufferLength];
		ArbitraryWaveformBufferLength++;
	}
	double AbsMax;
	if (fabs(fMin)>fabs(fMax)) AbsMax=fabs(fMin); else AbsMax=fabs(fMax);
	if (AbsMax>20) { //convert from Hz to kHz
		for (int i=0;i<ArbitraryWaveformBufferLength;i++) Waveform[i]=Waveform[i]/1000;
		fMin=fMin/1000;
		fMax=fMax/1000;
		AbsMax=AbsMax/1000;
	}
	double FrequencyRange=fMax-fMin;
	double VoltageRange=FrequencyRange/LineNoiseCompensationkHzPerVolt;
	double VoltageMin=fMin/LineNoiseCompensationkHzPerVolt;
	double VoltageMax=fMax/LineNoiseCompensationkHzPerVolt;
	if (VoltageRange>10) {
		ControlMessageBox("CSequence::LineNoiseCompensationApplyWaveform : Maximum Voltage range exceeded");
		return;
	}
	double VoltageOffset=0;
	double FrequencyOffset=0;
	if ((VoltageMin<-5) || (VoltageMax>5)) {
		double MiddleVoltage=VoltageRange*0.5;
		VoltageOffset=-(VoltageMax-MiddleVoltage);
		FrequencyOffset=VoltageOffset*LineNoiseCompensationkHzPerVolt;
		CString buf;
		buf.Format("CSequence::LineNoiseCompensationApplyWaveform :\nAdapting Voltage Offset. Results in %.4f kHz shift.",FrequencyOffset);
		ControlMessageBox(buf);
	}
	double SourceWaveformMax=AbsMax;
	if ((!in.eof()) || ((ArbitraryWaveformBufferLength)>MaxArbitraryWaveformBufferLength)) {
		ControlMessageBox("CSequence::LineNoiseCompensationApplyWaveform : waveform too long");
		return;
	}
	in.close();
	if (ArbitraryWaveformBufferLength==0) {
		ControlMessageBox("CSequence::LineNoiseCompensationApplyWaveform : file "+(*LineNoiseCompensationFile)+" empty");
		return;
	}
	double Rescale=1;
	if (LineNoiseCompensationSplineInterpolation) {
		//spline approximation
		CArray<POINT,POINT> SplineControlPoints;
		CArray<POINT, POINT> CurvePoints;
		SplineControlPoints.SetSize(3*ArbitraryWaveformBufferLength);
		for (int j=0;j<3;j++) {
			for (int i=0;i<ArbitraryWaveformBufferLength;i++) {
				//scale to int range
				SplineControlPoints[j*ArbitraryWaveformBufferLength+i]=CPoint((int)floor(16384.0*(j*ArbitraryWaveformBufferLength+i)/(3*ArbitraryWaveformBufferLength)),(int)floor(16384.0*(Waveform[i])/AbsMax));
			}
		}
		//create a spline object
		Spline spline(SplineControlPoints.GetData(), SplineControlPoints.GetSize());
		//generate a curve
		spline.Generate();
		//get the curve points number
		CurvePoints.SetSize(spline.GetCurveCount());
		//get the points number
		int PointCount = 0;
		spline.GetCurve(CurvePoints.GetData(), PointCount);

		long NrTimeSteps=LineNoiseCompensationInterpolationPoints*ArbitraryWaveformBufferLength;
		if (NrTimeSteps>MaxArbitraryWaveformBufferLength) {
			ControlMessageBox("CSequence::LineNoiseCompensationApplyWaveform : too much interpolation");
			return;
		}


		ofstream out(*DebugFilePath + "DebugLineNoiseCompensationSpline.dat");
		for (int i=0;i<CurvePoints.GetSize();i++) {
			out<<CurvePoints[i].x/16384.0<<" "<<CurvePoints[i].y/16384.0<<endl;
		}
		out.close();

		//extract values at given number of timesteps
		double sMin=9999999999;
		double sMax=-9999999999;
		for (int i=0;i<NrTimeSteps;i++) {
			double Time=((double)(i))/NrTimeSteps;  //scale: from 0..1
			int j=PointCount/3-2; //start to search after first spline period
			while ((j<CurvePoints.GetSize()) && ((3*CurvePoints[j].x/16384.0-1)<Time)) j++;
			int point1=j-1;
			int point2=j;
			if ((point1>=CurvePoints.GetSize()) || (point2>=CurvePoints.GetSize())) {
				ControlMessageBox("CSequence::LineNoiseCompensationApplyWaveform : spline error 1");
				return;
			} else {
				//linear interpolation
				double t1=(3.0*(double)CurvePoints[point1].x)/16384.0-1;
				double t2=(3.0*(double)CurvePoints[point2].x)/16384.0-1;
				if ((t1-t2)==0) {
					//ControlMessageBox("CSequence::FrequencyCombProgramAgilent : spline error 2");
					Waveform[i]=((double)CurvePoints[point1].y)/16384.0;
				} else {
					double a1=((double)CurvePoints[point1].y)/16384.0;
					double a2=((double)CurvePoints[point2].y)/16384.0;
					Waveform[i]=AbsMax*(a1+(a2-a1)*(Time-t1)/(t2-t1));
				}
			}
			if (Waveform[i]>sMax) sMax=Waveform[i];
			if (Waveform[i]<sMin) sMin=Waveform[i];
		}
		if (fabs(sMin)>fabs(sMax)) sMax=fabs(sMin); else sMax=fabs(sMax);
		Rescale=sMax/AbsMax;

		ArbitraryWaveformBufferLength=NrTimeSteps;

	}
	//rescale waveform and adjust phase
	if (LineNoiseCompensationArbitraryPhase<0) LineNoiseCompensationArbitraryPhase=0;
	if (LineNoiseCompensationArbitraryPhase>20) LineNoiseCompensationArbitraryPhase=20;
	int AktPoint=(int)(((20-LineNoiseCompensationArbitraryPhase)/20.0)*ArbitraryWaveformBufferLength);
	if (AktPoint>=ArbitraryWaveformBufferLength) AktPoint=0;
	double ShiftedMax=fabs(fMax+FrequencyOffset);
	if (fabs(fMin+FrequencyOffset)>ShiftedMax) ShiftedMax=fabs(fMin+FrequencyOffset);
	for (int i=0;i<ArbitraryWaveformBufferLength;i++) {
		WaveformDigitized[i]=(short int)floor(Rescale*2046.0*(( Waveform[AktPoint]+FrequencyOffset)/ShiftedMax));
		AktPoint++;
		if (AktPoint>=ArbitraryWaveformBufferLength) AktPoint=0;
	}

	//LinePeriod=20ms -> Stepsize= 20ms/NrPoints
	LineNoiseCompensationArbitraryWaveformGenerator->SetArbitraryWaveform(ArbitraryWaveformBufferLength,20E-3/ArbitraryWaveformBufferLength,WaveformDigitized); //Waveform in 11 bit +1 bit sign format = -2048..2047, Timestep in Seconds
	LineNoiseCompensationArbitraryWaveformGenerator->SetArbitrary();
	LineNoiseCompensationArbitraryWaveformGenerator->SelectArbitraryVolatile();
	LineNoiseCompensationArbitraryWaveformGenerator->SetOffset(0);
	double Vpp=2*ShiftedMax/(LineNoiseCompensationkHzPerVolt*Rescale);
	if (Vpp>10) {
		ControlMessageBox("CSequence::LineNoiseCompensationApplyWaveform : spline led to too much excursion.\nWaveform has smaller amplitude than requested.");
		Vpp=10;
	}
	LineNoiseCompensationArbitraryWaveformGenerator->SetAmplitudeVpp(Vpp); //*2 because of 50Ohm termination

	ofstream out2(*DebugFilePath + "DebugLineNoiseCompensation.dat");
	for (int i=0;i<ArbitraryWaveformBufferLength;i++) {
		out2<<i*20E-3/ArbitraryWaveformBufferLength<<" "<<WaveformDigitized[i]/*ShiftedMax/2046.0*/ << endl;
		}
		out2.close();
		delete[] Waveform;
		delete[] WaveformSpline;
		delete[] WaveformInterpolated;
		delete[] WaveformDigitized;
	}

	void CSequence::LineNoiseCompensationStartPhaseShiftFastForward() {
		SetFrequencyLineNoiseCompensationClock(9.99);
	}

	void CSequence::LineNoiseCompensationStartPhaseShiftFastBackward() {
		SetFrequencyLineNoiseCompensationClock(10.01);
	}

	void CSequence::LineNoiseCompensationStartPhaseShiftSlowForward() {
		SetFrequencyLineNoiseCompensationClock(9.999);
	}

	void CSequence::LineNoiseCompensationStartPhaseShiftSlowBackward() {
		SetFrequencyLineNoiseCompensationClock(10.001);
	}

	void CSequence::LineNoiseCompensationStopPhaseShift() {
		SetFrequencyLineNoiseCompensationClock(10);
	}

	void CSequence::LineNoiseCompensationPhaseShiftByGivenTime() {
		SetAssembleSequenceListMode();
		StartSequence();
		Wait(20);
		SetFrequencyLineNoiseCompensationClock((LineNoiseCompensationArbitraryPhaseShiftTime > 0) ? 10.01 : 9.99);
		Wait(LineNoiseCompensationArbitraryPhaseShiftTime * 100);
		SetFrequencyLineNoiseCompensationClock(10);
		Wait(20);
		StopSequence();
		SetWaveformGenerationMode();
		ExecuteSequenceList();
		EmptyNIcardFIFO();
	}





	//Utility CavityTUningTestAnalogOut
	long UtilityTest448nmCavityAnalogOutChannel;
	double UtilityTest448nmCavityAnalogOutMinVoltage;
	double UtilityTest448nmCavityAnalogOutMaxVoltage;
	double UtilityTest448nmCavityAnalogOutTime;
	double UtilityTest448nmCavityAnalogOutWaitTime;
	long UtilityTest448nmCavityAnalogOutRepetitions;

	double UtilityTest448nmCavityAnalogOutFunctionLastValue = 0;
	double UtilityTest448nmCavityAnalogOutShutterFunctionLastValue = 0;

	double UtilityTest448nmCavityAnalogOutFunction(double Value, bool GetValue) {
		if (GetValue) return UtilityTest448nmCavityAnalogOutFunctionLastValue;
		else {
			Output->AnalogOutScaled(UtilityTest448nmCavityAnalogOutChannel, Value, Value);
			UtilityTest448nmCavityAnalogOutFunctionLastValue = Value;
		}
		return 0;
	}

	double UtilityTest448nmCavityAnalogOutShutterFunction(double Value, bool GetValue) {
		if (GetValue) return UtilityTest448nmCavityAnalogOutShutterFunctionLastValue;
		else {
			Output->AnalogOutScaled(48, Value, Value);
			UtilityTest448nmCavityAnalogOutShutterFunctionLastValue = Value;
		}
		return 0;
	}


	bool CSequence::UtilityTest448nmCavityAnalogOut(unsigned int Message, CWnd* parent) {
		if (!AssemblingUtilityDialog()) {
			switch (Message) {
			case IDC_TEST_448nm_Cavity_ANALOG_OUT:
				SetAssembleSequenceListMode();
				StartSequence(NULL, NULL, false);
				for (int n = 0; n < UtilityTest448nmCavityAnalogOutRepetitions; n++) {
					//SwitchAQuRABlueZeemanSlowerUniblitzShutter(On);
					//SetIntensityAQuRABlueMOTAOM1(100);
					Waveform(new CRamp(&UtilityTest448nmCavityAnalogOutFunction, UtilityTest448nmCavityAnalogOutMinVoltage * 0.5 + 5, UtilityTest448nmCavityAnalogOutMaxVoltage * 0.5 + 5, UtilityTest448nmCavityAnalogOutTime));
					//					Waveform(new CRamp(&UtilityTest448nmCavityAnalogOutShutterFunction,UtilityTest448nmCavityAnalogOutMinVoltage*0.5+5,UtilityTest448nmCavityAnalogOutMaxVoltage*0.5+5,UtilityTest448nmCavityAnalogOutTime));			

										//TestAnalogOutFunction(2,false);
					Wait(UtilityTest448nmCavityAnalogOutTime);
					Waveform(new CRamp(&UtilityTest448nmCavityAnalogOutFunction, UtilityTest448nmCavityAnalogOutMaxVoltage * 0.5 + 5, UtilityTest448nmCavityAnalogOutMinVoltage * 0.5 + 5, UtilityTest448nmCavityAnalogOutTime));
					//					Waveform(new CRamp(&UtilityTest448nmCavityAnalogOutShutterFunction,UtilityTest448nmCavityAnalogOutMaxVoltage*0.5+5,UtilityTest448nmCavityAnalogOutMinVoltage*0.5+5,UtilityTest448nmCavityAnalogOutTime));			

										//TestAnalogOutFunction(0,false);
										//Wait(UtilityTest448nmCavityAnalogOutTime);
										//SwitchAQuRABlueZeemanSlowerUniblitzShutter(Off);
										//SetIntensityAQuRABlueMOTAOM1(0);
										//Wait(UtilityTest448nmCavityAnalogOutWaitTime);
				}
				Wait(10);
				UtilityTest448nmCavityAnalogOutFunction(0, True);// Boolen: True makes output value stay at last value instead of reset
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
			UtilityDialog->RegisterLong(&UtilityTest448nmCavityAnalogOutChannel, "UtilityTest448nmCavityAnalogOutChannel", 0, 10000, "UtilityTest448nmCavityAnalogOutChannel");
			UtilityDialog->RegisterDouble(&UtilityTest448nmCavityAnalogOutMinVoltage, "UtilityTest448nmCavityAnalogOutMinVoltage", -10, 10, "Min Voltage", "V");
			UtilityDialog->RegisterDouble(&UtilityTest448nmCavityAnalogOutMaxVoltage, "UtilityTest448nmCavityAnalogOutMaxVoltage", -10, 10, "Max Voltage", "V");
			UtilityDialog->RegisterDouble(&UtilityTest448nmCavityAnalogOutTime, "UtilityTest448nmCavityAnalogOutTime", 0, 60000, "Time", "ms");
			UtilityDialog->RegisterDouble(&UtilityTest448nmCavityAnalogOutWaitTime, "UtilityTest448nmCavityAnalogOutWaitTime", 0, 60000, "WaitTime", "ms");
			UtilityDialog->RegisterLong(&UtilityTest448nmCavityAnalogOutRepetitions, "UtilityTest448nmCavityAnalogOutRepetitions", 0, 10000, "Repetitions");


			UtilityDialog->AddButton(IDC_TEST_448nm_Cavity_ANALOG_OUT, Sequence);
			UtilityDialog->AddStatic("");
		}
		return true;
	}




	//Utility UtilityTestSequence
	bool TestOnlyOnce;
	double TestFrequency;
	double TestTime;
	double TestAmplitude;
	bool CSequence::UtilityTestSequence(unsigned int Message, CWnd* parent)
	{
		if (!AssemblingUtilityDialog()) {
			if (Message != IDM_TEST_SEQUENCE) return false;
			bool OnlyOnce = true;
			if (!OnlyOnce) {
				if ((CancelLoopDialog == NULL) && (parent)) {
					CancelLoopDialog = new CExecuteMeasurementDlg(parent, this);
					CancelLoopDialog->Create();
					CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
				}
			}
			while (CancelLoopDialog || OnlyOnce) {
				if (!OnlyOnce) {
					if (CancelLoopDialog) CancelLoopDialog->SetData("Testing", 0, 1);
					if (!CancelLoopDialog) return true;
				}
				SetAssembleSequenceListMode();
				DoUtilityTestSequence();
				SetWaveformGenerationMode();
				ExecuteSequenceList();
				if (DebugSequenceListOn) DebugSequenceList(*DebugFileName + "UtilityTestSequenceListAfterExecution.dat", 0);
				if (SetMemoryReadoutMode()) ExecuteSequenceList();
				EmptyNIcardFIFO();
				if (OnlyOnce) return true;
			}
		}
		else {
			UtilityDialog->RegisterBool(&TestOnlyOnce, "TestOnlyOnce", "Test Only Once");
			UtilityDialog->RegisterDouble(&TestFrequency, "TestFrequency", 0, 100000, "TestFrequency", "Hz");
			UtilityDialog->RegisterDouble(&TestTime, "TestTime", 0, 100000, "TestTime", "ms");
			UtilityDialog->RegisterDouble(&TestAmplitude, "TestAmplitude", 0, 100, "TestAmplitude", "dB");
			UtilityDialog->AddButton(IDM_TEST_SEQUENCE, Sequence);
			UtilityDialog->AddStatic("");
		}
		return true;
	}


	void CSequence::DoUtilityTestSequence()
	{
		SwitchForceWritingMode(On);
		StartSequence();

		//SwitchTestDigitalOut0(On);
		//SetStartFrequencyRFAntenna0(SimpleRFSweepFrequency[0]);
		WaitTillBusBufferEmpty();
		//StartNewWaveformGroup();
		//Waveform(new CRamp("SetIntensityRFAntenna0",LastValue,SimpleRFSweepIntensity[0],SimpleRFSweepIntensityRampTime[0]));		
		//WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());

		//SetIntensityRFAntenna0(0);
		//SwitchTestDigitalOut0(Off);
		Wait(10);
		StopSequence();
		SwitchForceWritingMode(Off);
	}


	void CSequence::SwitchAQuRAShutters(bool OnOff) {
		SwitchBlueMOTShutter(OnOff);
		SwitchZSShutter(OnOff);
		SwitchRedMOTShutter(OnOff);
		SwitchRedPumpShutter(OnOff);
		SwitchRepumpShutter(OnOff);
		SwitchPumpPolarization1Shutter(OnOff);
		SwitchPumpPolarization2Shutter(OnOff);
		SwitchCoarseSpectroscopyShutter(OnOff);
		SwitchExtraClockShutter0(OnOff);
		SwitchExtraClockShutter1(OnOff);
		SwitchExtraClockShutter2(OnOff);
		SwitchExtraClockShutter3(OnOff);
		SwitchExtraClockShutter4(OnOff);
		SwitchExtraClockShutter5(OnOff);
		SwitchAdditionalShutter0(OnOff);
		SwitchAdditionalShutter1(OnOff);
		SwitchAdditionalShutter2(OnOff);
		SwitchAdditionalShutter3(OnOff);
		SwitchAdditionalShutter4(OnOff);
		SwitchAdditionalShutter5(OnOff);
		SwitchAdditionalShutter6(OnOff);
		SwitchAdditionalShutter7(OnOff);
	}

	//Utility UtilityBlinkShutters
	bool CSequence::UtilityBlinkShutters(unsigned int Message, CWnd* parent)
	{
		static long BlinkShutterPeriod;
		if (!AssemblingUtilityDialog()) {
			if (Message != IDM_BLINK_SHUTTERS) return false;
			if ((CancelLoopDialog == NULL) && (parent)) {
				CancelLoopDialog = new CExecuteMeasurementDlg(parent, this);
				CancelLoopDialog->Create();
				CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
			}
			bool OnOff = On;
			while (CancelLoopDialog) {
				if (CancelLoopDialog) CancelLoopDialog->SetData((OnOff) ? "Shutters On" : "Shutters Off", (OnOff) ? 1 : 0, 1);
				if (!CancelLoopDialog) return true;
				SetAssembleSequenceListMode();
				StartSequence(NULL, parent, false);
				SwitchAQuRAShutters(OnOff);
				StopSequence();
				SetWaveformGenerationMode();
				ExecuteSequenceList();
				OnOff = !OnOff; // toggle shutters on/off
			}
		}
		else {
			UtilityDialog->RegisterLong(&BlinkShutterPeriod, "BlinkShutterPeriod", 0, 100000, "Blink Shutter Wait Time", "ms");
			UtilityDialog->AddButton(IDM_BLINK_SHUTTERS, Sequence);
			UtilityDialog->AddStatic("");
		}
		return true;
	}

	//Utility Torun coil drivers
	bool CSequence::UtilityTorunCoilDrivers(unsigned int Message, CWnd* parent)
	{
		static long CoilDriver3x3AState=0;
		if (!AssemblingUtilityDialog()) {
			if (Message == IDM_PROGRAM_TORUN_COIL_DRIVER_3X3A) {
				InitializeCoilDriverTorun3x3A(/*OnlyFast*/ false, /*setting*/0);
				return true;
			}
			else if (Message == IDM_SET_STATE_TORUN_COIL_DRIVER_3X3A) {
				//we execute SetTorunCoilDriverState in sequence mode, not manual mode, to replicate the same timing as in the main experimental sequence
				SetAssembleSequenceListMode();
				StartSequence(NULL, parent, false);
				SetTorunCoilDriverState(CoilDriver3x3AState);
				StopSequence();
				SetWaveformGenerationMode();
				ExecuteSequenceList();
				unsigned int state = 0;
				CoilDriverTorun3x3A->GetState(state);
				if (state != CoilDriver3x3AState) {
					CString buf;
					buf.Format("CSequence::UtilityTorunCoilDrivers : Coil Driver 3x3A set to state %u, but requested state is %u", state, CoilDriver3x3AState);
					ControlMessageBox(buf);
				}
			}
			else if (Message == IDM_ENABLE_TORUN_COIL_DRIVER_100A) {
				CoilDriverTorun100A->SetMode(1); // 1 = Current controlled through front panel SMA input
				return true;
			}
			else if (Message == IDM_DISABLE_TORUN_COIL_DRIVER_100A) {
				CoilDriverTorun100A->SetMode(0); // 0 = Current off
				return true;
			}
			return false;
		}
		else {
			UtilityDialog->AddStatic("Offset coil driver");
			UtilityDialog->AddButton(IDM_PROGRAM_TORUN_COIL_DRIVER_3X3A, Sequence);
			UtilityDialog->RegisterLong(&CoilDriver3x3AState, "CoilDriver3x3AState", 0, 3, "Coil Driver 3x3A State", "0..3");
			UtilityDialog->AddButton(IDM_SET_STATE_TORUN_COIL_DRIVER_3X3A, Sequence);
			UtilityDialog->AddStatic("");
			UtilityDialog->AddStatic("MOT coil driver");
			UtilityDialog->AddButton(IDM_ENABLE_TORUN_COIL_DRIVER_100A, Sequence);
			UtilityDialog->AddButton(IDM_DISABLE_TORUN_COIL_DRIVER_100A, Sequence);
			UtilityDialog->AddStatic("");
		}
		return true;
	}


	void CSequence::TestPulseSequence(CWnd* parent) {
		SetAssembleSequenceListMode();
		StartSequence();
		Wait(20);
		StartNewWaveformGroup();
		Waveform(new CPulse("SwitchTestDigitalOut1", TestPulseFrequency, 1000, TestPulseDutyCycle));
		WaitTillEndOfWaveformGroup(GetCurrentWaveformGroupNumber());
		StopSequence();
		SetWaveformGenerationMode();
		ExecuteSequenceList();
		EmptyNIcardFIFO();
	}


	//Utility UtilityRunClock
	bool CSequence::UtilityRunClock(unsigned int Message, CWnd* parent)
	{
		static double UtilityRunClockStretchedStateFrequencyDifference;
		static double UtilityRunClockSideOfFringeFrequencyDifference;
		static double UtilityRunClockIntegratorConstant;
		unsigned long RunNr = 0;
		if (!AssemblingUtilityDialog()) {
			if (Message != IDM_RUN_CLOCK) return false;
			if ((CancelLoopDialog == NULL) && (parent)) {
				CancelLoopDialog = new CExecuteMeasurementDlg(parent, this);
				CancelLoopDialog->Create();
				CancelLoopDialog->SetWindowPos(&CWnd::wndTop, 100, 200, 150, 150, SWP_NOZORDER | SWP_NOSIZE | SWP_DRAWFRAME);
			}
			//open file and write header 
			std::fstream file(*DataFilePath + "Control_AQuRA_Clock_log.dat", std::ios::out);
			if (!file.is_open()) {
				ControlMessageBox("CSequence::UtilityRunClock : Couldn't open log file for writing");
				return true;
			}
			CString buf;
			buf.Format("RunNr Result-9/2- Result+9/2- Result-9/2+ Result-9/2+ Zeeman_splitting Frequency_deviation Integral");
			file << buf.GetString() << std::endl;
			file.close();

			unsigned int TypeOfRun = 0;
			double ClockExitationResult[4] = { 0, 0, 0, 0 };
			double ZeemanSplitting = 0;
			double FrequencyDeviation = 0;
			double Integral = 0;
			while (CancelLoopDialog) {
				//TypeOfRun = 0: InterrogatePositivemFState = false, InterrogatePositiveSlope = false
				//TypeOfRun = 1: InterrogatePositivemFState = true, InterrogatePositiveSlope = false
				//TypeOfRun = 2: InterrogatePositivemFState = false, InterrogatePositiveSlope = true
				//TypeOfRun = 3: InterrogatePositivemFState = true, InterrogatePositiveSlope = true
				bool InterrogatePositivemFState = (TypeOfRun & 1) > 0; // 0 = mF = -9/2, 1 = mF = +9/2
				bool InterrogatePositiveSlope = (TypeOfRun & 2) > 0;   // 0 = negative slope, 1 = positive slope
				ClockInterrogationFrequency = 80 + Integral + ((InterrogatePositivemFState) ? 1 : -1) * UtilityRunClockStretchedStateFrequencyDifference +
					((InterrogatePositiveSlope) ? 1 : -1) * UtilityRunClockSideOfFringeFrequencyDifference;

				CString buf;
				buf.Format("Running clock: RunNr = %u, RunType = %u, mF = %s9/2, Detuning = %s, Frequency = %f\nResult = {%.3f, %.3f, %.3f, %.3f}\nZeeman splitting = %f, Frequency deviation = %f\nIntegral = %f", RunNr, TypeOfRun, (InterrogatePositivemFState) ? "+" : "-", (InterrogatePositiveSlope) ? "+" : "-", ClockInterrogationFrequency-80,
					ClockExitationResult[0], ClockExitationResult[1], ClockExitationResult[2], ClockExitationResult[3], 
					ZeemanSplitting, FrequencyDeviation,
					Integral
					);
				if (!CancelLoopDialog) return true;
				else CancelLoopDialog->SetData(buf, TypeOfRun, 3);
				
				DoExperimentalSequence(parent);
				ClockExitationResult[TypeOfRun] = ClockStateExcitation;
				
				TypeOfRun++;
				
				if (TypeOfRun > 3) {
					TypeOfRun = 0;
					//Analyze the data, correct the clock frequency and write to file
					double DeviationPositivemFStates = ClockExitationResult[1] - ClockExitationResult[3];
					double DeviationNegativemFStates = ClockExitationResult[0] - ClockExitationResult[2];

					ZeemanSplitting = (DeviationNegativemFStates - DeviationPositivemFStates) / 2;
					FrequencyDeviation = (DeviationNegativemFStates + DeviationPositivemFStates) / 2;
					
					Integral = FrequencyDeviation * UtilityRunClockIntegratorConstant;

					//open file to append to it
					std::fstream file(*DataFilePath + "Control_AQuRA_Clock_log.dat", std::ios::out | std::ios::app);
					if (!file.is_open()) {
						ControlMessageBox("CSequence::UtilityRunClock : Couldn't open log file for writing");
						return true;
					}
					CString buf;
					buf.Format("RunNr = %u, Result = %.3f, %.3f, %.3f, %.3f, Zeeman splitting = %f, Frequency deviation = %f Integral = %f", 
						RunNr, ClockExitationResult[0], ClockExitationResult[1], ClockExitationResult[2], ClockExitationResult[3],
						ZeemanSplitting, FrequencyDeviation,
						Integral
					);
					file << buf.GetString() << std::endl;
					file.close();
					RunNr++;
				}

			}
		}
		else {
			UtilityDialog->RegisterDouble(&UtilityRunClockStretchedStateFrequencyDifference, "UtilityRunClockStretchedStateFrequencyDifference", 0, 100000, "Zeeman shift mF=+9/2", "MHz");
			UtilityDialog->RegisterDouble(&UtilityRunClockSideOfFringeFrequencyDifference, "UtilityRunClockSideOfFringeFrequencyDifference", 0, 100000, "Half width half maximum", "MHz", "Half width half maximum of spectroscopy line of one mF state");
			UtilityDialog->RegisterDouble(&UtilityRunClockIntegratorConstant, "UtilityRunClockIntegratorConstant", -100000, 100000, "Integrator Constant", "");
			UtilityDialog->AddButton(IDM_RUN_CLOCK, Sequence);
			UtilityDialog->AddStatic("");
		}
		return true;
	}
