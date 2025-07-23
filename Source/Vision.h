// Vision.h: interface for the CVision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISION_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
#define AFX_VISION_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_

#include "NetworkClient.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int MaxNrCameras=16;

class CVision : public CNetworkClient
{
private:
	bool VisionInSerie;
	bool VisionSerieFirstPicture;	

	CString CameraDirectionAkt[MaxNrCameras]; 
	bool CameraUsedAkt[MaxNrCameras]; 
	int DisplayedPictureNumberAkt[MaxNrCameras]; 
	bool MainCameraAkt[MaxNrCameras]; 
	int StartXAkt[MaxNrCameras]; 
	int StartYAkt[MaxNrCameras]; 
	int WidthAkt[MaxNrCameras]; 
	int HeightAkt[MaxNrCameras]; 
	double CalXAkt[MaxNrCameras]; 
	double CalYAkt[MaxNrCameras]; 
	double ExposureTimeAkt[MaxNrCameras]; 
	double TemperatureAkt[MaxNrCameras]; 
	int BinningXAkt[MaxNrCameras]; 
	int BinningYAkt[MaxNrCameras]; 
	int hSpeedAkt[MaxNrCameras]; 
	bool FKSModeAkt[MaxNrCameras]; 
	int FKSDataImagesAkt[MaxNrCameras]; 
	int FKSReferenceImagesAkt[MaxNrCameras]; 
	int FKSHeightAkt[MaxNrCameras]; 
	int FKSvSpeedAkt[MaxNrCameras]; 
	int FKSDirtyLinesAkt[MaxNrCameras]; 
	int NumberPicturesAkt[MaxNrCameras]; 
	bool PrePictureAkt[MaxNrCameras]; 
	int FlushWaitAkt[MaxNrCameras]; 
	bool CameraInternalTriggerAfterExternalTriggerAkt[MaxNrCameras]; 
	int TriggerDelayAkt[MaxNrCameras]; 
	double AtomicMassAkt[MaxNrCameras]; 
	double AtomicWavelengthAkt[MaxNrCameras]; 
	double AtomicClebschGordonAkt[MaxNrCameras]; 
	double AtomicLinewidthAkt[MaxNrCameras]; 
	double AtomicMagneticMomentAkt[MaxNrCameras]; 
	CString AtomicNameAkt[MaxNrCameras]; 
	double ProbeDetuningAkt[MaxNrCameras]; 
	double ExpansionTimeAkt[MaxNrCameras];  
	bool FluorescencePictureAkt[MaxNrCameras]; 
	double MaxFluorescenceAkt[MaxNrCameras]; 
	double CalibrationAkt[MaxNrCameras]; 
	CString FinalTrapAkt[MaxNrCameras]; 
	CString EndStateAkt[MaxNrCameras]; 
	bool ExternalTriggerAkt[MaxNrCameras]; 
	long NumberOfAdditionalReferenceImagesAkt[MaxNrCameras]; 
	long DepthOfReferenceImageLibraryAkt[MaxNrCameras]; 


public:	
	void RunFinished();
	double GetIntegralAtomNumber();
	bool ConnectSocket(LPCTSTR lpszAddress,UINT port);
	void CCDFilmReferencePicture();
	void CCDFilmStart();
	void CCDFilmStop();
	bool SetBarPosition(double xp1, double dx1, double yp1, double dy1,
							 double xp2, double dx2, double yp2, double dy2);
	bool GetBarPosition(double &xp1, double &dx1, double &yp1, double &dy1,
							 double &xp2, double &dx2, double &yp2, double &dy2);	
	void GetPictureNumber(long &PicNr, long &SeriesNr);
	bool CheckReady();
	void Message(CString Message);	
	bool TakeProbePicture();	
	bool StartSerie(CString commentary, unsigned int ParamNr, CString ParamName[]);
	bool StopSerie();
	bool ContinueSerie();
	void RegisterInformation(unsigned int nr,double i[]);
	void SynchronizeParameters();
	bool SendDataFile(CString Filename,CString VisionFilename);
	void RegisterParameters(CString text);
	void NumberFit();
	void SimpleNumberFit();
	void TemperatureFit();
	void MOTLoadingTimeFit();
	void LifeTimeFit();
	void CheckNumber();
	bool PictureOk();
	bool CameraReady(unsigned short Camera=0);
	void SetNetCameraParameters(int CameraNr,CString CameraDirection, bool CameraUsed, int DisplayedPictureNumber, 
        bool MainCamera, int StartX,int StartY, int Width,int Height,
		double CalX,double CalY, double ExposureTime, double Temperature,int BinningX, int BinningY, 
		int hSpeed, bool FKSMode, int FKSDataImages, int FKSReferenceImages, int FKSHeight, int FKSvSpeed, int FKSDirtyLines, int NumberPictures,
		bool PrePicture, int FlushWait, bool CameraInternalTriggerAfterExternalTrigger, int TriggerDelay,
		double AtomicMass, double AtomicWavelength, double AtomicClebschGordon, double AtomicLinewidth,
		double AtomicMagneticMoment, CString AtomicName, double ProbeDetuning, double ExpansionTime, 
		bool FluorescencePicture, double MaxFluorescence, double Calibration, CString FinalTrap,
		CString EndState, bool ExternalTrigger,long NumberOfAdditionalReferenceImages,long DepthOfReferenceImageLibrary);
	bool TakeAbsorptionPicture(double TriggerDelay);
	void SendPictureData(double MOTFluorescence,double RecapFluo,
			double BarCurrent, double TransferCurrent,
			double MOTCurrent, double Bias, double Gradiant, double Curvature,
			double LoadingTime, double TimingError,double LastDataArray[],CString LastDataArrayName[],int LastDataArrayPoints);
	CVision();
	virtual ~CVision();
};

#endif // !defined(AFX_VISION_H__D3C2A5C8_2D51_4BFB_8061_4C5802EF5B3F__INCLUDED_)
