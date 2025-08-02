// OvenControl.cpp: implementation of the CEthernetMultiIOControllerOpticsFoundry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EthernetMultiIOControllerOpticsFoundry.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "MultiIO.h"
#include "Output.h"
#include <bitset>


constexpr unsigned __int32 MaxReconnectAttempts = 100;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const unsigned __int32 MaxFPGAProgramLength = 0x100000; //*8 to convert to bytes
const unsigned long MaxSequencerCommandListSize = 1024*16;
constexpr bool MinimizeEthernetCommunicationDirectionChanges = true;

CEthernetMultiIOControllerOpticsFoundry::CEthernetMultiIOControllerOpticsFoundry(unsigned int aMyMultiIO, COutput* amyOutput) :	
	CNetworkClient(/*ethernet communication mode */ 3, /* FastWrite*/false) {
	MyIPAddress = "undefined";
	MyPort = 0;
	DebugFileNumber = 0;
	DebugBusDeviceFileNumber = 0;
	previous_command_buffer = NULL;
	previous_command_buffer_length = 0;
	DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible = false;
	myOutput = amyOutput;
	MyMultiIO = aMyMultiIO;
	SequencerCommandList = new unsigned __int32[2 * MaxSequencerCommandListSize];
	SequencerCommandListSize = 0;
	Connected = false;
	DebugBufferOn = false;
	DebugBufferFile = NULL;
	ExternalTrigger0 = false;
	ExternalTrigger1 = false;
	ExternalClock0 = false;
	ExternalClock1 = false;
	PeriodicTriggerPeriod_in_s = 0;
	PeriodicTriggerAllowedWait_in_s = 0;
	FPGAUseStrobeGenerator = false;
	//FPGABuffer = new unsigned __int32[MaxFPGAProgramLength * 4];
	//FPGAAbsoluteTime = new unsigned __int32[MaxFPGAProgramLength];
	//FPGABufferUsed = 0;
	core_option_LED = false;
	core_option_SPI_CS = 0;
	core_option_dig_out = 0;
	core_option_PL_to_PS = 0;
	SetPeriodicTriggerAtBeginningOfNextSequence = false;
	WaitForPeriodicTriggerAtBeginningOfSequence = false;
	LastPeriodicTriggerPeriod_in_s = 0;
	DebugModeOn = false;
	InputBuffer = NULL;
}

CEthernetMultiIOControllerOpticsFoundry::~CEthernetMultiIOControllerOpticsFoundry()
{
	if (InputBuffer) {
		delete[] InputBuffer;
		InputBuffer = NULL;
	}
	if (DebugBufferFile) {
		delete DebugBufferFile;
		DebugBufferFile = NULL;
	}
	if (previous_command_buffer) {
		delete previous_command_buffer;
		previous_command_buffer = NULL;
	}
	if (Connected) CloseConnection();
	//delete FPGABuffer;
	//delete FPGAAbsoluteTime;
	delete SequencerCommandList;
}

bool CEthernetMultiIOControllerOpticsFoundry::ConnectSocket(LPCTSTR lpszAddress, UINT port, double aBusFrequency, double aFPGAClockFrequencyInHz, bool aFPGAUseExternalClock, bool aFPGAUseStrobeGenerator) {
	Connected = CNetworkClient::ConnectSocket(lpszAddress, port, "EthernetMultiIOController");
	BusFrequency = aBusFrequency;
	FPGAClockFrequencyInHz = aFPGAClockFrequencyInHz;
	FPGAUseExternalClock = aFPGAUseExternalClock;
	FPGAUseStrobeGenerator = aFPGAUseStrobeGenerator;
	MyIPAddress = lpszAddress;
	MyPort = port; 

	if (DebugFPGABufferCreation) {
		CString filename;
		filename.Format(*DebugFilePath + "DebugFPGABufferCreation_%s_%u.txt", (LPCTSTR)MyIPAddress, MyPort);
		DebugBuffer(filename);
	}
	if (DebugFPGACommunication) {
		CString filename;
		filename.Format(*DebugFilePath + "DebugFPGACommunication_%s_%u.txt", (LPCTSTR)MyIPAddress, MyPort);
		DebugStart(filename);
	}
	return Connected;
}

void CEthernetMultiIOControllerOpticsFoundry::AddSequencerCommandToBuffer(unsigned __int32* buffer, unsigned __int32 n, unsigned __int32 high_buffer, unsigned __int32 low_buffer) {

	buffer[n * 2 + 0] = low_buffer;
	buffer[n * 2 + 1] = high_buffer;

	if (DebugBufferFile) {
		CString buf;
		buf.Format("%8u CMD %x buffer = %08X %08X ", n, high_buffer, low_buffer);
		(*DebugBufferFile) << buf << endl;
	}
}

void CEthernetMultiIOControllerOpticsFoundry::ClearSequencerCommandList() {
	SequencerCommandListSize = 0;
}

unsigned int CEthernetMultiIOControllerOpticsFoundry::AddSequencerCommand(unsigned __int32 high_word, unsigned __int32 low_word) {
	if (SequencerCommandListSize >= MaxSequencerCommandListSize) {
		ControlMessageBox("EthernetMultiIOControllerOpticsFoundry.cpp: AddSequencerCommand(): sequencer command list has too little memory, increase MaxSequencerCommandListSize.");
		return MaxSequencerCommandListSize - 1;
	}
	SequencerCommandList[SequencerCommandListSize * 2] = low_word;
	SequencerCommandList[SequencerCommandListSize * 2 + 1] = high_word;
	unsigned int CurrentCommandNr = SequencerCommandListSize;
	SequencerCommandListSize++;
	return CurrentCommandNr;
}



void CEthernetMultiIOControllerOpticsFoundry::DebugBuffer(CString filename) {
	DebugBufferFile = new ofstream(filename, ios::out);
	DebugBufferOn = true;
}

/*
Assumptions for removal of unused bus entries:
Buffer[2 * i] = Data;
Buffer[2 * i + 1] = DirectionOutBit | StrobeBit | Address;
Address consists of 8 low lying main address bits and three (in principle up to six) high lying sub address bits
const unsigned int StrobeBit = 0x0100;

const unsigned int BusBitShift = 8;  //Attention: the subbus bits will get shifted by an additional two bits in CMultiIO::GetBusContents to make space for direction and strobe bits
const unsigned int Bus0 = 0x0 << BusBitShift;
...
const unsigned int BusSequencerSpecialCommand = 0x7 << BusBitShift;
const unsigned int Lock = 0x8 << BusBitShift;
*/


constexpr unsigned char CMD_STOP = 0;
constexpr unsigned char CMD_STEP = 1;
constexpr unsigned char CMD_STEP_AND_ENTER_FAST_MODE = 2;
constexpr unsigned char CMD_SET_OPTIONS = 3;
constexpr unsigned char CMD_LOAD_REG_LOW = 4;
constexpr unsigned char CMD_LOAD_REG_HIGH = 5;
constexpr unsigned char CMD_LATCH_STATE = 6;
constexpr unsigned char CMD_RESET_WAIT_CYCLES = 7;
constexpr unsigned char CMD_LONG_WAIT = 8;
constexpr unsigned char CMD_SET_STROBE_OPTIONS = 9;
constexpr unsigned char CMD_SET_INPUT_BUF_MEM = 10;
constexpr unsigned char CMD_WAIT_FOR_TRIGGER = 11;
constexpr unsigned char CMD_SET_LOOP_COUNT = 12;
constexpr unsigned char CMD_CONDITIONAL_JUMP_FORWARD = 13;
constexpr unsigned char CMD_CONDITIONAL_JUMP_BACKWARD = 14;
constexpr unsigned char CMD_I2C_OUT = 15;
constexpr unsigned char CMD_SPI_OUT_IN = 16;
constexpr unsigned char CMD_INPUT_REPEATED_OUT_IN = 17;
constexpr unsigned char CMD_SET_PERIODIC_TRIGGER_PERIOD = 18;
constexpr unsigned char CMD_SET_PERIODIC_TRIGGER_ALLOWED_WAIT_TIME = 19;
constexpr unsigned char CMD_WAIT_FOR_PERIODIC_TRIGGER = 20;
constexpr unsigned char CMD_WAIT_FOR_WAIT_CYCLE_NR = 21;
constexpr unsigned char CMD_DIG_IN = 22;
constexpr unsigned char CMD_TRIGGER_SECONDARY_PL_PS_INTERRUPT = 23;
constexpr unsigned char CMD_ANALOG_IN_OUT = 24;
constexpr unsigned char CMD_PL_TO_PS_COMMAND = 25;
constexpr unsigned char CMD_LOAD_COMMAND_BUFFER = 26;
constexpr unsigned char CMD_SAVE_CYCLE_COUNT_SINCE_STARTUP_IN_INPUT_BUF_MEM = 27;
constexpr unsigned char CMD_CALC_AD9854_FREQUENCY_TUNING_WORD = 28;

constexpr unsigned char NrCommands = 29;
const CString CommandNames[NrCommands] = { "CMD_STOP", "CMD_STEP", "CMD_STEP_AND_ENTER_FAST_MODE", "CMD_SET_OPTIONS", "CMD_LOAD_REG_LOW", "CMD_LOAD_REG_HIGH", "CMD_LATCH_STATE", "CMD_RESET_WAIT_CYCLES", "CMD_LONG_WAIT", "CMD_SET_STROBE_OPTIONS", "CMD_SET_INPUT_BUF_MEM", "CMD_WAIT_FOR_TRIGGER", "CMD_SET_LOOP_COUNT", "CMD_CONDITIONAL_JUMP_FORWARD", "CMD_CONDITIONAL_JUMP_BACKWARD", "CMD_I2C_OUT", "CMD_SPI_OUT_IN", "CMD_INPUT_REPEATED_OUT_IN", "CMD_SET_PERIODIC_TRIGGER_PERIOD", "CMD_SET_PERIODIC_TRIGGER_ALLOWED_WAIT_TIME", "CMD_WAIT_FOR_PERIODIC_TRIGGER", "CMD_WAIT_FOR_WAIT_CYCLE_NR", "CMD_DIG_IN", "CMD_TRIGGER_SECONDARY_PL_PS_INTERRUPT", "CMD_ANALOG_IN_OUT", "CMD_PL_TO_PS_COMMAND", "CMD_LOAD_COMMAND_BUFFER", "CMD_SAVE_CYCLE_COUNT_SINCE_STARTUP_IN_INPUT_BUF_MEM", "CMD_CALC_AD9854_FREQUENCY_TUNING_WORD"};
constexpr bool CommandUsesBuffer[NrCommands] = { false   , false     , false                         , true             , false             , false              , false            , false                  , true           , true                    , true                   , true                  , true                , true                          , true                           , true         , true            , true                       , true                             , true                                        , false                          , true                        , true        , false                                  , true               , true                  , false                , false												, true };
constexpr bool UseCommandBuffer = true;

void CEthernetMultiIOControllerOpticsFoundry::StartAnalogInAcquisition(unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_nr, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms) {
	//if (channel_nr < 2) {
	//	StartXADCAnalogInAcquisition(channel_nr, number_of_datapoints, delay_between_datapoints_in_ms);
//	} else {
		StartSPIAnalogInAcquisition(SPI_port, SPI_CS, channel_nr /* - 2*/, number_of_datapoints, delay_between_datapoints_in_ms);
	//}
}

