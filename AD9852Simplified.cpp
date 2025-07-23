// AD9852.cpp: implementation of the CAD9852 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "control.h"
#include "AD9852.h"
#include "MultiIO.h"
#include "MultiWriteDevice.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

unsigned long AD9852MasterResetValueContents[AD9852MaxValues]={0   ,   0,   0,   0,   0,0x40,   0,   0,   0,   0,0x10,0x64,0x01,0x20,   0,   0,   0};
unsigned char AD9852ValueLengthGlobal[AD9852MaxValues]=             {2   ,   2,   6,   6,   6,   4,   3,   2,   1,   2,   1,   1,   1,   1,   1,   1,   1};
unsigned char AD9852ValueBaseAddressGlobal[AD9852MaxValues]=        {0x00,0x02,0x04,0x0A,0x10,0x16,0x1A,0x21,0x25,0x26,0x1D,0x1E,0x1F,0x20,0x30,0x31,0x32};
const unsigned long AD9852ValueNotInBusBuffer=99999;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAD9852::CAD9852(unsigned short aBus,unsigned long aBaseAddress, double aExternalClockSpeed, unsigned char aPLLReferenceMultiplier, double aFrequencyMultiplier, CMultiIO* aMyMultiIO, bool aIndependentUpdate)
: CMultiWriteDevice() {

	for (int n = 0; n < AD9852MaxValues; n++) {
		if (aIndependentUpdate) {
			AD9852ValueLength[n] = AD9852ValueLengthGlobal[n] + 1;
			AD9852ValueBaseAddress[n] = AD9852ValueBaseAddressGlobal[n] - 1;
		}
		else {
			AD9852ValueLength[n] = AD9852ValueLengthGlobal[n];
			AD9852ValueBaseAddress[n] = AD9852ValueBaseAddressGlobal[n];
		}
	}
	IndependentUpdate = aIndependentUpdate;

	RequestedStartFrequency=0;
	RequestedStopFrequency=0;
	RequestedModulationFrequency=0.02;
	SortedStartFrequency=RequestedStartFrequency;
	SortedStopFrequency=RequestedStopFrequency;
	SortedModulationFrequency=RequestedModulationFrequency;
	LastStartFrequency=SortedStartFrequency;
	LastStopFrequency=SortedStopFrequency;
	LastModulationFrequency=SortedModulationFrequency;
	LastFSKMode=2;
	ForceUpdate=false;

	Bus=aBus;
	PLLReferenceMultiplier=aPLLReferenceMultiplier;
	PLLEnabled=PLLReferenceMultiplier!=1;
	InputClockSpeed=1E6*aExternalClockSpeed; //conversion MHz to Hz
	FrequencyMultiplier=aFrequencyMultiplier;  //This is the external frequency doubler option
	ClockSpeed=InputClockSpeed*((PLLEnabled) ? PLLReferenceMultiplier : 1);
	MaxFrequency=FrequencyMultiplier*(ClockSpeed*0.45E-6);
	//281474976710656=2^48
	FrequencyScale=281474976710656.0*((1.0/FrequencyMultiplier)*1.0E6/ClockSpeed);
	BaseAddress=aBaseAddress;
	MultiIOAddress=Bus+(unsigned char)BaseAddress*4; //this 
	MyMultiIO=aMyMultiIO;
	for (int i=0;i<AD9852MaxValues;i++) {
		AktValueContents[i]=AD9852MasterResetValueContents[i];
		ValueInBusBuffer[i]=AD9852ValueNotInBusBuffer;
		WritePrecision[i]=AD9852ValueLength[i];
	}
	for (unsigned int i=0;i<AD9852MaxBusBuffer;i++) BusBuffer[i]=0; 
	BusBufferStart=0;
	BusBufferEnd=0;
	BusBufferLength=0;
	UpdateRegistersModeAutomatic=true;

	AktValueContentsWritten=0;
	AktValueNrWritten=99;
	AktSubAddressWritten=0;
	ShapedKeying=false;
	FSK_BPSK_Hold=false;
	if (aBaseAddress>=64) {
		CString buf;
		buf.Format("CAD9852::CAD9852 : BaseAdress too high (BaseAddress=%x Bus=%x)",BaseAddress,Bus>>BusBitShift);
		ControlMessageBox(buf);
	}
}

CAD9852::~CAD9852()
{

}

