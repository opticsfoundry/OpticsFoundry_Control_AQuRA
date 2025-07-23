// SRS345.cpp: implementation of the CSRS345 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "SRS345.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSRS345::CSRS345(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion) {
}

CSRS345::~CSRS345()
{
}

bool CSRS345::SetFrequency(double Frequency) //Frequency in MHz
{
	CString buf;
	buf.Format("FREQ %.3f",Frequency*1000000); //conversion to Hz
	return WriteString(buf);
}

bool CSRS345::SetAmplitudeVpp(double Amplitude)  //Amplitude in Vpp
{
	CString buf;
	buf.Format("AMPL %.5f VP",Amplitude); 
	return WriteString(buf);
}

bool CSRS345::SetAmplitudedBm(double Amplitude)  //Amplitude in dBm
{
	CString buf;
	buf.Format("AMPL %.3f DB",Amplitude); 
	return WriteString(buf);
}

bool CSRS345::SetAmplitudeVrms(double Amplitude)  //Amplitude in Vrms
{
	CString buf;
	buf.Format("AMPL %.3f Vrms",Amplitude); 
	return WriteString(buf);
}

bool CSRS345::SetAmplitudeTTL() 
{
	return WriteString("ATTL");
}

bool CSRS345::SetAmplitudeECL() 
{
	return WriteString("AECL");
}

bool CSRS345::SetOffset(double Offset)  //Offset in V
{
	CString buf;
	buf.Format("OFFS %.3f",Offset); 
	return WriteString(buf);
}

