#pragma once

/************************************************
 * @file sls_detector_funcs.h
 * @short functions indices to call on server (detector/receiver)
 ***********************************************/
/**
 *@short functions indices to call on server (detector/receiver)
 */

enum detFuncs {
    F_EXEC_COMMAND = 0,
    F_GET_DETECTOR_TYPE,
    F_GET_EXTERNAL_SIGNAL_FLAG,
    F_SET_EXTERNAL_SIGNAL_FLAG,
    F_SET_TIMING_MODE,
    F_GET_FIRMWARE_VERSION,
    F_GET_SERVER_VERSION,
    F_GET_SERIAL_NUMBER,
    F_SET_FIRMWARE_TEST,
    F_SET_BUS_TEST,
    F_SET_IMAGE_TEST_MODE,
    F_GET_IMAGE_TEST_MODE,
    F_SET_DAC,
    F_GET_ADC,
    F_WRITE_REGISTER,
    F_READ_REGISTER,
    F_SET_MODULE,
    F_SET_SETTINGS,
    F_GET_THRESHOLD_ENERGY,
    F_START_ACQUISITION,
    F_STOP_ACQUISITION,
    F_GET_RUN_STATUS,
    F_START_AND_READ_ALL,
    F_GET_NUM_FRAMES,
    F_SET_NUM_FRAMES,
    F_GET_NUM_TRIGGERS,
    F_SET_NUM_TRIGGERS,
    F_GET_NUM_ADDITIONAL_STORAGE_CELLS,
    F_SET_NUM_ADDITIONAL_STORAGE_CELLS,
    F_GET_NUM_ANALOG_SAMPLES,
    F_SET_NUM_ANALOG_SAMPLES,
    F_GET_NUM_DIGITAL_SAMPLES,
    F_SET_NUM_DIGITAL_SAMPLES,
    F_GET_EXPTIME,
    F_SET_EXPTIME,
    F_GET_PERIOD,
    F_SET_PERIOD,
    F_GET_DELAY_AFTER_TRIGGER,
    F_SET_DELAY_AFTER_TRIGGER,
    F_GET_SUB_EXPTIME,
    F_SET_SUB_EXPTIME,
    F_GET_SUB_DEADTIME,
    F_SET_SUB_DEADTIME,
    F_GET_STORAGE_CELL_DELAY,
    F_SET_STORAGE_CELL_DELAY,
    F_GET_FRAMES_LEFT,
    F_GET_TRIGGERS_LEFT,
    F_GET_EXPTIME_LEFT,
    F_GET_PERIOD_LEFT,
    F_GET_DELAY_AFTER_TRIGGER_LEFT,
    F_GET_MEASURED_PERIOD,
    F_GET_MEASURED_SUBPERIOD,
    F_GET_FRAMES_FROM_START,
    F_GET_ACTUAL_TIME,
    F_GET_MEASUREMENT_TIME,
    F_SET_DYNAMIC_RANGE,
    F_SET_ROI,
    F_GET_ROI,
    F_LOCK_SERVER,
    F_GET_LAST_CLIENT_IP,
    F_SET_PORT,
    F_ENABLE_TEN_GIGA,
    F_SET_ALL_TRIMBITS,
    F_SET_PATTERN_IO_CONTROL,
    F_SET_PATTERN_WORD,
    F_SET_PATTERN_LOOP_ADDRESSES,
    F_SET_PATTERN_LOOP_CYCLES,
    F_SET_PATTERN_WAIT_ADDR,
    F_SET_PATTERN_WAIT_TIME,
    F_SET_PATTERN_MASK,
    F_GET_PATTERN_MASK,
    F_SET_PATTERN_BIT_MASK,
    F_GET_PATTERN_BIT_MASK,
    F_WRITE_ADC_REG,
    F_SET_COUNTER_BIT,
    F_PULSE_PIXEL,
    F_PULSE_PIXEL_AND_MOVE,
    F_PULSE_CHIP,
    F_SET_RATE_CORRECT,
    F_GET_RATE_CORRECT,
    F_SET_TEN_GIGA_FLOW_CONTROL,
    F_GET_TEN_GIGA_FLOW_CONTROL,
    F_SET_TRANSMISSION_DELAY_FRAME,
    F_GET_TRANSMISSION_DELAY_FRAME,
    F_SET_TRANSMISSION_DELAY_LEFT,
    F_GET_TRANSMISSION_DELAY_LEFT,
    F_SET_TRANSMISSION_DELAY_RIGHT,
    F_GET_TRANSMISSION_DELAY_RIGHT,
    F_PROGRAM_FPGA,
    F_RESET_FPGA,
    F_POWER_CHIP,
    F_ACTIVATE,
    F_THRESHOLD_TEMP,
    F_TEMP_CONTROL,
    F_TEMP_EVENT,
    F_AUTO_COMP_DISABLE,
    F_STORAGE_CELL_START,
    F_CHECK_VERSION,
    F_SOFTWARE_TRIGGER,
    F_LED,
    F_DIGITAL_IO_DELAY,
    F_COPY_DET_SERVER,
    F_REBOOT_CONTROLLER,
    F_SET_ADC_ENABLE_MASK,
    F_GET_ADC_ENABLE_MASK,
    F_SET_ADC_INVERT,
    F_GET_ADC_INVERT,
    F_EXTERNAL_SAMPLING_SOURCE,
    F_EXTERNAL_SAMPLING,
    F_SET_STARTING_FRAME_NUMBER,
    F_GET_STARTING_FRAME_NUMBER,
    F_SET_QUAD,
    F_GET_QUAD,
    F_SET_INTERRUPT_SUBFRAME,
    F_GET_INTERRUPT_SUBFRAME,
    F_SET_READ_N_LINES,
    F_GET_READ_N_LINES,
    F_SET_POSITION,
    F_SET_SOURCE_UDP_MAC,
    F_GET_SOURCE_UDP_MAC,
    F_SET_SOURCE_UDP_MAC2,
    F_GET_SOURCE_UDP_MAC2,
    F_SET_SOURCE_UDP_IP,
    F_GET_SOURCE_UDP_IP,
    F_SET_SOURCE_UDP_IP2,
    F_GET_SOURCE_UDP_IP2,
    F_SET_DEST_UDP_MAC,
    F_GET_DEST_UDP_MAC,
    F_SET_DEST_UDP_MAC2,
    F_GET_DEST_UDP_MAC2,
    F_SET_DEST_UDP_IP,
    F_GET_DEST_UDP_IP,
    F_SET_DEST_UDP_IP2,
    F_GET_DEST_UDP_IP2,
    F_SET_DEST_UDP_PORT,
    F_GET_DEST_UDP_PORT,
    F_SET_DEST_UDP_PORT2,
    F_GET_DEST_UDP_PORT2,
    F_SET_NUM_INTERFACES,
    F_GET_NUM_INTERFACES,
    F_SET_INTERFACE_SEL,
    F_GET_INTERFACE_SEL,
    F_SET_PARALLEL_MODE,
    F_GET_PARALLEL_MODE,
    F_SET_OVERFLOW_MODE,
    F_GET_OVERFLOW_MODE,
    F_SET_READOUT_MODE,
    F_GET_READOUT_MODE,
    F_SET_CLOCK_FREQUENCY,
    F_GET_CLOCK_FREQUENCY,
    F_SET_CLOCK_PHASE,
    F_GET_CLOCK_PHASE,
    F_GET_MAX_CLOCK_PHASE_SHIFT,
    F_SET_CLOCK_DIVIDER,
    F_GET_CLOCK_DIVIDER,
    F_SET_PIPELINE,
    F_GET_PIPELINE,
    F_SET_ON_CHIP_DAC,
    F_GET_ON_CHIP_DAC,
    F_SET_INJECT_CHANNEL,
    F_GET_INJECT_CHANNEL,
    F_SET_VETO_PHOTON,
    F_GET_VETO_PHOTON,
    F_SET_VETO_REFERENCE,
    F_GET_BURST_MODE,
    F_SET_BURST_MODE,
    F_SET_ADC_ENABLE_MASK_10G,
    F_GET_ADC_ENABLE_MASK_10G,
    F_SET_COUNTER_MASK,
    F_GET_COUNTER_MASK,
    F_GET_NUM_BURSTS,
    F_SET_NUM_BURSTS,
    F_GET_BURST_PERIOD,
    F_SET_BURST_PERIOD,
    F_GET_CURRENT_SOURCE,
    F_SET_CURRENT_SOURCE,
    F_GET_TIMING_SOURCE,
    F_SET_TIMING_SOURCE,
    F_GET_NUM_CHANNELS,
    F_UPDATE_RATE_CORRECTION,
    F_GET_RECEIVER_PARAMETERS,
    F_START_PATTERN,
    F_SET_NUM_GATES,
    F_GET_NUM_GATES,
    F_SET_GATE_DELAY,
    F_GET_GATE_DELAY,
    F_GET_EXPTIME_ALL_GATES,
    F_GET_GATE_DELAY_ALL_GATES,
    F_GET_VETO,
    F_SET_VETO,
    F_SET_PATTERN,
    F_GET_SCAN,
    F_SET_SCAN,
    F_GET_SCAN_ERROR_MESSAGE,
    F_GET_CDS_GAIN,
    F_SET_CDS_GAIN,
    F_GET_FILTER,
    F_SET_FILTER,
    F_SET_VETO_FILE,
    F_GET_ADC_CONFIGURATION,
    F_SET_ADC_CONFIGURATION,
    F_GET_BAD_CHANNELS,
    F_SET_BAD_CHANNELS,