void CAD9852::InitPLL() {
	SetPLLReferenceMultiplier(PLLReferenceMultiplier);
	SetBypassPLL(PLLReferenceMultiplier==1);
	SetPLLRange(true);
}

void CAD9852::FrequencyRamp(double StartFrequency, double StopFrequency, double RampTime) { //Frequencies in MHz, RampTime in ms
	if (StartFrequency<StopFrequency) {
		SetFrequencyTuningWord1(StartFrequency);
		SetFrequencyTuningWord2(StopFrequency);

	}
}

bool CAD9852::WriteToBus()
{	
	if (!Enabled) return false;
	if ((BusBufferLength==0) && (AktValueNrWritten==99)) return false;	
	if (AktValueNrWritten==99) {
		AktValueNrWritten=BusBuffer[BusBufferStart];
		AktValueContentsWritten=AktValueContents[AktValueNrWritten];
		int help=AD9852ValueLength[AktValueNrWritten]-WritePrecision[AktValueNrWritten];
		AktSubAddressWritten=(help>0) ? help : 0;
	    ValueInBusBuffer[AktValueNrWritten]=AD9852ValueNotInBusBuffer;
		BusBufferLength--;
		BusBufferStart++;
		if (BusBufferStart>=AD9852MaxBusBuffer) BusBufferStart=0;
	}	
	unsigned short MultiIOAddress=Bus+(unsigned char)BaseAddress*4; //this 	
	//UpdateRegisters: 		 Base: 0x30 Length: 1                ValueNr: x+128
	//MasterReset:			 Base: 0x31 Length: 1                ValueNr: 15
	//LoadLatches:           Base: 0x32 Length: 1                ValueNr: 16 
	//Load data to buffer for all other ValueNr
	
	if (AktValueNrWritten == 14) MultiIOAddress += 2; //Update Output (or load+update de[pending on hardware) 2
	else if (AktValueNrWritten == 15); // MultiIOAddress += 0;   // Master Reset Should be 0!!!!!!!!!!!!!!!!!!!!
	else if (AktValueNrWritten==16) MultiIOAddress+=3; // No extra strobe ???????
	else MultiIOAddress+=1;	//Load data into registers MultiIOAddress+=1

	unsigned char AktRegisterWritten=AD9852ValueBaseAddress[AktValueNrWritten]+AD9852ValueLength[AktValueNrWritten]-AktSubAddressWritten-1;
	if (ShapedKeying) AktRegisterWritten+=0x80;	
	if (FSK_BPSK_Hold) AktRegisterWritten+=0x40;			
	unsigned char AktByteWritten=(unsigned char)((AktValueContentsWritten >> (8*AktSubAddressWritten)) & 0xFF);
	AktSubAddressWritten++;
	if (AktSubAddressWritten>=AD9852ValueLength[AktValueNrWritten]) {
		//we produce a "update registers" signal during the same 
		//bus cycle as the last "Load data into IO buffer" signal and don´t need an additional
		//bus cycle just for that
		if ((AktValueNrWritten!=15) && (AktValueNrWritten!=16)) MultiIOAddress+=1;
		AktValueNrWritten=99;
	}
	ASSERT(MyMultiIO);
	MyMultiIO->WriteToBusBuffer(MultiIOAddress,AktRegisterWritten + (AktByteWritten<<8));
	return true;
}

//conversions from double to long int etc have yet to be programmed
unsigned short CAD9852::SetPhaseAdjustRegister1(unsigned short aPhaseAdjustRegister1, bool GetValue) {
	return (unsigned short)SetValue(0,aPhaseAdjustRegister1,GetValue);
}

unsigned short CAD9852::SetPhaseAdjustRegister2(unsigned short aPhaseAdjustRegister2, bool GetValue) {
	return (unsigned short)SetValue(1,aPhaseAdjustRegister2,GetValue);
}


////Frequency in MHz, ClockSpeed in Hz
//double CAD9852::SetFrequencyTuningWord1(double Frequency, bool GetValue) {
//	if (!Enabled) return 0;
//	__int64 FrequencyTuningWord1=0;
//	if (GetValue) {
//		 FrequencyTuningWord1=SetValue(2,FrequencyTuningWord1,true);
//		 return FrequencyMultiplier*((FrequencyTuningWord1*ClockSpeed)/0x1000000000000)/1E6;
//	} else {
//		if (Frequency>MaxFrequency) {			
//			CString buf;
//			buf.Format("CAD9852::SetFrequencyTuningWord1 : frequency out of range (BaseAddress=%x Bus=%x) fwanted=%.f MHz fmax=%.f MHz",BaseAddress,Bus>>BusBitShift,Frequency,MaxFrequency);
//			ControlMessageBox(buf);
//		}		
//		FrequencyTuningWord1=(__int64)(Frequency*FrequencyScale);
//		SetValue(2,FrequencyTuningWord1,false);
//		return Frequency;
//	}
//}