bool CSRS345::SetWaveformFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("FSMP %.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetSine() 
{
	return WriteString("FUNC0");
}

bool CSRS345::SetSquare() 
{
	return WriteString("FUNC1");
}

bool CSRS345::SetTriangle()
{
	return WriteString("FUNC2");
}

bool CSRS345::SetRamp()
{
	return WriteString("FUNC3");
}

bool CSRS345::SetNoise()
{
	return WriteString("FUNC4");
}

bool CSRS345::SetWaveformArbitrary()
{
	return WriteString("FUNC5");
}

bool CSRS345::SetOutputInvert(bool OnOff)
{
	if (OnOff) return WriteString("INVT1");
	else return WriteString("INVT0");
}

bool CSRS345::SetZeroPhase()
{
	return WriteString("PCLR");
}

bool CSRS345::SetPhase(double Phase)  //Phase in degrees
{
	if ((Phase>7199.999) || (Phase<0)) return Error("CSRS345::SetPhase : wrong phase");
	if (Phase<0.001) return SetZeroPhase();
	CString buf;
	buf.Format("PHSE %.3f",Phase); 
	return WriteString(buf);
}

bool CSRS345::Trigger()
{
	return WriteString("*TRG");
}

bool CSRS345::SetBurstCount(int BurstCount)  
{
	if ((BurstCount>30000) || (BurstCount<0)) return Error("CSRS345::SetBurstCount : wrong burst count");
	CString buf;
	buf.Format("BCNT %i",BurstCount); 
	return WriteString(buf);
}

bool CSRS345::SetAMModulationDepth(int Depth)  //in %
{
	if ((Depth>100) || (Depth<-100)) return Error("CSRS345::SetAMModulationDepth : wrong modulation depth");
	CString buf;
	buf.Format("DPTH %i",Depth); 
	return WriteString(buf);
}

bool CSRS345::SetFMSpan(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("FDEV %.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetModulationSingleSweep() 
{
	return WriteString("MDWF0");
}

bool CSRS345::SetModulationRamp()
{
	return WriteString("MDWF1");
}

bool CSRS345::SetModulationTriangle()
{
	return WriteString("MDWF2");
}

bool CSRS345::SetModulationSine() 
{
	return WriteString("MDWF3");
}

bool CSRS345::SetModulationSquare() 
{
	return WriteString("MDWF4");
}

bool CSRS345::SetModulationArbitrary()
{
	return WriteString("MDWF5");
}

bool CSRS345::SetModulationNone()
{
	return WriteString("MDWF6");
}

bool CSRS345::EnableModulation()
{
	return WriteString("MENA1");
}

bool CSRS345::DisableModulation()
{
	return WriteString("MENA0");
}

bool CSRS345::SetSweepMarkers()
{
	return WriteString("MKSP");
}

bool CSRS345::SetSweepMarkStartFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("MRKF0,%.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetSweepMarkStopFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("MRKF1,%.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetSweepMarkCenterFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("MRKF2,%.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetSweepMarkSpanFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("MRKF3,%.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetModulationTypeLinearSweep() 
{
	return WriteString("MTYP0");
}

bool CSRS345::SetModulationTypeLogarithmicSweep() 
{
	return WriteString("MTYP1");
}

bool CSRS345::SetModulationTypeInternalAM() 
{
	return WriteString("MTYP2");
}

bool CSRS345::SetModulationTypeFrequency() 
{
	return WriteString("MTYP3");
}

bool CSRS345::SetModulationTypePhase() 
{
	return WriteString("MTYP4");
}

bool CSRS345::SetModulationTypeBurst() 
{
	return WriteString("MTYP5");
}

bool CSRS345::SetPhaseModulationSpan(double Phase)  //Phase in degrees
{
	if ((Phase>7199.999) || (Phase<0)) return Error("CSRS345::SetPhaseModulationSpan : wrong phase");	
	CString buf;
	buf.Format("PDEV %.3f",Phase); 
	return WriteString(buf);
}

bool CSRS345::SetModulationRate(double Rate) //Rate in Hz
{
	CString buf;
	buf.Format("RATE%.3f",Rate);
	return WriteString(buf);
}

bool CSRS345::SetSweepSpan(double Span) //Span in Hz
{
	CString buf;
	buf.Format("SPAN%.3f",Span);
	return WriteString(buf);
}

bool CSRS345::SetSweepCenterFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("SPCF%.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetSweepStartFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("STFR%.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetSweepStopFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("SPFR%.3f",Frequency);
	return WriteString(buf);
}

bool CSRS345::SetSweepSpanToSweepMarkers() 
{
	return WriteString("SPMK");
}

bool CSRS345::SetTriggerRate(double Rate) //Rate in Hz
{
	CString buf;
	buf.Format("TRAT%.3f",Rate);
	return WriteString(buf);
}

bool CSRS345::SetTriggerSourceSingle()
{
	CString buf;
	buf.Format("TSRC0");
	return WriteString(buf);
}

bool CSRS345::SetTriggerSourceInternalRate()
{
	CString buf;
	buf.Format("TSRC1");
	return WriteString(buf);
}

bool CSRS345::SetTriggerSourcePositiveSlopeExternal()
{
	CString buf;
	buf.Format("TSRC2");
	return WriteString(buf);
}

bool CSRS345::SetTriggerSourceNegativeSlopeExternal()
{
	CString buf;
	buf.Format("TSRC3");
	return WriteString(buf);
}

bool CSRS345::SetTriggerSourceLine()
{
	CString buf;
	buf.Format("TSRC4");
	return WriteString(buf);
}

bool CSRS345::SetArbitraryModulationRateDivider(unsigned long Divider)  
{	
	CString buf;
	buf.Format("AMRT%i",Divider); 
	return WriteString(buf);
}

bool CSRS345::SetArbitraryFMWaveform(unsigned int NrPoints,double aWaveform[]) { //Waveform in MHz
	if (!GPIB.HardwareAccess) return false;
	if (NrPoints>1500) return Error("CSRS345::SetArbitraryFMWaveform : too many points");
	/*ofstream out;
	out.open(*DebugFilePath + "Debug.dat", CFile::modeCreate | CFile::modeWrite);
	for (unsigned int l=0;l<NrPoints;l++) {
		CString buf;
		buf.Format("%i %.3f",l,aWaveform[l]);
		out<<buf<<endl;
	}
	out.close();*/
	//ofstream out;
	//out.open(*DebugFilePath + "Debug.dat", CFile::modeCreate | CFile::modeWrite);	
	unsigned char *Waveform=new unsigned char[4*(NrPoints+1)]; //+1 for Checksum
	unsigned long Checksum=0;
	double scale=pow(2.0,32.0);
	for (unsigned int i=0;i<NrPoints;i++) {
		unsigned long Value=(unsigned long)(scale*aWaveform[i]/40.0);
	//		CString buf;
	//		buf.Format("%u %lu",i,Value);
	//		out<<buf<<endl;
		Checksum+=Value;
		for (int j=0;j<4;j++) {
			Waveform[i*4+j]=(unsigned char)( 0xFF & Value);
			Value=Value>>8;
		}
	}
    CString buf;
//  buf.Format("%u %lu",NrPoints,Checksum);
//	out<<buf<<endl;
//	out.close();
	for (int j=0;j<4;j++) {
		Waveform[NrPoints*4+j]=(unsigned char)( 0xFF & Checksum);
		Checksum=Checksum>>8;
	}	
	buf.Format("AMOD?%i",NrPoints); 
	if (!WriteString(buf)) {
		delete Waveform;
		return Error("CSRS345::SetArbitraryFMWaveform : error during AMOD?");
	}
	if (!ReadString(buf,2)) {
		delete Waveform;
		return Error("CSRS345::SetArbitraryFMWaveform : timeout during AMOD?");
	}	
	if (buf[0]!='1') {
		delete Waveform;
		return Error("CSRS345::SetArbitraryFMWaveform : no 1 received after AMOD?");
	}		
	if (!SendData(Waveform,4*(NrPoints+1))) {
		delete Waveform;
		return Error("CSRS345::SetArbitraryFMWaveform : could not send waveform");
	}
	delete Waveform;
	return true;
}
