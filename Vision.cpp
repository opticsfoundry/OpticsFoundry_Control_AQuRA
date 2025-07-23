// Vision.cpp: implementation of the CVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Vision.h"
#include "ParamList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVision::CVision() :
CNetworkClient(1) {
	VisionInSerie=false;
	VisionSerieFirstPicture=true;
	for (int i=0;i<MaxNrCameras;i++) {
		CameraDirectionAkt[i]=""; 
		CameraUsedAkt[i]=false; 
		DisplayedPictureNumberAkt[i]=0; 
		MainCameraAkt[i]=false; 
		StartXAkt[i]=0; 
		StartYAkt[i]=0; 
		WidthAkt[i]=0; 
		HeightAkt[i]=0; 
		CalXAkt[i]=0; 
		CalYAkt[i]=0; 
		ExposureTimeAkt[i]=0; 
		TemperatureAkt[i]=0; 
		BinningXAkt[i]=0; 
		BinningYAkt[i]=0; 
		hSpeedAkt[i]=0; 
		FKSModeAkt[i]=false; 
		FKSDataImagesAkt[i]=0; 
		FKSReferenceImagesAkt[i]=0; 
		FKSHeightAkt[i]=0; 
		FKSvSpeedAkt[i]=0; 
		FKSDirtyLinesAkt[i]=0; 
		NumberPicturesAkt[i]=0; 
		PrePictureAkt[i]=false; 
		FlushWaitAkt[i]=0; 
		CameraInternalTriggerAfterExternalTriggerAkt[i]=false; 
		TriggerDelayAkt[i]=0; 
		AtomicMassAkt[i]=0; 
		AtomicWavelengthAkt[i]=0; 
		AtomicClebschGordonAkt[i]=0; 
		AtomicLinewidthAkt[i]=0; 
		AtomicMagneticMomentAkt[i]=0; 
		AtomicNameAkt[i]=""; 
		ProbeDetuningAkt[i]=0; 
		ExpansionTimeAkt[i]=0;  
		FluorescencePictureAkt[i]=false; 
		MaxFluorescenceAkt[i]=0; 
		CalibrationAkt[i]=0; 
		FinalTrapAkt[i]=""; 
		EndStateAkt[i]=""; 
		ExternalTriggerAkt[i]=false; 
		NumberOfAdditionalReferenceImagesAkt[i]=0; 
		DepthOfReferenceImageLibraryAkt[i]=0; 
	}
}

CVision::~CVision()
{
}

bool CVision::ConnectSocket(LPCTSTR lpszAddress,UINT port) {
	return CNetworkClient::ConnectSocket(lpszAddress,port,"Vision");
}

void CVision::Message(CString Message) {
  if (Command("VisionMessage")) {
    WriteString(Message);
  }
}

bool CVision::TakeAbsorptionPicture(double TriggerDelay) 		
{
	bool ok=Command("VisionTakeAbsorptionPicture");
	if (ok) {
		WriteInteger(11);
		WriteDouble(TriggerDelay);
		return Ready();
	}
	return ok;
}

bool CVision::TakeProbePicture() {
  return Command("VisionTakeProbePicture");
}

bool CVision::StartSerie(CString string,unsigned int ParamNr,CString ParamName[])
{
	bool ok=Command("VisionStartSerie");
	if (ok) {
		WriteInteger(ParamNr);
		for (unsigned int j=0;j<ParamNr;j++) WriteString(ParamName[j]);
		WriteString(string);	
		VisionInSerie=true;
		VisionSerieFirstPicture=true;
  } 
  return ok;  
}

bool CVision::ContinueSerie() {
	VisionInSerie=true;
    return Command("VisionContinueSerie");
}

bool CVision::StopSerie() {
	VisionInSerie=false;
    return Command("VisionStopSerie");
}

void CVision::RegisterInformation(unsigned int nr,double i[]) {	
	if (Command("VisionRegisterInformation")) {
		for (unsigned int n=0;n<nr;n++) WriteDouble(i[n]);
	}
}

