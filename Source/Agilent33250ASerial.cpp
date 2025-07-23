// Agilent33250ASerial.cpp: implementation of the CAgilent33250ASerial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "Agilent33250ASerial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAgilent33250ASerial::CAgilent33250ASerial(int aAddress,unsigned char SubPort,CString aName,CString aQuestion)
:CSerialDevice(aAddress, SubPort,9600/*baud*//*9600 *//*57600/*38400 19200 9600  115200*/,/*bits*/8,/*parity*/'N',/*stopbits*/1,/*Handshake 0=off, 1=DTR 2= RTS*/1,aName,aQuestion) {
	
}

CAgilent33250ASerial::~CAgilent33250ASerial()
{
	
}

bool CAgilent33250ASerial::SetFrequency(double Frequency) //Frequency in MHz
{
	CString buf;
	buf.Format("FREQ %.3f KHZ\r\n",Frequency*1000); //conversion to kHz
	return WriteString(buf);
}

bool CAgilent33250ASerial::SetAmplitudeVpp(double Amplitude)  //Amplitude in Vpp
{
	CString buf;
	buf.Format("VOLT %.5f VPP\r\n",Amplitude); 
	return WriteString(buf);
}

bool CAgilent33250ASerial::SetAmplitudedBm(double Amplitude)  //Amplitude in dBm
{
	CString buf;
	buf.Format("VOLT %.5f DBM\r\n",Amplitude); 
	return WriteString(buf);
}

bool CAgilent33250ASerial::SetOffset(double Offset)  //Offset in V
{
	CString buf;
	buf.Format("VOLT:OFFS %.3f V\r\n",Offset); 
	return WriteString(buf);
}

bool CAgilent33250ASerial::SetSine() 
{
	return WriteString("FUNC:SHAP SIN\r\n");
}

bool CAgilent33250ASerial::SetSquare() 
{
	return WriteString("FUNC:SHAP SQU\r\n");
}

bool CAgilent33250ASerial::SetRamp() 
{
	return WriteString("FUNC:SHAP RAMP\r\n");
}

bool CAgilent33250ASerial::SetPulse() 
{
	return WriteString("FUNC:SHAP Pulse\r\n");
}

bool CAgilent33250ASerial::SetNoise() 
{
	return WriteString("FUNC:SHAP NOIS\r\n");
}

bool CAgilent33250ASerial::SetDC() 
{
	return WriteString("FUNC:SHAP DC\r\n");
}

bool CAgilent33250ASerial::SetArbitrary() 
{
	return WriteString("FUNC:SHAP USER\r\n");
}

bool CAgilent33250ASerial::SelectArbitraryVolatile() 
{
	return WriteString("FUNC:USER VOLATILE\r\n");
}

bool CAgilent33250ASerial::SelectArbitraryExponentialRise() 
{
	return WriteString("FUNC:USER EXP_RISE\r\n");
}

bool CAgilent33250ASerial::SelectArbitraryExponentialFall() 
{
	return WriteString("FUNC:USER EXP_FALL\r\n");
}

bool CAgilent33250ASerial::SelectArbitraryNegativeRamp()
{
	return WriteString("FUNC:USER NEG_RAMP\r\n");
}

bool CAgilent33250ASerial::SelectArbitrarySinc()  
{
	return WriteString("FUNC:USER SINC\r\n");
}

bool CAgilent33250ASerial::SelectArbitraryCardiac()  
{
	return WriteString("FUNC:USER CARDIAC\r\n");
}
bool CAgilent33250ASerial::SwitchFM(bool OnOff) 
{
	if (OnOff) return WriteString("FM:STAT ON\r\n");
	else return WriteString("FM:STAT OFF\r\n");
}

bool CAgilent33250ASerial::SetFMSourceExternal() 
{
	return WriteString("FM:SOUR EXT\r\n");
}

bool CAgilent33250ASerial::SetFMSourceInternal() 
{
	return WriteString("FM:SOUR INT\r\n");
}

bool CAgilent33250ASerial::SetFMDeviation(double Deviation) //in MHz
{  //in external mode, +5V adds Deviation to the center frequency
   //and -5V substracts Deviation from the center frequency
	CString buf; 
	buf.Format("FM:DEV %.3f KHZ\r\n",Deviation*1000.0); //conversion to kHz
	return WriteString(buf);
}