void CEthernetMultiIOControllerOpticsFoundry::AddSequencerCommandToSequenceList(unsigned __int32 high_buffer, unsigned __int32 low_buffer) {
	if (UseCommandBuffer) {
		const unsigned int command = low_buffer & 0x1F;
		if (command <= NrCommands) {
			if (CommandUsesBuffer[command]) {
				unsigned __int32 low_command_buffer = (low_buffer & 0xFFFFFFE0) | (0x1F & CMD_LOAD_COMMAND_BUFFER);
				myOutput->AddSequencerCommandToSequenceList(MyMultiIO, high_buffer, low_command_buffer);
			}
		}
	}
	myOutput->AddSequencerCommandToSequenceList(MyMultiIO, high_buffer, low_buffer);
}

void CEthernetMultiIOControllerOpticsFoundry::StartXADCAnalogInAcquisition(unsigned int channel_nr, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms) {

	//only for debugging
	//for (int i = 0; i < 15; i++) {
	//	AddCommandAnalogInOut(/* adc_register_address */ 3, /* adc_write_enable */ 0, /* adc_programming_out */ 0, /* dont_execute_now*/ 0, /* only_read_write*/ 0,/*wait_time*/ 150);
	//}
	/*
	* const u8 command_mask = 0x1F;  //5 bit
	CMD_ANALOG_IN_OUT: begin
                            adc_register_address <= command[14:8];  //to read standard analog in, this should be 3, see Xilinx user guide UG480
                            adc_write_enable <= command[15:15];
                            adc_programming_out <= command[31:16];   
                            wait_time[29:0] <= command[63:34];
                            wait_time[47:30] <= 0;                         
                            INPUT_REPEAT_state <= INPUT_REPEAT_IDLE;
                            if (command[32:32] == 0) begin  //if command[40:40] is high, the actual reading will be started trhough CMD_REPEAT
                                if (command[33:33] == 0) ANA_IN_state <= ANA_IN_START; //conversion and register read
                                else ANA_IN_state <= ANA_IN_TRIGGER_READ_WRITE; //only register read or write
                            end
                        end*/
	unsigned char command = CMD_ANALOG_IN_OUT;
	unsigned __int32 adc_register_address = 3;
	unsigned __int32 adc_write_enable = 0;
	unsigned __int32 adc_programming_out = 0;
	unsigned __int32 wait_time = 0;
	unsigned char start_now = 0;
	unsigned char adc_conversion = 1;
	unsigned __int32 low_buffer = ((adc_programming_out & 0xFFFF) << 16) | ((adc_write_enable & 0x01) << 15) | ((adc_register_address & 0x7F) << 8) | (0x1F & command);
	unsigned __int32 high_buffer = ((wait_time & 0x3FFF) << (34 - 32)) | ((start_now & 0x01) << (32 - 32)) | ((adc_conversion & 0x01) << (33 - 32));


	AddSequencerCommandToSequenceList( high_buffer, low_buffer);

	/*

	CMD_INPUT_REPEATED_OUT_IN : begin   //This is a two cycle operation. The last state has to be LOAD_EXTENDED_DATA, in order to avoid writing the flg given here to the channels. The opcode I2C_OUT is encountered in that state, and argument stored. Here we use this stored argument.
							INPUT_REPEAT_repeats <= command[27:8];
							INPUT_REPEAT_wait <= command[55:32]; //Use LOAD_EXTENDED_DATA before INPUT_REPEATED_OUT to copy 64-bit channel content to extended_data
							INPUT_REPEAT_trigger_secondary_interrupt_when_finished <= command[56:56];
							INPUT_REPEAT_state <= INPUT_REPEAT_START;
							INPUT_REPEAT_command <= command[59:57];
							if (bus_clock) bus_clock <= 0; else bus_clock <= 1;
							strobe_generator_state <= DELAY_CYCLE;
						end

					if (INPUT_REPEAT_command[0] == 1) SPI_state <= SPI_START;
					if (INPUT_REPEAT_command[1] == 1) DIG_IN_state <= DIG_IN_START;
					if (INPUT_REPEAT_command[2] == 1) ANA_IN_state <= ANA_IN_START;
	
	*/
	command = CMD_INPUT_REPEATED_OUT_IN;
	unsigned __int32 INPUT_REPEAT_repeats = number_of_datapoints;
	unsigned __int32 INPUT_REPEAT_wait = floor(delay_between_datapoints_in_ms * FPGAClockFrequencyInHz / 1000);
	unsigned __int32 INPUT_REPEAT_command = 4; //XADC analog in
	unsigned __int32 INPUT_REPEAT_trigger_secondary_interrupt_when_finished = 0;

	low_buffer = (INPUT_REPEAT_repeats << 8) | command;
	high_buffer = INPUT_REPEAT_wait | (INPUT_REPEAT_trigger_secondary_interrupt_when_finished << (56-32)) | (INPUT_REPEAT_command << (57-32));
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);
	
	unsigned __int32 DelayMultiplier = floor(FPGAClockFrequencyInHz / BusFrequency - 1);
	if (DelayMultiplier < 1) DelayMultiplier = 1;
	AddProgramLineToSequenceList(1, 0, DelayMultiplier*3-2); //CMD_STEP doing nothing, just in order to keep the time calculated by COutput aligned with the time used by the FPGA; needed because the two commands above only consume 2 FPGA cycles, but are accounted for with 2 bus cycles by COutput
}

/*
//Vitis command:
CMD_SET_INPUT_BUF_MEM: begin
							//output reg [11:0] input_buf_mem_address = 0;
							//output reg [31:0] input_buf_mem_data = 0;
							if (command_buffer[7:7] == 1) input_buf_mem_address <= command_buffer[20:8];
							//else input_buf_mem_address <= input_buf_mem_address + 1;
							input_buf_mem_data <= command_buffer[63:32];
							//input_buf_mem_write_next_cycle <= 2;
							INPUT_MEM_state <= INPUT_MEM_WRITE;
							wait_time <= 1;
							address <= address + 1;
						end
*/

//Vitis command converted to Visual Studio command:
void CEthernetMultiIOControllerOpticsFoundry::AddCommandWriteInputBuffer(unsigned long input_buf_mem_data, bool write_next_address, unsigned long input_buf_mem_address) {
	unsigned char command = CMD_SET_INPUT_BUF_MEM;
	unsigned __int32 low_buffer = ((input_buf_mem_address & 0x7FF) << 8) | (((write_next_address) ? 0 : 1) << 7) | (0x1F & command);
	unsigned __int32 high_buffer = input_buf_mem_data;
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);
}

void CEthernetMultiIOControllerOpticsFoundry::AddCommandWriteSystemTimeToInputMemory() {
	unsigned char command = CMD_SAVE_CYCLE_COUNT_SINCE_STARTUP_IN_INPUT_BUF_MEM;
	unsigned __int32 low_buffer =  (0x1F & command);
	unsigned __int32 high_buffer = 0;
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);
}

/*
CMD_CALC_AD9854_FREQUENCY_TUNING_WORD: begin
							//we assume that the result of the ADC conversion is available in input_buf_mem_data_SPI
							AD9854FTWIntermediate[15:0] <= input_buf_mem_data_SPI[15:0];
							AD9854FTWIntermediate[47:16] <= 0;
							calc_ad9854_ftw_state <= CALC_AD9854_FTW_START;
							address <= address + 1;
							wait_time <= 3; //wait for ftw calculation to finish. a wait_time of 2 should be sufficient. 3 for safety.
						end


...


//The frequency tuning word is proportional to the period of the DDS output frequency, i.e. it's 1/frequency
			//The desired frequency is
			// f = f0 + c * voltage = f0 + deltaf
			// voltage is the 16-bit value provided by the DAC
			//We don't want to use a multiplication. Instead we approximate, using epsilon = deltaf/f0 << 1.
			// delta f = c* voltage
			// ftw = 1/f = 1/(f0 + deltaf) = 1/ (f0 * (1 + deltaf/f0)) = (1/f0)*(1/(1+epsilon)) ~ ftw0 * (1-epsilon)
			//     = ftw0 - ftw0*deltaf/f0 = ftw0 - ftw0 * ftw0 * c * voltage = ftw0 - scale * ADC_value
			//We replace the multiplication by a bitshift, i.e. we allow scale = 2^n with n=[0...32].
			case (calc_ad9854_ftw_state)
				CALC_AD9854_FTW_IDLE: begin
					send_AD9854_ftw <= 0;
					end
				CALC_AD9854_FTW_START: begin
					//shift 16-bit ADC value by up to 32 bit to align with 48-bit ftw
					//here we assume unsigend bit shift. For signed, use <<<
					AD9854FTWShifted <= AD9854FTWIntermediate << command_buffer[12:8];
					calc_ad9854_ftw_state <= CALC_AD9854_FTW_STOP;
				end
				CALC_AD9854_FTW_STOP: begin
				   AD9854FTW = command_buffer[63:16] - AD9854FTWShifted; //user provided FTW - shifted ADC value
				   calc_ad9854_ftw_state <= CALC_AD9854_FTW_IDLE;
				   send_AD9854_ftw <= 1;
				   AD9854_ftw_byte_shifts_to_do <= 6;
				end
				default: begin
				   calc_ad9854_ftw_state <= CALC_AD9854_FTW_IDLE;
				end
			endcase
*/

void CEthernetMultiIOControllerOpticsFoundry::AddCommandCalcAD9854FrequencyTuningWord(uint64_t ftw0, uint8_t bit_shift) {
	unsigned char command = CMD_CALC_AD9854_FREQUENCY_TUNING_WORD;
	uint16_t help = bit_shift;
	uint32_t help2 = (ftw0 << 16) & 0xFFFF0000;
	unsigned __int32 low_buffer =  (ftw0 << 16) & (help << 8) & (0x1F & command);
	unsigned __int32 high_buffer = ftw0 >> 16;
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);
}

/*
//Vitis command:
void AddCommandAnalogInOut(u8 adc_register_address, u8 adc_write_enable, u16 adc_programming_out, u8 dont_execute_now, u8 only_read_write, u32 wait_time) {
	const u8 command_mask = 0x1F;  //5 bit
	u8 command = 24; //CMD_ANALOG_IN_OUT
	source[next_command * 2 + 0] = ((adc_programming_out & 0xFFFF) << 16) | ((adc_write_enable & 0x01) << 15) | ((adc_register_address & 0x7F) << 8) | (command_mask & command);
	source[next_command * 2 + 1] = ((wait_time & 0x3FFF) << (34 - 32)) | ((dont_execute_now & 0x01) << (32 - 32)) | ((only_read_write & 0x01) << (33 - 32));
	next_command++;
}*/

//Vitis command converted to Visual Studio command:
void CEthernetMultiIOControllerOpticsFoundry::AddCommandAnalogInOut(unsigned char adc_register_address, unsigned char adc_write_enable, unsigned __int16 adc_programming_out, unsigned char dont_execute_now, unsigned char only_read_write, unsigned __int32 wait_time) {
	
	



	/*
		CMD_ANALOG_IN_OUT: begin
                            adc_register_address <= command[14:8];  //to read standard analog in, this should be 3, see Xilinx user guide UG480
                            adc_write_enable <= command[15:15];
                            adc_programming_out <= command[31:16];   
                            wait_time[29:0] <= command[63:34];
                            wait_time[47:30] <= 0;                         
                            INPUT_REPEAT_state <= INPUT_REPEAT_IDLE;
                            if (command[32:32] == 0) begin  //if command[40:40] is high, the actual reading will be started trhough CMD_REPEAT
                                if (command[33:33] == 0) ANA_IN_state <= ANA_IN_START; //conversion and register read
                                else ANA_IN_state <= ANA_IN_TRIGGER_READ_WRITE; //only register read or write
                            end
                        end*/
	unsigned char command = CMD_ANALOG_IN_OUT;
	unsigned __int32 low_buffer = ((adc_programming_out & 0xFFFF) << 16) | ((adc_write_enable & 0x01) << 15) | ((adc_register_address & 0x7F) << 8) | (0x1F & command);
	unsigned __int32 high_buffer = ((wait_time & 0x3FFF) << (34 - 32)) | ((dont_execute_now & 0x01) << (32 - 32)) | ((only_read_write & 0x01) << (33 - 32));
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);
}