    NUM_DET_FUNCTIONS,
    RECEIVER_ENUM_START = 256, /**< detector function should not exceed this
                                  (detector server should not compile anyway) */

    F_EXEC_RECEIVER_COMMAND,
    F_LOCK_RECEIVER,
    F_GET_LAST_RECEIVER_CLIENT_IP,
    F_SET_RECEIVER_PORT,
    F_GET_RECEIVER_VERSION,
    F_RECEIVER_SET_ROI,
    F_RECEIVER_SET_NUM_FRAMES,
    F_SET_RECEIVER_NUM_TRIGGERS,
    F_SET_RECEIVER_NUM_BURSTS,
    F_SET_RECEIVER_NUM_ADD_STORAGE_CELLS,
    F_SET_RECEIVER_TIMING_MODE,
    F_SET_RECEIVER_BURST_MODE,
    F_RECEIVER_SET_NUM_ANALOG_SAMPLES,
    F_RECEIVER_SET_NUM_DIGITAL_SAMPLES,
    F_RECEIVER_SET_EXPTIME,
    F_RECEIVER_SET_PERIOD,
    F_RECEIVER_SET_SUB_EXPTIME,
    F_RECEIVER_SET_SUB_DEADTIME,
    F_SET_RECEIVER_DYNAMIC_RANGE,
    F_SET_RECEIVER_STREAMING_FREQUENCY,
    F_GET_RECEIVER_STREAMING_FREQUENCY,
    F_GET_RECEIVER_STATUS,
    F_START_RECEIVER,
    F_STOP_RECEIVER,
    F_SET_RECEIVER_FILE_PATH,
    F_GET_RECEIVER_FILE_PATH,
    F_SET_RECEIVER_FILE_NAME,
    F_GET_RECEIVER_FILE_NAME,
    F_SET_RECEIVER_FILE_INDEX,
    F_GET_RECEIVER_FILE_INDEX,
    F_GET_RECEIVER_FRAME_INDEX,
    F_GET_RECEIVER_FRAMES_CAUGHT,
    F_GET_NUM_MISSING_PACKETS,
    F_SET_RECEIVER_FILE_WRITE,
    F_GET_RECEIVER_FILE_WRITE,
    F_SET_RECEIVER_MASTER_FILE_WRITE,
    F_GET_RECEIVER_MASTER_FILE_WRITE,
    F_SET_RECEIVER_OVERWRITE,
    F_GET_RECEIVER_OVERWRITE,
    F_ENABLE_RECEIVER_TEN_GIGA,
    F_SET_RECEIVER_FIFO_DEPTH,
    F_RECEIVER_ACTIVATE,
    F_SET_RECEIVER_STREAMING,
    F_GET_RECEIVER_STREAMING,
    F_RECEIVER_STREAMING_TIMER,
    F_SET_FLIPPED_DATA_RECEIVER,
    F_SET_RECEIVER_FILE_FORMAT,
    F_GET_RECEIVER_FILE_FORMAT,
    F_SET_RECEIVER_STREAMING_PORT,
    F_GET_RECEIVER_STREAMING_PORT,
    F_SET_RECEIVER_STREAMING_SRC_IP,
    F_GET_RECEIVER_STREAMING_SRC_IP,
    F_SET_RECEIVER_SILENT_MODE,
    F_GET_RECEIVER_SILENT_MODE,
    F_RESTREAM_STOP_FROM_RECEIVER,
    F_SET_ADDITIONAL_JSON_HEADER,
    F_GET_ADDITIONAL_JSON_HEADER,
    F_RECEIVER_UDP_SOCK_BUF_SIZE,
    F_RECEIVER_REAL_UDP_SOCK_BUF_SIZE,
    F_SET_RECEIVER_FRAMES_PER_FILE,
    F_GET_RECEIVER_FRAMES_PER_FILE,
    F_RECEIVER_CHECK_VERSION,
    F_SET_RECEIVER_DISCARD_POLICY,
    F_GET_RECEIVER_DISCARD_POLICY,
    F_SET_RECEIVER_PADDING,
    F_GET_RECEIVER_PADDING,
    F_SET_RECEIVER_DEACTIVATED_PADDING,
    F_GET_RECEIVER_DEACTIVATED_PADDING,
    F_RECEIVER_SET_READOUT_MODE,
    F_RECEIVER_SET_ADC_MASK,
    F_SET_RECEIVER_DBIT_LIST,
    F_GET_RECEIVER_DBIT_LIST,
    F_SET_RECEIVER_DBIT_OFFSET,
    F_GET_RECEIVER_DBIT_OFFSET,
    F_SET_RECEIVER_QUAD,
    F_SET_RECEIVER_READ_N_LINES,
    F_SET_RECEIVER_UDP_IP,
    F_SET_RECEIVER_UDP_IP2,
    F_SET_RECEIVER_UDP_PORT,
    F_SET_RECEIVER_UDP_PORT2,
    F_SET_RECEIVER_NUM_INTERFACES,
    F_RECEIVER_SET_ADC_MASK_10G,
    F_RECEIVER_SET_NUM_COUNTERS,
    F_INCREMENT_FILE_INDEX,
    F_SET_ADDITIONAL_JSON_PARAMETER,
    F_GET_ADDITIONAL_JSON_PARAMETER,
    F_GET_RECEIVER_PROGRESS,
    F_SETUP_RECEIVER,
    F_SET_RECEIVER_NUM_GATES,
    F_SET_RECEIVER_GATE_DELAY,
    F_GET_RECEIVER_THREAD_IDS,
    F_GET_RECEIVER_STREAMING_START_FNUM,
    F_SET_RECEIVER_STREAMING_START_FNUM,

