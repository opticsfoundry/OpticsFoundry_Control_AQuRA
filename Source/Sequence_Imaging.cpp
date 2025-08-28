
#include "stdafx.h"
#include "sequence.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif





bool VisionTriggered = false;
unsigned long TriggerVisionTime = 0;

bool CSequence::TriggerVision(bool LastChance) {
	double TriggerDelay = LastExperimentalRunTime - ((DoTakeFKSAbsorptionPicture) ? CameraSoftPreTriggerTimeFKS : CameraSoftPreTriggerTime);
	if (VisionTriggered) {
		//if ((LastChance) && (TriggerDelay<0)) {
		//	if ((GetTickCount()-TriggerVisionTime)<(-TriggerDelay)) {
		//		//ControlMessageBox("CSequence::TriggerVision : Vision trigger delay 1");
		//		//Wait((-TriggerDelay)-(GetTickCount()-TriggerVisionTime));
		//	}
		//} 
		return true;
	}
	else {
		if ((DoTakeAbsorptionPicture || DoTakeFKSAbsorptionPicture || DoTakeFluorescencePicture)) {
			/*double help=TriggerDelay;*/
			if (TriggerDelay<0) TriggerDelay=0;
			if (!Vision->TakeAbsorptionPicture(TriggerDelay)) {
				ControlMessageBox("CSequence::DoExperimentalSequence : Vision not ready");
				return false;
			}
			TriggerVisionTime = GetTickCount();
			VisionTriggered = true;
			//			if ((LastChance) && (help<0)) {
							//ControlMessageBox("CSequence::TriggerVision : Vision trigger delay 2");
						//	Wait(-help);
			//			}
		}
	}
	return true;
}


void CSequence::SwitchAllCameraTriggers(bool OnOff) {
	//if (CameraUsed[0]) 
	SwitchCameraTrigger0(OnOff);
	//if (CameraUsed[1]) 
	SwitchCameraTrigger1(OnOff);
}

int CSequence::GetMaxNumberPictures() {
	int MaxNumberPictures = 0;
	for (int i = 0; i < NrCameras; i++) if ((CameraUsed[i]) && (CameraNumberPictures[i] > MaxNumberPictures)) MaxNumberPictures = CameraNumberPictures[i];
	return MaxNumberPictures;
}

void CSequence::SwitchAllCameraShutters(bool OnOff) {
	//if (CameraUsed[0]) SwitchCameraShutterAndor0(OnOff);
}



void CSequence::SwitchAQuRAImagingAOMHeatingOn() {
	//SetFrequencyAQuRAImgAOM1(AQuRAImgAOM1HeatingFrequency);
}



void CSequence::SwitchImagingShutters(bool OnOff, bool SrFlashUsed, bool RbFlashUsed) {
	if (OnOff) {
		if (ImagingShuttersOpen) return;
		double Start = GetTime();
		GoBackInTime(100, 1516);			//changed by Simon Feb 2012
		//waveplate commands here
//      Wait(50); //Shutters take 100ms, Waveplates take 150ms
		if (SrFlashUsed) {



		}
		//if (RbFlashUsed) {

		//}
		ImagingShuttersOpen = true;
		GoToTime(Start, 2346);
		//GoBackInTime(150,1516);

		//GoToTime(Start,3215);	
	}
	else {
		if (!ImagingShuttersOpen) return;
		double Start = GetTime();
		if (SrFlashUsed) {

		}
		//if (RbFlashUsed) {

		//}
		ImagingShuttersOpen = false;

		//move imaging waveplates here	

		Wait(ShutterOffDelay, 4914);
		//SwitchImagingAOMHeatingOn();
		SwitchAQuRAImagingAOMHeatingOn();
		GoToTime(Start, 4584);
	}
}

void CSequence::AbsorptionPictureSrFlash(bool FKS, double& MaxEndOfFlashTime, long FlashTypeNr, int ImagingUsed, bool Probe) {
	//ImagingUsed is only a security option to inhibit the user from accidentally activating the dipole trap flash
	// ImagingUsed = 0: normal imaging
	// ImagingUsed = 1: dipole trap flash (or future option)	
	/*bool SrImaging1DPAOMSetting[2]={On,Off};
	bool SrImaging2DPAOMSetting[2]={On,Off};
	bool SrBlueDipoleTrapAOMSetting[2]={Off,On};	*/
	//no servo shutter commands allowed in this routine since it can be called several times in rapid sequence
	//the shutters have to be prepared in advance in AbsorptionPictureFlashSequence
	double Start = GetTime();
	GoBackInTime(0.1, 3445);
	//if (ImagingFlashUseAQuRAImagingAOM1[FlashTypeNr]) SetFrequencyAQuRAImgAOM1(AQuRAImagingAOM1FrequencyOfFlash[FlashTypeNr]);
	//if (ImagingFlashUseAQuRAImagingAOM3[FlashTypeNr]) SetFrequencyAQuRAImgAOM3(AQuRAImagingAOM3FrequencyOfFlash[FlashTypeNr]);
	//if (ImagingFlashUseAQuRAImagingAOM4[FlashTypeNr]) SetFrequencyAQuRAImgAOM4(AQuRAImagingAOM4FrequencyOfFlash[FlashTypeNr]);

	GoToTime(Start);
	//create well defined initial conditions
	//SetIntensityAQuRAImgAOM1(0);
	//SetIntensityAQuRAImgAOM3(0);
	//SetIntensityAQuRAImgAOM4(0);

	//SetIntensitySrImaging1DPAOM(0);
	//SetIntensitySrImaging2DPAOM(0);
	//SetIntensitySrBlueZSSPAOM(0);
	//SetIntensityTransCoolDPAOM(0);
	//SetIntensitySrBlueMOTSPAOM(0);
	//these two 10 us waits are very important to assure that the flash duration does not
	//depend on what happened before or what will happen afterwards on the bus. If not we can
	//not assure that probe and reference pulse are of equal length.
	Wait(0.01); // important 10us
	if (ImagingUsed == 0) {
		double IntMult;
		if (FKS) {
			IntMult = (Probe) ? 1 : SrAbsImageFlashRelativeIntensityReferenceFKS[FlashTypeNr];
		}
		else {
			IntMult = (Probe) ? 1 : SrAbsImageFlashRelativeIntensityReferenceNormal[FlashTypeNr];
		}
		//if (ImagingFlashUseAQuRAImagingAOM1[FlashTypeNr]) SetIntensityAQuRAImgAOM1(IntMult*AQuRAImagingAOM1IntensityOfFlash[FlashTypeNr]);
		//if (ImagingFlashUseAQuRAImagingAOM3[FlashTypeNr]) SetIntensityAQuRAImgAOM3(IntMult*AQuRAImagingAOM3IntensityOfFlash[FlashTypeNr]);
		//if (ImagingFlashUseAQuRAImagingAOM4[FlashTypeNr]) SetIntensityAQuRAImgAOM4(IntMult*AQuRAImagingAOM4IntensityOfFlash[FlashTypeNr]);

	}
	//if (ImagingFlashUseDipTrapAOM[FlashTypeNr] && (ImagingUsed==1)) SetAttenuation100WDipoleDDS0(-39);
	// test flash sequence timing: dig 0 first flash on, second off
	Wait(SrAbsImageFlashDurationOfFlash[FlashTypeNr], 1840);
	//if (ImagingFlashUseAQuRAImagingAOM1[FlashTypeNr]) SetIntensityAQuRAImgAOM1(0);
	//if (ImagingFlashUseAQuRAImagingAOM3[FlashTypeNr]) SetIntensityAQuRAImgAOM3(0);
	//if (ImagingFlashUseAQuRAImagingAOM4[FlashTypeNr]) SetIntensityAQuRAImgAOM4(0);

	Wait(0.01);  // important 10us
	if (GetTime() > MaxEndOfFlashTime) MaxEndOfFlashTime = GetTime();
}

