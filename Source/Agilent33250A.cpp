// Agilent33250A.cpp: implementation of the CAgilent33250A class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Agilent33250A.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAgilent33250A::CAgilent33250A(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion) {
	
}

CAgilent33250A::~CAgilent33250A()
{
	
}

bool CAgilent33250A::SetFrequency(double Frequency) //Frequency in MHz
{
	CString buf;
	buf.Format("FREQ %.3f KHZ",Frequency*1000); //conversion to kHz
	return WriteString(buf);
}

bool CAgilent33250A::SetAmplitudeVpp(double Amplitude)  //Amplitude in Vpp
{
	CString buf;
	buf.Format("VOLT %.5f VPP",Amplitude); 
	return WriteString(buf);
}

bool CAgilent33250A::SetAmplitudedBm(double Amplitude)  //Amplitude in dBm
{
	CString buf;
	buf.Format("VOLT %.5f DBM",Amplitude); 
	return WriteString(buf);
}

bool CAgilent33250A::SetOffset(double Offset)  //Offset in V
{
	CString buf;
	buf.Format("VOLT:OFFS %.3f V",Offset); 
	return WriteString(buf);
}

bool CAgilent33250A::SetSine() 
{
	return WriteString("FUNC:SHAP SIN");
}

bool CAgilent33250A::SetSquare() 
{
	return WriteString("FUNC:SHAP SQU");
}

bool CAgilent33250A::SetRamp() 
{
	return WriteString("FUNC:SHAP RAMP");
}

bool CAgilent33250A::SetPulse() 
{
	return WriteString("FUNC:SHAP Pulse");
}

bool CAgilent33250A::SetNoise() 
{
	return WriteString("FUNC:SHAP NOIS");
}

bool CAgilent33250A::SetDC() 
{
	return WriteString("FUNC:SHAP DC");
}

bool CAgilent33250A::SetArbitrary() 
{
	return WriteString("FUNC:SHAP USER");
}

bool CAgilent33250A::SelectArbitraryVolatile() 
{
	return WriteString("FUNC:USER VOLATILE");
}

bool CAgilent33250A::SelectArbitraryExponentialRise() 
{
	return WriteString("FUNC:USER EXP_RISE");
}

bool CAgilent33250A::SelectArbitraryExponentialFall() 
{
	return WriteString("FUNC:USER EXP_FALL");
}

bool CAgilent33250A::SelectArbitraryNegativeRamp()
{
	return WriteString("FUNC:USER NEG_RAMP");
}

bool CAgilent33250A::SelectArbitrarySinc()  
{
	return WriteString("FUNC:USER SINC");
}

bool CAgilent33250A::SelectArbitraryCardiac()  
{
	return WriteString("FUNC:USER CARDIAC");
}
bool CAgilent33250A::SwitchFM(bool OnOff) 
{
	if (OnOff) return WriteString("FM:STAT ON");
	else return WriteString("FM:STAT OFF");
}

bool CAgilent33250A::SetFMSourceExternal() 
{
	return WriteString("FM:SOUR EXT");
}

bool CAgilent33250A::SetFMSourceInternal() 
{
	return WriteString("FM:SOUR INT");
}

bool CAgilent33250A::SetFMDeviation(double Deviation) //in MHz
{  //in external mode, +5V adds Deviation to the center frequency
   //and -5V substracts Deviation from the center frequency
	CString buf; 
	buf.Format("FM:DEV %.3f KHZ",Deviation*1000.0); //conversion to kHz
	return WriteString(buf);
}

bool CAgilent33250A::SetTriggerExternal() 
{
	return WriteString("TRIG:SOUR EXT");
}

bool CAgilent33250A::SetTriggerInternal() 
{
	return WriteString("TRIG:SOUR IMM");
}

bool CAgilent33250A::SetTriggerGPIB() 
{
	return WriteString("TRIG:SOUR BUS");
}

bool CAgilent33250A::SetTriggerSlopePositive() 
{
	return WriteString("TRIG:SLOP POS");
}

bool CAgilent33250A::SetTriggerSlopeNegative() 
{
	return WriteString("TRIG:SLOP NEG");
}

bool CAgilent33250A::Trigger() 
{
	return WriteString("*TRG");
}

bool CAgilent33250A::SetFMCarrierSin() 
{	
	return WriteString("FUNC:SHAP SIN");
}

bool CAgilent33250A::SetFMCarrierSquare() 
{	
	return WriteString("FM:SHAP SQU");
}

bool CAgilent33250A::SetFMCarrierRamp() 
{	
	return WriteString("FM:SHAP RAMP");
}

bool CAgilent33250A::SetFMCarrierUser() 
{	
	return WriteString("FM:SHAP USER");
}