void CEthernetMultiIOControllerOpticsFoundry::AddCommandSetCoreOption_LED(bool a_core_option_LED) {
	core_option_LED = a_core_option_LED;
	AddCommandSetCoreOptions();
}

void CEthernetMultiIOControllerOpticsFoundry::AddCommandSetCoreOption_SPI_CS(unsigned char a_core_option_SPI_CS) {
	core_option_SPI_CS = a_core_option_SPI_CS;
	AddCommandSetCoreOptions();
}

void CEthernetMultiIOControllerOpticsFoundry::AddCommandSetCoreOption_dig_out(unsigned char a_core_option_dig_out) {
	core_option_dig_out = a_core_option_dig_out;
	AddCommandSetCoreOptions();
}

void CEthernetMultiIOControllerOpticsFoundry::AddCommandSetCoreOption_PL_to_PS(unsigned char a_core_option_PL_to_PS) {
	core_option_PL_to_PS = a_core_option_PL_to_PS;
	AddCommandSetCoreOptions();
}

//Vitis command:
/*
	CMD_SET_OPTIONS:begin
			   options <= command[63:32];
						end

		options[0] = core_options_LED;
		options[1..4] = SPI_CS;
		options[8..15] = core_dig_out;
		options[24..31] = options_PL_to_PS;

	*/
	//Vitis command converted to Visual Studio command:
void CEthernetMultiIOControllerOpticsFoundry::AddCommandSetCoreOptions() {
	unsigned char command = CMD_SET_OPTIONS;
	unsigned __int32 low_buffer = command;
	unsigned __int32 high_buffer = ((core_option_PL_to_PS & 0xFF) << 24) | ((core_option_dig_out & 0xFF) << 8) | ((core_option_SPI_CS & 0x0F) << 1) | (core_option_LED & 0x01);
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);
}
//Vitis command:
/*
CMD_PL_TO_PS_COMMAND: begin
PL_to_PS_command <= command_buffer[23:8];
address <= address + 1;
wait_time <= 0;
end
*/
//Vitis command converted to Visual Studio command:
constexpr unsigned int PLToPSCommandTest = 1 << 8;
constexpr unsigned int PLToPSCommandIgnoreTCPIP = (2<<8) | 1;
constexpr unsigned int PLToPSCommandAcceptTCPIP = (2 << 8);
void CEthernetMultiIOControllerOpticsFoundry::AddCommandSetPLToPSCommand(unsigned int PLToPSCommand) {
	unsigned char command = CMD_PL_TO_PS_COMMAND;
	unsigned __int32 low_buffer = ((PLToPSCommand & 0xFFFF) << 8) | command;
	unsigned __int32 high_buffer = 0;
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);
}


void CEthernetMultiIOControllerOpticsFoundry::AddCommandWait(unsigned long Wait_in_FPGA_clock_cycles) {
	/*
			 CMD_STEP:begin
							wait_time[30:0] <= command[35:5];
							wait_time[47:31] <= 0;
							bus_data <= command[63:36];
							if (bus_clock) bus_clock <= 0; else bus_clock <= 1;
							strobe_generator_state <= DELAY_CYCLE;
						end
			*/
	unsigned char command = CMD_STEP;
	if (Wait_in_FPGA_clock_cycles == 0) Wait_in_FPGA_clock_cycles = 1;
	unsigned __int32 low_buffer = (Wait_in_FPGA_clock_cycles <<5 ) | command;
	unsigned __int32 high_buffer = 0;
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);
}



/*
CMD_SET_LOOP_COUNT: begin
	loop_count <= command_buffer[63:32];
	address <= address + 1;
	wait_time <= 1;
end
*/
void CEthernetMultiIOControllerOpticsFoundry::AddCommandSetLoopCount(unsigned int loop_count) {
	unsigned char command = CMD_SET_LOOP_COUNT;
	unsigned __int32 low_buffer = command;
	unsigned __int32 high_buffer = (loop_count & 0xFFFFFFFF);
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);
}

/*
CMD_CONDITIONAL_JUMP_FORWARD: begin  //here we assume that the program assembling the sequence has made sure that the jump is within the current BRAM half
	if ((condition_0 && (command_buffer[8:8] == 1)) || (condition_1 && (command_buffer[9:9] == 1)) || (condition_PS && (command_buffer[10:10] == 1)) || (command_buffer[11:11] == 1))
		address <= address + command_buffer[15:8];
	else address <= address + 1;
	wait_time <= 1;
end
*/
void CEthernetMultiIOControllerOpticsFoundry::AddCommandJumpForward(unsigned int jump_length, bool unconditional_jump, bool condition_0, bool condition_1, bool condition_PS) {
	unsigned char command = CMD_CONDITIONAL_JUMP_FORWARD;
	unsigned __int32 low_buffer = ((jump_length & 0xFF) << 8) | (condition_0 ? 0x100 : 0) | (condition_1 ? 0x200 : 0) | (condition_PS ? 0x400 : 0) |  (unconditional_jump ? 0x800 : 0) | command;
	unsigned __int32 high_buffer = 0;
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);
}

/*
CMD_CONDITIONAL_JUMP_BACKWARD: begin  //here we assume that the program assembling the sequence has made sure that the jump is within the current BRAM half
	if (((condition_0 && (command_buffer[8:8] == 1)) || (condition_1 && (command_buffer[9:9] == 1)) || (condition_PS && (command_buffer[10:10] == 1)) ||
			(command_buffer[11:11] == 1) || ((command_buffer[12:12] == 1) && (loop_count > 0))))
		address <= address - command_buffer[15:8];
	else address <= address + 1;
	loop_count <= loop_count - 1;
	wait_time <= 1;
end
*/

void CEthernetMultiIOControllerOpticsFoundry::AddCommandJumpBackward(unsigned int jump_length, bool unconditional_jump, bool condition_0, bool condition_1, bool condition_PS, bool loop_count_greater_zero) {
	unsigned char command = CMD_CONDITIONAL_JUMP_BACKWARD;
	unsigned __int32 low_buffer = ((jump_length & 0xFF) << 8) | (condition_0 ? 0x100 : 0) | (condition_1 ? 0x200 : 0) | (condition_PS ? 0x400 : 0) | (unconditional_jump ? 0x800 : 0) | (loop_count_greater_zero ? 0x1000 : 0) | command;
	unsigned __int32 high_buffer = 0;
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);
}


void CEthernetMultiIOControllerOpticsFoundry::WriteReadSPI(unsigned int chip_select, unsigned int number_of_bits_out, unsigned __int64 data_high, unsigned __int64 data_low, unsigned int number_of_bits_in) {

	//total data length can be up to 128-(2*5) = 118 bits (because we need 2x 5 bits for the two load commands).
	unsigned char command = CMD_LOAD_REG_LOW;
	unsigned __int32 low_buffer = ((data_low & 0x07FFFFFF) << 5)  | command; //lowest 27 bit
	unsigned __int32 high_buffer = (data_low >> 27) & 0xFFFFFFFF; //bit 28 to 59 (5 bits missing)
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);
	unsigned char NrCommands = 2;
	if (number_of_bits_out > 59) {
		command = CMD_LOAD_REG_HIGH;
		low_buffer = ((data_high & 0x003FFFFF) << 10) | ((data_low >> (27 + 32)) & 0x1F) | command; /*last 5 bits of data_low; if needed data can be extended*/
		high_buffer = (data_high >> 22) & 0xFFFFFFFF;  //data_high can have up 54 bits.
		AddSequencerCommandToSequenceList(high_buffer, low_buffer);
		NrCommands = 3;
	}


	/*
	CMD_SPI_OUT_IN : begin
							SPI_OUT_length <= command[14:8];
							SPI_IN_length <= command[20:16];
							SPI_SEL_next <= command[33:32];
							SPI_chip_select_next <= command[37:34];
							SPI_data <= register;  //Use CMD_LOAD_REG_LOW and CMD_LOAD_REG_HIGH before CMD_SPI_OUT_IN to copy 117-bit I2C data to register
							INPUT_REPEAT_state <= INPUT_REPEAT_IDLE;
							if (command[40:40] == 0) SPI_state <= SPI_START;
						end
	*/

	command = CMD_SPI_OUT_IN;
	unsigned __int32 SPI_out_length = number_of_bits_out;
	unsigned __int32 SPI_in_length = number_of_bits_in;
	//unsigned __int32 SPI_CS = 1;  
	unsigned __int32 wait_time = 0;
	unsigned char SPI_SEL_next = 0x01; //2 SPI ports, 1 means active under PL control; if inactive they are under PS control
	unsigned char SPI_chip_select_next = chip_select; //4 CS lines, low means active.
	unsigned int start_now = 1;
	low_buffer = (SPI_in_length << 16) | (SPI_out_length << 8) | command;
	high_buffer = (start_now << (40 - 32)) | (SPI_chip_select_next << 2) | (0x03 & SPI_SEL_next);
	AddSequencerCommandToSequenceList(high_buffer, low_buffer);

	unsigned __int32 DelayMultiplier = floor(FPGAClockFrequencyInHz / BusFrequency - 1);
	if (DelayMultiplier < 1) DelayMultiplier = 1;
	//the following code needs to be checked if timing is crucial
	if (DelayMultiplier == 1) {
		AddProgramLineToSequenceList(1, 0, DelayMultiplier * NrCommands - 2); //CMD_STEP doing nothing, just in order to keep the time calculated by COutput aligned with the time used by the FPGA; needed because the three commands above only consume 3 FPGA cycles, but are accounted for with 3 bus cycles by COutput
		AddProgramLineToSequenceList(1, 0, DelayMultiplier * NrCommands - 2); //CMD_STEP doing nothing, just in order to keep the time calculated by COutput aligned with the time used by the FPGA; needed because the three commands above only consume 3 FPGA cycles, but are accounted for with 3 bus cycles by COutput
	}
	else {
		if ((DelayMultiplier - 1) *NrCommands>0)
			AddProgramLineToSequenceList(1, 0, (DelayMultiplier-1) * NrCommands); //CMD_STEP doing nothing, just in order to keep the time calculated by COutput aligned with the time used by the FPGA; needed because the three commands above only consume 3 FPGA cycles, but are accounted for with 3 bus cycles by COutput
	}

}


