#ifndef REGISTERS_G_H
#define REGISTERS_G_H


#include "sls_detector_defs.h"


/* Definitions for FPGA*/
#define CSP0 0x20200000
#define MEM_SIZE 0x100000 

/* FPGA Version register */
#define FPGA_VERSION_REG      			(0x00 << 11)

#define BOARD_REVISION_OFST				(0)
#define BOARD_REVISION_MSK				(0x00FFFFFF << BOARD_REVISION_OFST)
#define DETECTOR_TYPE_OFST   			(24)
#define DETECTOR_TYPE_MSK   			(0x000000FF << DETECTOR_TYPE_OFST)



/* Fix pattern register */
#define FIX_PATT_REG          			(0x01 << 11)

#define FIX_PATT_VAL    				0xACDC2014


/* Status register */
#define STATUS_REG            			(0x02 << 11)

#define RUN_BUSY_OFST					(0)
#define RUN_BUSY_MSK      				(0x00000001 << RUN_BUSY_BIT_OFST)
#define WAITING_FOR_TRIGGER_OFST  		(3)
#define WAITING_FOR_TRIGGER_MSK  		(0x00000001 << WAITING_FOR_TRIGGER_OFST)
#define DELAYBEFORE_OFST  				(4)											//Not used in software
#define DELAYBEFORE_MSK  				(0x00000001 << DELAYBEFORE_OFST)			//Not used in software
#define DELAYAFTER_OFST  				(5)											//Not used in software
#define DELAYAFTER_MSK  				(0x00000001 << DELAYAFTER_OFST)				//Not used in software
#define STOPPED_OFST  					(15)
#define STOPPED_MSK  					(0x00000001 << STOPPED_OFST)
#define RUNMACHINE_BUSY_OFST  			(17)
#define RUNMACHINE_BUSY_MSK  			(0x00000001 << RUNMACHINE_BUSY_OFST)


/* Look at me register */
#define LOOK_AT_ME_REG          		(0x03 << 11)								//Not used in firmware or software

/* System Status register */
#define SYSTEM_STATUS_REG       		(0x04 << 11)								//Not used in software

#define DDR3_CAL_DONE_OFST				(0)											//Not used in software
#define DDR3_CAL_DONE_MSK 				(0x00000001 << DDR3_CAL_DONE_OFST)			//Not used in software
#define DDR3_CAL_FAIL_OFST				(1)											//Not used in software
#define DDR3_CAL_FAIL_MSK 				(0x00000001 << DDR3_CAL_FAIL_OFST)			//Not used in software
#define DDR3_INIT_DONE_OFST				(2)											//Not used in software
#define DDR3_INIT_DONE_MSK 				(0x00000001 << DDR3_INIT_DONE_OFST)			//Not used in software
#define RECONFIG_PLL_LCK_OFST			(3)											//Not used in software
#define RECONFIG_PLL_LCK_MSK 			(0x00000001 << RECONFIG_PLL_LCK_OFST)		//Not used in software
#define PLL_A_LCK_OFST					(4)											//Not used in software
#define PLL_A_LCK_MSK 					(0x00000001 << PLL_A_LCK_OFST)				//Not used in software
#define DD3_PLL_LCK_OFST				(5)											//Not used in software
#define DD3_PLL_LCK_MSK 				(0x00000001 << DD3_PLL_LCK_OFST)			//Not used in software


/* Module Control Board Serial Number Register */
#define MOD_SERIAL_NUM_REG				(0x0A << 11)								//Not used in software

#define HARDWARE_SERIAL_NUM_OFST		(0)											//Not used in software
#define HARDWARE_SERIAL_NUM_MSK			(0x000000FF << HARDWARE_SERIAL_NUM_OFST)	//Not used in software
#define HARDWARE_VERSION_NUM_OFST		(16)										//Not used in software
#define HARDWARE_VERSION_NUM_MSK		(0x0000003F << HARDWARE_VERSION_NUM_OFST)	//Not used in software


/* Time from Start 64 bit register */
#define TIME_FROM_START_LSB_REG   		(0x10 << 11)
#define TIME_FROM_START_MSB_REG   		(0x11 << 11)

/* Get Delay 64 bit register */
#define GET_DELAY_LSB_REG     			(0x12 << 11)
#define GET_DELAY_MSB_REG     			(0x13 << 11)