bool CAgilent33250A::SetFMModulationShapeUser() 
{	
	return WriteString("FM:INT:FUNC USER");
}

bool CAgilent33250A::SetFMModulationShapeSin() 
{	
	return WriteString("FM:INT:FUNC SIN");
}

bool CAgilent33250A::SetFMModulationShapeSquare() 
{	
	return WriteString("FM:INT:FUNC SQU");
}

bool CAgilent33250A::SetFMModulationShapeRamp() 
{	
	return WriteString("FM:INT:FUNC RAMP");
}

bool CAgilent33250A::SetFMModulationShapeNegativeRamp() 
{	
	return WriteString("FM:INT:FUNC NRAM");
}

bool CAgilent33250A::SetFMModulationShapeTriangle() 
{	
	return WriteString("FM:INT:FUNC TRI");
}

bool CAgilent33250A::SetFMModulationShapeNoise() 
{	
	return WriteString("FM:INT:FUNC NOIS");
}

bool CAgilent33250A::SetFMModulationFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("FM:INT:FREQ %.8f HZ",Frequency); //conversion to Hz
	return WriteString(buf);
}

bool CAgilent33250A::SetFMModulationSourceInternal() 
{
	return WriteString("FM:SOUR INT");
}

bool CAgilent33250A::SetFMModulationSourceExternal() 
{
	return WriteString("FM:SOUR EXT");
}

bool CAgilent33250A::SetBinaryTransferByteOrderMSBFirst() 
{
	return WriteString("FORM:BORD NORM");
}

bool CAgilent33250A::SetBinaryTransferByteOrderLSBFirst() 
{
	return WriteString("FORM:BORD SWAP");
}

bool CAgilent33250A::SetSweepModeLinear() 
{
	return WriteString("SWE:SPAC LIN");
}

bool CAgilent33250A::SetSweepModeLogarithmic() 
{
	return WriteString("SWE:SPAC LOG");
}

bool CAgilent33250A::SetSweepTime(double time) //in ms
{
	CString buf;
	buf.Format("SWE:TIME %.4f",time/1000); //conversion to sec
	return WriteString(buf);	
}

bool CAgilent33250A::SwitchSweep(bool OnOff) 
{
	if (OnOff) return WriteString("SWE:STAT ON");
	else return WriteString("SWE:STAT OFF");
}

bool CAgilent33250A::SetSweepStartStopFrequency(double StartFrequency, double StopFrequency) //Frequency in MHz
{
	CString buf;
	buf.Format("FREQ:STAR %.4f KHZ",StartFrequency*1000); //conversion to kHz
	bool err=WriteString(buf);	
	buf.Format("FREQ:Stop %.4f KHZ",StopFrequency*1000); //conversion to kHz
	return err && WriteString(buf);
}

bool CAgilent33250A::SetArbitraryFMWaveform(unsigned int NrPoints,double TimeStep, double aWaveform[]) { //Waveform in MHz
	if (!GPIB.HardwareAccess) return false;
	if (NrPoints!=8*1024) return Error("CAgilent33250A::SetArbitraryFMWaveform : not 8*1024 points");
	bool ok=true;
	ok=ok && SetBinaryTransferByteOrderLSBFirst();
	double MaxFrequency=aWaveform[0];
	ok=ok && SetFrequency(MaxFrequency/2);
	ok=ok && SetFMDeviation(MaxFrequency/2); 
	ok=ok && SetFMModulationFrequency(1.0/(NrPoints*TimeStep));//Timestep in s //conversion to Hz
	if (!ok) {
		return Error("Agilent33250A::SetArbitraryFMWaveform : error preparing waveform mode");
	}
	//ofstream out;
	//out.open(*DebugFilePath + "Debug.dat", CFile::modeCreate | CFile::modeWrite);
	//for (unsigned int l=0;l<NrPoints;l++) {
		//CString buf;
		//buf.Format("%i %.3f",l,aWaveform[l]);
		//out<<buf<<endl;
	//}
	//out.close();
//	ofstream out;
//	out.open(*DebugFilePath + "Debug.dat", CFile::modeCreate | CFile::modeWrite);	
	unsigned char *Waveform=new unsigned char[2*NrPoints+27];	
	CString buf;
	buf="DATA:DAC VOLATILE";
	for (unsigned int i=0;i<NrPoints;i++) {
		short int Value=(short int)(2047*(aWaveform[i]-MaxFrequency/2)/(MaxFrequency/2));
		CString buf2;
		buf2.Format(", %i",Value);
		buf=buf+buf2;
	//		CString buf;
	//		buf.Format("%u %lu",i,Value);
//		out<<buf2<<endl;		
		for (int j=0;j<2;j++) {
			Waveform[26+i*2+j]=(unsigned char)( 0xFF & Value);
			Value=Value>>8;
		}
	}
//	out<<endl;
//	out<<buf<<endl;
//	out.close();
	if (!WriteString(buf)) {
		delete Waveform;
		return  Error("Agilent33250A::SetArbitraryFMWaveform : error during DATA:DAC VOLATILE, ...");
	}
	delete Waveform;
	return true;

/*	WriteString("DATA:DAC VOLATILE, 2000, 1500, 1000, 500, 0, -500, -1000, -1500, -2000");*/
//	return true;
/*	char head[27]="DATA:DAC VOLATILE, #516384";
	for (i=0;i<26;i++) Waveform[i]=head[i];
	Waveform[2*NrPoints+26]=0;
	if (!WriteString(Waveform)) {
		delete Waveform;
		return  Error("Agilent33250A::SetArbitraryFMWaveform : error during DATA:DAC VOLATILE, #516384");
	}
	/*DWORD StartTickCount=GetTickCount();
	while (GetTickCount()<(StartTickCount+2)) ; 
	if (!SendData(Waveform,2*NrPoints)) {
		delete Waveform;
		return Error("Agilent33250A::SetArbitraryFMWaveform : could not send waveform");
	}*/
	//delete Waveform;
	//return true;	
}