//Frequency in MHz, ClockSpeed in Hz
double CAD9852::SetFrequencyTuningWord1(double Frequency, bool GetValue) {
	if (!Enabled) return 0;
	__int64 FrequencyTuningWord1=0;
	CString buf;

	if (GetValue) {
		 FrequencyTuningWord1=SetValue(2,FrequencyTuningWord1,true);
		 return FrequencyMultiplier*((FrequencyTuningWord1*ClockSpeed)/0x1000000000000)/1E6;
	} else {
		if (Frequency>MaxFrequency) {
			buf.Format("CAD9852::SetFrequencyTuningWord1 : frequency out of range (BaseAddress=%x Bus=%x) fwanted=%.f MHz fmax=%.f MHz",BaseAddress,Bus>>BusBitShift,Frequency,MaxFrequency);
			ControlMessageBox(buf);
		}

//		if (RequestedStartFrequency!=Frequency) {
			//RequestedStartFrequency=Frequency;
			FrequencyTuningWord1=(__int64)(Frequency*FrequencyScale);
			SetValue(2,FrequencyTuningWord1,false);
			LastStartFrequency=Frequency;

			//buf.Format("%f MHz f2=%f MHz",LastStartFrequency,Frequency);
			//ControlMessageBox(buf);
			//UpdateFrequencyValues();
//		}
		return RequestedStartFrequency;
	}
}

//Frequency in MHz, ClockSpeed in Hz
//value is transferred as double so that it's compatible with COutput. It should be ok, as a double contains more than 48 significant bits.
double CAD9852::SetFrequencyTuningWord1AsBinary(double _FrequencyTuningWord, bool GetValue) {
	if (!Enabled) return 0;
	__int64 FrequencyTuningWord = _FrequencyTuningWord;
	CString buf;

	if (GetValue) {
		return SetValue(2, FrequencyTuningWord, true);
	}
	else {
		SetValue(2, FrequencyTuningWord, false);
		return _FrequencyTuningWord;
	}
}

//Frequency in MHz, ClockSpeed in Hz
//value is transferred as double so that it's compatible with COutput. It should be ok, as a double contains more than 48 significant bits.
double CAD9852::SetFrequencyTuningWord2AsBinary(double _FrequencyTuningWord, bool GetValue) {
	if (!Enabled) return 0;
	__int64 FrequencyTuningWord = _FrequencyTuningWord;
	CString buf;

	if (GetValue) {
		return SetValue(3, FrequencyTuningWord, true);
	}
	else {
		SetValue(3, FrequencyTuningWord, false);
		return _FrequencyTuningWord;
	}
}





//for ramped FSK we need to do
//1) set FSK bit to low
//2) enter Mode 010
//3) Program Start Frequency FT1
//4) Program End Frequency FT2
//5) Program Ramp Rate clock
//6) Program Delta Frequency Word
//7) Set triangle bit high

void CAD9852::FrequencyModulation(double StartFrequency, double StopFrequency, double ModulationFrequency) { //Frequencies in MHz
	SetFSK_BPSK_Hold(/*aFSK_BPSK_Hold*/ false, /*GetValue*/ false);
	SetModeRampedFrequencyShiftKeying(/*Set*/ true,/*GetValue*/ false);
	//SetModeFrequencyShiftKeying(/*Set*/ true,/*GetValue*/ false);
	SetFrequencyTuningWord1(StartFrequency,/*GetValue*/ false);
	SetFrequencyTuningWord2(StopFrequency,/*GetValue*/ false);
	SetRampRateClock(/*RampRateClock*/1,/*GetValue*/ false);
	SetModulationFrequency(ModulationFrequency, /*GetValue*/ false);
	SetTriangle(/*Triangle*/ true,/*GetValue*/ false);
}