/* Get Cycles 64 bit register */
#define GET_CYCLES_LSB_REG    			(0x14 << 11)
#define GET_CYCLES_MSB_REG    			(0x15 << 11)

/* Get Frames 64 bit register */
#define GET_FRAMES_LSB_REG   			(0x16 << 11)
#define GET_FRAMES_MSB_REG    			(0x17 << 11)

/* Get Period 64 bit register */
#define GET_PERIOD_LSB_REG    			(0x18 << 11)
#define GET_PERIOD_MSB_REG    			(0x19 << 11)

/* Get Period 64 bit register */
#define GET_EXPTIME_LSB_REG    			(0x1A << 11)
#define GET_EXPTIME_MSB_REG    			(0x1B << 11)

/* Get Period 64 bit register */
#define GET_GATES_LSB_REG    			(0x1C << 11)
#define GET_GATES_MSB_REG    			(0x1D << 11)

/* Get Frames from Start 64 bit register (frames from start Data Streaming) ask Carlos used in software firmware_funcs.c getFramesFromStart, but not in firmware*/
#define FRAMES_FROM_START_LSB_REG 		(0x22 << 11)								/*Not used in firmware,used in software*/
#define FRAMES_FROM_START_MSB_REG		(0x23 << 11)								/*Not used in firmware,used in software*/

/* Get Frames from Start 64 bit register (frames from start Run Control) ask Carlos*/
#define FRAMES_FROM_START_PG_LSB_REG	(0x24 << 11)
#define FRAMES_FROM_START_PG_MSB_REG 	(0x25 << 11)

/* Measurement Time 64 bit register (start frame time) tell Carlos it should be measurement started time? */
#define MEASUREMENT_START_TIME_LSB_REG	(0x26 << 11)
#define MEASUREMENT_START_TIME_MSB_REG 	(0x27 << 11)

/* SPI (Serial Peripheral Interface) Register */
#define SPI_REG							(0x40 << 11)

#define DAC_SERIAL_DIGITAL_OUT_OFST		(0)
#define DAC_SERIAL_DIGITAL_OUT_MSK		(0x00000001 << DAC_SERIAL_DIGITAL_OUT_OFST)
#define DAC_SERIAL_CLK_OUT_OFST			(1)
#define DAC_SERIAL_CLK_OUT_MSK			(0x00000001 << DAC_SERIAL_CLK_OUT_OFST)
#define DAC_SERIAL_CS_OUT_OFST			(2)
#define DAC_SERIAL_CS_OUT_MSK			(0x00000001 << DAC_SERIAL_CS_OUT_OFST)
#define HV_SERIAL_DIGITAL_OUT_OFST		(8)
#define HV_SERIAL_DIGITAL_OUT_MSK		(0x00000001 << HV_SERIAL_DIGITAL_OUT_OFST)
#define HV_SERIAL_CLK_OUT_OFST			(9)
#define HV_SERIAL_CLK_OUT_MSK			(0x00000001 << HV_SERIAL_CLK_OUT_OFST)
#define HV_SERIAL_CS_OUT_OFST			(10)
#define HV_SERIAL_CS_OUT_MSK			(0x00000001 << HV_SERIAL_CS_OUT_OFST)




//Constants
#define HALFSPEED_DBIT_PIPELINE 0x7f7c
#define QUARTERSPEED_DBIT_PIPELINE 0x8981
#define HALFSPEED_ADC_PIPELINE 0x20
#define QUARTERSPEED_ADC_PIPELINE 0x10
#define HALFSPEED_CONF 0x0
#define QUARTERSPEED_CONF 0xf
#define HALFSPEED_ADC_PHASE 65
#define QUARTERSPEED_ADC_PHASE 25

/* values defined for FPGA */
#define MCSNUM            0x0



#define FPGA_INIT_PAT     0x60008
#define FPGA_INIT_ADDR    0xb0000000

//#ifdef JUNGFRAU_DHANYA
#define POWER_ON_REG 				0x5e<<11
#define ADCREG1 					0x8  //same as PLL_BANDWIDTH_REG
#define ADCREG2 					0x14
#define ADCREG3 					0x4  //same as PLL_M_COUNTER_REG
#define ADCREG4 					0x5  //same as PLL_C_COUNTER_REG
#define ADCREG_VREFS 				0x18
#define DBIT_PIPELINE_REG 			0x59<<11 //same PATTERN_N_LOOP2_REG
#define MEM_MACHINE_FIFOS_REG 		0x4f<<11 //same as CONTROL_REG
#define CONFGAIN_REG 				0x5d<<11 //same as DAQ_REG
#define ADC_PIPELINE_REG 			0x42<<11 //same as ADC_OFFSET_REG
//#endif