void CVision::RunFinished() {	
	if (Command("VisionRunFinished")) {		
	}
}

void CVision::SynchronizeParameters() {
	CString buf;    
	if (Command("VisionSynchronizeParameters")) {
		GetCommand(buf);
		ParamList->Send(buf=="SendAll");		
	}
	Command("VisionSynchronizeParametersEnd");
}

//text contains comments, eg. the parameters of the serie under measurement
void CVision::RegisterParameters(CString text) {	
	if (Command("VisionRegisterParameters")) {
		SendCommand(text);
		Command("VisionRegisterParametersEnd");
	}
}

void CVision::NumberFit() {
  Command("VisionNumberFit");
}

void CVision::CCDFilmReferencePicture() {
  Command("VisionTakeAltaRefPic");
}

void CVision::CCDFilmStart() {
  Command("VisionCCDFilmStart");
}

void CVision::CCDFilmStop() {
  Command("VisionCCDFilmStop");
}

void CVision::SimpleNumberFit()
{
  Command("VisionSimpleNumberFit");
}

void CVision::TemperatureFit()
{
  Command("VisionTemperatureFit");
}

void CVision::MOTLoadingTimeFit() {
  Command("VisionMOTLoadingTimeFit");
}

void CVision::LifeTimeFit() {
  Command("VisionLifetimeFit");
}

void CVision::CheckNumber() {
  Command("VisionCheckNumber");
}

void CVision::GetPictureNumber(long &PicNr, long &SeriesNr) {
	Command("VisionPictureNumber");
	CString buf;
	if (GetCommand(buf)) PicNr=atoi(buf);
	if (GetCommand(buf)) SeriesNr=atoi(buf);
}

double CVision::GetIntegralAtomNumber() {
	Command("VisionGetIntegralAtomNumber");
	CString buf;
	if (GetCommand(buf)) return atof(buf);
	else return 0;    
}

bool CVision::PictureOk() {
	CString buf;
  if (GetCommand(buf)) return buf=="VisionPictureOk";
  else return true;    
}