//double CAD9852::SetFrequencyTuningWord2(double Frequency, bool GetValue) {
//	if (!Enabled) return 0;
//	__int64 FrequencyTuningWord2=0;
//	if (GetValue) {
//		 FrequencyTuningWord2=SetValue(3,FrequencyTuningWord2,true);
//		 return FrequencyMultiplier*((FrequencyTuningWord2*ClockSpeed)/0x1000000000000)/1E6;
//	} else {
//		if (Frequency>MaxFrequency) {
//			CString buf;
//			buf.Format("CAD9852::SetFrequencyTuningWord2 : frequency out of range (BaseAddress=%x Bus=%x) fwanted=%.f MHz fmax=%.f MHz",BaseAddress,Bus>>BusBitShift,Frequency,MaxFrequency);
//			ControlMessageBox(buf);			
//		}				
//		FrequencyTuningWord2=(__int64)(Frequency*FrequencyScale);
//		RequestedStopFrequency=Frequency;
//		SetValue(3,FrequencyTuningWord2,false);
//		LastStopFrequency=Frequency;
//		return Frequency;
//	}
//}

double CAD9852::SetFrequencyTuningWord2(double Frequency, bool GetValue) {
	if (!Enabled) return 0;
	__int64 FrequencyTuningWord2=0;
	__int64 DeltaFrequencyWord=0;
	unsigned int ClockUpdateSteps=0;
//	double NewLastModulationFrequency;

	CString buf;

	if (GetValue) {
		 FrequencyTuningWord2=SetValue(3,FrequencyTuningWord2,true);
		 return FrequencyMultiplier*((FrequencyTuningWord2*ClockSpeed)/0x1000000000000)/1E6;
	} else {
		if (Frequency>MaxFrequency) {
			buf.Format("CAD9852::SetFrequencyTuningWord2 : frequency out of range (BaseAddress=%x Bus=%x) fwanted=%.f MHz fmax=%.f MHz",BaseAddress,Bus>>BusBitShift,Frequency,MaxFrequency);
			ControlMessageBox(buf);			
		}				
		FrequencyTuningWord2 = (__int64)(SortedStopFrequency * FrequencyScale);
		SetValue(3, FrequencyTuningWord2, false);		// FTW2 = SetValue(3,FrequencyTuningWord2, bool GetValue);
		return Frequency;
	}
}


unsigned long CAD9852::SetRampRateClock(unsigned long aRampRateClock, bool GetValue) {	
	if (GetValue) {
		return aRampRateClock=(unsigned long)SetValue(6,aRampRateClock,GetValue);
	} else {
		if ((aRampRateClock<1) || (aRampRateClock>1048576)) {
			CString buf;
			buf.Format("CAD9852::SetRampRateClock : Ramp rate clock out of range (BaseAddress=%x Bus=%x) aRampRateClock=%u",BaseAddress,Bus>>BusBitShift,aRampRateClock);
			ControlMessageBox(buf);	
			aRampRateClock=1;
		}
		return (unsigned long)SetValue(6,aRampRateClock,GetValue);

	}
}

__int64 CAD9852::SetDeltaFrequencyWord(__int64 aDeltaFrequencyWord, bool GetValue) {
	return SetValue(4,aDeltaFrequencyWord,GetValue);
}

double CAD9852::SetRampTime(double RampTime, bool GetValue) {  //RampTime in ms
	if (GetValue) {
		return SetModulationFrequency(0.5*0.001/RampTime, /*GetValue*/ true);
	} else {
		SetModulationFrequency(0.5*0.001/RampTime, /*GetValue*/ false);
		return RampTime;
	}
}