void CEthernetMultiIOControllerOpticsFoundry::StartSPIAnalogInAcquisition(unsigned char SPI_port, unsigned char SPI_CS, unsigned int channel_nr, unsigned int number_of_datapoints, double delay_between_datapoints_in_ms) {
	

	//only for debugging
	//AddCommandSetCoreOption_LED(true);
	//AddCommandWriteInputBuffer(/*write_next_address*/ false, /* input_buf_mem_address */ 0, /*input_buf_mem_data*/0xDEADBEEF, /* wait_time_in_FPGA_cycles*/ 5);
	//AddCommandWriteInputBuffer(/*write_next_address*/ false, /* input_buf_mem_address */ 1, /*input_buf_mem_data*/0x1234ABCD, /* wait_time_in_FPGA_cycles*/ 5);

	/*
	CMD_LOAD_REG_LOW:begin
                            register[58:0] <= command[63:5];
                        end
    CMD_LOAD_REG_HIGH:begin
                            register[117:59] <= command[63:5];
                        end 
	*/

	unsigned __int32 SPI_SINGLE_ENDED_INPUT = 1;
	unsigned __int32 SPI_ANALOG_IN_NR = channel_nr;
	unsigned __int32 SPI_IN_NR_REVERSED = (((SPI_ANALOG_IN_NR & 1)>0) ? 4 : 0) + (((SPI_ANALOG_IN_NR & 2)>0) ? 2 : 0) + (((SPI_ANALOG_IN_NR & 4)>0) ? 1 : 0);
	unsigned __int32 SPI_DATA = 1 + (SPI_SINGLE_ENDED_INPUT << 1) + (SPI_IN_NR_REVERSED << 2);
	//SPI_DATA = 1+2;
	
	unsigned char command = CMD_LOAD_REG_LOW;
	unsigned __int32 low_buffer = (SPI_DATA << 5) | command;
	unsigned __int32 high_buffer = 0;
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);
	
	command = CMD_LOAD_REG_HIGH;
	low_buffer = command;
	high_buffer = 0;
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);


	/*
	CMD_SPI_OUT_IN : begin
                            SPI_OUT_length <= command[14:8];
                            SPI_IN_length <= command[20:16];
                            SPI_SEL_next <= command[33:32];
                            SPI_chip_select_next <= command[37:34];
                            SPI_data <= register;  //Use CMD_LOAD_REG_LOW and CMD_LOAD_REG_HIGH before CMD_SPI_OUT_IN to copy 117-bit I2C data to register                                          
                            INPUT_REPEAT_state <= INPUT_REPEAT_IDLE;
                            if (command[40:40] == 0) SPI_state <= SPI_START;                                                         
                        end  
	*/

	command = CMD_SPI_OUT_IN;
	unsigned __int32 SPI_out_length = 6;
	unsigned __int32 SPI_in_length = 13;
	//unsigned __int32 SPI_CS = 1;  
	unsigned __int32 wait_time = 0;
	unsigned char SPI_SEL_next = (SPI_port == 0) ? 0x01 : 0x02; //2 SPI ports, port 0 is controlled by bit zero here, port 1 is bit one here; 1 means active under PL control; if inactive they are under PS control
	unsigned char SPI_chip_select_next = (~(1 << SPI_CS)) & 0x0F;// 2 + 4 + 8; //4 CS lines, low means active; ToDo: in V2 we intend to install a multiplexer on the backplane, then every 16 bit value is valid and the code needs to be changed here.
	unsigned int start_now = 0;
	low_buffer =  (SPI_in_length << 16) | (SPI_out_length << 8) | command;
	high_buffer = (start_now << (40-32)) | (SPI_chip_select_next << 2) | (0x03 & SPI_SEL_next);
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);

	/*

	CMD_INPUT_REPEATED_OUT_IN : begin   //This is a two cycle operation. The last state has to be LOAD_EXTENDED_DATA, in order to avoid writing the flg given here to the channels. The opcode I2C_OUT is encountered in that state, and argument stored. Here we use this stored argument.
							INPUT_REPEAT_repeats <= command[27:8];
							INPUT_REPEAT_wait <= command[55:32]; //Use LOAD_EXTENDED_DATA before INPUT_REPEATED_OUT to copy 64-bit channel content to extended_data
							INPUT_REPEAT_trigger_secondary_interrupt_when_finished <= command[56:56];
							INPUT_REPEAT_state <= INPUT_REPEAT_START;
							INPUT_REPEAT_command <= command[59:57];
							if (bus_clock) bus_clock <= 0; else bus_clock <= 1;
							strobe_generator_state <= DELAY_CYCLE;
						end

					if (INPUT_REPEAT_command[0] == 1) SPI_state <= SPI_START;
					if (INPUT_REPEAT_command[1] == 1) DIG_IN_state <= DIG_IN_START;
					if (INPUT_REPEAT_command[2] == 1) ANA_IN_state <= ANA_IN_START;

	*/
	command = CMD_INPUT_REPEATED_OUT_IN;
	unsigned __int32 INPUT_REPEAT_repeats = number_of_datapoints;
	unsigned __int32 INPUT_REPEAT_wait = floor(delay_between_datapoints_in_ms * FPGAClockFrequencyInHz / 1000);
	unsigned __int32 INPUT_REPEAT_command = 1;  //SPI input
	unsigned __int32 INPUT_REPEAT_trigger_secondary_interrupt_when_finished = 1;//this is needed if input BRAM buffer should be copied to DDR when half buffer full 

	low_buffer = (INPUT_REPEAT_repeats << 8) | command;
	high_buffer = INPUT_REPEAT_wait | (INPUT_REPEAT_trigger_secondary_interrupt_when_finished << (56 - 32)) | (INPUT_REPEAT_command << (57 - 32));
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);

	unsigned __int32 DelayMultiplier = floor(FPGAClockFrequencyInHz / BusFrequency - 1);
	if (DelayMultiplier < 1) DelayMultiplier = 1;
	if (DelayMultiplier == 1) {
		AddProgramLineToSequenceList(1, 0, DelayMultiplier * 3 - 2); //CMD_STEP doing nothing, just in order to keep the time calculated by COutput aligned with the time used by the FPGA; needed because the four commands above only consume 4 FPGA cycles, but are accounted for with 2 bus cycles by COutput
		AddProgramLineToSequenceList(1, 0, DelayMultiplier * 3 - 2); //CMD_STEP doing nothing, just in order to keep the time calculated by COutput aligned with the time used by the FPGA; needed because the four commands above only consume 4 FPGA cycles, but are accounted for with 2 bus cycles by COutput
	} else {
		AddProgramLineToSequenceList(1, 0, DelayMultiplier * 3 - 4); //CMD_STEP doing nothing, just in order to keep the time calculated by COutput aligned with the time used by the FPGA; needed because the four commands above only consume 4 FPGA cycles, but are accounted for with 2 bus cycles by COutput
	}

	//debugging of BRAM operation only
	//for (unsigned long n=0;n<0xFF;n++) 
	//	AddCommandWriteInputBuffer(/*write_next_address*/ false, /* input_buf_mem_address */ n*2, /*input_buf_mem_data*/0xDEAD0000+n, /* wait_time_in_FPGA_cycles*/ 5);
	
}

void CEthernetMultiIOControllerOpticsFoundry::SwitchDebugLED(bool OnOff) {
	AddCommandSetCoreOption_LED(OnOff);
}

void CEthernetMultiIOControllerOpticsFoundry::IgnoreTCPIP(bool OnOff) {
	AddCommandSetPLToPSCommand((OnOff) ? PLToPSCommandIgnoreTCPIP : PLToPSCommandAcceptTCPIP);
}

void CEthernetMultiIOControllerOpticsFoundry::AddMarker(unsigned char Marker) {
	AddCommandSetPLToPSCommand(PLToPSCommandTest | Marker);
}

void CEthernetMultiIOControllerOpticsFoundry::AddProgramLineToSequenceList(unsigned char command, unsigned __int32 data, unsigned __int32 delay) {
	const unsigned __int32 delay_mask_low = 0x7FFFFFF; //27 bit
	const unsigned __int32 delay_mask_high = 0x0F; // 4 bit -> total of 31 bits
	const unsigned __int32 bus_data_mask = 0x0FFFFFFF;
	const unsigned char command_mask = 0x1F;  //5 bit
	//unsigned char command = 1; //CMD_STEP

	unsigned __int32 low_buffer = ((delay & delay_mask_low) << 5) + (command_mask & command);
	unsigned __int32 high_buffer = ((bus_data_mask & data) << 4) | ((delay >> 27) & delay_mask_high);
	AddSequencerCommandToSequenceList( high_buffer, low_buffer);
}

void CEthernetMultiIOControllerOpticsFoundry::AddProgramLine(unsigned __int32* buffer, unsigned __int32 n, unsigned char command, unsigned __int32 data, unsigned __int32 delay ) {
	const unsigned __int32 delay_mask_low = 0x7FFFFFF; //27 bit
	const unsigned __int32 delay_mask_high = 0x0F; // 4 bit -> total of 31 bits
	const unsigned __int32 bus_data_mask = 0x0FFFFFFF;
	const unsigned char command_mask = 0x1F;  //5 bit
	//unsigned char command = 1; //CMD_STEP
	
	buffer[n * 2 + 0] = ((delay & delay_mask_low) << 5) + (command_mask & command);  
	buffer[n * 2 + 1] = ((bus_data_mask & data) << 4) | ((delay >> 27) & delay_mask_high);  


	if (DebugBufferFile) {
		CString buf;
		buf.Format("%8u CMD %x data = %08X wait = %08X", n, command, data, delay);
		(*DebugBufferFile) << buf << endl;
	}
}


void CEthernetMultiIOControllerOpticsFoundry::SetStrobeOptions(unsigned __int32* buffer, unsigned __int32 n, unsigned char strobe_choice, unsigned char strobe_low_length, unsigned char strobe_high_length) {
	const unsigned char command_mask = 0x1F;  //5 bit
	unsigned char command = CMD_SET_STROBE_OPTIONS;

	if (UseCommandBuffer) {
		buffer[n * 2 + 0] = ((strobe_high_length & 0xFF) << 24) | ((strobe_low_length & 0xFF) << 16) | ((strobe_choice & 0x07) << 8) | (command_mask & CMD_LOAD_COMMAND_BUFFER);
		buffer[n * 2 + 1] = 0;
		buffer[(n+1) * 2 + 0] = ((strobe_high_length & 0xFF) << 24) | ((strobe_low_length & 0xFF) << 16) | ((strobe_choice & 0x07) << 8) | (command_mask & command);
		buffer[(n+1) * 2 + 1] = 0;
	}
	else {
		buffer[n * 2 + 0] = ((strobe_high_length & 0xFF) << 24) | ((strobe_low_length & 0xFF) << 16) | ((strobe_choice & 0x07) << 8) | (command_mask & command);
		buffer[n * 2 + 1] = 0;
	}

	if (DebugBufferFile) {
		CString buf;
		buf.Format("%8u CMD %x data = %08X %08X", n, command, buffer[n * 2 + 1], buffer[n * 2 + 0]);
		(*DebugBufferFile) << buf << endl;
	}
}

void CEthernetMultiIOControllerOpticsFoundry::SetBufferWriteSystemTimeToInputMemory(unsigned __int32* buffer, unsigned __int32 n) {
	const unsigned char command_mask = 0x1F;  //5 bit
	unsigned char command = CMD_SAVE_CYCLE_COUNT_SINCE_STARTUP_IN_INPUT_BUF_MEM;
	buffer[n * 2 + 0] = (command_mask & command);
	buffer[n * 2 + 1] = 0;

	if (DebugBufferFile) {
		CString buf;
		buf.Format("%8u CMD %x data = %08X %08X", n, command, buffer[n * 2 + 1], buffer[n * 2 + 0]);
		(*DebugBufferFile) << buf << endl;
	}
}

