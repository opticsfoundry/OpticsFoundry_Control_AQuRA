// TektronixTDS524A.cpp: implementation of the CTektronixTDS524A class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Control.h"
#include "TektronixTDS524A.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTektronixTDS524A::CTektronixTDS524A(int aAddress,CString aName,CString aQuestion)
:CGPIBDevice(aAddress,aName,aQuestion)
{
	AktChannel=1;
}

CTektronixTDS524A::~CTektronixTDS524A()
{

}

bool CTektronixTDS524A::GetFrequency(unsigned int Channel,double &Frequency)
{
	Frequency=0;
	if ((Channel>15) || (Channel==0)) return false;
	CString buf;
	DWORD start=GetTickCount();
	while ((GetTickCount()-start)<500);
	buf.Format("FN%i",Channel); 
	if (!WriteString(buf)) return false;//Error("CTektronixTDS524A::ReadFrequency : error during FN");	
	start=GetTickCount();
	while ((GetTickCount()-start)<2000);
	if (!ReadString(buf,19)) return false;//Error("CTektronixTDS524A::ReadFrequency : timeout during FN");
	//Format:"F    +9.9999999E+06"
	buf.SetAt(0,' ');
	Frequency=atof(buf);
	return true;
}

bool CTektronixTDS524A::SetSourceChannel(int channel) 
{	
	switch (channel) {
		case 1:
			AktChannel=1;
			return WriteString("DATA:SOURCE CH1");
		break;
		case 2:
			AktChannel=2;
			return WriteString("DATA:SOURCE CH2");
		break;
		default: return false;
	}	
}

bool CTektronixTDS524A::SetEncodingBinary() 
{
	return WriteString("DAT:ENC RIB");
}

bool CTektronixTDS524A::SetDataWidth1Byte() 
{
	return WriteString("DAT:WID 1");
}

bool CTektronixTDS524A::SetDataStart() 
{
	return WriteString("DAT:STAR 1");
}

bool CTektronixTDS524A::SetDataStop() 
{
	return WriteString("DAT:Stop 15000");
}

bool CTektronixTDS524A::GetWaveformNrPoints(long &length) 
{
	CString buf;
	buf.Format("WFMP:CH%i:NR_P?",AktChannel);
	bool ok=WriteString(buf);	
	if (ok) {
		ok=ReadString(buf);
		length=atoi(buf);
	}
	return ok;
}

bool CTektronixTDS524A::GetTriggerPosition(long &position) 
{
	CString buf;
	buf.Format("WFMP:CH%i:PT_OFF?",AktChannel);
	bool ok=WriteString(buf);	
	if (ok) {
		ok=ReadString(buf);
		position=atoi(buf);
	}
	return ok;
}

bool CTektronixTDS524A::GetYMultiplier(double &YMultiplier) 
{
	CString buf;
	buf.Format("WFMP:CH%i:YMU?",AktChannel);
	bool ok=WriteString(buf);	
	if (ok) {
		ok=ReadString(buf);
		YMultiplier=atof(buf);
	}
	return ok;
}

bool CTektronixTDS524A::GetYOffset(double &YOffset) 
{
	CString buf;
	buf.Format("WFMP:CH%i:YOF?",AktChannel);
	bool ok=WriteString(buf);	
	if (ok) {
		ok=ReadString(buf);
		YOffset=atof(buf);
	}
	return ok;
}


bool CTektronixTDS524A::GetXMultiplier(double &XMultiplier) 
{
	CString buf;
	buf.Format("WFMP:CH%i:XIN?",AktChannel);
	bool ok=WriteString(buf);	
	if (ok) {
		ok=ReadString(buf);
		XMultiplier=atof(buf);
	}
	return ok;
}

bool CTektronixTDS524A::GetXUnits(CString &XUnits) 
{
	CString buf;
	buf.Format("WFMP:CH%i:XUN?",AktChannel);
	bool ok=WriteString(buf);	
	if (ok) {
		ok=ReadString(XUnits);	
	}
	return ok;
}

bool CTektronixTDS524A::GetYUnits(CString &YUnits) 
{
	CString buf;
	buf.Format("WFMP:CH%i:YUN?",AktChannel);
	bool ok=WriteString(buf);	
	if (ok) {
		ok=ReadString(YUnits);	
	}
	return ok;
}

bool CTektronixTDS524A::GetWaveform(int channel,double &DeltaTime, CString &XUnits, 
									CString &YUnits, long &TriggerPosition, long &NrPoints, 
									double *&waveform) 
{
	waveform=NULL;
	NrPoints=0;
	DeltaTime=0;
	TriggerPosition=0;
	XUnits="";
	YUnits="";
	if ((channel>2) || (channel<1)) return false;
	bool ok=SetSourceChannel(channel);
	ok=ok && SetEncodingBinary();
	ok=ok && SetDataWidth1Byte();
	ok=ok && SetDataStart();
	ok=ok && SetDataStop();
	long length;		
	ok=ok && GetTriggerPosition(TriggerPosition);	
	double YMultiplier;
	ok=ok && GetYMultiplier(YMultiplier);
	ok=ok && GetXMultiplier(DeltaTime);	
	ok=ok && GetXUnits(XUnits);	
	ok=ok && GetYUnits(YUnits);	
	double YOffset;
	ok=ok && GetYOffset(YOffset);
	ok=ok && GetWaveformNrPoints(length);
	WriteString("CURVE?");	
	//DWORD Start=GetTickCount();
	//while ((GetTickCount()-Start)<200);
	unsigned char* buf;
	ok=ok && ReadData(buf,length+5);	
	if ((ok) && (buf)) {
		waveform=new double[length];
		NrPoints=length;
		for (int i=0;i<length;i++) {
			int help=(char)buf[i+5];
			waveform[i]=(help-YOffset)*YMultiplier;
		}
	}	
	//weird timimng behavior seems to make this neccesary
	ok=ok && GetYMultiplier(YMultiplier);	
	ok=ok && GetXUnits(XUnits);	
	ok=ok && GetYUnits(YUnits);	
	ok=ok && GetWaveformNrPoints(length);
	ok=ok && GetTriggerPosition(TriggerPosition);
	if (buf) delete buf;
	TriggerPosition--;
	return ok;
}