void CSequence::AbsorptionPictureRbFlash(bool FKS, double& MaxEndOfFlashTime, long FlashTypeNr, int ImagingUsed, bool Probe) {
	double Start = GetTime();
	GoBackInTime(0.1, 3445);

	GoToTime(Start);

	//these two 10 s waits are very important to assure that the flash duration does not
	//depend on what happened before or what will happen afterwards on the bus. If not we can
	//not assure that probe and reference pulse are of equal length.
	Wait(0.01);
	double IntMult;
	if (FKS) IntMult = (Probe) ? 1 : RbAbsImageFlashRelativeIntensityReferenceFKS[FlashTypeNr];
	IntMult = (Probe) ? 1 : RbAbsImageFlashRelativeIntensityReferenceNormal[FlashTypeNr];
	//SetIntensityRbImage1DPAOMA4(IntMult*RbImage1DPAOMA4IntensityOfFlash[FlashTypeNr]);
	//SetIntensityRbImage2SPAOMA5(IntMult*RbImage2SPAOMA5IntensityOfFlash[FlashTypeNr]);
	//SetIntensityRbRepumpDPAOMB2(IntMult*RbRepumpDPAOMB2IntensityOfFlash[FlashTypeNr]);
	// test flash sequence timing: dig 0 first flash on, second off
	Wait(RbAbsImageFlashDurationOfFlash[FlashTypeNr], 1841);
	//SetIntensityRbImage1DPAOMA4(0);
	//SetIntensityRbImage2SPAOMA5(0);
	//SetIntensityRbRepumpDPAOMB2(0);

	//SetIntensityRbRepZeeDPAOMB3(RbRepZeeDPAOMB3Intensity);
	//SetIntensityRbZSDPAOMA2(RbZSDPAOMA2Intensity);
	//SetIntensityRbMOTDPAOMA3(RbMOTDPAOMA3Intensity);
	Wait(0.01);
	if (GetTime() > MaxEndOfFlashTime) MaxEndOfFlashTime = GetTime();
}

void CSequence::AbsorptionPictureFlashSequence(bool FKS, bool FirstFlash, bool SecondFlash, double& Duration, double& MaxEndOfFirstFlashTime, double TimeToNextFlash, bool Probe) {

	//	SetIntensityAQuRAZCaptureAOM(100);
	//	SwitchAQuRAZCaptureShutter(On);
	//	Wait(2000/*-1*/,4675);


	int NrSubPictures = 0;
	int i = 0;
	while ((i < NrPicturesMax) && (ElementOfPicture[i] != 0)) {
		NrSubPictures++;
		i++;
	}
	/*if (!FKS) {
		FirstFlash=true;
		SecondFlash=false;
		NrSubPictures=1;
	}*/
	bool SrFlashUsed = false;
	bool RbFlashUsed = false;
	for (int i = 0; i < NrSubPictures; i++) {
		switch (ElementOfPicture[i]) {
		case 1: //Strontium
			SrFlashUsed = true;
			break;
		case 2: //Rubidium
			RbFlashUsed = true;
			break;
		default:;//nothing;
		}
	}
	//All Servo motor shutters have to be prepared here.
	//These commands serve only to open shutters in advance, the AOMs are not really activated

	if ((AbsPicturePrepareShutters) && ((SrFlashUsed) || (RbFlashUsed)))
	{
		SwitchImagingShutters(On, SrFlashUsed, RbFlashUsed);
	}

	//if ((AbsPicturePrepareShutters) && (RbFlashUsed)){
	//	double Start=GetTime();
	//	GoBackInTime(200,1517);		
	//	//SwitchCameraRb1VertShutter(On); // must open only once, they were shutters right in front of the cameras
	//	//SwitchCameraRb2HorShutter(On);
	//	GoToTime(Start,1517);
	//}


	double Start = GetTime();
	double MaxEndTime = Start;
	//Calculate minimum used expansion time
	double MinExpansionTime = 10000;
	MaxEndOfFirstFlashTime = 0;
	for (int i = 0; i < NrSubPictures; i++) MinExpansionTime = Min(ExpansionTimeOfPicture[i], MinExpansionTime);


	//open camera shutter 
	Wait(MinExpansionTime, 1520);

	GoBackInTime(CameraShutterPreTrigger, 5030);
	SwitchAllCameraShutters(On);
	Wait(CameraShutterPreTrigger, 1530);
	MaxEndTime = Max(MaxEndTime, GetTime());
	GoToTime(Start);
	//Trigger cameras
	if (FKS) {
		if (CameraTriggerPreTriggerFKS < 0.01) CameraTriggerPreTriggerFKS = 0.01;
		Wait(MinExpansionTime, 1540);
		GoBackInTime(CameraTriggerPreTriggerFKS, 5040);
		SwitchAllCameraTriggers(On);

		Wait(2, 1550);
		SwitchAllCameraTriggers(Off);
		Wait(CameraTriggerPreTriggerFKS - 10, 1560);
		MaxEndTime = Max(MaxEndTime, GetTime());
	}
	else {
		if (CameraTriggerPreTrigger < 0.1) CameraTriggerPreTrigger = 0.1;
		Wait(MinExpansionTime, 1570);
		GoBackInTime(CameraTriggerPreTrigger, 5050);
		SwitchAllCameraTriggers(On);
		Wait(2, 1580); // CameraTriggerPulseWidth
		SwitchAllCameraTriggers(Off);
		Wait(CameraTriggerPreTrigger - 10, 1590);
		MaxEndTime = Max(MaxEndTime, GetTime());
	}
	for (int i = 0; i < NrSubPictures; i++) {
		GoToTime(Start);
		Wait(ExpansionTimeOfPicture[i], 1600);
		if (FirstFlash) {
			switch (ElementOfPicture[i]) {
			case 1: AbsorptionPictureSrFlash(FKS, MaxEndOfFirstFlashTime, FlashTypeOfPicture[i], ImagingSystemConfigurationUsed, (FKS) ? true : Probe); break;
				//case 2: AbsorptionPictureRbFlash(FKS,MaxEndOfFirstFlashTime,FlashTypeOfPicture[i],ImagingSystemConfigurationUsed,(FKS) ? true : Probe); break;
			default:;
			}
		}
		//After last picture with atoms let atoms fly away for reference image without atoms
		if (i == (NrSubPictures - 1)) {
			GoToTime(MaxEndOfFirstFlashTime);
			SwitchDipoleTrapOff(AbsPictureLeaveLeaveDipoleTrapFreq);
		}

		if (SecondFlash) {  //can only be true for FKS images
			GoToTime(Start);

			//			CString buf;


						//if ((InitialExpansionTime+ExpansionTimeOfPicture[i]+CameraExposureTimeFKS+FKSSecondFlashWait)>FKSExpansionTime) {
						//	Wait(InitialExpansionTime+ExpansionTimeOfPicture[i]+CameraExposureTimeFKS+FKSSecondFlashWait,1620);
			if ((InitialExpansionTime + ExpansionTimeOfPicture[i] + CameraExposureTimeFKS + FKSSecondFlashWait) > FKSExpansionTime) {
				Wait(CameraExposureTimeFKS + ExpansionTimeOfPicture[i] + FKSSecondFlashWait, 1620);
				//			buf.Format("Exp Time1: %f %f",CameraExposureTimeFKS+ExpansionTimeOfPicture[i]+FKSSecondFlashWait,ExpansionTimeOfPicture[i]);
			}
			else {
				Wait(FKSExpansionTime + ExpansionTimeOfPicture[i] - InitialExpansionTime, 1620);
				//			buf.Format("Exp Time2: %f %f",FKSExpansionTime+ExpansionTimeOfPicture[i]-InitialExpansionTime,ExpansionTimeOfPicture[i]);
			}

			double MaxEndOfSecondFlashTime;
			switch (ElementOfPicture[i]) {
			case 1: AbsorptionPictureSrFlash(FKS, MaxEndOfSecondFlashTime, FlashTypeOfPicture[i], ImagingSystemConfigurationUsed, false); break;
				//case 2: AbsorptionPictureRbFlash(FKS,MaxEndOfSecondFlashTime,FlashTypeOfPicture[i],ImagingSystemConfigurationUsed,false); break;
			default:;
			}
		}
		MaxEndTime = Max(MaxEndTime, GetTime());
	}
	GoToTime(MaxEndTime);
	SwitchAllCameraShutters(Off);
	Duration = MaxEndTime - Start;
	Start = GetTime();
	Wait(100, 5555);
	//Now shutters have to close again
	if (((RbFlashUsed) || (SrFlashUsed)) && (TimeToNextFlash > (ShutterOnPreTrigger + ShutterOffDelay))) SwitchImagingShutters(Off, SrFlashUsed, RbFlashUsed);
	Wait(100, 5555);
	//Put frequencies of AOMs back to MOT conditions
	GoToTime(Start);
}