void CEthernetMultiIOControllerOpticsFoundry::AddExternalTrigger(unsigned __int32* buffer, unsigned __int32 n, bool ExternalTrigger0, bool ExternalTrigger1, bool FPGASoftwareTrigger) {
// if (UseCommandBuffer) uses two command slots	ELSE uses one command slot
	if (ExternalTrigger0 || ExternalTrigger1 || FPGASoftwareTrigger) {
		//CMD_WAIT_FOR_TRIGGER: begin
		//	if ((trigger_0 && (command[8:8] == 1)) || (trigger_1 && (command[9:9] == 1)) || (trigger_PS && (command[10:10] == 1))) address <= address + 1;
		//end

		const unsigned char command_mask = 0x1F;  //5 bit
		unsigned char command = CMD_WAIT_FOR_TRIGGER;
		unsigned char trigger0 = (ExternalTrigger0) ? 1 : 0;
		unsigned char trigger1 = (ExternalTrigger1) ? 2 : 0;
		unsigned char softtrigger = (ExternalTrigger1) ? 4 : 0;

		if (UseCommandBuffer) {
			buffer[n * 2 + 0] = ((trigger0 | trigger1 | softtrigger) << 8) | (command_mask & CMD_LOAD_COMMAND_BUFFER);
			buffer[n * 2 + 1] = 0;
			buffer[(n+1) * 2 + 0] = ((trigger0 | trigger1 | softtrigger) << 8) | (command_mask & command);
			buffer[(n+1) * 2 + 1] = 0;
		}
		else {
			buffer[n * 2 + 0] = ((trigger0 | trigger1 | softtrigger) << 8) | (command_mask & command);
			buffer[n * 2 + 1] = 0;
		}
	}
	else {
		AddProgramLine(buffer, n, 1, 0, 1); //CMD_STEP
		if (UseCommandBuffer) AddProgramLine(buffer, n + 1, 1, 0, 1); //CMD_STEP
	}
	if (DebugBufferFile) {
		CString buf;
		buf.Format("%8u CMD %x data = %08X %08X", n, 11, buffer[n * 2 + 1], buffer[n * 2 + 0]);
		(*DebugBufferFile) << buf << endl;
	}
}

void CEthernetMultiIOControllerOpticsFoundry::SetTriggerOptions( unsigned __int32* buffer, unsigned __int32 n, bool ExternalTrigger0, bool ExternalTrigger1) {
// if (UseCommandBuffer) uses six command slots	ELSE uses three command slot
	if (SetPeriodicTriggerAtBeginningOfNextSequence && (PeriodicTriggerPeriod_in_s > 0)) {
		SetPeriodicTriggerAtBeginningOfNextSequence = false;
		//CMD_SET_PERIODIC_TRIGGER_PERIOD: begin
		//	periodic_trigger_period <= command[55:8]; // >>8 =  [47:0] = 48 bit;  55:32 = 23:0 = 24 bit 
		//end
		unsigned __int64 PeriodicTriggerPeriod = floor(PeriodicTriggerPeriod_in_s * FPGAClockFrequencyInHz / 1000);
		const unsigned char command_mask = 0x1F;  //5 bit
		unsigned char command = CMD_SET_PERIODIC_TRIGGER_PERIOD;
		
		if (UseCommandBuffer) {
			buffer[n * 2 + 0] = ((PeriodicTriggerPeriod & 0xFFFFFF) << 8) | (command_mask & CMD_LOAD_COMMAND_BUFFER);  //low 24 bit << 8
			buffer[n * 2 + 1] = (PeriodicTriggerPeriod >> 24) & 0xFFFFFF; // high 24 bit
			buffer[(n+1) * 2 + 0] = ((PeriodicTriggerPeriod & 0xFFFFFF) << 8) | (command_mask & command);  //low 24 bit << 8
			buffer[(n+1) * 2 + 1] = (PeriodicTriggerPeriod >> 24) & 0xFFFFFF; // high 24 bit
		}
		else {
			buffer[n * 2 + 0] = ((PeriodicTriggerPeriod & 0xFFFFFF) << 8) | (command_mask & command);  //low 24 bit << 8
			buffer[n * 2 + 1] = (PeriodicTriggerPeriod >> 24) & 0xFFFFFF; // high 24 bit
		}
		//CMD_SET_PERIODIC_TRIGGER_ALLOWED_WAIT_TIME: begin 
		//	periodic_trigger_allowed_wait_cycles <= command[55:8]; // >>8 =  [47:0] = 48 bit;  55:32 = 23:0 = 24 bit 
		//end
		unsigned __int64 PeriodicTriggerAllowedWaitCycles = floor(PeriodicTriggerAllowedWait_in_s * FPGAClockFrequencyInHz / 1000);
		command = CMD_SET_PERIODIC_TRIGGER_ALLOWED_WAIT_TIME;
		if (UseCommandBuffer) {
			buffer[(n + 2) * 2 + 0] = ((PeriodicTriggerAllowedWaitCycles & 0xFFFFFF) << 8) | (command_mask & CMD_LOAD_COMMAND_BUFFER);  //low 24 bit << 8
			buffer[(n + 2) * 2 + 1] = (PeriodicTriggerAllowedWaitCycles >> 24) & 0xFFFFFF; // high 24 bit
			buffer[(n + 3) * 2 + 0] = ((PeriodicTriggerAllowedWaitCycles & 0xFFFFFF) << 8) | (command_mask & command);  //low 24 bit << 8
			buffer[(n + 3) * 2 + 1] = (PeriodicTriggerAllowedWaitCycles >> 24) & 0xFFFFFF; // high 24 bit

		}
		else {
			buffer[(n + 1) * 2 + 0] = ((PeriodicTriggerAllowedWaitCycles & 0xFFFFFF) << 8) | (command_mask & command);  //low 24 bit << 8
			buffer[(n + 1) * 2 + 1] = (PeriodicTriggerAllowedWaitCycles >> 24) & 0xFFFFFF; // high 24 bit
		}
		LastPeriodicTriggerPeriod_in_s = PeriodicTriggerPeriod_in_s;
		AddExternalTrigger(buffer, (UseCommandBuffer) ? n + 4 : n + 2, ExternalTrigger0, ExternalTrigger1, false);
	}
	else if (WaitForPeriodicTriggerAtBeginningOfSequence && (LastPeriodicTriggerPeriod_in_s > 0)) {


		

		if (UseCommandBuffer) {
			
			core_option_LED = true;
			unsigned char command = CMD_LOAD_COMMAND_BUFFER;
			buffer[(n + 0) * 2 + 0] =  command;
			buffer[(n + 0) * 2 + 1] = ((core_option_PL_to_PS & 0xFF) << 24) | ((core_option_dig_out & 0xFF) << 8) | ((core_option_SPI_CS & 0x0F) << 1) | (core_option_LED & 0x01);
			command = CMD_SET_OPTIONS;
			buffer[(n + 1) * 2 + 0] = command;
			buffer[(n + 1) * 2 + 1] = ((core_option_PL_to_PS & 0xFF) << 24) | ((core_option_dig_out & 0xFF) << 8) | ((core_option_SPI_CS & 0x0F) << 1) | (core_option_LED & 0x01);

			
			
			//AddProgramLine(buffer, n, CMD_STEP, 0, 0);
			//AddProgramLine(buffer, n + 1, CMD_STEP, 0, 0);
			AddProgramLine(buffer, n + 2, CMD_STEP, 0, 1);
			//buffer[(n + 2) * 2 + 0] = CMD_WAIT_FOR_PERIODIC_TRIGGER;
			//buffer[(n + 2) * 2 + 1] = 0;
			buffer[(n + 3) * 2 + 0] = CMD_WAIT_FOR_PERIODIC_TRIGGER;
			buffer[(n + 3) * 2 + 1] = 0;

			
			
			core_option_LED = false;
			command = CMD_LOAD_COMMAND_BUFFER;
			buffer[(n + 4) * 2 + 0] = command;
			buffer[(n + 4) * 2 + 1] = ((core_option_PL_to_PS & 0xFF) << 24) | ((core_option_dig_out & 0xFF) << 8) | ((core_option_SPI_CS & 0x0F) << 1) | (core_option_LED & 0x01);
			command = CMD_SET_OPTIONS;
			buffer[(n + 5) * 2 + 0] = command;
			buffer[(n + 5) * 2 + 1] = ((core_option_PL_to_PS & 0xFF) << 24) | ((core_option_dig_out & 0xFF) << 8) | ((core_option_SPI_CS & 0x0F) << 1) | (core_option_LED & 0x01);

			
			//AddProgramLine(buffer, n + 4, CMD_STEP, 0, 0);
			//AddProgramLine(buffer, n + 5, CMD_STEP, 0, 0);
		}
		else {
			AddProgramLine(buffer, n, CMD_STEP, 0, 1);
			AddProgramLine(buffer, n + 1, CMD_STEP, 0, 1);
			buffer[(n + 2) * 2 + 0] = CMD_WAIT_FOR_PERIODIC_TRIGGER;
			buffer[(n + 2) * 2 + 1] = 0;
		}
	}
	else {
		if (UseCommandBuffer) {
			AddProgramLine(buffer, n, CMD_STEP, 0, 1);
			AddProgramLine(buffer, n + 1, CMD_STEP, 0, 1);
			AddProgramLine(buffer, n + 2, CMD_STEP, 0, 1);
			AddProgramLine(buffer, n + 3, CMD_STEP, 0, 1);
			AddExternalTrigger(buffer, n + 4, ExternalTrigger0, ExternalTrigger1, false);
		}
		else {
			AddProgramLine(buffer, n, CMD_STEP, 0, 1);
			AddProgramLine(buffer, n + 1, CMD_STEP, 0, 1);
			AddExternalTrigger(buffer, n + 2, ExternalTrigger0, ExternalTrigger1, false);
		}
	}
}


//CString CommandNames[NrCommands] = { "CMD_STOP", "CMD_STEP", "CMD_STEP_AND_ENTER_FAST_MODE", "CMD_SET_OPTIONS", "CMD_LOAD_REG_LOW", "CMD_LOAD_REG_HIGH", "CMD_LATCH_STATE", "CMD_RESET_WAIT_CYCLES", "CMD_LONG_WAIT", "CMD_SET_STROBE_OPTIONS", "CMD_SET_INPUT_BUF_MEM", "CMD_WAIT_FOR_TRIGGER", "CMD_SET_LOOP_COUNT", "CMD_CONDITIONAL_JUMP_FORWARD", "CMD_CONDITIONAL_JUMP_BACKWARD", "CMD_I2C_OUT", "CMD_SPI_OUT_IN", "CMD_INPUT_REPEATED_OUT_IN", "CMD_SET_PERIODIC_TRIGGER_PERIOD", "CMD_SET_PERIODIC_TRIGGER_ALLOWED_WAIT_TIME", "CMD_WAIT_FOR_PERIODIC_TRIGGER", "CMD_WAIT_FOR_WAIT_CYCLE_NR", "CMD_DIG_IN", "CMD_TRIGGER_SECONDARY_PL_PS_INTERRUPT", "CMD_ANALOG_IN_OUT", "CMD_PL_TO_PS_COMMAND" };

std::string format_binary(unsigned int value) {
	std::string bits = std::bitset<16>(value).to_string();
	std::string formatted;

	for (size_t i = 0; i < bits.size(); ++i) {
		formatted += bits[i];
		if ((i + 1) % 4 == 0 && i != bits.size() - 1) {
			formatted += ' ';
		}
	}

	return formatted;
}