//#define ADC_OFFSET_REG      		66<<11 //same as CONFGAIN_REG
#define ADC_INVERSION_REG   		0x43<<11

//ADC
#define ADC_WRITE_REG         		65<<11//0x18<<11
//#define ADC_SYNC_REG          66<<11//0x19<<11
//#define HV_REG                67<<11//0x20<<11




//#define MUTIME_REG            0x1a<<11
//temperature
#define TEMP_IN_REG           0x1b<<11
#define TEMP_OUT_REG          0x1c<<11
//configure MAC
#define TSE_CONF_REG          0x1d<<11
#define ENET_CONF_REG         0x1e<<11
//#define WRTSE_SHAD_REG        0x1f<<11
//HV





#define CONTROL_REG           79<<11//0x24<<11

#define CONFIG_REG            77<<11//0x26<<11
#define EXT_SIGNAL_REG        78<<11//	  0x4E<<11
#define FPGA_SVN_REG    	  0x29<<11


#define CHIP_OF_INTRST_REG    0x2A<<11

//FIFO



#define FIFO_DATA_REG 6<<11
#define FIFO_STATUS_REG  7<<11


#define FIFO_DATA_REG_OFF     0x50<<11 ///////
//to read back dac registers
#define MOD_DACS1_REG         0x65<<11
#define MOD_DACS2_REG         0x66<<11
#define MOD_DACS3_REG         0x67<<11

//user entered


#define NSAMPLES_REG 95<<11









#define SET_DELAY_LSB_REG     96<<11 //0x68<<11
#define SET_DELAY_MSB_REG     97<<11 //0x69<<11


#define SET_CYCLES_LSB_REG    98<<11//0x6c<<11
#define SET_CYCLES_MSB_REG    99<<11//0x6d<<11


#define SET_FRAMES_LSB_REG    100<<11//0x70<<11
#define SET_FRAMES_MSB_REG    101<<11//0x71<<11


#define SET_PERIOD_LSB_REG    102<<11//0x74<<11
#define SET_PERIOD_MSB_REG    103<<11//0x75<<11


#define SET_EXPTIME_LSB_REG   104<<11//0x78<<11
#define SET_EXPTIME_MSB_REG   105<<11//0x79<<11

#define SET_GATES_LSB_REG     106<<11//0x7c<<11
#define SET_GATES_MSB_REG     107<<11//0x7d<<11


#define DATA_IN_LSB_REG 30<<11
#define DATA_IN_MSB_REG 31<<11

#define PATTERN_OUT_LSB_REG 32<<11
#define PATTERN_OUT_MSB_REG 33<<11


 
   

#define PLL_PARAM_REG  80<<11//0x37<<11
#define PLL_PARAM_OUT_REG 5<<11 //0x38<<11
#define PLL_CNTRL_REG 81<<11//0x34<<11


#ifdef NEW_GBE_INTERFACE
#define GBE_PARAM_OUT_REG 40<<11
#define GBE_PARAM_REG 69<<11
#define GBE_CNTRL_REG 70<<11
#else
#define RX_UDP_AREG    69<<11 //rx_udpip_AReg_c     : integer:= 69; *\/ 
#define UDPPORTS_AREG 70<<11// udpports_AReg_c   : integer:= 70; *\/
#define RX_UDPMACL_AREG 71<<11//rx_udpmacL_AReg_c   : integer:= 71; *\/ 
#define RX_UDPMACH_AREG 72<<11//rx_udpmacH_AReg_c   : integer:= 72; *\/
#define DETECTORMACL_AREG 73<<11//detectormacL_AReg_c : integer:= 73; *\/
#define DETECTORMACH_AREG 74<<11//detectormacH_AReg_c : integer:= 74; *\/ 
#define DETECTORIP_AREG 75<<11//detectorip_AReg_c   : integer:= 75; *\/
#define IPCHKSUM_AREG 76<<11//ipchksum_AReg_c : integer:= 76; *\/ */
#endif


