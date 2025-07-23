// TektronixAWG5105.cpp: implementation of the CTektronixAWG5105 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "TektronixAWG5105.h"
#include "DoublePoint.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const unsigned int StartAddress[2]={1,100000};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTektronixAWG5105::CTektronixAWG5105(int aAddress,CString aName,double aFrequency, unsigned int aNrChannelsUsed)
:CWaveformGenerator(aAddress,aName,aFrequency,aNrChannelsUsed) {
	VoltageOffset=0;
	VoltagePeakPeak=7;	
}

CTektronixAWG5105::~CTektronixAWG5105()
{
	
}

void CTektronixAWG5105::TransmitWaveform(unsigned int ChannelNr)
{
	bool CheckIt=false;
	SwitchOutput(ChannelNr,false);
	SetTimeBase(ChannelNr,Frequency);
	if (CheckIt) CheckReady();
	SetTriggerModeOneCycleTrigger(ChannelNr);
	SetTriggerModeGPIB(ChannelNr);
	if (CheckIt) CheckReady();
	SetVppAndOffset(ChannelNr,VoltagePeakPeak,VoltageOffset);
	if (CheckIt) CheckReady();
	SwitchOutput(ChannelNr,true);
	SetClockExternal(ChannelNr);
	if (CheckIt) CheckReady();
	unsigned int NrPoints=Waveform[ChannelNr]->GetSize();	
	if (NrPoints==0) {
		Waveform[ChannelNr]->Add(new CDoublePoint(0,StartVoltage[ChannelNr]));
		NrPoints=1;
	}
/*	ofstream *Out;
	Out=new ofstream("Debug.dat",ios::out);	
	double Time[100];
	double Voltage[100];
	for (unsigned int i=0;i<NrPoints;i++) {
		Time[i]=((CDoublePoint*)Waveform[ChannelNr]->GetAt(i))->Time;
		Voltage[i]=((CDoublePoint*)Waveform[ChannelNr]->GetAt(i))->Voltage;
		*Out<<Time[i]<<" "<<Voltage[i]<<endl;
	}
	delete Out;*/
	
	if (CheckIt) CheckReady();
	for (unsigned int i=0;i<(NrPoints-1);i++) {
		SetPoint(ChannelNr,((CDoublePoint*)Waveform[ChannelNr]->GetAt(i))->Time,((CDoublePoint*)Waveform[ChannelNr]->GetAt(i))->Voltage);
		if (CheckIt) CheckReady();
		SetPoint(ChannelNr,((CDoublePoint*)Waveform[ChannelNr]->GetAt(i+1))->Time-(1000.0/Frequency),((CDoublePoint*)Waveform[ChannelNr]->GetAt(i))->Voltage);
		if (CheckIt) CheckReady();
		DrawLine(ChannelNr,((CDoublePoint*)Waveform[ChannelNr]->GetAt(i))->Time,((CDoublePoint*)Waveform[ChannelNr]->GetAt(i+1))->Time-(1000.0/Frequency));
		CheckReady();
	}
	SetPoint(ChannelNr,((CDoublePoint*)Waveform[ChannelNr]->GetAt(NrPoints-1))->Time,((CDoublePoint*)Waveform[ChannelNr]->GetAt(NrPoints-1))->Voltage);
	if (CheckIt) CheckReady();
	SetPoint(ChannelNr,((CDoublePoint*)Waveform[ChannelNr]->GetAt(NrPoints-1))->Time+(10000.0/Frequency),((CDoublePoint*)Waveform[ChannelNr]->GetAt(NrPoints-1))->Voltage);
	if (CheckIt) CheckReady();
	DrawLine(ChannelNr,((CDoublePoint*)Waveform[ChannelNr]->GetAt(NrPoints-1))->Time,((CDoublePoint*)Waveform[ChannelNr]->GetAt(NrPoints-1))->Time+(10000.0/Frequency));
	CheckReady();
	SetMemoryStartEnd(ChannelNr,0,((CDoublePoint*)Waveform[ChannelNr]->GetAt(NrPoints-1))->Time+(10000.0/Frequency));	
	CheckReady();
	UpdateArbitraryWaveform();
	CheckReady();
	GPIBTrigger();
	CheckReady();
	SetTriggerModeExternal(ChannelNr);
	CheckReady();
}