void CEthernetMultiIOControllerOpticsFoundry::WriteBusDeviceCommandsToFile(unsigned __int32* buffer, unsigned long length) {
	ofstream out;
	CString DebugFilename;
	DebugFilename.Format("%sDebugBusDevice_IP%s_Addr(%u...%u)_%u.dat", *DebugFilePath, MyIPAddress, DebugBusDeviceAddressStart, DebugBusDeviceAddressStop, DebugBusDeviceFileNumber);
	DebugBusDeviceFileNumber++;
	out.open(DebugFilename);// , CFile::modeCreate | CFile::modeWrite);
	CString buf;
	double time = 0;
	unsigned __int32 low_command_buffer = 0;
	unsigned __int32 high_command_buffer = 0;
	if (DebugBusDeviceOnlyBusContent) {
		//buf.Format(" Addr = %02X Bus = %04X = MSB %s LSB Delay = %08X\n", (bus_data >> 16) & 0xFF, data, format_binary(data).c_str(), delay_low);
		out << "Addr Busdata \n";
	}

	for (unsigned long n = 0; n < length; n++) {
		unsigned __int32 low_buffer = buffer[2 * n];
		unsigned __int32 high_buffer = buffer[2 * n + 1];
		unsigned char command = low_buffer & 0x1F;
		if (command == CMD_STEP) {
			/*
			 CMD_STEP:begin
							wait_time[30:0] <= command[35:5];
							wait_time[47:31] <= 0;
							bus_data <= command[63:36];
							if (bus_clock) bus_clock <= 0; else bus_clock <= 1;
							strobe_generator_state <= DELAY_CYCLE;
						end
			*/
			unsigned __int32 delay_low = ((high_buffer & 0xF) << (32 - 5)) | (low_buffer >> 5);
			unsigned __int32 bus_data = high_buffer >> 4;
			unsigned __int32 bus_device_address = (bus_data >> 16) & 0xFF; //highest 12 bits
			if ((bus_device_address >= DebugBusDeviceAddressStart) && (bus_device_address <= DebugBusDeviceAddressStop)) {
				if (DebugBusDeviceOnlyBusContent) {
					unsigned int data = bus_data & 0xFFFF;
					//buf.Format(" Addr = %02X Bus = %04X = MSB %s LSB Delay = %08X\n", (bus_data >> 16) & 0xFF, data, format_binary(data).c_str(), delay_low);
					buf.Format("%02X %04X\n", (bus_data >> 16) & 0xFF, data);
				} else {
					buf.Format("%05X: t= % 04.5f ms ", n, time);
					out << buf;
					buf.Format("%08X %08X %s", high_buffer, low_buffer, CommandNames[command]);
					out << buf;
					buf = "";
					buf.Format(" bus = %07X ; delay = %08X\n", bus_data, delay_low);
				}
				out << buf;
			}
			time += delay_low * 10 * 0.000001;
		}
	}
	out.close();
}

void CEthernetMultiIOControllerOpticsFoundry::WriteBufferToFile(unsigned __int32* buffer, unsigned long length) { //length in command lines
	ofstream out;
	CString DebugFilename;
	DebugFilename.Format("%sDebugBuf_IP%s_%u.dat", *DebugFilePath, MyIPAddress, DebugFileNumber);
	DebugFileNumber++;
	out.open(DebugFilename);// , CFile::modeCreate | CFile::modeWrite);
	CString buf;
	double time = 0;
	unsigned __int32 low_command_buffer = 0;
	unsigned __int32 high_command_buffer = 0;

	for (unsigned long n = 0; n < length; n++) {
		//buf.Format("%05lu: ", n);
		buf.Format("%05X: t= % 04.5f ms ", n, time);
		out << buf;
		unsigned __int32 low_buffer = buffer[2 * n];
		unsigned __int32 high_buffer = buffer[2 * n + 1];
		unsigned char command = low_buffer & 0x1F;
		buf.Format("%08X %08X %s", high_buffer, low_buffer, CommandNames[command]);
		out << buf;
		buf = "";
		if (command == CMD_LOAD_COMMAND_BUFFER) {
			low_command_buffer = low_buffer;
			high_command_buffer = high_buffer;
		}
		else if (CommandUsesBuffer[command]) {
			if (UseCommandBuffer) {
				low_buffer = low_command_buffer;
				high_buffer = high_command_buffer;
			}
		}
		if (command == CMD_STEP) {
			/*
			 CMD_STEP:begin
							wait_time[30:0] <= command[35:5];
							wait_time[47:31] <= 0;
							bus_data <= command[63:36];
							if (bus_clock) bus_clock <= 0; else bus_clock <= 1;
							strobe_generator_state <= DELAY_CYCLE;
						end
			*/
			unsigned __int32 delay_low = ((high_buffer & 0xF) << (32 - 5)) | (low_buffer >> 5);
			unsigned __int32 bus_data = high_buffer >> 4;
			buf.Format(" bus = %07X ; delay = %08X", bus_data, delay_low);
			time += delay_low * 10 * 0.000001;
		}
		else if (command == CMD_INPUT_REPEATED_OUT_IN) {
			/*
			CMD_INPUT_REPEATED_OUT_IN : begin   //This is a two cycle operation. The last state has to be LOAD_EXTENDED_DATA, in order to avoid writing the flg given here to the channels. The opcode I2C_OUT is encountered in that state, and argument stored. Here we use this stored argument.
								INPUT_REPEAT_repeats <= command[27:8];
								INPUT_REPEAT_wait <= command[55:32]; //Use LOAD_EXTENDED_DATA before INPUT_REPEATED_OUT to copy 64-bit channel content to extended_data
								INPUT_REPEAT_trigger_secondary_interrupt_when_finished <= command[56:56];
								INPUT_REPEAT_state <= INPUT_REPEAT_START;
								INPUT_REPEAT_command <= command[59:57];
								if (bus_clock) bus_clock <= 0; else bus_clock <= 1;
								strobe_generator_state <= DELAY_CYCLE;
							end
			*/
			unsigned __int32 repeats = (low_buffer >> 8) & 0x3FFF;
			unsigned __int32 wait = high_buffer & 0x3FFF;
			unsigned __int32 trigger_secondary_interrupt_when_finished = (high_buffer >> 24) & 0x01;
			unsigned __int32 command = (high_buffer >> 25) & 0x07;
			/*
					if (INPUT_REPEAT_command[0] == 1) SPI_state <= SPI_START;
                    if (INPUT_REPEAT_command[1] == 1) DIG_IN_state <= DIG_IN_START;
                    if (INPUT_REPEAT_command[2] == 1) ANA_IN_state <= ANA_IN_START;
			*/

			buf.Format(" repeats = %u ; wait = %u * 10ns ; trigger_secondary_interrupt_when_finished = %01X ; command = %01X", repeats, wait, trigger_secondary_interrupt_when_finished, command);
		}
		else if (command == CMD_SET_PERIODIC_TRIGGER_PERIOD) {
			/*
			CMD_SET_PERIODIC_TRIGGER_PERIOD: begin
				periodic_trigger_period <= command[55:8]; // >>8 =  [47:0] = 48 bit;  55:32 = 23:0 = 24 bit
			end
			*/
			unsigned __int32 periodic_trigger_period = (low_buffer >> 8) & 0xFFFFFF;
			buf.Format(" periodic_trigger_period = %06X%06X", high_buffer, periodic_trigger_period);
		}
		else if (command == CMD_SET_PERIODIC_TRIGGER_ALLOWED_WAIT_TIME) {
			/*
			CMD_SET_PERIODIC_TRIGGER_ALLOWED_WAIT_TIME: begin
				periodic_trigger_allowed_wait_cycles <= command[55:8]; // >>8 =  [47:0] = 48 bit;  55:32 = 23:0 = 24 bit
			end
			*/
			unsigned __int32 periodic_trigger_allowed_wait_cycles = (low_buffer >> 8) & 0xFFFFFF;
			buf.Format(" periodic_trigger_allowed_wait_cycles = %06X%06X", high_buffer, periodic_trigger_allowed_wait_cycles);
		}
		else if (command == CMD_SPI_OUT_IN) {
			/*
			CMD_SPI_OUT_IN : begin
								SPI_OUT_length <= command[14:8];
								SPI_IN_length <= command[20:16];
								SPI_SEL_next <= command[34:32];
								SPI_data <= register;  //Use CMD_LOAD_REG_LOW and CMD_LOAD_REG_HIGH before CMD_SPI_OUT_IN to copy 117-bit I2C data to register
								INPUT_REPEAT_state <= INPUT_REPEAT_IDLE;
								if (command[40:40] == 0) SPI_state <= SPI_START;
							end
			*/
			unsigned __int32 SPI_OUT_length = (low_buffer >> 8) & 0x3F;
			unsigned __int32 SPI_IN_length = (low_buffer >> 16) & 0x1F;
			unsigned __int32 SPI_SEL_next = (low_buffer >> 24) & 0x07;
			unsigned __int32 start_now = (high_buffer >> 8) & 0x01;
			buf.Format(" SPI_OUT_length = %02X ; SPI_IN_length = %02X ; SPI_SEL_next = %01X ; start_now = %01X", SPI_OUT_length, SPI_IN_length, SPI_SEL_next, start_now);
		}
		else if (command == CMD_ANALOG_IN_OUT) {
			/*
			CMD_ANALOG_IN_OUT: begin
								adc_register_address <= command[14:8];  //to read standard analog in, this should be 3, see Xilinx user guide UG480
								adc_write_enable <= command[15:15];
								adc_programming_out <= command[31:16];
								wait_time[29:0] <= command[63:34];
								wait_time[47:30] <= 0;
								INPUT_REPEAT_state <= INPUT_REPEAT_IDLE;
								if (command[32:32] == 0) begin  //if command[40:40] is high, the actual reading will be started trhough CMD_REPEAT
									if (command[33:33] == 0) ANA_IN_state <= ANA_IN_START; //conversion and register read
									else ANA_IN_state <= ANA_IN_TRIGGER_READ_WRITE; //only register read or write
								end
							end
			*/
			unsigned __int32 adc_register_address = (low_buffer >> 8) & 0x7F;
			unsigned __int32 adc_write_enable = (low_buffer >> 15) & 0x01;
			unsigned __int32 adc_programming_out = (low_buffer >> 16) & 0xFFFF;
			unsigned __int32 wait_time = (low_buffer >> 2) & 0x3FFF;
			unsigned __int32 start_now = (high_buffer) & 0x01;
			unsigned __int32 adc_conversion = (high_buffer >> 1) & 0x01;
			buf.Format(" adc_register_address = %02X ; adc_write_enable = %01X ; adc_programming_out = %04X ; wait_time = %04X ; start_now = %01X ; adc_conversion = %01X", adc_register_address, adc_write_enable, adc_programming_out, wait_time, start_now, adc_conversion);
		}
		else if (command == CMD_SET_STROBE_OPTIONS) {
			/*
			CMD_SET_STROBE_OPTIONS: begin
							strobe_choice <= command[11:8]; // 3 bit
							strobe_low_length <= command[23:16]; // 8 bit
							strobe_high_length <= command[31:24]; // 8 bit
						end
			*/
			unsigned __int32 strobe_choice = (low_buffer >> 8) & 0x07;
			unsigned __int32 strobe_low_length = (low_buffer >> 16) & 0xFF;
			unsigned __int32 strobe_high_length = (low_buffer >> 24) & 0xFF;
			buf.Format(" strobe_choice = %01X ; strobe_low_length = %02X ; strobe_high_length = %02X", strobe_choice, strobe_low_length, strobe_high_length);
		}
		else if (command == CMD_LOAD_REG_LOW) {
			/*
			CMD_LOAD_REG_LOW:begin
							register[58:0] <= command[63:5];
						end
			*/
			unsigned __int32 register_data = (low_buffer >> 5) & 0x7FFFFFFFF;
			buf.Format(" register_data = %010X", register_data);
		}
		else if (command == CMD_LOAD_REG_HIGH) {
			/*
			CMD_LOAD_REG_HIGH:begin
							register[117:59] <= command[63:5];
						end
			*/
			unsigned __int32 register_data = (low_buffer >> 5) & 0x7FFFFFFFF;
			buf.Format(" register_data = %010X", register_data);
		}
		else if (command == CMD_WAIT_FOR_TRIGGER) {
			/*
			CMD_WAIT_FOR_TRIGGER: begin
							if ((trigger_0 && (command[8:8] == 1)) || (trigger_1 && (command[9:9] == 1)) || (trigger_PS && (command[10:10] == 1))) address <= address + 1;
						end
			*/
			unsigned __int32 trigger0 = (low_buffer >> 8) & 0x01;
			unsigned __int32 trigger1 = (low_buffer >> 9) & 0x01;
			unsigned __int32 trigger_PS = (low_buffer >> 10) & 0x01;
			buf.Format(" trigger0 = %01X ; trigger1 = %01X ; trigger_PS = %01X", trigger0, trigger1, trigger_PS);
		}
		else if (command == CMD_SET_LOOP_COUNT) {
			/*
			CMD_SET_LOOP_COUNT: begin
							loop_count <= command[63:32];
						end
			*/
			buf.Format(" loop_count = %06X", high_buffer);
		}
		else if (command == CMD_CONDITIONAL_JUMP_FORWARD) {
			/*
			CMD_CONDITIONAL_JUMP_FORWARD: begin  //here we assume that the program assembling the sequence has made sure that the jump is within the current BRAM half
							if ((condition_0 && (command[8:8] == 1)) || (condition_1 && (command[9:9] == 1)) || (condition_PS && (command[10:10] == 1)) || (command[11:11] == 1))
								address <= address + command[15:8];
							else address <= address + 1;
						end
			*/
			unsigned __int32 condition0 = (low_buffer >> 8) & 0x01;
			unsigned __int32 condition1 = (low_buffer >> 9) & 0x01;
			unsigned __int32 condition_PS = (low_buffer >> 10) & 0x01;
			unsigned __int32 condition = (low_buffer >> 11) & 0x01;
			unsigned __int32 jump = (low_buffer >> 8) & 0xFF;
			buf.Format(" condition0 = %01X ; condition1 = %01X ; condition_PS = %01X ; condition = %01X ; jump = %02X", condition0, condition1, condition_PS, condition, jump);
		}
		out << buf << endl;
	}
	out.close();
}