double CAD9852::SetModulationFrequency(double ModulationFrequency, bool GetValue) {  //ModulationFrequency in MHz
	if (!Enabled) return 0;
	__int64 FrequencyTuningWord=0;
	if (GetValue) {
		 FrequencyTuningWord=SetDeltaFrequencyWord(FrequencyTuningWord,true);
		 //Convert back from 48 bit twos complement
		 //281474976710656=2^48
		 FrequencyTuningWord=281474976710656-FrequencyTuningWord+1;
		 ModulationFrequency=((AktValueContents[3]-AktValueContents[2])*(FrequencyTuningWord/((AktValueContents[6]+1)*ClockSpeed)));
		 return ModulationFrequency/1E6;
	} else {
		if (ModulationFrequency<0) {
			CString buf;
			buf.Format("CAD9852::SetModulationFrequency : modulation frequency out of range (BaseAddress=%x Bus=%x) fmodwanted=%.f Hz",BaseAddress,Bus>>BusBitShift,ModulationFrequency);
			ControlMessageBox(buf);			
			ModulationFrequency=1;
		}
						
		FrequencyTuningWord=(__int64)((AktValueContents[3]-AktValueContents[2])*((ModulationFrequency*1E6)/(0.5*ClockSpeed/(AktValueContents[6]+1))));
		if (FrequencyTuningWord>(281474976710656/2)) {
			CString buf;
			buf.Format("CAD9852::SetModulationFrequency : frequency out of range (BaseAddress=%x Bus=%x) fwanted=%.f MHz",BaseAddress,Bus>>BusBitShift,ModulationFrequency);
			ControlMessageBox(buf);			
		}
		//Calculate 48bit twos complement
		//281474976710656=2^48
		FrequencyTuningWord=(0x1000000000000-FrequencyTuningWord) & 0xFFFFFFFFFFFF;
		SetDeltaFrequencyWord(FrequencyTuningWord,false);
		return ModulationFrequency;
	}
}

unsigned long CAD9852::SetUpdateClock(unsigned long aUpdateClock, bool GetValue) {
	return (unsigned long)SetValue(5,aUpdateClock,GetValue);
}

unsigned short CAD9852::SetOutputShapeKeyMult(unsigned short aOutputShapeKeyMult, bool GetValue) {
	return (unsigned short)SetValue(7,aOutputShapeKeyMult,GetValue);
}

unsigned char CAD9852::SetOutputShapeKeyRampRate(unsigned char aOutputShapeKeyRampRate, bool GetValue) {
	return (unsigned char)SetValue(8,aOutputShapeKeyRampRate,GetValue);
}

unsigned short CAD9852::SetControlDAC(unsigned short aControlDAC, bool GetValue) {
	return (unsigned short)SetValue(9,aControlDAC,GetValue);
}

__int64 CAD9852::SetValue(unsigned char ValueNr, __int64 Value, bool GetValue)
{
	if (!Enabled) return 0;
	if (ValueNr>=AD9852MaxValues) return 0;
	if (GetValue) {
		return AktValueContents[ValueNr];
	} else {
		if ((AktValueContents[ValueNr]!=Value) || (ForceWriting)) {
			if (BusBufferLength>=AD9852MaxBusBuffer) {				
				CString buf;
				buf.Format("CAD9852::SetValue : Bus Buffer exceeded (BaseAddress=%x Bus=%x)",BaseAddress,Bus>>BusBitShift);
				ControlMessageBox(buf);
				return 0;
			}			
			if (ValueInBusBuffer[ValueNr]!=AD9852ValueNotInBusBuffer) {
				AktValueContents[ValueNr]=Value;				
			} else {
				AktValueContents[ValueNr]=Value;
				BusBuffer[BusBufferEnd]=ValueNr;
				ValueInBusBuffer[ValueNr]=BusBufferEnd;
				BusBufferLength++;
				BusBufferEnd++;
				if (BusBufferEnd>=AD9852MaxBusBuffer) BusBufferEnd=0;			
				if (UpdateRegistersModeAutomatic) UpdateRegisters();
			}
		}
	}
	return 0;
}

void CAD9852::MasterReset() {
	if (!Enabled) return;
	if (BusBufferLength>=AD9852MaxBusBuffer) {		
		CString buf;
		buf.Format("CAD9852::SetValue : Bus Buffer exceeded (BaseAddress=%x Bus=%x)",BaseAddress,Bus>>BusBitShift);
		ControlMessageBox(buf);
		return;
	}	

	for (int i=0;i<AD9852MaxValues;i++) {
		AktValueContents[i]=AD9852MasterResetValueContents[i];
		ValueInBusBuffer[i]=AD9852ValueNotInBusBuffer;
		WritePrecision[i]=AD9852ValueLength[i];
	}
	for (unsigned int i=0;i<AD9852MaxBusBuffer;i++) BusBuffer[i]=0; 
	AktValueContentsWritten=0;
	AktValueNrWritten=99;
	AktSubAddressWritten=0;
	ShapedKeying=false;
	FSK_BPSK_Hold=false;	

	//delete also buffer
	BusBufferStart=0;
	BusBufferEnd=0;
	BusBufferLength=0;
	BusBuffer[BusBufferEnd]=15;	//MasterReset: BaseAddress*4+0
	BusBufferLength++;
	BusBufferEnd++;			
	if (BusBufferEnd>=AD9852MaxBusBuffer) BusBufferEnd=0;			
}