// FlashAQuRAImagingProbe (for aligning beams on the MOT or DT)
const int NrFlashAQuRAImagingProbe = 10;
bool DoFlashAQuRAImagingProbe[NrFlashAQuRAImagingProbe];
double FlashAQuRAImagingProbeWait[NrFlashAQuRAImagingProbe];
double FlashAQuRAImagingProbeDuration[NrFlashAQuRAImagingProbe];
double FlashAQuRAImagingProbeAOMFrequency[NrFlashAQuRAImagingProbe];
double FlashAQuRAImagingProbeAOMIntensity[NrFlashAQuRAImagingProbe];
bool FlashAQuRAImagingProbeOpenShutter[NrFlashAQuRAImagingProbe];
bool FlashAQuRAImagingProbeCloseShutter[NrFlashAQuRAImagingProbe];
void CSequence::FlashAQuRAImagingProbe(int Nr, int imgnum, bool openshutter, bool closeshutter) {
	if (!AssemblingParamList()) {
		if (!Decision("DoFlashAQuRAImagingProbe" + itos(Nr))) return;

		double Start = GetTime();
		if (openshutter) {
			if (FlashAQuRAImagingProbeOpenShutter[Nr]) {
				GoBackInTime(100);
				switch (imgnum) {
				case 1:
					//SetIntensityAQuRAImgAOM1(0);
					//SetFrequencyAQuRAImgAOM1(FlashAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg1Shutter(On);
					break;
				case 2:
					//SetIntensityAQuRAImgAOM1(0);
					//SetFrequencyAQuRAImgAOM1(FlashAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg2Shutter(On);
					break;
				case 3:
					//SetIntensityAQuRAImgAOM3(0);
					//SetFrequencyAQuRAImgAOM3(FlashAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg3Shutter(On);
					break;
				case 4:
					//SetIntensityAQuRAImgAOM4(0);
					//SetFrequencyAQuRAImgAOM4(FlashAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg4Shutter(On);
					break;
				}
				GoToTime(Start);
			}
		}
		switch (imgnum) {
		case 1:
			//SetFrequencyAQuRAImgAOM1(FlashAQuRAImagingProbeAOMFrequency[Nr]);
			//SetIntensityAQuRAImgAOM1(FlashAQuRAImagingProbeAOMIntensity[Nr]); 
			break;
		case 2:
			//SetFrequencyAQuRAImgAOM1(FlashAQuRAImagingProbeAOMFrequency[Nr]);
			//SetIntensityAQuRAImgAOM1(FlashAQuRAImagingProbeAOMIntensity[Nr]); 
			break;
		case 3:
			//SetFrequencyAQuRAImgAOM3(FlashAQuRAImagingProbeAOMFrequency[Nr]);
			//SetIntensityAQuRAImgAOM3(FlashAQuRAImagingProbeAOMIntensity[Nr]); 
			break;
		case 4:
			//SetFrequencyAQuRAImgAOM4(FlashAQuRAImagingProbeAOMFrequency[Nr]);
			//SetIntensityAQuRAImgAOM4(FlashAQuRAImagingProbeAOMIntensity[Nr]);
			break;
		}

		Wait(FlashAQuRAImagingProbeDuration[Nr], 3457);
		switch (imgnum) {
		case 1:
			//SetIntensityAQuRAImgAOM1(0);
			break;
		case 2:
			//SetIntensityAQuRAImgAOM1(0); 
			break;
		case 3:
			//SetIntensityAQuRAImgAOM3(0); 
			break;
		case 4:
			//SetIntensityAQuRAImgAOM4(0);
			break;
		}

		double Start2 = GetTime();

		if (closeshutter) {
			if (FlashAQuRAImagingProbeCloseShutter[Nr]) {
				switch (imgnum) {
				case 1:
					//SwitchAQuRAImg1Shutter(Off); break;
				case 2:
					//SwitchAQuRAImg2Shutter(Off); break;
				case 3:
					//SwitchAQuRAImg3Shutter(Off); break;
				case 4:
					//SwitchAQuRAImg4Shutter(Off); break;
					break;
				}
				Wait(100);
				switch (imgnum) {
				case 1:
					//SetFrequencyAQuRAImgAOM1(AQuRAImgAOM1HeatingFrequency);
					//SetIntensityAQuRAImgAOM1(AQuRAImgAOM1HeatingIntensity); 
					break;
				case 2:
					//SetFrequencyAQuRAImgAOM1(AQuRAImgAOM1HeatingFrequency);
					//SetIntensityAQuRAImgAOM1(AQuRAImgAOM1HeatingIntensity); 
					break;
				case 3:
					//SetFrequencyAQuRAImgAOM3(AQuRAImgAOM3HeatingFrequency);
					//SetIntensityAQuRAImgAOM3(AQuRAImgAOM3HeatingIntensity); 
					break;
				case 4:
					//SetFrequencyAQuRAImgAOM4(AQuRAImgAOM4HeatingFrequency);
					//SetIntensityAQuRAImgAOM4(AQuRAImgAOM4HeatingIntensity);
					break;
				}
				GoToTime(Start2);
			}
		}
		Wait(FlashAQuRAImagingProbeWait[Nr], 3457);
	}
	else {
		if (Nr >= NrFlashAQuRAImagingProbe) { ControlMessageBox("CSequence::FlashAQuRAImagingProbe : too many code blocks of this type."); return; }
		ParamList->RegisterBool(&DoFlashAQuRAImagingProbe[Nr], "DoFlashAQuRAImagingProbe" + itos(Nr), "Flash Imaging" + itos(imgnum) + " " + itos(Nr) + " ?", "FI" + itos(Nr));
		if (openshutter) {
			ParamList->RegisterBool(&FlashAQuRAImagingProbeOpenShutter[Nr], "FlashAQuRAImagingProbeOpenShutter" + itos(Nr), "Open Shutter?");
		}
		switch (imgnum) {
		case 1:
			ParamList->RegisterDouble(&FlashAQuRAImagingProbeAOMFrequency[Nr], "FlashAQuRAImagingProbeAOMFrequency" + itos(Nr), 190, 220, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz"); break;
		case 2:
			ParamList->RegisterDouble(&FlashAQuRAImagingProbeAOMFrequency[Nr], "FlashAQuRAImagingProbeAOMFrequency" + itos(Nr), 190, 220, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz"); break;
		case 3:
			ParamList->RegisterDouble(&FlashAQuRAImagingProbeAOMFrequency[Nr], "FlashAQuRAImagingProbeAOMFrequency" + itos(Nr), 70, 90, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz"); break;
		case 4:
			ParamList->RegisterDouble(&FlashAQuRAImagingProbeAOMFrequency[Nr], "FlashAQuRAImagingProbeAOMFrequency" + itos(Nr), 190, 220, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz");
		}

		ParamList->RegisterDouble(&FlashAQuRAImagingProbeAOMIntensity[Nr], "FlashAQuRAImagingProbeAOMIntensity" + itos(Nr), 0, 100, "Intensity Imaging" + itos(imgnum) + " AOM ", "0..100%");
		ParamList->RegisterDouble(&FlashAQuRAImagingProbeDuration[Nr], "FlashAQuRAImagingProbeDuration" + itos(Nr), 0, 10000, "Duration", "ms");
		if (closeshutter) {
			ParamList->RegisterBool(&FlashAQuRAImagingProbeCloseShutter[Nr], "FlashAQuRAImagingProbeCloseShutter" + itos(Nr), "Close Shutter?");
		}
		ParamList->RegisterDouble(&FlashAQuRAImagingProbeWait[Nr], "FlashAQuRAImagingProbeWait" + itos(Nr), 0, 10000, "Wait", "ms");
	}
}


// SwitchAQuRAImagingProbe (for aligning beams on the MOT or DT)
const int NrSwitchAQuRAImagingProbe = 10;
bool DoSwitchAQuRAImagingProbe[NrSwitchAQuRAImagingProbe];
double SwitchAQuRAImagingProbeWait[NrSwitchAQuRAImagingProbe];
//double SwitchAQuRAImagingProbeDuration[NrSwitchAQuRAImagingProbe];
double SwitchAQuRAImagingProbeAOMFrequency[NrSwitchAQuRAImagingProbe];
double SwitchAQuRAImagingProbeAOMIntensity[NrSwitchAQuRAImagingProbe];
bool SwitchAQuRAImagingProbeOpenShutter[NrSwitchAQuRAImagingProbe];
bool SwitchAQuRAImagingProbeCloseShutter[NrSwitchAQuRAImagingProbe];
void CSequence::SwitchAQuRAImagingProbe(int Nr, int imgnum, bool openshutter, bool closeshutter) {
	if (!AssemblingParamList()) {
		if (!Decision("DoSwitchAQuRAImagingProbe" + itos(Nr))) return;

		double Start = GetTime();
		if (openshutter) {
			if (SwitchAQuRAImagingProbeOpenShutter[Nr]) {
				GoBackInTime(100);
				switch (imgnum) {
				case 1:
					//SetIntensityAQuRAImgAOM1(0);
					//SetFrequencyAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg1Shutter(On);
					break;
				case 2:
					//SetIntensityAQuRAImgAOM1(0);
					//SetFrequencyAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg2Shutter(On);
					break;
				case 3:
					//SetIntensityAQuRAImgAOM3(0);
					//SetFrequencyAQuRAImgAOM3(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg3Shutter(On);
					break;
				case 4:
					//SetIntensityAQuRAImgAOM4(0);
					//SetFrequencyAQuRAImgAOM4(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					//SwitchAQuRAImg4Shutter(On);
					break;
				}

				GoToTime(Start);
			}
			else {
				switch (imgnum) {
				case 1:
					//SetFrequencyAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					break;
				case 2:
					//SetFrequencyAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					break;
				case 3:
					//SetFrequencyAQuRAImgAOM3(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					break;
				case 4:
					//SetFrequencyAQuRAImgAOM4(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
					break;
				}
			}
		}
		else {
			switch (imgnum) {
			case 1:
				//SetFrequencyAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
				break;
			case 2:
				//SetFrequencyAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
				break;
			case 3:
				//SetFrequencyAQuRAImgAOM3(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
				break;
			case 4:
				//SetFrequencyAQuRAImgAOM4(SwitchAQuRAImagingProbeAOMFrequency[Nr]);
				break;
			}
		}
		switch (imgnum) {
		case 1:
			//SetIntensityAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMIntensity[Nr]); 
			break;
		case 2:
			//SetIntensityAQuRAImgAOM1(SwitchAQuRAImagingProbeAOMIntensity[Nr]); 
			break;
		case 3:
			//SetIntensityAQuRAImgAOM3(SwitchAQuRAImagingProbeAOMIntensity[Nr]); 
			break;
		case 4:
			//SetIntensityAQuRAImgAOM4(SwitchAQuRAImagingProbeAOMIntensity[Nr]);
			break;
		}

		//Wait(SwitchAQuRAImagingProbeDuration[Nr],3457);
		Wait(1);
		//switch(imgnum) {
		//	case 1:
		//		SetIntensityAQuRAImgAOM1(0); break;
		//	case 2:
		//		SetIntensityAQuRAImgAOM1(0); break;
		//	case 3:
		//		SetIntensityAQuRAImgAOM3(0); break;
		//	case 4:
		//		SetIntensityAQuRAImgAOM4(0); break;
		//}

		double Start2 = GetTime();

		if (closeshutter) {
			if (SwitchAQuRAImagingProbeCloseShutter[Nr]) {
				switch (imgnum) {
				case 1:
					//SwitchAQuRAImg1Shutter(Off); break;
				case 2:
					//SwitchAQuRAImg2Shutter(Off); break;
				case 3:
					//SwitchAQuRAImg3Shutter(Off); break;
				case 4:
					//SwitchAQuRAImg4Shutter(Off); break;
					break;
				}
				Wait(100);
				switch (imgnum) {
				case 1:
					//SetFrequencyAQuRAImgAOM1(AQuRAImgAOM1HeatingFrequency);
					//SetIntensityAQuRAImgAOM1(AQuRAImgAOM1HeatingIntensity); 
					break;
				case 2:
					//SetFrequencyAQuRAImgAOM1(AQuRAImgAOM1HeatingFrequency);
					//SetIntensityAQuRAImgAOM1(AQuRAImgAOM1HeatingIntensity);
					break;
				case 3:
					//SetFrequencyAQuRAImgAOM3(AQuRAImgAOM3HeatingFrequency);
					//SetIntensityAQuRAImgAOM3(AQuRAImgAOM3HeatingIntensity); 
					break;
				case 4:
					//SetFrequencyAQuRAImgAOM4(AQuRAImgAOM4HeatingFrequency);
					//SetIntensityAQuRAImgAOM4(AQuRAImgAOM4HeatingIntensity); 
					break;
				}
				GoToTime(Start2);
			}
		}
		Wait(SwitchAQuRAImagingProbeWait[Nr]);
	}
	else {
		if (Nr >= NrSwitchAQuRAImagingProbe) { ControlMessageBox("CSequence::SwitchAQuRAImagingProbe : too many code blocks of this type."); return; }
		ParamList->RegisterBool(&DoSwitchAQuRAImagingProbe[Nr], "DoSwitchAQuRAImagingProbe" + itos(Nr), "Switch Imaging" + itos(imgnum) + " " + itos(Nr) + " ?", "FI" + itos(Nr));
		if (openshutter) {
			ParamList->RegisterBool(&SwitchAQuRAImagingProbeOpenShutter[Nr], "SwitchAQuRAImagingProbeOpenShutter" + itos(Nr), "Open Shutter?");
		}
		switch (imgnum) {
		case 1:
			ParamList->RegisterDouble(&SwitchAQuRAImagingProbeAOMFrequency[Nr], "SwitchAQuRAImagingProbeAOMFrequency" + itos(Nr), 190, 220, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz"); break;
		case 2:
			ParamList->RegisterDouble(&SwitchAQuRAImagingProbeAOMFrequency[Nr], "SwitchAQuRAImagingProbeAOMFrequency" + itos(Nr), 190, 220, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz"); break;
		case 3:
			ParamList->RegisterDouble(&SwitchAQuRAImagingProbeAOMFrequency[Nr], "SwitchAQuRAImagingProbeAOMFrequency" + itos(Nr), 70, 90, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz"); break;
		case 4:
			ParamList->RegisterDouble(&SwitchAQuRAImagingProbeAOMFrequency[Nr], "SwitchAQuRAImagingProbeAOMFrequency" + itos(Nr), 190, 230, "Frequency Imaging" + itos(imgnum) + " AOM ", "MHz");
		}

		ParamList->RegisterDouble(&SwitchAQuRAImagingProbeAOMIntensity[Nr], "SwitchAQuRAImagingProbeAOMIntensity" + itos(Nr), 0, 100, "Intensity Imaging" + itos(imgnum) + " AOM ", "0..100%");
		//ParamList->RegisterDouble(&SwitchAQuRAImagingProbeDuration[Nr],"SwitchAQuRAImagingProbeDuration"+itos(Nr),0,10000,"Duration","ms");
		if (closeshutter) {
			ParamList->RegisterBool(&SwitchAQuRAImagingProbeCloseShutter[Nr], "SwitchAQuRAImagingProbeCloseShutter" + itos(Nr), "Close Shutter?");
		}
		ParamList->RegisterDouble(&SwitchAQuRAImagingProbeWait[Nr], "SwitchAQuRAImagingProbeWait" + itos(Nr), 0, 10000, "Wait", "ms");
	}
}




void CSequence::SwitchTrapOff(bool LeaveFieldsOn, bool LeaveDipoleTrapOn, bool AbsPictureLeaveTranspOn, bool AbsPictureLeaveRepumpTranspOn, bool AbsPictureLeaveLeaveDipoleTrapFreq)
{
	switch (AktTrap) {
	case IDT_MOT:
	case IDT_CMOT:
	case IDT_OPTICAL_TRAP:
	case IDT_RECAPTURE_MOT:
	case IDT_NO_TRAP:
		break;
	default:;
	}
	if (!AbsPictureLeaveTranspOn) {

	}


	if (!AbsPictureLeaveRepumpTranspOn) {

	}
	if (!LeaveDipoleTrapOn) SwitchDipoleTrapOff(AbsPictureLeaveLeaveDipoleTrapFreq);
	if (!LeaveFieldsOn) {



		SetSpareAnalogOut0(0);
		SetSpareAnalogOut1(0);
		SetSpareAnalogOut2(0);
		SetSpareAnalogOut3(0);
		AnalogOut("SetSpareAnalogOut0", 1);
		AnalogOut("SetSpareAnalogOut1", 1);



	}
}


void CSequence::SwitchMOTOffForImaging(bool LeaveAQuRABlueMOT, bool LeaveAQuRARed2DMOT, bool LeaveAQuRARed3DMOT, bool LeaveAQuRABlueMOTRepump, bool AbsPictureLeaveZITCTranspOn, bool AbsPictureLeaveSOLDOn)
{
	switch (AktTrap) {
	case IDT_MOT:
	case IDT_CMOT:
	case IDT_OPTICAL_TRAP:
	case IDT_RECAPTURE_MOT:







		break;
	case IDT_NO_TRAP:
		break;
	default:
		ControlMessageBox("CSequence::SwitchSrBlueMOTOff : unknown trap type");
	}
}

void CSequence::SwitchDipoleTrapOff(bool AbsPictureLeaveLeaveDipoleTrapFreq) {
	IOList->StoreAnalogOutValue("SetLatticePowerSetpoint");
	IOList->AnalogOut("SetLatticePowerSetpoint", DipoleTrapOffPowerSetpoint);
}


void CSequence::RampTrapOff(bool LeaveFieldsOn, bool LeaveDipoleTrapOn) {

}

bool CSequence::TakeAbsorptionPicture() {
	if (!Decision("DoTakeAbsorptionPicture")) return false;


	PrePictureTrigger(/*Abs*/ true, /*FKS*/ false);
	SwitchTrapOff(/*LeaveFieldsOn*/AbsPictureLeaveFieldsOn,/*LeaveDipoleTrapOn*/AbsPictureLeaveLeaveDipoleTrapOn, AbsPictureLeaveTranspOn, AbsPictureLeaveRepumpTranspOn, AbsPictureLeaveLeaveDipoleTrapFreq);
	if (!AbsPictureLeaveMOTOn) SwitchMOTOffForImaging(AbsPictureLeaveAQuRABlueMOT, AbsPictureLeaveAQuRARed2DMOT, AbsPictureLeaveAQuRARed3DMOT, AbsPictureLeaveAQuRABlueMOTRepump, AbsPictureLeaveZITCTranspOn, AbsPictureLeaveSOLDOn);
	double Start1 = GetTime();

	if (AbsPictureSwitchOffFieldJustBeforeImaging) {
		Wait(InitialExpansionTime - FieldSwitchingTimeBeforeImaging);
		SwitchTrapOff(/*LeaveFieldsOn*/false,/*LeaveDipoleTrapOn*/AbsPictureLeaveLeaveDipoleTrapOn, AbsPictureLeaveTranspOn, AbsPictureLeaveRepumpTranspOn, AbsPictureLeaveLeaveDipoleTrapFreq);
		GoToTime(Start1);
	}
	if (AbsPictureRbRepumpPulseJustBeforeImaging) {
		Wait(InitialExpansionTime - RbRepumpTimeBeforeImaging);

		GoToTime(Start1);
	}

	double ExpantionTimeStart = GetTime();

	if (AQuRADoBlowAway1) {
		int BANr = 0;

		Wait(AQuRABlowAwayFlashStartTime[BANr]);

		double BlowAwayStart = GetTime();

		// Open all the necessary shutters
		GoBackInTime(100);

		// turn on all the blow away beams

		GoToTime(BlowAwayStart);
		if (AQuRABlowAwayTranspAOM1FlashPreTime[BANr] > 0) GoBackInTime(AQuRABlowAwayTranspAOM1FlashPreTime[BANr]);
		//SetFrequencyAQuRATransparencyBeamAOM1(AQuRABlowAwayTranspAOM1FlashFrequency[BANr]);
		//SetIntensityAQuRATransparencyBeamAOM1(AQuRABlowAwayTranspAOM1FlashIntensity[BANr]);

		GoToTime(BlowAwayStart);
		if (AQuRABlowAwayTranspHorizAOM1FlashPreTime[BANr] > 0) GoBackInTime(AQuRABlowAwayTranspHorizAOM1FlashPreTime[BANr]);
		//SetFrequencyAQuRAHorizTransparencyAOM1(AQuRABlowAwayTranspHorizAOM1FlashFrequency[BANr]);
		//SetIntensityAQuRAHorizTransparencyAOM1(AQuRABlowAwayTranspHorizAOM1FlashIntensity[BANr]);


		GoToTime(BlowAwayStart);


		// wait the blow away duration

		Wait(AQuRABlowAwayFlashDuration[BANr]);

		// turn off all the blow away beams and close the shutters

		double BlowAwayStop = GetTime();

		if (AQuRABlowAwayTranspAOM1FlashPostTime[BANr] > 0) Wait(AQuRABlowAwayTranspAOM1FlashPostTime[BANr]);
		//SetFrequencyAQuRATransparencyBeamAOM1(AQuRABlowAwayTranspAOM1PostFlashFrequency[BANr]);
		//SetIntensityAQuRATransparencyBeamAOM1(AQuRABlowAwayTranspAOM1PostFlashIntensity[BANr]);

		GoToTime(BlowAwayStop);
		if (AQuRABlowAwayTranspHorizAOM1FlashPostTime[BANr] > 0) Wait(AQuRABlowAwayTranspHorizAOM1FlashPostTime[BANr]);
		//SetFrequencyAQuRAHorizTransparencyAOM1(AQuRABlowAwayTranspHorizAOM1PostFlashFrequency[BANr]);
		//SetIntensityAQuRAHorizTransparencyAOM1(AQuRABlowAwayTranspHorizAOM1PostFlashIntensity[BANr]);

		GoToTime(BlowAwayStop);
		if (AQuRABlowAway679AOM1FlashPostTime[BANr] > 0) Wait(AQuRABlowAway679AOM1FlashPostTime[BANr]);

		GoToTime(BlowAwayStop);
		if (AQuRABlowAway707AOM1FlashPostTime[BANr] > 0) Wait(AQuRABlowAway707AOM1FlashPostTime[BANr]);

		// reset to the heating parameters for beams for which shutters are now closed

		GoToTime(BlowAwayStop);
		Wait(100 + max(max(AQuRABlowAway707AOM1FlashPostTime[BANr], AQuRABlowAway679AOM1FlashPostTime[BANr]), max(AQuRABlowAwayTranspHorizAOM1FlashPostTime[BANr], AQuRABlowAwayTranspAOM1FlashPostTime[BANr])));

		if (AQuRABlowAwayMOTYCloseShutter[BANr]) {

		}

		if (AQuRABlowAwayTransparency1CloseShutter[BANr]) {
			//SetFrequencyAQuRATransparencyBeamAOM1(AQuRATransparencyAOM1Frequency);
			//SetIntensityAQuRATransparencyBeamAOM1(AQuRATransparencyAOM1Intensity);
		}
		if ((AQuRABlowAwayTransparencyHorizCloseShutter[BANr]) || (AQuRABlowAwayTransparency1CloseShutter[BANr])) {
			//SetFrequencyAQuRAHorizTransparencyAOM1(AQuRAHorizTransparencyAOM1Frequency);
			//SetIntensityAQuRAHorizTransparencyAOM1(AQuRAHorizTransparencyAOM1Intensity);
		}
		if ((AQuRABlowAway679RepumpCloseShutter[BANr]) || (AQuRABlowAwayRepumpCloseShutter[BANr])) {
			/*SetFrequencyAQuRARepump679AOM1(AQuRARepump679AOM1Frequency);
			SetIntensityAQuRARepump679AOM1(AQuRARepump679AOM1Intensity);*/
		}
		if ((AQuRABlowAway707RepumpCloseShutter[BANr]) || (AQuRABlowAwayRepumpCloseShutter[BANr])) {
			/*SetFrequencyAQuRARepump707AOM1(AQuRARepump707AOM1Frequency);
			SetIntensityAQuRARepump707AOM1(AQuRARepump707AOM1Intensity);*/
		}

		GoToTime(ExpantionTimeStart);
	}

	Wait(InitialExpansionTime/*-1*/, 4675);

	double MaxEndTime;
	double EndOfFirstFlash;
	AbsorptionPictureFlashSequence(/*FKS*/false,/*FirstFlash*/DoFirstFlash,/*SecondFlash*/false, MaxEndTime, EndOfFirstFlash, AndorCameraSecondImageTriggerDelay,/*Probe*/true);


	double Start = GetTime();
	RampTrapOff(/*LeaveFieldsOn*/false,/*LeaveDipoleTrapOn*/true);
	GoToTime(Start);
	Wait(AndorCameraSecondImageTriggerDelay, 1730);
	if (AbsPictureSwitchMOTBackOn) {
		GoBackInTime(InitialExpansionTime);
		//GoBackInTime(InitialExpansionTime+1);
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump1AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump2AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump3AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump4AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump5AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrBlueMOTSPAOM");
		//Wait(1);
		if (!AbsPictureLeaveMOTOn) SwitchMOTOffForImaging(AbsPictureLeaveAQuRABlueMOT, AbsPictureLeaveAQuRARed2DMOT, AbsPictureLeaveAQuRARed3DMOT, AbsPictureLeaveAQuRABlueMOTRepump, AbsPictureLeaveZITCTranspOn, AbsPictureLeaveSOLDOn);
		Wait(InitialExpansionTime, 4675);
	}
	if (UseMultipleReferenceImages) {
		for (int i = 0; i < NumberOfAdditionalReferenceImages; i++) {
			AbsorptionPictureFlashSequence(/*FKS*/false,/*FirstFlash*/true,/*SecondFlash*/false, MaxEndTime, EndOfFirstFlash, AndorCameraSecondImageTriggerDelay,/*Probe*/false);
			Wait(AndorCameraSecondImageTriggerDelay, 1730);
			if (AbsPictureSwitchMOTBackOn) {
				//GoBackInTime(InitialExpansionTime+1);
				GoBackInTime(InitialExpansionTime);

				//Wait(1);
				if (!AbsPictureLeaveMOTOn) SwitchMOTOffForImaging(AbsPictureLeaveAQuRABlueMOT, AbsPictureLeaveAQuRARed2DMOT, AbsPictureLeaveAQuRARed3DMOT, AbsPictureLeaveAQuRABlueMOTRepump, AbsPictureLeaveZITCTranspOn, AbsPictureLeaveSOLDOn);
				Wait(InitialExpansionTime, 4675);
			}
		}
	}

	double MaxEndTime2;
	AbsorptionPictureFlashSequence(/*FKS*/false,/*FirstFlash*/true,/*SecondFlash*/false, MaxEndTime2, EndOfFirstFlash, 1000,/*Probe*/false);



	Wait(AndorCameraSecondImageTriggerDelay, 1760);

	if (AbsPictureSwitchMOTBackOn) {
		//GoBackInTime(InitialExpansionTime+1);
		GoBackInTime(InitialExpansionTime);
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump1AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump2AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump3AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump4AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrRepump5AOM");
		//IOList->RecallAnalogOutValue("SetIntensitySrBlueMOTSPAOM");
		//Wait(1);
		if (!AbsPictureLeaveMOTOn) SwitchMOTOffForImaging(AbsPictureLeaveAQuRABlueMOT, AbsPictureLeaveAQuRARed2DMOT, AbsPictureLeaveAQuRARed3DMOT, AbsPictureLeaveAQuRABlueMOTRepump, AbsPictureLeaveZITCTranspOn, AbsPictureLeaveSOLDOn);
		Wait(InitialExpansionTime, 4675);
	}
	Start = GetTime();
	Wait(ExpansionTimeOfPicture[0], 1780);
	GoBackInTime(0.1 + CameraShutterPreTrigger, 5050);
	SwitchAllCameraTriggers(On);
	SwitchAllCameraShutters(On);
	Wait(1, 1790);
	SwitchAllCameraTriggers(Off);
	GoToTime(MaxEndTime + Start);
	SwitchAllCameraShutters(Off);
	Wait(EndOfRunWait, 1800);
	return true;
}


bool CSequence::TakeFKSAbsorptionPicture() {
	if (!Decision("DoTakeFKSAbsorptionPicture")) return false;

	PrePictureTrigger(/*Abs*/ true, /*FKS*/ true);
	SwitchTrapOff(/*LeaveFieldsOn*/AbsPictureLeaveFieldsOn,/*LeaveDipoleTrapOn*/AbsPictureLeaveLeaveDipoleTrapOn, AbsPictureLeaveTranspOn, AbsPictureLeaveRepumpTranspOn, AbsPictureLeaveLeaveDipoleTrapFreq);
	if (!AbsPictureLeaveMOTOn) SwitchMOTOffForImaging(AbsPictureLeaveAQuRABlueMOT, AbsPictureLeaveAQuRARed2DMOT, AbsPictureLeaveAQuRARed3DMOT, AbsPictureLeaveAQuRABlueMOTRepump, AbsPictureLeaveZITCTranspOn, AbsPictureLeaveSOLDOn);


	double Start1 = GetTime();

	if (AbsPictureSwitchOffFieldJustBeforeImaging) {
		Wait(InitialExpansionTime + ExpansionTimeOfPicture[1] - FieldSwitchingTimeBeforeImaging);
		SwitchTrapOff(/*LeaveFieldsOn*/false,/*LeaveDipoleTrapOn*/AbsPictureLeaveLeaveDipoleTrapOn, AbsPictureLeaveTranspOn, AbsPictureLeaveRepumpTranspOn, AbsPictureLeaveLeaveDipoleTrapFreq);
		GoToTime(Start1);
	}
	if (AbsPictureRbRepumpPulseJustBeforeImaging) {
		Wait(InitialExpansionTime + ExpansionTimeOfPicture[1] - RbRepumpTimeBeforeImaging);
		GoToTime(Start1);
	}

	Wait(InitialExpansionTime/*-1*/, 4675);

	double MaxEndTime;
	double EndOfFirstFlash;


	AbsorptionPictureFlashSequence(/*FKS*/true,/*FirstFlash*/DoFirstFlash,/*SecondFlash*/true, MaxEndTime, EndOfFirstFlash, AndorCameraSecondImageTriggerDelayFKS,/*Probe*/true);


	double Start = GetTime();
	GoToTime(EndOfFirstFlash);
	SwitchTrapOff(/*LeaveFieldsOn*/false,/*LeaveDipoleTrapOn*/false, AbsPictureLeaveTranspOn, AbsPictureLeaveRepumpTranspOn, AbsPictureLeaveLeaveDipoleTrapFreq);
	GoToTime(Start);
	Wait(AndorCameraSecondImageTriggerDelayFKS, 1810);
	double MaxEndTime2;
	AbsorptionPictureFlashSequence(/*FKS*/true,/*FirstFlash*/false,/*SecondFlash*/false, MaxEndTime2, EndOfFirstFlash, AndorCameraSecondImageTriggerDelayFKS,/*Probe*/true);


	if (UseMultipleReferenceImages) {
		for (int i = 0; i < NumberOfAdditionalReferenceImages; i++) {
			AbsorptionPictureFlashSequence(/*FKS*/true,/*FirstFlash*/true,/*SecondFlash*/true, MaxEndTime, EndOfFirstFlash, AndorCameraSecondImageTriggerDelayFKS,/*Probe*/false);
			Wait(AndorCameraSecondImageTriggerDelayFKS, 1820);
		}
	}




	Wait(AndorCameraSecondImageTriggerDelayFKS, 1820);


	AbsorptionPictureFlashSequence(/*FKS*/true,/*FirstFlash*/false,/*SecondFlash*/false, MaxEndTime2, EndOfFirstFlash, 1000,/*Probe*/false);




	Wait(EndOfRunWait, 1800);
	return true;
}

void CSequence::TriggerCameras() {
	SwitchAllCameraTriggers(On);
	Wait(1);
	SwitchAllCameraTriggers(Off);
}

void CSequence::PrePictureTrigger(bool Abs, bool FKS) {
	double Start = GetTime();
	if (Abs) {
		GoBackInTime((FKS) ? AndorCameraSecondImageTriggerDelayFKS : AndorCameraSecondImageTriggerDelay, 5060);
	}
	else GoBackInTime(AndorCameraSecondImageTriggerDelayFluo, 5060);
	SwitchCameraTrigger0(CameraPrePicture[0]);
	SwitchCameraTrigger1(CameraPrePicture[1]);		
	Wait(1, 1840);
	SwitchAllCameraTriggers(Off);
	GoToTime(Start);
}

void CSequence::TakeOneFluoPicture() {
	double Start = GetTime();
	GoBackInTime(CameraShutterPreTriggerFluo, 5070);
	if (FluoPictureUseCameraShutter) SwitchAllCameraShutters(On);
	GoToTime(Start);
	GoBackInTime(CameraTriggerPreTriggerFluo);
	SwitchAllCameraTriggers(On);
	Wait(1, 1850);
	SwitchAllCameraTriggers(Off);
	GoToTime(Start);
	Wait(FluorescenceImageShutterOpenTime, 1860);
	SwitchAllCameraShutters(Off);
}

bool CSequence::TakeFluorescencePicture() {
	if (!Decision("DoTakeFluorescencePicture")) return false;
	if (CameraTriggerPreTriggerFluo < 0.1) CameraTriggerPreTriggerFluo = 0.1;

	//if (!AbsPictureLeaveMOTOn) SwitchMOTOffForImaging(AbsPictureLeaveAQuRABlueMOT, AbsPictureLeaveAQuRARed2DMOT, AbsPictureLeaveAQuRARed3DMOT, AbsPictureLeaveZITCTranspOn);// 20160306 Modify
	int NumberPictures = GetMaxNumberPictures();
	if (NumberPictures == 0) return true;
	double Start = GetTime();
	PrePictureTrigger(/*Abs*/ false, /*FKS*/ false);
	GoToTime(Start);

	TakeOneFluoPicture();

	GoToTime(Start, 2566);
	Wait(CameraExposureTimeFluo);

	//add more fluo picture conditions here, similar to next code line
	//if (DoPrepareRedFluoPicture[0]) PrepareRedFluoPicture(0,/*ReferencePicture*/ true);	

	//Release atoms, switch MOT back on
	if (NumberPictures == 1) return true;
	

	//Empty traps, i.e. let all atoms fly out of imaging region
	IOList->StoreAnalogOutValue("SetIntensityRedMOTAOM");
	SetIntensityRedMOTAOM(0);
	IOList->StoreAnalogOutValue("SetFrequencyBlueMOTDPAOM");
	SetFrequencyBlueMOTDPAOM(0);
	IOList->StoreAnalogOutValue("SetFrequencyBlueDetectionDPAOM");
	SetFrequencyBlueDetectionDPAOM(0);
	SetMOTCoilCurrent(0);
	/*IOList->StoreAnalogOutValue("SetFrequencyLatticeAOM");
	SetFrequencyLatticeAOM(0);*/
	IOList->StoreAnalogOutValue("SetIntensityRedPumpAOM");
	SetIntensityRedPumpAOM(0);
	IOList->StoreAnalogOutValue("SetLatticePowerSetpoint");
	SetLatticePowerSetpoint(0);
	//SwitchTrapOff(/*LeaveFieldsOn*/false,/*LeaveDipoleTrapOn*/AbsPictureLeaveLeaveDipoleTrapOn, AbsPictureLeaveTranspOn, AbsPictureLeaveRepumpTranspOn, AbsPictureLeaveLeaveDipoleTrapFreq);

	

	//Wait(10);


	GoToTime(Start, 2566);
	Wait(AndorCameraSecondImageTriggerDelayFluo, 1880);
	Start = GetTime();


	//set light environment back to the one used for fluorescence imaging
	IOList->RecallAnalogOutValue("SetIntensityRedMOTAOM");
	IOList->RecallAnalogOutValue("SetFrequencyBlueMOTDPAOM");
	IOList->RecallAnalogOutValue("SetFrequencyBlueDetectionDPAOM");
	//IOList->RecallAnalogOutValue("SetFrequencyLatticeAOM");
	IOList->RecallAnalogOutValue("SetIntensityRedPumpAOM");
	IOList->RecallAnalogOutValue("SetLatticePowerSetpoint");
	
	//Take background image
	TakeOneFluoPicture();
	GoToTime(Start, 2566);
	Wait(CameraExposureTimeFluo);

	//Switch off all beams, just as during image with atoms
	SetIntensityRedMOTAOM(0);
	SetFrequencyBlueMOTDPAOM(0);
	SetFrequencyBlueDetectionDPAOM(0);
	SetFrequencyLatticeAOM(0);
	SetIntensityRedPumpAOM(0);
	
	GoToTime(Start, 2566);
	if (NumberPictures == 2) return true;
	Wait(AndorCameraSecondImageTriggerDelayFluo, 1880);
	SwitchAllCameraTriggers(On);
	Wait(1, 1850);
	SwitchAllCameraTriggers(Off);
	Wait(CameraExposureTimeFluo);

	Wait(EndOfRunWait, 1800);
	return true;
}

void CSequence::TakePicture() {
	if (!AssemblingParamList()) {
		LastExperimentalRunTime = GetTime();
		//Only one type of picture can be taken
		ImagingSystemConfigurationUsed = 0;
		if (UseDipoleTrapBeamForImaging) ImagingSystemConfigurationUsed = 1;
		if (!TakeAbsorptionPicture()) {
			if (!TakeFKSAbsorptionPicture()) {
				if (!TakeFluorescencePicture()) {
				
				}
			}
		}
	}
	else {
		ParamList->AddTakePicture();
	}
}