bool CAgilent33250A::SetArbitraryWaveform(unsigned int NrPoints,double TimeStep, short int aWaveform[]) { //Waveform in 11 bit +1 bit sign format = -2048..2047, Timestep in Seconds
	if (!GPIB.HardwareAccess) return false;
	if (NrPoints>64*1024) return Error("CAgilent33250A::SetArbitraryWaveform : not 64*1024 points");
	bool ok=true;
	ok=ok && SetBinaryTransferByteOrderLSBFirst();	
	ok=ok && SetFrequency((1.0/(TimeStep*NrPoints))/1E6);	
	if (!ok) {
		return Error("Agilent33250A::SetArbitraryWaveform : error preparing waveform mode");
	}
	//ofstream out;
	//out.open(*DebugFilePath + "Debug.dat", CFile::modeCreate | CFile::modeWrite);
	//for (unsigned int l=0;l<NrPoints;l++) {
		//CString buf;
		//buf.Format("%i %.3f",l,aWaveform[l]);
		//out<<buf<<endl;
	//}
	//out.close();
//	ofstream out;
//	out.open(*DebugFilePath + "Debug.dat", CFile::modeCreate | CFile::modeWrite);	
	unsigned char *Waveform=new unsigned char[2*NrPoints+100];	
	CString buf;
//	buf.Format("DATA:DAC VOLATILE, #6%06u",2*NrPoints);	
	buf.Format("DATA:DAC VOLATILE, #6%06u ",2*NrPoints);	
	for (int j=0;j<buf.GetLength();j++) Waveform[j]=buf[j];
	int pos=buf.GetLength();
	//buf="DATA:DAC VOLATILE";
	for (unsigned int i=0;i<NrPoints;i++) {		
	//	CString buf2;
	//	buf2.Format(", %i",aWaveform[i]);
	//	buf=buf+buf2;
	//		CString buf;
	//		buf.Format("%u %lu",i,Value);
//		out<<buf2<<endl;		
		short int Value=aWaveform[i];
		for (int j=0;j<2;j++) {
			Waveform[pos]=(unsigned char)( 0xFF & Value);
			Value=Value>>8;
			pos++;
		}
	}
	Waveform[pos]=0;
//	out<<endl;
//	out<<buf<<endl;
//	out.close();
	if (!SendData(Waveform,pos)) {
		//delete Waveform;
		return  Error("Agilent33250A::SetArbitraryWaveform : error during DATA:DAC VOLATILE, ...");
	}
	delete[] Waveform;
	return true;

/*	WriteString("DATA:DAC VOLATILE, 2000, 1500, 1000, 500, 0, -500, -1000, -1500, -2000");*/
//	return true;
/*	char head[27]="DATA:DAC VOLATILE, #516384";
	for (i=0;i<26;i++) Waveform[i]=head[i];
	Waveform[2*NrPoints+26]=0;
	if (!WriteString(Waveform)) {
		delete Waveform;
		return  Error("Agilent33250A::SetArbitraryFMWaveform : error during DATA:DAC VOLATILE, #516384");
	}
	/*DWORD StartTickCount=GetTickCount();
	while (GetTickCount()<(StartTickCount+2)) ; 
	if (!SendData(Waveform,2*NrPoints)) {
		delete Waveform;
		return Error("Agilent33250A::SetArbitraryFMWaveform : could not send waveform");
	}*/
	//delete Waveform;
	//return true;	
}