void CTektronixAWG5105::AnalogOutDirect(unsigned int Channel, double Voltage) {
	CWaveformGenerator::AnalogOutDirect(Channel,Voltage);
	if (Channel>NrChannelsUsed) {
		ControlMessageBox("CTektronixAWG5105::AnalogOutDirect");
		return;
	}	
//	SetVppAndOffset(Channel,0,Voltage);
}

bool CTektronixAWG5105::SetVppAndOffset(unsigned int Channel, double Voltage, double Offset) {
	CString buf;
	if (Voltage>7.5) Voltage=7.5;
	if (Voltage<-7.5) Voltage=-7.5;
	if (Offset>7.5) Offset=7.5;
	if (Offset<-7.5) Offset=-7.5;	
	bool ok=(Channel<NrChannelsUsed);
	buf.Format("SOUR%u:VOLT:AMPL %fVPP;OFFS %fV",Channel+1,Voltage,Offset); 
	if (ok) ok=WriteString(buf);
	return ok;
	
}

unsigned int CTektronixAWG5105::GetMaxChannels()
{
	return 2;
}

bool CTektronixAWG5105::SetPoint(unsigned int Channel,double Time, double Voltage)  //Amplitude in Vpp
{	
	int Data=(int)(2047.0*(Voltage-VoltageOffset)/(VoltagePeakPeak));
	unsigned long Address=(unsigned long)((Time*0.001)*Frequency);
	bool ok=true;
	if (Data>2047) Data=2047;
	else if (Data<-2047) Data=-2047;
	if (Address<0) {
		Address=0;
		ok=false;
	}
	if (Address>65535) {
		Address=65535;
		ok=false;
	}
	if (!ok) ControlMessageBox("CTektronixAWG5105::SetPoint : Address out of range");
	CString buf;
	buf.Format(":ARB:ADDR %u",StartAddress[Channel]+Address); 
	ok=ok && WriteString(buf);
	buf.Format(":ARB:DATA %d",Data); 
	ok=ok && WriteString(buf);
	return ok;
}

bool CTektronixAWG5105::DrawLine(unsigned int Channel,double Time1, double Time2) { 
	bool ok=true;
	unsigned long Address1=(unsigned long)((Time1*0.001)*Frequency);
	unsigned long Address2=(unsigned long)((Time2*0.001)*Frequency);	
	if (Address1>65534) {
		Address1=65534;
		ok=false;
	}
	if (Address2>65535) {
		Address2=65535;
		ok=false;
	}
	if (Address2<=Address1) {
		Address2=Address1+1;
		ok=false;
	}
	CString buf;
	buf.Format(":ARB:DRAW %u,%u",StartAddress[Channel]+Address1,StartAddress[Channel]+Address2); 
	if (ok) ok=WriteString(buf);
	return ok;
}

bool CTektronixAWG5105::SetMemoryStartEnd(unsigned int Channel, double StartTime, double EndTime)
{
	bool ok=true;
	unsigned long Address1=(unsigned long)((StartTime*0.001)*Frequency);
	unsigned long Address2=(unsigned long)((EndTime*0.001)*Frequency);	
	if (Address1>65534) {
		Address1=65534;
		ok=false;
	}
	if (Address2>65535) {
		Address2=65535;
		ok=false;
	}
	if (Address2<=Address1) {
		Address2=Address1+1;
		ok=false;
	}
	CString buf;
	unsigned long length=Address2-Address1+1;
	if ((length%2)!=0) length--;
	buf.Format(":ARB%u:STAR %u;LENG %u",Channel+1,StartAddress[Channel]+Address1,length); 
	if (ok) ok=WriteString(buf);
	return ok;
}