void CAD9852::UpdateRegisters() {
	if (!Enabled) return;
	if (BusBufferLength>=AD9852MaxBusBuffer) {		
		CString buf;
		buf.Format("CAD9852::UpdateRegisters : Bus Buffer exceeded (BaseAddress=%x Bus=%x)",BaseAddress,Bus>>BusBitShift);
		ControlMessageBox(buf);
		return;
	}		
}

void CAD9852::LoadLatches() {
	if (!Enabled) return;
	if (BusBufferLength>=AD9852MaxBusBuffer) {		
		CString buf;
		buf.Format("CAD9852::LoadLatches : Bus Buffer exceeded (BaseAddress=%x Bus=%x)",BaseAddress,Bus>>BusBitShift);
		ControlMessageBox(buf);
		return;
	}	
	BusBuffer[BusBufferEnd]=16;	
	BusBufferLength++;
	BusBufferEnd++;
	if (BusBufferEnd>=AD9852MaxBusBuffer) BusBufferEnd=0;			
}

void CAD9852::SetUpdateRegistersModeAutomatic() {
	if (!Enabled) return;
	UpdateRegistersModeAutomatic=true;
}

void CAD9852::SetUpdateRegistersModeManual() {
	if (!Enabled) return;
	UpdateRegistersModeAutomatic=false;
}

//Control Registers:
bool CAD9852::SetComparatorPowerDown(bool aPowerDown, bool GetValue) {
	return SetControlBit(0,4,aPowerDown,GetValue);	
}

bool CAD9852::SetControlDACPowerDown(bool aPowerDown, bool GetValue) {
	return SetControlBit(0,2,aPowerDown,GetValue);		
}

bool CAD9852::SetDACPowerDown(bool aPowerDown, bool GetValue) {
	return SetControlBit(0,1,aPowerDown,GetValue);			
}

bool CAD9852::SetDigitalPowerDown(bool aPowerDown, bool GetValue) {
	return SetControlBit(0,0,aPowerDown,GetValue);				
}

bool CAD9852::SetPLLRange(bool aPLLRange, bool GetValue) {
	return SetControlBit(1,6,aPLLRange,GetValue);					
}

bool CAD9852::SetBypassPLL(bool aByPassPLL, bool GetValue) {
	if (!GetValue) {
		PLLEnabled=!aByPassPLL;
		ClockSpeed=InputClockSpeed*((PLLEnabled) ? PLLReferenceMultiplier : 1);	
		MaxFrequency=FrequencyMultiplier*(ClockSpeed*0.45E-6);
		//281474976710656=2^48
		FrequencyScale=281474976710656.0*((1.0/FrequencyMultiplier)*1.0E6/ClockSpeed);
	}
	return SetControlBit(1,5,aByPassPLL,GetValue);						
}

unsigned char CAD9852::SetPLLReferenceMultiplier(unsigned char aPLLReferenceMultiplier, bool GetValue) {
	if (!Enabled) return aPLLReferenceMultiplier;
	if (GetValue) {
		return (unsigned char)(AktValueContents[11] & 0x1F);
	} else {
		if (aPLLReferenceMultiplier>20) aPLLReferenceMultiplier=20;		
		unsigned char help=(unsigned char)AktValueContents[11];
		unsigned long NewValue = help & ~0x1F;
		NewValue = NewValue | (aPLLReferenceMultiplier & 0x1F);
		SetValue(11, NewValue, false);
		PLLReferenceMultiplier=aPLLReferenceMultiplier;
		ClockSpeed=InputClockSpeed*((PLLEnabled) ? PLLReferenceMultiplier : 1);	
		MaxFrequency=FrequencyMultiplier*(ClockSpeed*0.45E-6);
		//281474976710656=2^48
		FrequencyScale=281474976710656.0*((1.0/FrequencyMultiplier)*1.0E6/ClockSpeed);
		return aPLLReferenceMultiplier;
	}
}

bool CAD9852::SetClearACC1(bool aClearACC1, bool GetValue) {
	return SetControlBit(2,7,aClearACC1,GetValue);
}

bool CAD9852::SetClearACC2(bool aClearACC2, bool GetValue) {
	return SetControlBit(2,6,aClearACC2,GetValue);	
}

bool CAD9852::SetTriangle(bool aTriangle, bool GetValue) {
	return SetControlBit(2,5,aTriangle,GetValue);	
}