bool CAgilent33250ASerial::SetTriggerExternal() 
{
	return WriteString("TRIG:SOUR EXT\r\n");
}

bool CAgilent33250ASerial::SetTriggerInternal() 
{
	return WriteString("TRIG:SOUR IMM\r\n");
}

bool CAgilent33250ASerial::SetTriggerGPIB() 
{
	return WriteString("TRIG:SOUR BUS\r\n");
}

bool CAgilent33250ASerial::SetTriggerSlopePositive() 
{
	return WriteString("TRIG:SLOP POS\r\n");
}

bool CAgilent33250ASerial::SetTriggerSlopeNegative() 
{
	return WriteString("TRIG:SLOP NEG\r\n");
}

bool CAgilent33250ASerial::Trigger() 
{
	return WriteString("*TRG\r\n");
}

bool CAgilent33250ASerial::SetFMCarrierSin() 
{	
	return WriteString("FUNC:SHAP SIN\r\n");
}

bool CAgilent33250ASerial::SetFMCarrierSquare() 
{	
	return WriteString("FM:SHAP SQU\r\n");
}

bool CAgilent33250ASerial::SetFMCarrierRamp() 
{	
	return WriteString("FM:SHAP RAMP\r\n");
}

bool CAgilent33250ASerial::SetFMCarrierUser() 
{	
	return WriteString("FM:SHAP USER\r\n");
}

bool CAgilent33250ASerial::SetFMModulationShapeUser() 
{	
	return WriteString("FM:INT:FUNC USER\r\n");
}

bool CAgilent33250ASerial::SetFMModulationShapeSin() 
{	
	return WriteString("FM:INT:FUNC SIN\r\n");
}

bool CAgilent33250ASerial::SetFMModulationShapeSquare() 
{	
	return WriteString("FM:INT:FUNC SQU\r\n");
}

bool CAgilent33250ASerial::SetFMModulationShapeRamp() 
{	
	return WriteString("FM:INT:FUNC RAMP\r\n");
}

bool CAgilent33250ASerial::SetFMModulationShapeNegativeRamp() 
{	
	return WriteString("FM:INT:FUNC NRAM\r\n");
}

bool CAgilent33250ASerial::SetFMModulationShapeTriangle() 
{	
	return WriteString("FM:INT:FUNC TRI\r\n");
}

bool CAgilent33250ASerial::SetFMModulationShapeNoise() 
{	
	return WriteString("FM:INT:FUNC NOIS\r\n");
}

bool CAgilent33250ASerial::SetFMModulationFrequency(double Frequency) //Frequency in Hz
{
	CString buf;
	buf.Format("FM:INT:FREQ %.8f HZ\r\n",Frequency); //conversion to Hz
	return WriteString(buf);
}

bool CAgilent33250ASerial::SetFMModulationSourceInternal() 
{
	return WriteString("FM:SOUR INT\r\n");
}

bool CAgilent33250ASerial::SetFMModulationSourceExternal() 
{
	return WriteString("FM:SOUR EXT\r\n");
}

bool CAgilent33250ASerial::SetBinaryTransferByteOrderMSBFirst() 
{
	return WriteString("FORM:BORD NORM\r\n");
}

bool CAgilent33250ASerial::SetBinaryTransferByteOrderLSBFirst() 
{
	return WriteString("FORM:BORD SWAP\r\n");
}

bool CAgilent33250ASerial::SetSweepModeLinear() 
{
	return WriteString("SWE:SPAC LIN\r\n");
}

bool CAgilent33250ASerial::SetSweepModeLogarithmic() 
{
	return WriteString("SWE:SPAC LOG\r\n");
}

bool CAgilent33250ASerial::SetSweepTime(double time) //in ms
{
	CString buf;
	buf.Format("SWE:TIME %.4f\r\n",time/1000); //conversion to sec
	return WriteString(buf);	
}

bool CAgilent33250ASerial::SwitchSweep(bool OnOff) 
{
	if (OnOff) return WriteString("SWE:STAT ON\r\n");
	else return WriteString("SWE:STAT OFF\r\n");
}