    NUM_REC_FUNCTIONS
};

// clang-format off
#ifdef __cplusplus
inline const char* getFunctionNameFromEnum(enum detFuncs func) {
#else
const char* getFunctionNameFromEnum(enum detFuncs func) {
#endif
	switch (func) {
	case F_EXEC_COMMAND:					return "F_EXEC_COMMAND";
	case F_GET_DETECTOR_TYPE:				return "F_GET_DETECTOR_TYPE";
    case F_GET_EXTERNAL_SIGNAL_FLAG:        return "F_GET_EXTERNAL_SIGNAL_FLAG";
	case F_SET_EXTERNAL_SIGNAL_FLAG:		return "F_SET_EXTERNAL_SIGNAL_FLAG";
	case F_SET_TIMING_MODE:					return "F_SET_TIMING_MODE";
	case F_GET_FIRMWARE_VERSION:			return "F_GET_FIRMWARE_VERSION";	
	case F_GET_SERVER_VERSION:				return "F_GET_SERVER_VERSION";
	case F_GET_SERIAL_NUMBER:				return "F_GET_SERIAL_NUMBER";
	case F_SET_FIRMWARE_TEST:				return "F_SET_FIRMWARE_TEST";	
	case F_SET_BUS_TEST:					return "F_SET_BUS_TEST";
	case F_SET_IMAGE_TEST_MODE:				return "F_SET_IMAGE_TEST_MODE";	
	case F_GET_IMAGE_TEST_MODE:				return "F_GET_IMAGE_TEST_MODE";	
	case F_SET_DAC:							return "F_SET_DAC";
	case F_GET_ADC:							return "F_GET_ADC";
	case F_WRITE_REGISTER:					return "F_WRITE_REGISTER";
	case F_READ_REGISTER:					return "F_READ_REGISTER";
	case F_SET_MODULE:						return "F_SET_MODULE";
	case F_SET_SETTINGS:					return "F_SET_SETTINGS";
	case F_GET_THRESHOLD_ENERGY:			return "F_GET_THRESHOLD_ENERGY";
	case F_START_ACQUISITION:				return "F_START_ACQUISITION";
	case F_STOP_ACQUISITION:				return "F_STOP_ACQUISITION";
	case F_GET_RUN_STATUS:					return "F_GET_RUN_STATUS";
	case F_START_AND_READ_ALL:				return "F_START_AND_READ_ALL";
	case F_GET_NUM_FRAMES:					return "F_GET_NUM_FRAMES";
	case F_SET_NUM_FRAMES:					return "F_SET_NUM_FRAMES";
	case F_GET_NUM_TRIGGERS:				return "F_GET_NUM_TRIGGERS";
	case F_SET_NUM_TRIGGERS:				return "F_SET_NUM_TRIGGERS";
	case F_GET_NUM_ADDITIONAL_STORAGE_CELLS:return "F_GET_NUM_ADDITIONAL_STORAGE_CELLS";
	case F_SET_NUM_ADDITIONAL_STORAGE_CELLS:return "F_SET_NUM_ADDITIONAL_STORAGE_CELLS";
	case F_GET_NUM_ANALOG_SAMPLES:			return "F_GET_NUM_ANALOG_SAMPLES";
	case F_SET_NUM_ANALOG_SAMPLES:			return "F_SET_NUM_ANALOG_SAMPLES";
	case F_GET_NUM_DIGITAL_SAMPLES:			return "F_GET_NUM_DIGITAL_SAMPLES";
	case F_SET_NUM_DIGITAL_SAMPLES:			return "F_SET_NUM_DIGITAL_SAMPLES";
	case F_GET_EXPTIME:						return "F_GET_EXPTIME";
	case F_SET_EXPTIME:						return "F_SET_EXPTIME";
	case F_GET_PERIOD:						return "F_GET_PERIOD";
	case F_SET_PERIOD:						return "F_SET_PERIOD";
	case F_GET_DELAY_AFTER_TRIGGER:			return "F_GET_DELAY_AFTER_TRIGGER";
	case F_SET_DELAY_AFTER_TRIGGER:			return "F_SET_DELAY_AFTER_TRIGGER";
	case F_GET_SUB_EXPTIME:					return "F_GET_SUB_EXPTIME";
	case F_SET_SUB_EXPTIME:					return "F_SET_SUB_EXPTIME";
	case F_GET_SUB_DEADTIME:				return "F_GET_SUB_DEADTIME";
	case F_SET_SUB_DEADTIME:				return "F_SET_SUB_DEADTIME";
	case F_GET_STORAGE_CELL_DELAY:			return "F_GET_STORAGE_CELL_DELAY";
	case F_SET_STORAGE_CELL_DELAY:			return "F_SET_STORAGE_CELL_DELAY";
	case F_GET_FRAMES_LEFT:					return "F_GET_FRAMES_LEFT";
	case F_GET_TRIGGERS_LEFT:				return "F_GET_TRIGGERS_LEFT";
	case F_GET_EXPTIME_LEFT:				return "F_GET_EXPTIME_LEFT";
	case F_GET_PERIOD_LEFT:					return "F_GET_PERIOD_LEFT";
	case F_GET_DELAY_AFTER_TRIGGER_LEFT:	return "F_GET_DELAY_AFTER_TRIGGER_LEFT";
	case F_GET_MEASURED_PERIOD:				return "F_GET_MEASURED_PERIOD";
	case F_GET_MEASURED_SUBPERIOD:			return "F_GET_MEASURED_SUBPERIOD";
	case F_GET_FRAMES_FROM_START:			return "F_GET_FRAMES_FROM_START";
	case F_GET_ACTUAL_TIME:					return "F_GET_ACTUAL_TIME";
	case F_GET_MEASUREMENT_TIME:			return "F_GET_MEASUREMENT_TIME";
	case F_SET_DYNAMIC_RANGE:				return "F_SET_DYNAMIC_RANGE";
	case F_SET_ROI:							return "F_SET_ROI";
	case F_GET_ROI:							return "F_GET_ROI";
	case F_LOCK_SERVER:						return "F_LOCK_SERVER";
	case F_GET_LAST_CLIENT_IP:				return "F_GET_LAST_CLIENT_IP";
	case F_SET_PORT:						return "F_SET_PORT";
	case F_ENABLE_TEN_GIGA:					return "F_ENABLE_TEN_GIGA";
	case F_SET_ALL_TRIMBITS:				return "F_SET_ALL_TRIMBITS";
	case F_SET_PATTERN_IO_CONTROL:			return "F_SET_PATTERN_IO_CONTROL";
	case F_SET_PATTERN_WORD:				return "F_SET_PATTERN_WORD";
	case F_SET_PATTERN_LOOP_ADDRESSES:		return "F_SET_PATTERN_LOOP_ADDRESSES";
	case F_SET_PATTERN_LOOP_CYCLES:			return "F_SET_PATTERN_LOOP_CYCLES";
	case F_SET_PATTERN_WAIT_ADDR:			return "F_SET_PATTERN_WAIT_ADDR";
	case F_SET_PATTERN_WAIT_TIME:			return "F_SET_PATTERN_WAIT_TIME";
	case F_SET_PATTERN_MASK:				return "F_SET_PATTERN_MASK";
	case F_GET_PATTERN_MASK:				return "F_GET_PATTERN_MASK";
	case F_SET_PATTERN_BIT_MASK:			return "F_SET_PATTERN_BIT_MASK";
	case F_GET_PATTERN_BIT_MASK:			return "F_GET_PATTERN_BIT_MASK";
	case F_WRITE_ADC_REG:					return "F_WRITE_ADC_REG";
	case F_SET_COUNTER_BIT:					return "F_SET_COUNTER_BIT";
	case F_PULSE_PIXEL:						return "F_PULSE_PIXEL";
	case F_PULSE_PIXEL_AND_MOVE:			return "F_PULSE_PIXEL_AND_MOVE";
	case F_PULSE_CHIP:						return "F_PULSE_CHIP";
	case F_SET_RATE_CORRECT:				return "F_SET_RATE_CORRECT";
	case F_GET_RATE_CORRECT:				return "F_GET_RATE_CORRECT";
	case F_SET_TEN_GIGA_FLOW_CONTROL:		return "F_SET_TEN_GIGA_FLOW_CONTROL";
	case F_GET_TEN_GIGA_FLOW_CONTROL:		return "F_GET_TEN_GIGA_FLOW_CONTROL";
	case F_SET_TRANSMISSION_DELAY_FRAME:	return "F_SET_TRANSMISSION_DELAY_FRAME";	
	case F_GET_TRANSMISSION_DELAY_FRAME:	return "F_GET_TRANSMISSION_DELAY_FRAME";	
	case F_SET_TRANSMISSION_DELAY_LEFT:		return "F_SET_TRANSMISSION_DELAY_LEFT";	
	case F_GET_TRANSMISSION_DELAY_LEFT:		return "F_GET_TRANSMISSION_DELAY_LEFT";
	case F_SET_TRANSMISSION_DELAY_RIGHT:	return "F_SET_TRANSMISSION_DELAY_RIGHT";		
	case F_GET_TRANSMISSION_DELAY_RIGHT:	return "F_GET_TRANSMISSION_DELAY_RIGHT";
	case F_PROGRAM_FPGA:					return "F_PROGRAM_FPGA";
	case F_RESET_FPGA:						return "F_RESET_FPGA";
	case F_POWER_CHIP:						return "F_POWER_CHIP";
	case F_ACTIVATE:						return "F_ACTIVATE";
	case F_THRESHOLD_TEMP:                  return "F_THRESHOLD_TEMP";
	case F_TEMP_CONTROL:                    return "F_TEMP_CONTROL";
	case F_TEMP_EVENT:                      return "F_TEMP_EVENT";
    case F_AUTO_COMP_DISABLE:               return "F_AUTO_COMP_DISABLE";
    case F_STORAGE_CELL_START:              return "F_STORAGE_CELL_START";
    case F_CHECK_VERSION:              		return "F_CHECK_VERSION";
    case F_SOFTWARE_TRIGGER:              	return "F_SOFTWARE_TRIGGER";
    case F_LED:              				return "F_LED";
	case F_DIGITAL_IO_DELAY:              	return "F_DIGITAL_IO_DELAY";
    case F_COPY_DET_SERVER:              	return "F_COPY_DET_SERVER";
    case F_REBOOT_CONTROLLER:              	return "F_REBOOT_CONTROLLER";
	case F_SET_ADC_ENABLE_MASK:          	return "F_SET_ADC_ENABLE_MASK";
	case F_GET_ADC_ENABLE_MASK:          	return "F_GET_ADC_ENABLE_MASK";
	case F_SET_ADC_INVERT:					return "F_SET_ADC_INVERT";	
	case F_GET_ADC_INVERT:					return "F_GET_ADC_INVERT";
	case F_EXTERNAL_SAMPLING_SOURCE:		return "F_EXTERNAL_SAMPLING_SOURCE";				
	case F_EXTERNAL_SAMPLING:				return "F_EXTERNAL_SAMPLING";	
	case F_SET_STARTING_FRAME_NUMBER:		return "F_SET_STARTING_FRAME_NUMBER";
	case F_GET_STARTING_FRAME_NUMBER:		return "F_GET_STARTING_FRAME_NUMBER";
	case F_SET_QUAD:						return "F_SET_QUAD";
	case F_GET_QUAD:						return "F_GET_QUAD";
	case F_SET_INTERRUPT_SUBFRAME:			return "F_SET_INTERRUPT_SUBFRAME";
	case F_GET_INTERRUPT_SUBFRAME:			return "F_GET_INTERRUPT_SUBFRAME";
	case F_SET_READ_N_LINES:				return "F_SET_READ_N_LINES";
	case F_GET_READ_N_LINES:				return "F_GET_READ_N_LINES";
	case F_SET_POSITION:					return "F_SET_POSITION";
	case F_SET_SOURCE_UDP_MAC:				return "F_SET_SOURCE_UDP_MAC";
	case F_GET_SOURCE_UDP_MAC:				return "F_GET_SOURCE_UDP_MAC";
	case F_SET_SOURCE_UDP_MAC2:				return "F_SET_SOURCE_UDP_MAC2";
	case F_GET_SOURCE_UDP_MAC2:				return "F_GET_SOURCE_UDP_MAC2";
	case F_SET_SOURCE_UDP_IP:				return "F_SET_SOURCE_UDP_IP";
	case F_GET_SOURCE_UDP_IP:				return "F_GET_SOURCE_UDP_IP";
	case F_SET_SOURCE_UDP_IP2:				return "F_SET_SOURCE_UDP_IP2";
	case F_GET_SOURCE_UDP_IP2:				return "F_GET_SOURCE_UDP_IP2";
	case F_SET_DEST_UDP_MAC:				return "F_SET_DEST_UDP_MAC";
	case F_GET_DEST_UDP_MAC:				return "F_GET_DEST_UDP_MAC";
	case F_SET_DEST_UDP_MAC2:				return "F_SET_DEST_UDP_MAC2";
	case F_GET_DEST_UDP_MAC2:				return "F_GET_DEST_UDP_MAC2";
	case F_SET_DEST_UDP_IP:					return "F_SET_DEST_UDP_IP";
	case F_GET_DEST_UDP_IP:					return "F_GET_DEST_UDP_IP";	
	case F_SET_DEST_UDP_IP2:				return "F_SET_DEST_UDP_IP2";
	case F_GET_DEST_UDP_IP2:				return "F_GET_DEST_UDP_IP2";
	case F_SET_DEST_UDP_PORT:				return "F_SET_DEST_UDP_PORT";
	case F_GET_DEST_UDP_PORT:				return "F_GET_DEST_UDP_PORT";
	case F_SET_DEST_UDP_PORT2:				return "F_SET_DEST_UDP_PORT2";
	case F_GET_DEST_UDP_PORT2:				return "F_GET_DEST_UDP_PORT2";
	case F_SET_NUM_INTERFACES:				return "F_SET_NUM_INTERFACES";
	case F_GET_NUM_INTERFACES:				return "F_GET_NUM_INTERFACES";
	case F_SET_INTERFACE_SEL:				return "F_SET_INTERFACE_SEL";
	case F_GET_INTERFACE_SEL:				return "F_GET_INTERFACE_SEL";	
	case F_SET_PARALLEL_MODE:				return "F_SET_PARALLEL_MODE";	
	case F_GET_PARALLEL_MODE:				return "F_GET_PARALLEL_MODE";	
	case F_SET_OVERFLOW_MODE:				return "F_SET_OVERFLOW_MODE";	
	case F_GET_OVERFLOW_MODE:				return "F_GET_OVERFLOW_MODE";	
	case F_SET_READOUT_MODE:				return "F_SET_READOUT_MODE";	
	case F_GET_READOUT_MODE:				return "F_GET_READOUT_MODE";
	case F_SET_CLOCK_FREQUENCY:				return "F_SET_CLOCK_FREQUENCY";
	case F_GET_CLOCK_FREQUENCY:				return "F_GET_CLOCK_FREQUENCY";
	case F_SET_CLOCK_PHASE:					return "F_SET_CLOCK_PHASE";
	case F_GET_CLOCK_PHASE:					return "F_GET_CLOCK_PHASE";
	case F_GET_MAX_CLOCK_PHASE_SHIFT:		return "F_GET_MAX_CLOCK_PHASE_SHIFT";
	case F_SET_CLOCK_DIVIDER:				return "F_SET_CLOCK_DIVIDER";	
	case F_GET_CLOCK_DIVIDER:				return "F_GET_CLOCK_DIVIDER";
	case F_SET_PIPELINE:					return "F_SET_PIPELINE";
	case F_GET_PIPELINE:					return "F_GET_PIPELINE";
	case F_SET_ON_CHIP_DAC:					return "F_SET_ON_CHIP_DAC";
	case F_GET_ON_CHIP_DAC:					return "F_GET_ON_CHIP_DAC";
	case F_SET_INJECT_CHANNEL:				return "F_SET_INJECT_CHANNEL";
	case F_GET_INJECT_CHANNEL:				return "F_GET_INJECT_CHANNEL";
	case F_SET_VETO_PHOTON:					return "F_SET_VETO_PHOTON";
	case F_GET_VETO_PHOTON:					return "F_GET_VETO_PHOTON";
	case F_SET_VETO_REFERENCE:				return "F_SET_VETO_REFERENCE";	
	case F_GET_BURST_MODE:					return "F_GET_BURST_MODE";
	case F_SET_BURST_MODE:					return "F_SET_BURST_MODE";
	case F_SET_ADC_ENABLE_MASK_10G:         return "F_SET_ADC_ENABLE_MASK_10G";
	case F_GET_ADC_ENABLE_MASK_10G:         return "F_GET_ADC_ENABLE_MASK_10G";
	case F_SET_COUNTER_MASK:         		return "F_SET_COUNTER_MASK";
	case F_GET_COUNTER_MASK:         		return "F_GET_COUNTER_MASK";
	case F_GET_NUM_BURSTS:					return "F_GET_NUM_BURSTS";
	case F_SET_NUM_BURSTS:					return "F_SET_NUM_BURSTS";
	case F_GET_BURST_PERIOD:				return "F_GET_BURST_PERIOD";	
	case F_SET_BURST_PERIOD:				return "F_SET_BURST_PERIOD";
	case F_GET_CURRENT_SOURCE:				return "F_GET_CURRENT_SOURCE";
	case F_SET_CURRENT_SOURCE:				return "F_SET_CURRENT_SOURCE";
	case F_GET_TIMING_SOURCE:				return "F_GET_TIMING_SOURCE";
	case F_SET_TIMING_SOURCE:				return "F_SET_TIMING_SOURCE";
	case F_GET_NUM_CHANNELS:				return "F_GET_NUM_CHANNELS";
	case F_UPDATE_RATE_CORRECTION:			return "F_UPDATE_RATE_CORRECTION";
	case F_GET_RECEIVER_PARAMETERS:			return "F_GET_RECEIVER_PARAMETERS";
    case F_START_PATTERN:                   return "F_START_PATTERN";
    case F_SET_NUM_GATES:                   return "F_SET_NUM_GATES";           
    case F_GET_NUM_GATES:                   return "F_GET_NUM_GATES";           
    case F_SET_GATE_DELAY:                  return "F_SET_GATE_DELAY";       
    case F_GET_GATE_DELAY:                  return "F_GET_GATE_DELAY";     
    case F_GET_EXPTIME_ALL_GATES:           return "F_GET_EXPTIME_ALL_GATES";        
    case F_GET_GATE_DELAY_ALL_GATES:        return "F_GET_GATE_DELAY_ALL_GATES";            
    case F_GET_VETO:                        return "F_GET_VETO";
    case F_SET_VETO:                        return "F_SET_VETO";
    case F_SET_PATTERN:                     return "F_SET_PATTERN";
    case F_GET_SCAN:                        return "F_GET_SCAN"; 
    case F_SET_SCAN:                        return "F_SET_SCAN"; 
    case F_GET_SCAN_ERROR_MESSAGE:          return "F_GET_SCAN_ERROR_MESSAGE"; 
    case F_GET_CDS_GAIN:                    return "F_GET_CDS_GAIN";
    case F_SET_CDS_GAIN:                    return "F_SET_CDS_GAIN";
    case F_GET_FILTER:                      return "F_GET_FILTER";
    case F_SET_FILTER:                      return "F_SET_FILTER";
    case F_SET_VETO_FILE:                   return "F_SET_VETO_FILE";
    case F_SET_ADC_CONFIGURATION:           return "F_SET_ADC_CONFIGURATION";       
    case F_GET_ADC_CONFIGURATION:           return "F_GET_ADC_CONFIGURATION";       
    case F_GET_BAD_CHANNELS:                return "F_GET_BAD_CHANNELS";
    case F_SET_BAD_CHANNELS:                return "F_SET_BAD_CHANNELS";

    case NUM_DET_FUNCTIONS:              	return "NUM_DET_FUNCTIONS";
    case RECEIVER_ENUM_START:				return "RECEIVER_ENUM_START";

	case F_EXEC_RECEIVER_COMMAND:			return "F_EXEC_RECEIVER_COMMAND";
	case F_LOCK_RECEIVER: 					return "F_LOCK_RECEIVER";
	case F_GET_LAST_RECEIVER_CLIENT_IP: 	return "F_GET_LAST_RECEIVER_CLIENT_IP";
	case F_SET_RECEIVER_PORT: 				return "F_SET_RECEIVER_PORT";
	case F_GET_RECEIVER_VERSION: 			return "F_GET_RECEIVER_VERSION";
	case F_RECEIVER_SET_ROI: 				return "F_RECEIVER_SET_ROI";
	case F_RECEIVER_SET_NUM_FRAMES:			return "F_RECEIVER_SET_NUM_FRAMES";
	case F_SET_RECEIVER_NUM_TRIGGERS:		return "F_SET_RECEIVER_NUM_TRIGGERS";
	case F_SET_RECEIVER_NUM_BURSTS:			return "F_SET_RECEIVER_NUM_BURSTS";
	case F_SET_RECEIVER_NUM_ADD_STORAGE_CELLS: return "F_SET_RECEIVER_NUM_ADD_STORAGE_CELLS";
	case F_SET_RECEIVER_TIMING_MODE:		return "F_SET_RECEIVER_TIMING_MODE";
	case F_SET_RECEIVER_BURST_MODE:			return "F_SET_RECEIVER_BURST_MODE";
	case F_RECEIVER_SET_NUM_ANALOG_SAMPLES:	return "F_RECEIVER_SET_NUM_ANALOG_SAMPLES";
	case F_RECEIVER_SET_NUM_DIGITAL_SAMPLES:return "F_RECEIVER_SET_NUM_DIGITAL_SAMPLES";
	case F_RECEIVER_SET_EXPTIME:			return "F_RECEIVER_SET_EXPTIME";
	case F_RECEIVER_SET_PERIOD:				return "F_RECEIVER_SET_PERIOD";
	case F_RECEIVER_SET_SUB_EXPTIME:		return "F_RECEIVER_SET_SUB_EXPTIME";
	case F_RECEIVER_SET_SUB_DEADTIME:		return "F_RECEIVER_SET_SUB_DEADTIME";
	case F_SET_RECEIVER_DYNAMIC_RANGE:  	return "F_SET_RECEIVER_DYNAMIC_RANGE";
	case F_SET_RECEIVER_STREAMING_FREQUENCY: return "F_SET_RECEIVER_STREAMING_FREQUENCY";
	case F_GET_RECEIVER_STREAMING_FREQUENCY: return "F_GET_RECEIVER_STREAMING_FREQUENCY";
	case F_GET_RECEIVER_STATUS:				return "F_GET_RECEIVER_STATUS";
	case F_START_RECEIVER:					return "F_START_RECEIVER";
	case F_STOP_RECEIVER:					return "F_STOP_RECEIVER";
	case F_SET_RECEIVER_FILE_PATH: 			return "F_SET_RECEIVER_FILE_PATH";
	case F_GET_RECEIVER_FILE_PATH: 			return "F_GET_RECEIVER_FILE_PATH";
	case F_SET_RECEIVER_FILE_NAME: 			return "F_SET_RECEIVER_FILE_NAME";
	case F_GET_RECEIVER_FILE_NAME: 			return "F_GET_RECEIVER_FILE_NAME";
	case F_SET_RECEIVER_FILE_INDEX: 		return "F_SET_RECEIVER_FILE_INDEX";
	case F_GET_RECEIVER_FILE_INDEX: 		return "F_GET_RECEIVER_FILE_INDEX";
	case F_GET_RECEIVER_FRAME_INDEX:		return "F_GET_RECEIVER_FRAME_INDEX";
	case F_GET_RECEIVER_FRAMES_CAUGHT:		return "F_GET_RECEIVER_FRAMES_CAUGHT";
	case F_GET_NUM_MISSING_PACKETS:			return "F_GET_NUM_MISSING_PACKETS";
	case F_SET_RECEIVER_FILE_WRITE:			return "F_SET_RECEIVER_FILE_WRITE";
	case F_GET_RECEIVER_FILE_WRITE:			return "F_GET_RECEIVER_FILE_WRITE";
	case F_SET_RECEIVER_MASTER_FILE_WRITE: 	return "F_SET_RECEIVER_MASTER_FILE_WRITE";	
	case F_GET_RECEIVER_MASTER_FILE_WRITE: 	return "F_GET_RECEIVER_MASTER_FILE_WRITE";	
	case F_SET_RECEIVER_OVERWRITE:			return "F_SET_RECEIVER_OVERWRITE";
	case F_GET_RECEIVER_OVERWRITE:			return "F_GET_RECEIVER_OVERWRITE";
	case F_ENABLE_RECEIVER_TEN_GIGA:		return "F_ENABLE_RECEIVER_TEN_GIGA";
	case F_SET_RECEIVER_FIFO_DEPTH:			return "F_SET_RECEIVER_FIFO_DEPTH";
	case F_RECEIVER_ACTIVATE:				return "F_RECEIVER_ACTIVATE";
	case F_SET_RECEIVER_STREAMING:			return "F_SET_RECEIVER_STREAMING";
	case F_GET_RECEIVER_STREAMING:			return "F_GET_RECEIVER_STREAMING";
	case F_RECEIVER_STREAMING_TIMER:		return "F_RECEIVER_STREAMING_TIMER";
	case F_SET_FLIPPED_DATA_RECEIVER:		return "F_SET_FLIPPED_DATA_RECEIVER";
	case F_SET_RECEIVER_FILE_FORMAT:		return "F_SET_RECEIVER_FILE_FORMAT";
	case F_GET_RECEIVER_FILE_FORMAT:		return "F_GET_RECEIVER_FILE_FORMAT";
	case F_SET_RECEIVER_STREAMING_PORT: 	return "F_SET_RECEIVER_STREAMING_PORT";
	case F_GET_RECEIVER_STREAMING_PORT: 	return "F_GET_RECEIVER_STREAMING_PORT";
	case F_SET_RECEIVER_STREAMING_SRC_IP: 	return "F_SET_RECEIVER_STREAMING_SRC_IP";
	case F_GET_RECEIVER_STREAMING_SRC_IP: 	return "F_GET_RECEIVER_STREAMING_SRC_IP";
	case F_SET_RECEIVER_SILENT_MODE:		return "F_SET_RECEIVER_SILENT_MODE";
	case F_GET_RECEIVER_SILENT_MODE:		return "F_GET_RECEIVER_SILENT_MODE";
	case F_RESTREAM_STOP_FROM_RECEIVER:		return "F_RESTREAM_STOP_FROM_RECEIVER";
    case F_SET_ADDITIONAL_JSON_HEADER:      return "F_SET_ADDITIONAL_JSON_HEADER";
    case F_GET_ADDITIONAL_JSON_HEADER:      return "F_GET_ADDITIONAL_JSON_HEADER";
    case F_RECEIVER_UDP_SOCK_BUF_SIZE:  	return "F_RECEIVER_UDP_SOCK_BUF_SIZE";
    case F_RECEIVER_REAL_UDP_SOCK_BUF_SIZE: return "F_RECEIVER_REAL_UDP_SOCK_BUF_SIZE";
    case F_SET_RECEIVER_FRAMES_PER_FILE:	return "F_SET_RECEIVER_FRAMES_PER_FILE";
    case F_GET_RECEIVER_FRAMES_PER_FILE:	return "F_GET_RECEIVER_FRAMES_PER_FILE";
    case F_RECEIVER_CHECK_VERSION:			return "F_RECEIVER_CHECK_VERSION";
    case F_SET_RECEIVER_DISCARD_POLICY:		return "F_SET_RECEIVER_DISCARD_POLICY";
    case F_GET_RECEIVER_DISCARD_POLICY:		return "F_GET_RECEIVER_DISCARD_POLICY";
    case F_SET_RECEIVER_PADDING:			return "F_SET_RECEIVER_PADDING";
    case F_GET_RECEIVER_PADDING:			return "F_GET_RECEIVER_PADDING";
    case F_SET_RECEIVER_DEACTIVATED_PADDING: return "F_SET_RECEIVER_DEACTIVATED_PADDING";
    case F_GET_RECEIVER_DEACTIVATED_PADDING: return "F_GET_RECEIVER_DEACTIVATED_PADDING";
    case F_RECEIVER_SET_READOUT_MODE: 		return "F_RECEIVER_SET_READOUT_MODE";
	case F_RECEIVER_SET_ADC_MASK:			return "F_RECEIVER_SET_ADC_MASK";
	case F_SET_RECEIVER_DBIT_LIST:			return "F_SET_RECEIVER_DBIT_LIST";		
	case F_GET_RECEIVER_DBIT_LIST:			return "F_GET_RECEIVER_DBIT_LIST";		
	case F_SET_RECEIVER_DBIT_OFFSET:		return "F_SET_RECEIVER_DBIT_OFFSET";
	case F_GET_RECEIVER_DBIT_OFFSET:		return "F_GET_RECEIVER_DBIT_OFFSET";
	case F_SET_RECEIVER_QUAD:				return "F_SET_RECEIVER_QUAD";
	case F_SET_RECEIVER_READ_N_LINES:		return "F_SET_RECEIVER_READ_N_LINES";
	case F_SET_RECEIVER_UDP_IP:				return "F_SET_RECEIVER_UDP_IP";
	case F_SET_RECEIVER_UDP_IP2:			return "F_SET_RECEIVER_UDP_IP2";
	case F_SET_RECEIVER_UDP_PORT:			return "F_SET_RECEIVER_UDP_PORT";
	case F_SET_RECEIVER_UDP_PORT2:			return "F_SET_RECEIVER_UDP_PORT2";
	case F_SET_RECEIVER_NUM_INTERFACES:		return "F_SET_RECEIVER_NUM_INTERFACES";
	case F_RECEIVER_SET_ADC_MASK_10G:		return "F_RECEIVER_SET_ADC_MASK_10G";
	case F_RECEIVER_SET_NUM_COUNTERS:		return "F_RECEIVER_SET_NUM_COUNTERS";
	case F_INCREMENT_FILE_INDEX:			return "F_INCREMENT_FILE_INDEX";
	case F_SET_ADDITIONAL_JSON_PARAMETER:	return "F_SET_ADDITIONAL_JSON_PARAMETER";
	case F_GET_ADDITIONAL_JSON_PARAMETER:	return "F_GET_ADDITIONAL_JSON_PARAMETER";
	case F_GET_RECEIVER_PROGRESS:			return "F_GET_RECEIVER_PROGRESS";
	case F_SETUP_RECEIVER:					return "F_SETUP_RECEIVER";
    case F_SET_RECEIVER_NUM_GATES:          return "F_SET_RECEIVER_NUM_GATES";
    case F_SET_RECEIVER_GATE_DELAY:         return "F_SET_RECEIVER_GATE_DELAY";
    case F_GET_RECEIVER_THREAD_IDS:         return "F_GET_RECEIVER_THREAD_IDS";

    case F_GET_RECEIVER_STREAMING_START_FNUM: return "F_GET_RECEIVER_STREAMING_START_FNUM";
    case F_SET_RECEIVER_STREAMING_START_FNUM: return "F_SET_RECEIVER_STREAMING_START_FNUM";

    case NUM_REC_FUNCTIONS: 				return "NUM_REC_FUNCTIONS";
	default:								return "Unknown Function";
	}

    // clang-format on
}