#define PATTERN_CNTRL_REG 82<<11
#define PATTERN_LIMITS_AREG 83<<11

#define PATTERN_LOOP0_AREG 84<<11
#define PATTERN_N_LOOP0_REG 85<<11

#define PATTERN_LOOP1_AREG 86<<11
#define PATTERN_N_LOOP1_REG 87<<11

#define PATTERN_LOOP2_AREG 88<<11
#define PATTERN_N_LOOP2_REG 89<<11

#define PATTERN_WAIT0_AREG 90<<11
#define PATTERN_WAIT1_AREG 91<<11
#define PATTERN_WAIT2_AREG 92<<11



//#define DAQ_REG   0x5d<<11 //same as CONFGAIN_REG
#define ADC_LATCH_DISABLE_REG   94<<11

#define HV_REG 95<<11
   
#define PATTERN_IOCTRL_REG_LSB 108<<11
#define PATTERN_IOCTRL_REG_MSB 109<<11

#define PATTERN_IOCLKCTRL_REG_LSB 110<<11
#define PATTERN_IOCLKCTRL_REG_MSB 111<<11
#define PATTERN_IN_REG_LSB 112<<11
#define PATTERN_IN_REG_MSB 113<<11
#define PATTERN_WAIT0_TIME_REG_LSB 114<<11
#define PATTERN_WAIT0_TIME_REG_MSB 115<<11
#define PATTERN_WAIT1_TIME_REG_LSB 116<<11
#define PATTERN_WAIT1_TIME_REG_MSB 117<<11
#define PATTERN_WAIT2_TIME_REG_LSB 118<<11
#define PATTERN_WAIT2_TIME_REG_MSB 119<<11
   
#define DAC_REG_OFF 120 
#define DAC_0_1_VAL_REG 120<<11 
#define DAC_2_3_VAL_REG 121<<11  
#define DAC_4_5_VAL_REG 122<<11  
#define DAC_6_7_VAL_REG 123<<11  
#define DAC_8_9_VAL_REG 124<<11  
#define DAC_10_11_VAL_REG 125<<11  
#define DAC_12_13_VAL_REG 126<<11  
#define DAC_14_15_VAL_REG 127<<11  
   
 
 






/* registers defined in FPGA */
#define GAIN_REG              0
//#define FLOW_CONTROL_REG      0x11<<11
//#define FLOW_STATUS_REG       0x12<<11
//#define FRAME_REG             0x13<<11
#define MULTI_PURPOSE_REG     0
//#define TIME_FROM_START_REG   0x16<<11


#define ROI_REG 0 // 0x35<<11
#define OVERSAMPLING_REG 0 // 0x36<<11
#define MOENCH_CNTR_REG 0 // 0x31<<11
#define MOENCH_CNTR_OUT_REG 0 // 0x33<<11
#define MOENCH_CNTR_CONF_REG 0 // 0x32<<11



//image
#define DARK_IMAGE_REG     0 // 0x81<<11
#define GAIN_IMAGE_REG     0 // 0x82<<11

//counter block memory
#define COUNTER_MEMORY_REG 0 // 0x85<<11


//not used
//#define MCB_DOUT_REG_OFF      0 // 0x200000
//#define FIFO_CNTRL_REG_OFF    0 // 0x300000
//#define FIFO_COUNTR_REG_OFF   0 // 0x400000
//not used so far
//#define SPEED_REG        0 // 0x006000
//#define SET_NBITS_REG    0 // 0x008000
//not used
//#define GET_SHIFT_IN_REG      0 // 0x022000



#define SHIFTMOD 2
#define SHIFTFIFO 9






/* for control register (16bit only)*/
#define START_ACQ_BIT       0x0001
#define STOP_ACQ_BIT        0x0002
#define START_FIFOTEST_BIT  0x0004 // ?????
#define STOP_FIFOTEST_BIT   0x0008  // ??????
#define START_READOUT_BIT   0x0010  
#define STOP_READOUT_BIT    0x0020 
#define START_EXPOSURE_BIT  0x0040  
#define STOP_EXPOSURE_BIT   0x0080  
#define START_TRAIN_BIT     0x0100  
#define STOP_TRAIN_BIT      0x0200   
#define FIFO_RESET_BIT      0x8000  
#define SYNC_RESET          0x0400  
#define GB10_RESET_BIT      0x0800   
#define MEM_RESET_BIT       0x1000   