unsigned char CAD9852::GetLastFSKMode() {
	return LastFSKMode;
}


unsigned char CAD9852::SetMode(unsigned char aMode, bool GetValue) {
	if (!Enabled) return aMode;
	if (GetValue) {
		return (unsigned char)(AktValueContents[12] & 0x0E) >> 1;
	} else {
		unsigned char help=(unsigned char)AktValueContents[12];
		unsigned long NewValue = help & ~0x0E;
		NewValue = NewValue | ((aMode<<1) & 0x0E);
		unsigned char OldFSKMode=(unsigned char)(AktValueContents[12] & 0x0E) >> 1;
		SetValue(12, NewValue, false);
		return aMode;
	}
}


// FSK MODE 0
bool CAD9852::SetModeSingleTone(bool Set, bool GetValue) {
	if (GetValue) {
		return (SetMode(0,true)==0);
	} else {
		SetMode(0,false);
		return true;
	}
}

bool CAD9852::SetModeFrequencyShiftKeying(bool Set, bool GetValue) {
	if (GetValue) {
		return (SetMode(1,true)==0);
	} else {
		SetMode(1,false);
		return true;
	}
}	

bool CAD9852::SetModeRampedFrequencyShiftKeying(bool Set, bool GetValue) {
	if (GetValue) {
		return (SetMode(2,true)==0);
	} else {
		SetMode(2,false);
		return true;

	}
}	
void CAD9852::SetModeRampedFrequencyShiftKeyingConfiguration() {
		//SetExternalUpdateClock(/*bool aExternalUpdateClock*/true, /*bool GetValue*/false);
		SetClearACC1(/*bool aClearACC1*/false, /*bool GetValue*/false);						// 	SetClearACC1DDSAD9852(DDSNumber,Off);
		//SetTriangle(/*bool aTriangle*/false, /*bool GetValue*/false);							//	SetTriangleBitDDSAD9852(DDSNumber,Off); SetFSK_BPSK_Hold(bool aFSK_BPSK_Hold, bool GetValue)
		//SetFSK_BPSK_Hold(/*bool aFSK_BPSK_Hold*/false, /*bool GetValue*/false);				//	SetFSKBitDDSAD9852(DDSNumber,Off);
		//SetRampRateClock(/*RampRateClock*/1,/*GetValue*/ false);							//	SetRampRateClockDDSAD9852(DDSNumber,1);
		////SetAttenuationDDSAD9852(DDSNumber,DDSAD9852AttenuationMax);
		//SetFrequencyTuningWord1(LastStartFrequency,/*GetValue*/false) ;						//	SetFrequencyDDSAD9852(DDSNumber,StartFrequency);
		//SetFrequencyTuningWord2(LastStopFrequency,/*GetValue*/false) ;						//	SetFrequency2DDSAD9852(DDSNumber,StartFrequency);
		//SetModulationFrequency(LastModulationFrequency, /*GetValue*/false);					//	SetModulationFrequencyDDSAD9852(DDSNumber,0);
}	

bool CAD9852::SetModeModeChirp(bool Set, bool GetValue) {
	if (GetValue) {
		return (SetMode(3,true)==0);
	} else {
		SetMode(3,false);

		//SetExternalUpdateClock(/*bool aExternalUpdateClock*/false, /*bool GetValue*/false);		//AD9852[DDSNumber]->SetExternalUpdateClock(false); //External Update Clock
		//SetClearACC1(/*bool aClearACC1*/false, /*bool GetValue*/false);							//SetClearACC1DDSAD9852(DDSNumber,Off);
		////Output->Wait(10);
		//SetClearACC2(/*bool aClearACC2*/true, /*bool GetValue*/false);							//SetClearACC2DDSAD9852(DDSNumber,On); // ACC2 bits ON
		////Wait(10);
		//SetFSK_BPSK_Hold(/*bool aFSK_BPSK_Hold*/false, /*bool GetValue*/false);					//SetFSKBitDDSAD9852(DDSNumber,Off);
		////Wait(1);
		//SetRampRateClock(/*RampRateClock*/1,/*GetValue*/ false);								//SetRampRateClockDDSAD9852(DDSNumber,1);
		//	
		//SetFrequencyTuningWord1(LastStartFrequency,/*GetValue*/false) ;							//	SetFrequencyDDSAD9852(DDSNumber,StartFrequency);
		//SetFrequencyTuningWord2(LastStopFrequency,/*GetValue*/false) ;							//	SetFrequency2DDSAD9852(DDSNumber,StartFrequency);
		//SetModulationFrequency(LastModulationFrequency, /*GetValue*/false);						//	SetModulationFrequencyDDSAD9852(DDSNumber,0);
		//SetTriangle(/*bool aTriangle*/true, /*bool GetValue*/false);							//	SetTriangleBitDDSAD9852(DDSNumber,On);
		////Wait(1);
		//SetClearACC1(/*bool aClearACC1*/true, /*bool GetValue*/false);							//SetClearACC1DDSAD9852(DDSNumber,On);
		//SetClearACC2(/*bool aClearACC2*/false, /*bool GetValue*/false);							//SetClearACC2DDSAD9852(DDSNumber,Off);

		return true;
	}
}