bool CAgilent33250ASerial::SetSweepStartStopFrequency(double StartFrequency, double StopFrequency) //Frequency in MHz
{
	CString buf;
	buf.Format("FREQ:STAR %.4f KHZ\r\n",StartFrequency*1000); //conversion to kHz
	bool err=WriteString(buf);	
	buf.Format("FREQ:Stop %.4f KHZ\r\n",StopFrequency*1000); //conversion to kHz
	return err && WriteString(buf);
}

bool CAgilent33250ASerial::SetArbitraryFMWaveform(unsigned int NrPoints,double TimeStep, double aWaveform[]) { //Waveform in MHz	
	if (!Serial.HardwareAccess) return false;
	if (NrPoints!=8*1024) return Error("CAgilent33250ASerial::SetArbitraryFMWaveform : not 8*1024 points");
	bool ok=true;
	ok=ok && SetBinaryTransferByteOrderLSBFirst();
	double MaxFrequency=aWaveform[0];
	ok=ok && SetFrequency(MaxFrequency/2);
	ok=ok && SetFMDeviation(MaxFrequency/2); 
	ok=ok && SetFMModulationFrequency(1.0/(NrPoints*TimeStep));//Timestep in s //conversion to Hz
	if (!ok) {
		return Error("Agilent33250ASerial::SetArbitraryFMWaveform : error preparing waveform mode");
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
		return  Error("Agilent33250ASerial::SetArbitraryFMWaveform : error during DATA:DAC VOLATILE, ...");
	}
	delete Waveform;
	return true;

/*	WriteString("DATA:DAC VOLATILE, 2000, 1500, 1000, 500, 0, -500, -1000, -1500, -2000\r\n");*/
//	return true;
/*	char head[27]="DATA:DAC VOLATILE, #516384";
	for (i=0;i<26;i++) Waveform[i]=head[i];
	Waveform[2*NrPoints+26]=0;
	if (!WriteString(Waveform)) {
		delete Waveform;
		return  Error("Agilent33250ASerial::SetArbitraryFMWaveform : error during DATA:DAC VOLATILE, #516384\r\n");
	}
	/*DWORD StartTickCount=GetTickCount();
	while (GetTickCount()<(StartTickCount+2)) ; 
	if (!SendData(Waveform,2*NrPoints)) {
		delete Waveform;
		return Error("Agilent33250ASerial::SetArbitraryFMWaveform : could not send waveform\r\n");
	}*/
	//delete Waveform;
	//return true;	
}


bool CAgilent33250ASerial::SetArbitraryWaveform(unsigned int NrPoints,double TimeStep, short int aWaveform[]) { //Waveform in 11 bit +1 bit sign format = -2048..2047, Timestep in Seconds
	if (!Serial.HardwareAccess) return false;
	if (NrPoints>64*1024) return Error("CAgilent33250ASerial::SetArbitraryWaveform : not 64*1024 points");
	bool ok=true;
	ok=ok && SetBinaryTransferByteOrderLSBFirst();	
	ok=ok && SetFrequency((1.0/(TimeStep*NrPoints))/1E6);	
	if (!ok) {
		return Error("Agilent33250ASerial::SetArbitraryWaveform : error preparing waveform mode");
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
	buf.Format("DATA:DAC VOLATILE, #6%06u",2*NrPoints);	
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
		return  Error("Agilent33250ASerial::SetArbitraryWaveform : error during DATA:DAC VOLATILE, ...");
	}
	delete[] Waveform;
	return true;

/*	WriteString("DATA:DAC VOLATILE, 2000, 1500, 1000, 500, 0, -500, -1000, -1500, -2000\r\n");*/
//	return true;
/*	char head[27]="DATA:DAC VOLATILE, #516384";
	for (i=0;i<26;i++) Waveform[i]=head[i];
	Waveform[2*NrPoints+26]=0;
	if (!WriteString(Waveform)) {
		delete Waveform;
		return  Error("Agilent33250ASerial::SetArbitraryFMWaveform : error during DATA:DAC VOLATILE, #516384\r\n");
	}
	/*DWORD StartTickCount=GetTickCount();
	while (GetTickCount()<(StartTickCount+2)) ; 
	if (!SendData(Waveform,2*NrPoints)) {
		delete Waveform;
		return Error("Agilent33250ASerial::SetArbitraryFMWaveform : could not send waveform\r\n");
	}*/
	//delete Waveform;
	//return true;	
}