void CTektronixAWG5105::Init()
{
	/*for (unsigned int i=0;i<NrChannelsUsed;i++) {		
		SwitchOutput(i,true);
	}*
	for (unsigned int i=0;i<NrChannelsUsed;i++) SetTriggerModeExternal(i);
	CheckReady();*/
}

bool CTektronixAWG5105::SetTriggerModeExternal(unsigned int Channel)
{
	CString buf;
	buf.Format(":TRIG%u:SOUR EXT; SLOP POS; DEL 0US; LEV 2V",Channel+1);	
	return WriteString(buf);

}

bool CTektronixAWG5105::SwitchOutput(unsigned int Channel, bool OnOff)
{	
	CString buf;
	buf.Format(":OUTP%u %s",Channel+1, (OnOff) ? "ON" : "OFF");	
	return WriteString(buf);
}

bool CTektronixAWG5105::SetTimeBase(unsigned int Channel, double Frequency)
{
	long SamplingInterval=(long)(1E9/Frequency);
	bool ok=(SamplingInterval>=20) && (SamplingInterval<1500000000);
	if (!ok) ControlMessageBox("CTektronixAWG5105::SetTimeBase : invalid frequency");
	CString buf;
	buf.Format(":ARB%u:PRAT %dNS",Channel+1,SamplingInterval);	
	return WriteString(buf);
}

bool CTektronixAWG5105::SetTriggerModeOneCycleTrigger(unsigned int Channel)
{
	CString buf;
	buf.Format(":TRIG%u:MODE TRIG",Channel+1);	
	return WriteString(buf);
}

bool CTektronixAWG5105::SetTriggerModeGPIB(unsigned int Channel)
{	
	CString buf;
	buf.Format(":TRIG%u:SOUR BUS",Channel+1);	
	return WriteString(buf);
}

bool CTektronixAWG5105::SetClockExternal(unsigned int Channel)
{
	CString buf;
	buf.Format(":SOUR%u:ROSC:SOUR EXT",Channel+1);	
	return WriteString(buf);
}


void CTektronixAWG5105::InitFirstTime()
{	
	/*SetPrimaryAddress();
	ConfigureEOSMode(0x140A);
	EnableEOIMode();
	RemoteEnable();*/
	CheckIdentity();
	WriteString("*PSC 1");
	WriteString("*RST");
	WriteString("*SRE 0");
	WriteString("*ESE 255");
	WriteString("*CLS");
	WriteString(":ARB:AUPD OFF");
	CheckReady();
}

bool CTektronixAWG5105::SetAmplitude(double Amplitude)
{
	CString buf;
	buf.Format(":VOLT:AMPL %.3f",Amplitude);	
	return WriteString(buf);
}

bool CTektronixAWG5105::UpdateArbitraryWaveform()
{
	return WriteString(":ARB:UPD");
}

bool CTektronixAWG5105::CheckReady()
{
	bool ok=WriteString("*STB?");
	CString buf;
	ReadString(buf);
	buf=buf.Left(buf.GetLength()-1);
	unsigned int ErrorCode=atoi(buf);
	ErrorCode=ErrorCode & 0xff;//7e;
	ok= ((ErrorCode==0) && ok);
	if (!ok) ControlMessageBox("CTektronixAWG5105::CheckReady : Error");
	return ok;
}

bool CTektronixAWG5105::CheckIdentity()
{
	bool ok=WriteString("*IDN?");
	CString buf;
	ReadString(buf);
	buf=buf.Left(17);
	ok= (buf=="TEKTRONIX,AWG5105" && ok);
	if (!ok) ControlMessageBox("CTektronixAWG5105::CheckIdentity : Error");
	return ok;
}