/* for fifo status register */
#define FIFO_ENABLED_BIT         0x80000000
#define FIFO_DISABLED_BIT        0x01000000
#define FIFO_ERROR_BIT           0x08000000
#define FIFO_EMPTY_BIT           0x04000000
#define FIFO_DATA_READY_BIT      0x02000000
#define FIFO_COUNTER_MASK        0x000001ff
#define FIFO_NM_MASK             0x00e00000
#define FIFO_NM_OFF              21
#define FIFO_NC_MASK             0x001ffe00
#define FIFO_NC_OFF              9

/* for config register *///not really used yet
#define TOT_ENABLE_BIT           0x00000002
#define TIMED_GATE_BIT           0x00000004
#define CONT_RO_ENABLE_BIT       0x00080000
#define GB10_NOT_CPU_BIT		 0x00001000



/* for speed register */
#define CLK_DIVIDER_MASK              0x000000ff 
#define CLK_DIVIDER_OFFSET            0 
#define SET_LENGTH_MASK               0x00000f00 
#define SET_LENGTH_OFFSET             8 
#define WAIT_STATES_MASK              0x0000f000
#define WAIT_STATES_OFFSET            12  
#define TOTCLK_DIVIDER_MASK           0xff000000 
#define TOTCLK_DIVIDER_OFFSET         24  
#define TOTCLK_DUTYCYCLE_MASK         0x00ff0000 
#define TOTCLK_DUTYCYCLE_OFFSET       16

/* for external signal register */
#define SIGNAL_OFFSET                 4
#define SIGNAL_MASK                   0xF
#define EXT_SIG_OFF                   0x0
#define EXT_GATE_IN_ACTIVEHIGH        0x1
#define EXT_GATE_IN_ACTIVELOW         0x2
#define EXT_TRIG_IN_RISING            0x3
#define EXT_TRIG_IN_FALLING           0x4
#define EXT_RO_TRIG_IN_RISING         0x5
#define EXT_RO_TRIG_IN_FALLING        0x6
#define EXT_GATE_OUT_ACTIVEHIGH        0x7
#define EXT_GATE_OUT_ACTIVELOW         0x8
#define EXT_TRIG_OUT_RISING            0x9
#define EXT_TRIG_OUT_FALLING           0xA
#define EXT_RO_TRIG_OUT_RISING         0xB
#define EXT_RO_TRIG_OUT_FALLING        0xC



/* for temperature register */
#define T1_CLK_BIT             0x00000001
#define T1_CS_BIT              0x00000002
#define T2_CLK_BIT             0x00000004
#define T2_CS_BIT              0x00000008



/* fifo control register */
//#define FIFO_RESET_BIT              0x00000001 
//#define FIFO_DISABLE_TOGGLE_BIT     0x00000002 


//chip shiftin register meaning
#define OUTMUX_OFF 20
#define OUTMUX_MASK   0x1f
#define PROBES_OFF 4
#define PROBES_MASK 0x7f
#define OUTBUF_OFF 0
#define OUTBUF_MASK 1


/* multi purpose register */
#define PHASE_STEP_BIT               0x00000001
#define PHASE_STEP_OFFSET            0
// #define xxx_BIT                   0x00000002
#define RESET_COUNTER_BIT            0x00000004
#define RESET_COUNTER_OFFSET         2
//#define xxx_BIT                    0x00000008
//#define xxx_BIT                    0x00000010
#define SW1_BIT                      0x00000020
#define SW1_OFFSET                   5
#define WRITE_BACK_BIT               0x00000040
#define WRITE_BACK_OFFSET            6
#define RESET_BIT                    0x00000080
#define RESET_OFFSET                 7
#define ENET_RESETN_BIT              0x00000800
#define ENET_RESETN_OFFSET           11
#define INT_RSTN_BIT                 0x00002000
#define INT_RSTN_OFFSET              13
#define DIGITAL_TEST_BIT             0x00004000
#define DIGITAL_TEST_OFFSET          14
//#define CHANGE_AT_POWER_ON_BIT       0x00008000
//#define CHANGE_AT_POWER_ON_OFFSET    15