void CVision::SetNetCameraParameters(int CameraNr, CString CameraDirection, bool CameraUsed, int DisplayedPictureNumber, 
        bool MainCamera, int StartX,int StartY, int Width,int Height,
		double CalX,double CalY, double ExposureTime, double Temperature,int BinningX, int BinningY, 
		int hSpeed, bool FKSMode, int FKSDataImages, int FKSReferenceImages, int FKSHeight, int FKSvSpeed, int FKSDirtyLines, int NumberPictures, bool PrePicture,
		int FlushWait, bool CameraInternalTriggerAfterExternalTrigger, int TriggerDelay,
		double AtomicMass, double AtomicWavelength, double AtomicClebschGordon, double AtomicLinewidth,
		double AtomicMagneticMoment, CString AtomicName, double ProbeDetuning, double ExpansionTime, 
		bool FluorescencePicture, double MaxFluorescence, double Calibration, CString FinalTrap,
		CString EndState, bool ExternalTrigger,long NumberOfAdditionalReferenceImages,long DepthOfReferenceImageLibrary) {

	bool ParamsChanged=false;
	int i=CameraNr;
	ParamsChanged|=CameraDirectionAkt[i]!=CameraDirection;
	ParamsChanged|=CameraUsedAkt[i]!=CameraUsed; 
	ParamsChanged|=DisplayedPictureNumberAkt[i]!=DisplayedPictureNumber; 
	ParamsChanged|=MainCameraAkt[i]!=MainCamera; 
	ParamsChanged|=StartXAkt[i]!=StartX; 
	ParamsChanged|=StartYAkt[i]!=StartY; 
	ParamsChanged|=WidthAkt[i]!=Width; 
	ParamsChanged|=HeightAkt[i]!=Height; 
	ParamsChanged|=CalXAkt[i]!=CalX; 
	ParamsChanged|=CalYAkt[i]!=CalY; 
	ParamsChanged|=ExposureTimeAkt[i]!=ExposureTime; 
	ParamsChanged|=TemperatureAkt[i]!=Temperature; 
	ParamsChanged|=BinningXAkt[i]!=BinningX; 
	ParamsChanged|=BinningYAkt[i]!=BinningY; 
	ParamsChanged|=hSpeedAkt[i]!=hSpeed; 
	ParamsChanged|=FKSModeAkt[i]!=FKSMode; 
	ParamsChanged|=FKSDataImagesAkt[i]!=FKSDataImages; 
	ParamsChanged|=FKSReferenceImagesAkt[i]!=FKSReferenceImages; 
	ParamsChanged|=FKSHeightAkt[i]!=FKSHeight; 
	ParamsChanged|=FKSvSpeedAkt[i]!=FKSvSpeed; 
	ParamsChanged|=FKSDirtyLinesAkt[i]!=FKSDirtyLines; 
	ParamsChanged|=NumberPicturesAkt[i]!=NumberPictures; 
	ParamsChanged|=PrePictureAkt[i]!=PrePicture; 
	ParamsChanged|=FlushWaitAkt[i]!=FlushWait; 
	ParamsChanged|=CameraInternalTriggerAfterExternalTriggerAkt[i]!=CameraInternalTriggerAfterExternalTrigger; 
	ParamsChanged|=TriggerDelayAkt[i]!=TriggerDelay; 
	ParamsChanged|=AtomicMassAkt[i]!=AtomicMass; 
	ParamsChanged|=AtomicWavelengthAkt[i]!=AtomicWavelength; 
	ParamsChanged|=AtomicClebschGordonAkt[i]!=AtomicClebschGordon; 
	ParamsChanged|=AtomicLinewidthAkt[i]!=AtomicLinewidth; 
	ParamsChanged|=AtomicMagneticMomentAkt[i]!=AtomicMagneticMoment; 
	ParamsChanged|=AtomicNameAkt[i]!=AtomicName; 
	ParamsChanged|=ProbeDetuningAkt[i]!=ProbeDetuning; 
	ParamsChanged|=ExpansionTimeAkt[i]!=ExpansionTime;  
	ParamsChanged|=FluorescencePictureAkt[i]!=FluorescencePicture; 
	ParamsChanged|=MaxFluorescenceAkt[i]!=MaxFluorescence; 
	ParamsChanged|=CalibrationAkt[i]!=Calibration; 
	ParamsChanged|=FinalTrapAkt[i]!=FinalTrap; 
	ParamsChanged|=EndStateAkt[i]!=EndState; 
	ParamsChanged|=ExternalTriggerAkt[i]!=ExternalTrigger; 
	ParamsChanged|=NumberOfAdditionalReferenceImagesAkt[i]!=NumberOfAdditionalReferenceImages; 
	ParamsChanged|=DepthOfReferenceImageLibraryAkt[i]!=DepthOfReferenceImageLibrary; 

	if (!ParamsChanged) return;

	if (Command("VisionSetCameraParameters")) {
		WriteInteger(11);//Camera type: NetAndor
		WriteDouble(CalX*0.001);
		WriteDouble(CalY*0.001);
		WriteInteger(StartX);
		WriteInteger(StartY);
		WriteInteger(Width);
		WriteInteger(Height);		
		WriteInteger(BinningX);		//BinnningX
		WriteInteger(BinningY);		//BinnningY
		WriteInteger(CameraNr);
		WriteString(CameraDirection);
		WriteInteger((CameraUsed) ? 1 : 0);
		WriteInteger(DisplayedPictureNumber);
		WriteInteger((MainCamera) ? 1 : 0);
		WriteDouble(ExposureTime);
		WriteDouble(Temperature);		
		WriteInteger(ExternalTrigger); //for external put 1, internal 0
		WriteInteger(/*vSpeed*/ 16); //this is fixed at 16 µs for our cameras
		WriteInteger(/*hSpeed*/ hSpeed);
		WriteInteger((FKSMode) ? 1 : 0);
		WriteInteger(FKSHeight);
		WriteInteger(FKSDataImages);
		WriteInteger(FKSReferenceImages);
		WriteInteger(FKSvSpeed);
		WriteInteger(/*FKSExternalTriggerStart*/ 1);
		WriteInteger(FKSDirtyLines);
		WriteInteger(NumberPictures);
		WriteInteger((PrePicture) ? 1 : 0);
		WriteInteger(FlushWait);
		WriteInteger((CameraInternalTriggerAfterExternalTrigger) ? 1 : 0);
		WriteDouble(TriggerDelay);//TriggerDelay		
		WriteDouble(AtomicMass);
		WriteDouble(AtomicWavelength*1E-9);
		WriteDouble(AtomicClebschGordon);
		WriteDouble(AtomicLinewidth*1E6);
		WriteDouble(AtomicMagneticMoment);  //the magnetic moment of this specific atomic state
		WriteString(AtomicName);  //Format example: "6Li |3/2,3/2>" for 6 Lithium F=3/2, mF=3/2
		WriteDouble(ProbeDetuning*1E6);
		WriteDouble(ExpansionTime);
		WriteInteger((FluorescencePicture) ? 1 : 0);
		WriteDouble(MaxFluorescence);
		WriteDouble(Calibration);
		WriteInteger(NumberOfAdditionalReferenceImages);
		WriteInteger(DepthOfReferenceImageLibrary);
		WriteString(FinalTrap);
		WriteString(EndState);
    
		CameraDirectionAkt[i]=CameraDirection;
		CameraUsedAkt[i]=CameraUsed; 
		DisplayedPictureNumberAkt[i]=DisplayedPictureNumber; 
		MainCameraAkt[i]=MainCamera; 
		StartXAkt[i]=StartX; 
		StartYAkt[i]=StartY; 
		WidthAkt[i]=Width; 
		HeightAkt[i]=Height; 
		CalXAkt[i]=CalX; 
		CalYAkt[i]=CalY; 
		ExposureTimeAkt[i]=ExposureTime; 
		TemperatureAkt[i]=Temperature; 
		BinningXAkt[i]=BinningX; 
		BinningYAkt[i]=BinningY; 
		hSpeedAkt[i]=hSpeed; 
		FKSModeAkt[i]=FKSMode; 
		FKSDataImagesAkt[i]=FKSDataImages; 
		FKSReferenceImagesAkt[i]=FKSReferenceImages; 
		FKSHeightAkt[i]=FKSHeight; 
		FKSvSpeedAkt[i]=FKSvSpeed; 
		FKSDirtyLinesAkt[i]=FKSDirtyLines; 
		NumberPicturesAkt[i]=NumberPictures; 
		PrePictureAkt[i]=PrePicture; 
		FlushWaitAkt[i]=FlushWait; 
		CameraInternalTriggerAfterExternalTriggerAkt[i]=CameraInternalTriggerAfterExternalTrigger; 
		TriggerDelayAkt[i]=TriggerDelay; 
		AtomicMassAkt[i]=AtomicMass; 
		AtomicWavelengthAkt[i]=AtomicWavelength; 
		AtomicClebschGordonAkt[i]=AtomicClebschGordon; 
		AtomicLinewidthAkt[i]=AtomicLinewidth; 
		AtomicMagneticMomentAkt[i]=AtomicMagneticMoment; 
		AtomicNameAkt[i]=AtomicName; 
		ProbeDetuningAkt[i]=ProbeDetuning; 
		ExpansionTimeAkt[i]=ExpansionTime;  
		FluorescencePictureAkt[i]=FluorescencePicture; 
		MaxFluorescenceAkt[i]=MaxFluorescence; 
		CalibrationAkt[i]=Calibration; 
		FinalTrapAkt[i]=FinalTrap; 
		EndStateAkt[i]=EndState; 
		ExternalTriggerAkt[i]=ExternalTrigger; 
		NumberOfAdditionalReferenceImagesAkt[i]=NumberOfAdditionalReferenceImages; 
		DepthOfReferenceImageLibraryAkt[i]=DepthOfReferenceImageLibrary; 
	}
}