void CEthernetMultiIOControllerOpticsFoundry::SetPeriodicTrigger(double aPeriodicTriggerPeriod_in_s, double aPeriodicTriggerAllowedWaitTime_in_s) {
	PeriodicTriggerPeriod_in_s = aPeriodicTriggerPeriod_in_s;
	PeriodicTriggerAllowedWait_in_s = aPeriodicTriggerAllowedWaitTime_in_s;
	SetPeriodicTriggerAtBeginningOfNextSequence = true;
	WaitForPeriodicTrigger(true);
}

void CEthernetMultiIOControllerOpticsFoundry::WaitForPeriodicTrigger(bool aWaitForPeriodicTriggerAtBeginningOfSequence) {
	WaitForPeriodicTriggerAtBeginningOfSequence = aWaitForPeriodicTriggerAtBeginningOfSequence;
}

void CEthernetMultiIOControllerOpticsFoundry::TransmitOnlyDifferenceBetweenCommandSequenceIfPossible(bool aDoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible) {
	DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible = aDoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible;
	if (!DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible) {
		delete previous_command_buffer;
		previous_command_buffer = NULL;
		previous_command_buffer_length = 0;
	}
}

bool CEthernetMultiIOControllerOpticsFoundry::ModifySequence(unsigned long differences, unsigned __int32 difference_index_table[], unsigned __int32 difference_command_table[]) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptModifySequence(differences, difference_index_table, difference_command_table))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::OptimizedCommand(CString CommandString) {
	if (MinimizeEthernetCommunicationDirectionChanges) 
		return Command(CommandString + "0", /*DontWaitForReady*/ true);
	else 
		return Command(CommandString);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptModifySequence(unsigned long differences, unsigned __int32 difference_index_table[], unsigned __int32 difference_command_table[]) {
	if (!/*Optimized*/Command("modify_sequence")) return false;
	if (!WriteInteger(12 * differences)) return false;
	if (!SendData((unsigned char*)difference_index_table, 4 * differences)) return false;
	if (!SendData((unsigned char*)difference_command_table, 8 * differences)) return false;
	return true;
}


bool CEthernetMultiIOControllerOpticsFoundry::SendSequence(unsigned __int32 DataSize, unsigned __int32* buffer) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptSendSequence(DataSize, buffer))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptSendSequence(unsigned __int32 DataSize, unsigned __int32* buffer) {
	if (!/*Optimized*/Command("send_sequence")) return false;
	if (!WriteInteger(DataSize)) return false;
	if (!SendData((unsigned char*)buffer, DataSize)) return false;
	return true;
}


bool CEthernetMultiIOControllerOpticsFoundry::AddData(unsigned __int32* BusData, unsigned __int32* Spacing,/* unsigned __int32* AbsoluteTime,*/ unsigned long Count, unsigned long TickCount, bool IgnoreSpecialFPGACommand) {
	//delete AbsoluteTime;
	//AbsoluteTime = NULL;
	//if (!Connected) return true;
	
	if (DebugBufferFile) {
		(*DebugBufferFile) << endl;
	}
	////Timestamp.Mark("AddData");
	unsigned __int32 PreambleProgramLines = (UseCommandBuffer) ? 9 : 5;  //make sure you put the number of program lines in the preamble here
	unsigned __int32 PostProgramLines = 11;  //make sure you put the number of program lines in the preamble here
	unsigned __int32 DataSize = (PreambleProgramLines + PostProgramLines + Count) * 8; //data size in byte

	//CString buf;
	//buf.Format("CNetwork::ReceiveMsg :: sending  %u bytes", DataSize);
	//ControlMessageBox(buf);

	unsigned __int32* buffer = new unsigned __int32[(PreambleProgramLines + PostProgramLines + Count + MaxSequencerCommandListSize) * 4];

	//Add preamble for external clock and trigger setup

	unsigned __int32 DelayMultiplier = floor(FPGAClockFrequencyInHz / BusFrequency - 1);
	if (DelayMultiplier < 1) DelayMultiplier = 1;

	unsigned int StrobeDelay = ((DelayMultiplier + 1) / 3) - 1;

	SetBufferWriteSystemTimeToInputMemory(buffer, 0); //write time stamp to first command line
	//strobe/clock output pin content: 0: clock 1: strobe, 2: low, 3: high, 4: flags_hi[31]
	SetStrobeOptions(buffer, 1, (FPGAUseStrobeGenerator) ? 1 : 0, StrobeDelay, StrobeDelay); // this command fills 1 command lines or if (UseCommandBuffer) 2 command lines
	SetTriggerOptions(buffer, (UseCommandBuffer) ? 3 : 2, ExternalTrigger0, ExternalTrigger1); // this command fills 3 command lines or if (UseCommandBuffer) 6 command lines

	//end of preamble
	unsigned __int32 FPGA_Special_Command = BusSequencerSpecialCommand << (2 + 16);
	unsigned __int32 FPGA_Special_Command_Mask = 0xF << (BusBitShift + 2 + 16);
	unsigned __int32 AdditionalSteps = 0;

	for (unsigned __int32 n = 0; n < Count; n++) {
		if (((BusData[n] & FPGA_Special_Command_Mask) == FPGA_Special_Command) && (!IgnoreSpecialFPGACommand)) {
			//ToDo: add devices that can add special commands, e.g. analog input, I2C etc. 
			unsigned __int16 bus_data_without_address = BusData[n] & 0xFFFF;
			if (bus_data_without_address < SequencerCommandListSize) {
				AddSequencerCommandToBuffer(buffer, n + AdditionalSteps + PreambleProgramLines, SequencerCommandList[bus_data_without_address * 2 + 1], SequencerCommandList[bus_data_without_address * 2]);
				if (Spacing[n] > 0) {
					//This code should be necessary for timing to be correct. However when it's added input doesn't work anymore. Also NI status bar doesn't work and WaitForSequenceEnd sent too early.
					AdditionalSteps++;
					AddProgramLine(buffer, n + AdditionalSteps + PreambleProgramLines, CMD_STEP, 0, Spacing[n] * DelayMultiplier - 1);
				}
			}
			else {
				ControlMessageBox("EthernetMultiIOControllerOpticsFoundry.cpp: AddData(): special command not found in special command list.");
				AddProgramLine(buffer, n + AdditionalSteps + PreambleProgramLines, CMD_STEP, 0, DelayMultiplier);
			}
		}
		else {
			if (Spacing[n] < 1) Spacing[n] = 1; //2024 08 18: current minimum spacing is 1 clock cycle, to give BRAM time to deliver data; would need pipeline in Vivado core.sv to reduce this
			//if (n == (Count - 1)) {
			//	Spacing[n] = 1; //2025 05 30 : bug: last step has enormous delay, quick fix here
			//}
			AddProgramLine(buffer, n + AdditionalSteps + PreambleProgramLines, CMD_STEP, BusData[n], Spacing[n] * DelayMultiplier);
		}
	}
	//on Z-turn, one of the bus lines is currently on the buzzer. The following puts the bus to zero to avoid buzzing endlessly
	//When adding the feature to add code lines while code is running, it's important to overwrite this line with the first real additional code line.
	AddProgramLine(buffer, PreambleProgramLines + AdditionalSteps + Count, CMD_STEP, 0, DelayMultiplier);
	for (unsigned __int32 n = (PreambleProgramLines + AdditionalSteps + Count + 1); n < (PreambleProgramLines + AdditionalSteps + PostProgramLines - 1 + Count); n++) { //add a few code lines to make sure program is not too short for DMA transfer to start and stop correctly; might be better solved by improving MicroZed code
		AddProgramLine(buffer, n, CMD_STOP, 0, 0);
		//AddProgramLine(buffer, n, CMD_STEP, 0, DelayMultiplier); 
	}
	//AddProgramLine(buffer, Count + PreambleProgramLines + PostProgramLines - 2, CMD_STOP, 0, 0);
	AddProgramLine(buffer, Count + AdditionalSteps + PreambleProgramLines + PostProgramLines - 1, CMD_STOP, 0, 0);
	DataSize += 8 * AdditionalSteps;
	//only for debugging
	if (DebugFPGABuffer) WriteBufferToFile(buffer, DataSize / 8);
	if (DebugFPGABusDevice) WriteBusDeviceCommandsToFile(buffer, DataSize / 8);
	
	if (!Connected) {
		delete buffer;
		return true;
	}
	if (!(DataSize < MaxFPGAProgramLength * 8)) {
		ControlMessageBox("CEthernetMultiIOControllerOpticsFoundry::AddData : Program longer than PYNQ memory allows");
		delete buffer;
		return false;
	}

	if (DoTransmitOnlyDifferenceBetweenCommandSequenceIfPossible) {
		bool possible = (previous_command_buffer != NULL) && (previous_command_buffer_length == DataSize);
		if (possible) {
			constexpr unsigned long max_differences = 1000; //maximum number of differences to check; if more differences are found, the program is sent as is
			unsigned long differences = 0;
			unsigned __int32 difference_index_table[max_differences];
			unsigned __int32 difference_command_table[2 * max_differences];
			for (unsigned __int32 n = 0; n < DataSize / 8; n++) {
				if ((buffer[2*n] != previous_command_buffer[2*n]) || (buffer[2*n+1] != previous_command_buffer[2*n+1])) {
					if (differences < max_differences) {
						difference_index_table[differences] = n;
						difference_command_table[2 * differences] = buffer[2 * n ];
						difference_command_table[2 * differences + 1] = buffer[2 * n + 1];
						previous_command_buffer[2 * n] = buffer[2 * n];
						previous_command_buffer[2 * n + 1] = buffer[2 * n + 1];
						differences++;
					}
					else {
						possible = false;
						delete previous_command_buffer;
						previous_command_buffer = NULL;
						previous_command_buffer_length = 0;
						break;
					}
				}
			}			
			if (possible) {
				delete buffer;
				//arrange data as two consecutive tables
				//the first table is a list of 32-bit values, containing the list index of the 64 bit command that needs to be modified
				//the second table contains a list of the new 64-bit commands.
				/*for (unsigned __int32 n = 0; n < differences; n++) {
					difference_index_table[differences + 2 * n] = difference_command_table[2 * n];
					difference_index_table[differences + 2 * n + 1] = difference_command_table[2 * n + 1];
				}*/
				if (differences > 0) {
					ModifySequence(differences, difference_index_table, difference_command_table);					
				}
				//Command("print_sequence");//only for debug
				ClearSequencerCommandList();
				return true;
			}
		}
	}
	////Timestamp.Mark("send_sequence");
	bool ok = SendSequence(DataSize, buffer);
	if (ok) {
		CString buf;
		buf.Format("send data, %u bytes sent", DataSize);
		////Timestamp.Mark(buf);
		if (previous_command_buffer) {
			delete previous_command_buffer;
		}
		previous_command_buffer = buffer;
		previous_command_buffer_length = DataSize;
		ClearSequencerCommandList();
		//Command("print_sequence");//only for debug
		////Timestamp.Mark("end AddData");
		return true;
	}
	ClearSequencerCommandList();
	return false;
}