/* settings/conf gain register */
#define GAIN_MASK                    0x0000ffff
#define GAIN_OFFSET                  0  
#define SPEED_GAIN_MASK     		0xf0000000
#define SPEED_GAIN_OFFSET   		28



/* CHIP_OF_INTRST_REG */
#define CHANNEL_MASK 			    0xffff0000
#define CHANNEL_OFFSET				16
#define ACTIVE_ADC_MASK 		    0x0000001f



/**ADC SYNC CLEAN FIFO*/
#define ADCSYNC_CLEAN_FIFO_BITS     0x300000
#define CLEAN_FIFO_MASK				0x0fffff






#define PLL_CNTR_ADDR_OFF 16 //PLL_CNTR_REG bits 21 downto 16 represent the counter address

#define PLL_CNTR_RECONFIG_RESET_BIT 0
#define PLL_CNTR_READ_BIT 1
#define PLL_CNTR_WRITE_BIT 2
#define PLL_CNTR_PLL_RESET_BIT 3


#define PLL_CNTR_PHASE_EN_BIT 8
#define PLL_CNTR_UPDN_BIT 9
#define PLL_CNTR_CNTSEL_OFF 10
		




#define PLL_MODE_REG 0x0
#define PLL_STATUS_REG 0x1
#define PLL_START_REG 0x2
#define PLL_N_COUNTER_REG 0x3
#define PLL_M_COUNTER_REG 0x4
#define PLL_C_COUNTER_REG 0x5 //which ccounter stands in param 22:18; 7:0 lowcount 15:8 highcount; 16 bypassenable; 17 oddivision
#define PLL_PHASE_SHIFT_REG 0x6 // which ccounter stands in param 16:20; 21 updown (1 up, 0 down)
#define PLL_K_COUNTER_REG 0x7
#define PLL_BANDWIDTH_REG 0x8
#define PLL_CHARGEPUMP_REG 0x9
#define PLL_VCO_DIV_REG 0x1c
#define PLL_MIF_REG 0x1f

#define PPL_M_CNT_PARAM_DEFAULT 0x4040
#define PPL_N_CNT_PARAM_DEFAULT 0x20D0C
#define PPL_C0_CNT_PARAM_DEFAULT 0x20D0C
#define PPL_C1_CNT_PARAM_DEFAULT 0xA0A0
#define PPL_C2_CNT_PARAM_DEFAULT 0x20D0C
#define PPL_C3_CNT_PARAM_DEFAULT 0x0808
#define PPL_BW_PARAM_DEFAULT 0x2EE0
#define PPL_VCO_PARAM_DEFAULT 0x1

#define NEW_PLL_RECONFIG

#ifdef NEW_PLL_RECONFIG
#define PLL_VCO_FREQ_MHZ 400//480//800
#else
#define PLL_VCO_FREQ_MHZ 480//800
#endif





/*
  GBE parameter and control registers definitions
*/

#define GBE_CTRL_WSTROBE 0 
#define GBE_CTRL_VAR_OFFSET 16
#define GBE_CTRL_VAR_MASK 0XF
#define GBE_CTRL_RAMADDR_OFFSET 24
#define GBE_CTRL_RAMADDR_MASK 0X3F
#define GBE_CTRL_INTERFACE 23

#define RX_UDP_IP_ADDR 0
#define RX_UDP_PORTS_ADDR 1
#define RX_UDP_MAC_L_ADDR 2
#define RX_UDP_MAC_H_ADDR 3
#define IPCHECKSUM_ADDR 4
#define GBE_DELAY_ADDR 5
#define GBE_RESERVED1_ADDR 6
#define GBE_RESERVED2_ADDR 7
#define DETECTOR_MAC_L_ADDR 8
#define DETECTOR_MAC_H_ADDR 9
#define DETECTOR_IP_ADDR 10
   


/**------------------
-- pattern registers definitions
--------------------------------------------- */
#define  IOSIGNALS_MASK 0xfffffffffffff
#define ADC_ENABLE_BIT 63
#define  APATTERN_MASK 0xffff
#define ASTART_OFFSET 0
#define ASTOP_OFFSET 16
#define PATTERN_CTRL_WRITE_BIT 0
#define PATTERN_CTRL_READ_BIT 1
#define PATTERN_CTRL_ADDR_OFFSET 16
#define MAX_PATTERN_LENGTH 1024


#endif