bool CVision::CameraReady(unsigned short Camera) {
	CString buf;
	if (Command("VisionCameraReady")) {
		WriteInteger(Camera);
    if (GetCommand(buf)) return buf=="VisionCameraReady";
    else {
      ControlMessageBox("CVision::CameraReady : no response from Vision");
      return false;
    }
  } else {
    ControlMessageBox("CVision::CameraReady : no response from Vision");
    return false;
  }
}

void CVision::SendPictureData(double MOTFluorescence,double RecapFluo,
          double BarCurrent, double TransferCurrent,
	double MOTCurrent, double Bias, double Gradiant, double Curvature,
	double LoadingTime, double TimingError,double LastDataArray[],CString LastDataArrayName[],int LastDataArrayPoints) {
  SynchronizeParameters();
  WriteDouble(MOTFluorescence);
  WriteDouble(RecapFluo);
  WriteDouble(0);//final trap type  
  WriteDouble(BarCurrent);
  WriteDouble(TransferCurrent);
  WriteDouble(MOTCurrent);  
  WriteDouble(Bias);
  WriteDouble(Gradiant);
  WriteDouble(Curvature);
  WriteDouble(LoadingTime);
  WriteDouble(TimingError);
  WriteDouble(0);//Li7f0
  WriteDouble(0);//Li7fend
  WriteInteger(LastDataArrayPoints);
  for (int i=0;i<LastDataArrayPoints;i++) WriteDouble(LastDataArray[i]); 
  //for (int i=0;i<LastDataArrayPoints;i++) WriteString(LastDataArrayName[i]); 

}