bool CAD9852::SetModeBiphaseShiftKeying(bool Set, bool GetValue) {
	if (GetValue) {
		return (SetMode(4,true)==0);
	} else {
		SetMode(4,false);
		return true;
	}
}

bool CAD9852::SetExternalUpdateClock(bool aExternalUpdateClock, bool GetValue) {
	return SetControlBit(2,0,!aExternalUpdateClock,GetValue);		
}

bool CAD9852::SetBypassInverseSinc(bool aBypassInverseSinc, bool GetValue) {		
	return SetControlBit(3,6,aBypassInverseSinc,GetValue);		
}

bool CAD9852::SetOSKEnable(bool aOSKEnable, bool GetValue) {
	return SetControlBit(3,5,aOSKEnable,GetValue);			
}

bool CAD9852::SetOSKInternal(bool aOSKInternal, bool GetValue) {
	return SetControlBit(3,4,aOSKInternal,GetValue);				
}

bool CAD9852::SetLSBFirst(bool aLSBFirst, bool GetValue) {
	return SetControlBit(3,1,aLSBFirst,GetValue);					
}

bool CAD9852::SetSDOActive(bool aSDOActive, bool GetValue) {
	return SetControlBit(3,0,aSDOActive,GetValue);						
}

bool CAD9852::SetShapedKeying(bool aShapedKeying, bool GetValue) {
	if (!Enabled) return aShapedKeying;
	if (GetValue) return ShapedKeying;
	else {
		ShapedKeying=aShapedKeying;
		LoadLatches();
		return aShapedKeying;
	}
}

bool CAD9852::SetFSK_BPSK_Hold(bool aFSK_BPSK_Hold, bool GetValue) {
	if (!Enabled) return aFSK_BPSK_Hold;
	if (GetValue) return FSK_BPSK_Hold;
	else {
		FSK_BPSK_Hold=aFSK_BPSK_Hold;		
		LoadLatches();
		return aFSK_BPSK_Hold;
	}
}

bool CAD9852::SetControlBit(unsigned char RegisterNr, unsigned char BitNr, bool Value, bool GetValue)
{
	if (!Enabled) return false;
	if (GetValue) {
		return ((AktValueContents[10+RegisterNr] & (1 << BitNr)) > 0);
	} else {
		unsigned long NewValue;
		unsigned char help=(unsigned char)AktValueContents[10+RegisterNr];
		if (Value) {
			NewValue = help | (1 << BitNr);
		} else {
			NewValue = help &  ~(1 << BitNr);
		}
		SetValue(10+RegisterNr, NewValue, false);
		return Value;
	}
}

//Precision x corresponds to frequency precision y on 150 MHz signal
// x = y = 150MHz/(256^x)
// 1 = 0.5 MHz
// 2 = 2.2 kHz
// 3 = 8 Hz
// 4 = 0.03 Hz
// 5 = 0.13 mHz
// 6 = 0.5 microHz
unsigned char CAD9852::SetFrequencyWritePrecision(unsigned char Precision, bool GetValue)
{
	if (!Enabled) return Precision;
	if (GetValue) {
		if (IndependentUpdate)
			return WritePrecision[2]-1;
		else 
			return WritePrecision[2];
	} else {
		if (Precision > 6) Precision = 6;
		if (IndependentUpdate) {
			WritePrecision[2] = Precision + 1;
			WritePrecision[3] = Precision + 1;
		}
		else {
			WritePrecision[2] = Precision;
			WritePrecision[3] = Precision;
		}
		return Precision;
	}
}