double CEthernetMultiIOControllerOpticsFoundry::MeasureEthernetBandwidth(unsigned __int32 DataSize, double MinimumExpected) {
	bool ok = Command("send_sequence"); //cheap way of assuring ethernet is up. Can't overcome outages within the (ok) branch.
	if (ok) {
		
		WriteInteger(DataSize);
		unsigned __int32* buffer = new unsigned __int32[DataSize];
		for (unsigned __int32 n = 0; n < DataSize; n++) {
			buffer[n] = 0;
		}
		DWORD StartTickCount = GetTickCount();
		SendData((unsigned char*)buffer, DataSize);
		CheckReady(10);
		DWORD EndTickCount = GetTickCount();
		delete buffer;
		double Bandwidth = 0.000001*DataSize * 8 / ((EndTickCount - StartTickCount) / 1000.0); //in Mbit/s
		if (Bandwidth < MinimumExpected) {
			CString message;
			message.Format("CEthernetMultiIOControllerOpticsFoundry::MeasureEthernetBandwidth : Ethernet Bandwidth = %.1f MBit/s is lower than expected. Check ethernet connection.", Bandwidth);
			ControlMessageBox(message);
		}
		return Bandwidth;
	}
	if (MinimumExpected > 0) ControlMessageBox("CEthernetMultiIOControllerOpticsFoundry::MeasureEthernetBandwidth : No ethernet connection to FPGA.");
	return -1;
}

bool CEthernetMultiIOControllerOpticsFoundry::GetAktWaveformPoint(unsigned long long& DataPointsWritten, bool& running) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptGetAktWaveformPoint(DataPointsWritten, running))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptGetAktWaveformPoint(unsigned long long& DataPointsWritten, bool &running) {
	if (!Connected) return true;
	//progress bar without communication
	//DWORD TickCounts = GetTickCount();
	//DataPointsWritten = (TickCounts - StartTickCounts)*(BusFrequency/1000);
	//return true;
	//////Timestamp.Mark("CEthernetMultiIOControllerOpticsFoundry::GetAktWaveformPoint");
	//progress bar with communication
	if (!OptimizedCommand("get_current_waveform_point")) return false;
	unsigned long long AktWaveformEntry;
	if (!ReadInt64(AktWaveformEntry)) return false; //100MHz clock cycles
	if (!ReadBool(running)) return false;
	DataPointsWritten = AktWaveformEntry/(FPGAClockFrequencyInHz/ BusFrequency); //conversion to 2MHz clock cycles
	return true;	
}

//bool CEthernetMultiIOControllerOpticsFoundry::AttemptNetworkCommand(tBoolFunction fCommand) {
//	if (!Connected) return true;
//	unsigned int attempts = 0;
//	while ((attempts < MaxReconnectAttempts) && (!fCommand())) {
//		Network->ResetConnection();
//		Sleep_ms(100);
//		attempts++;
//	}
//	return (attempts < MaxReconnectAttempts);
//}

bool CEthernetMultiIOControllerOpticsFoundry::GetNextCycleNumber(long& NextCycleNumber) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptGetNextCycleNumber(NextCycleNumber))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptGetNextCycleNumber(long& NextCycleNumber) {
	if (!Connected) return true;
	if (!/*Optimized*/Command("get_sequence_number")) return false;
	return ReadLong(NextCycleNumber);
}

bool CEthernetMultiIOControllerOpticsFoundry::SwitchDebugMode(bool OnOff) {
	DebugModeOn = OnOff;
	if (OnOff) return Command("switch_debug_mode_on");
	else return Command("switch_debug_mode_off");
	/*if (OnOff) return AttemptNetworkCommand([this]() {return Command("switch_debug_mode_on"); });
	else return AttemptNetworkCommand([this]() {return Command("switch_debug_mode_off"); }); */
}

bool CEthernetMultiIOControllerOpticsFoundry::ResetCycleNumber() {
	return Command("reset_sequence_number");
	//return AttemptNetworkCommand([this]() {return Command("reset_sequence_number"); });
}

bool CEthernetMultiIOControllerOpticsFoundry::CloseConnection() {
	Connected = false;
	return Command("close");
//	return AttemptNetworkCommand([this]() {return Command("close"); });
}

bool CEthernetMultiIOControllerOpticsFoundry::Reset() {
	//this commands resets the whole FPGA core. It should only be used if an error occured
	return Command("reset");
//	return AttemptNetworkCommand([this]() {return Command("reset"); });
}

bool CEthernetMultiIOControllerOpticsFoundry::WaitTillFinished() {
	return Command("wait_till_finished");
//	return AttemptNetworkCommand([this]() {return Command("wait_till_finished"); });
}

bool CEthernetMultiIOControllerOpticsFoundry::Start() {
	if (!Connected) return true;
	//Network->Reconnect(/*ShowErrorMessages*/ false,/*delay_ms*/1000); //2025 01 30: reset connection before starting in an attempt to avoid st
	StartTickCounts = GetTickCount();
	////Timestamp.Mark("CEthernetMultiIOControllerOpticsFoundry::Start");
	return Command("start");
//	return AttemptNetworkCommand([this]() {return Command("start"); });
}

bool CEthernetMultiIOControllerOpticsFoundry::Stop() {
	return true;
	//if (!Connected) return true;
	//return AttemptNetworkCommand([this]() {return Command("stop"); });
}


bool CEthernetMultiIOControllerOpticsFoundry::SetFrequency(double Frequency) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptSetFrequency(Frequency))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptSetFrequency(double Frequency) {
	if (!Command("set_frequency")) return false;
	return WriteDouble(Frequency);
}

void CEthernetMultiIOControllerOpticsFoundry::SetExternalTrigger(bool aExternalTrigger0, bool aExternalTrigger1) {
	ExternalTrigger0 = aExternalTrigger0;
	ExternalTrigger1 = aExternalTrigger1;
}

bool CEthernetMultiIOControllerOpticsFoundry::SetExternalClock(bool aExternalClock0, bool aExternalClock1) {
	bool changed = (ExternalClock0 != aExternalClock0) || (ExternalClock1 != aExternalClock1);
	ExternalClock0 = aExternalClock0;
	ExternalClock1 = aExternalClock1;
	if (!Connected) return true;
	if (changed) {
		if (ExternalClock0) {
			return Command("select_external_clock_0");
		}
		else if (ExternalClock1) {
			return Command("select_external_clock_1");
		}
		else {
			return Command("select_internal_clock");
		}
		/*if (ExternalClock0) {
			return AttemptNetworkCommand([this]() {return Command("select_external_clock_0"); });
		} else if (ExternalClock1) {
			return AttemptNetworkCommand([this]() {return Command("select_external_clock_1"); });
		} else {
			return AttemptNetworkCommand([this]() {return Command("select_internal_clock"); });
		}*/
	} 
	return true;
}



bool CEthernetMultiIOControllerOpticsFoundry::GetFrequency(double &Frequency) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptGetFrequency(Frequency))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptGetFrequency(double& Frequency) {
	if (!Command("get_frequency")) return false;
	return ReadDouble(Frequency);
}



bool CEthernetMultiIOControllerOpticsFoundry::GetPeriodicTriggerError(bool& Error) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptGetPeriodicTriggerError(Error))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptGetPeriodicTriggerError(bool& Error) {
	if (!/*Optimized*/Command("get_periodic_trigger_error")) return false;
	int Errori;
	bool err = ReadInt(Errori);
	Error = Errori != 0;
	return err;
}


bool CEthernetMultiIOControllerOpticsFoundry::CheckReady(double timeout_in_seconds) {
	return Command("check_ready");  //Do not use OptimizedCommand here. The whole point is to get a "Ready" back.
	//return AttemptNetworkCommand([this]() {return Command("check_ready"); }); //Do not use OptimizedCommand here. The whole point is to get a "Ready" back.
}

bool CEthernetMultiIOControllerOpticsFoundry::WaitTillEndOfSequenceThenGetInputData(unsigned char*& buffer, unsigned long& buffer_length, DWORD& EndTimeOfCycle, double timeout_in_s, bool auto_delete_buffer) {
	if (!Connected) return true;
	unsigned int attempts = 0;
	while ((attempts < MaxReconnectAttempts) && (!AttemptWaitTillEndOfSequenceThenGetInputData(buffer, buffer_length, EndTimeOfCycle, timeout_in_s, auto_delete_buffer))) {
		Network->ResetConnection();
		Sleep_ms(100);
		attempts++;
	}
	return (attempts < MaxReconnectAttempts);
}

bool CEthernetMultiIOControllerOpticsFoundry::AttemptWaitTillEndOfSequenceThenGetInputData(unsigned char * &buffer, unsigned long & buffer_length, DWORD& EndTimeOfCycle, double timeout_in_s, bool auto_delete_buffer) {
	if (InputBuffer != NULL) {
		if (InputBuffer != buffer) {
			delete[] InputBuffer;
			InputBuffer = NULL;
		}
	}
	if (buffer) delete[] buffer;
	buffer = NULL;
	buffer_length = 0;
	if (auto_delete_buffer) InputBuffer = buffer;
	//progress bar without communication
	//DWORD TickCounts = GetTickCount();
	//DataPointsWritten = (TickCounts - StartTickCounts)*(BusFrequency/1000);
	//return true;
	//progress bar with communication
	if (!Command("wait_till_end_of_sequence_then_get_input_data")) return false;
	
	int PeriodicTriggerError;
	if (!ReadInt(PeriodicTriggerError, timeout_in_s)) return false;
	EndTimeOfCycle = GetTickCount();
	//int InputBufferContentOriginSequence;
	//ReadInt(InputBufferContentOriginSequence);
	//int InputBufferContentsLength;
	//ReadInt(InputBufferContentsLength);
	unsigned long long InputBufferContentOriginSequence;
	if (!ReadInt64(InputBufferContentOriginSequence)) return false;
	unsigned long long InputBufferContentsLength;
	if (!ReadInt64(InputBufferContentsLength)) return false;
	if (InputBufferContentsLength > 0) {
		buffer = new unsigned char[InputBufferContentsLength];
		buffer_length = InputBufferContentsLength;	
		if (auto_delete_buffer) InputBuffer = buffer;
		if (Network) return Network->ReceiveData(buffer, buffer_length, /*timeout_in_ms = */ 5000);
		else {
			delete[] buffer;
			buffer = NULL;
			buffer_length = 0;
			if (auto_delete_buffer) InputBuffer = buffer;
			return false;
		}
	}
	return true;
}