bool CVision::SendDataFile(CString Filename,CString VisionFilename) {	
	if (Command("VisionSendDataFile")) {
		SendCommand(VisionFilename);
		char buf[10000];
		ifstream in;	
		in.open(Filename,/*ios::nocreate|*/ios::in);
		if (!in.is_open()) {
			ControlMessageBox("CVision::WriteDataFile : file "+Filename+" not found");
			return false;
		}	
		while (!in.eof()) {
			in.getline(buf,10000);
			SendCommand(buf);
		}	
		in.close();
	}
	Command("VisionSendDataFileEnd");
	return true;
}

bool CVision::CheckReady() {
	return Command("VisionReady");
}

bool CVision::SetBarPosition(double xp1, double dx1, double yp1, double dy1,
							 double xp2, double dx2, double yp2, double dy2) {
	if (Command("VisionSetBarPosition")) {	
		WriteDouble(xp1);
		WriteDouble(xp1+dx1);
		WriteDouble(yp1);
		WriteDouble(yp1+dy1);
		WriteDouble(xp2);
		WriteDouble(xp2+dx2);
		WriteDouble(yp2);
		WriteDouble(yp2+dy2);
		return true;
	} else return false;
}

bool CVision::GetBarPosition(double &xp1, double &dx1, double &yp1, double &dy1,
							 double &xp2, double &dx2, double &yp2, double &dy2) {
	Command("VisionGetBarPosition");
	CString buf;
	bool error=false;
	if (GetCommand(buf)) xp1=atof(buf); else error=true;
	if (GetCommand(buf)) dx1=atof(buf)-xp1; else error=true;
	if (GetCommand(buf)) yp1=atof(buf); else error=true;
	if (GetCommand(buf)) dy1=atof(buf)-yp1; else error=true;
	if (GetCommand(buf)) xp2=atof(buf); else error=true;
	if (GetCommand(buf)) dx2=atof(buf)-xp2; else error=true;
	if (GetCommand(buf)) yp2=atof(buf); else error=true;
	if (GetCommand(buf)) dy2=atof(buf)-yp2; else error=true;
	return error;    	
}