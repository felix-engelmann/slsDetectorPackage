#include "sls_detector_defs.h"
#include "sls_receiver_defs.h"
#include "server_funcs.h"
#include "server_defs.h"
#include "firmware_funcs.h"
#include "mcb_funcs.h"
#include "registers_m.h"
#include "gitInfoJungfrau.h"



extern int lockStatus;
extern char lastClientIP[INET_ADDRSTRLEN];
extern char thisClientIP[INET_ADDRSTRLEN];
extern int differentClients;

//defined in the detector specific make file
#ifdef MYTHEND
const enum detectorType myDetectorType=MYTHEN;
#elif PROPIXD
const enum detectorType myDetectorType=PROPIX;
#elif GOTTHARDD
const enum detectorType myDetectorType=GOTTHARD;
#elif EIGERD
const enum detectorType myDetectorType=EIGER;
#elif PICASSOD
const enum detectorType myDetectorType=PICASSO;
#elif MOENCHD
const enum detectorType myDetectorType=MOENCH;
#elif JUNGFRAUD
const enum detectorType myDetectorType=JUNGFRAU;
#else
const enum detectorType myDetectorType=GENERIC;
#endif



// Global variables
int (*flist[256])(int);
char mess[MAX_STR_LENGTH];

int digitalTestBit = 0; /** Carlos will we use this somewhere */
int adcvpp = 0x4;			/** Carlos will we use this somewhere */





int init_detector(int controlserver) {
	if(controlserver){
		defineGPIOpins();
		resetFPGA();
	}else
		usleep(300*1000);

	if (mapCSP0()==FAIL) {
		printf("Could not map memory\n");
		exit(1);
	}
	if (controlserver)
		initializeDetector();

	//common for both control and stop server
	strcpy(mess,"dummy message");
	strcpy(lastClientIP,"none");
	strcpy(thisClientIP,"none1");
	lockStatus=0;
	return OK;
}


int decode_function(int file_des) {
	int fnum,n;
	int retval=FAIL;
#ifdef VERBOSE
	printf( "receive data\n");
#endif
	n = receiveDataOnly(file_des,&fnum,sizeof(fnum));
	if (n <= 0) {
#ifdef VERBOSE
		printf("ERROR reading from socket %d, %d %d\n", n, fnum, file_des);
#endif
		return FAIL;
	}
#ifdef VERBOSE
	else
		printf("size of data received %d\n",n);
#endif

#ifdef VERBOSE
	printf( "calling function fnum = %d %x %x %x\n",fnum,(unsigned int)(flist[fnum]), (unsigned int)(flist[F_READ_REGISTER]),(unsigned int)(&read_register));
#endif
	if (fnum<0 || fnum>255)
		fnum=255;
	retval=(*flist[fnum])(file_des);
	if (retval==FAIL)
		printf( "Error executing the function = %d \n",fnum);
	return retval;
}


int function_table() {
	int i;
	for (i=0;i<256;i++){
		flist[i]=&M_nofunc;
	}
	flist[F_EXIT_SERVER]=&exit_server;
	flist[F_EXEC_COMMAND]=&exec_command;
	flist[F_GET_DETECTOR_TYPE]=&get_detector_type;
	flist[F_SET_NUMBER_OF_MODULES]=&set_number_of_modules;
	flist[F_GET_MAX_NUMBER_OF_MODULES]=&get_max_number_of_modules;
	flist[F_SET_EXTERNAL_SIGNAL_FLAG]=&set_external_signal_flag;
	flist[F_SET_EXTERNAL_COMMUNICATION_MODE]=&set_external_communication_mode;
	flist[F_GET_ID]=&get_id;
	flist[F_DIGITAL_TEST]=&digital_test;
	flist[F_WRITE_REGISTER]=&write_register;
	flist[F_READ_REGISTER]=&read_register;
	flist[F_SET_DAC]=&set_dac;
	flist[F_GET_ADC]=&get_adc;
	flist[F_SET_CHANNEL]=&set_channel;
	flist[F_SET_CHIP]=&set_chip;
	flist[F_SET_MODULE]=&set_module;
	flist[F_GET_CHANNEL]=&get_channel;
	flist[F_GET_CHIP]=&get_chip;
	flist[F_GET_MODULE]=&get_module;
	flist[F_GET_THRESHOLD_ENERGY]=&get_threshold_energy;
	flist[F_SET_THRESHOLD_ENERGY]=&set_threshold_energy;
	flist[F_SET_SETTINGS]=&set_settings;
	flist[F_START_ACQUISITION]=&start_acquisition;
	flist[F_STOP_ACQUISITION]=&stop_acquisition;
	flist[F_START_READOUT]=&start_readout;
	flist[F_GET_RUN_STATUS]=&get_run_status;
	flist[F_READ_FRAME]=&read_frame;
	flist[F_READ_ALL]=&read_all;
	flist[F_START_AND_READ_ALL]=&start_and_read_all;
	flist[F_SET_TIMER]=&set_timer;
	flist[F_GET_TIME_LEFT]=&get_time_left;
	flist[F_SET_DYNAMIC_RANGE]=&set_dynamic_range;
	flist[F_SET_ROI]=&set_roi;
	flist[F_SET_SPEED]=&set_speed;
	flist[F_SET_READOUT_FLAGS]=&set_readout_flags;
	flist[F_EXECUTE_TRIMMING]=&execute_trimming;
	flist[F_LOCK_SERVER]=&lock_server;
	flist[F_SET_PORT]=&set_port;
	flist[F_GET_LAST_CLIENT_IP]=&get_last_client_ip;
	flist[F_UPDATE_CLIENT]=&update_client;
	flist[F_CONFIGURE_MAC]=&configure_mac;
	flist[F_LOAD_IMAGE]=&load_image;
	flist[F_SET_MASTER]=&set_master;
	flist[F_SET_SYNCHRONIZATION_MODE]=&set_synchronization;
	flist[F_READ_COUNTER_BLOCK]=&read_counter_block;
	flist[F_RESET_COUNTER_BLOCK]=&reset_counter_block;
	flist[F_START_RECEIVER]=&start_receiver;
	flist[F_STOP_RECEIVER]=&stop_receiver;
	flist[F_CALIBRATE_PEDESTAL]=&calibrate_pedestal;
	flist[F_WRITE_ADC_REG]=&write_adc_register;
	flist[F_PROGRAM_FPGA]=&program_fpga;
	flist[F_RESET_FPGA]=&reset_fpga;
	flist[F_POWER_CHIP]=&power_chip;

	return OK;
}


int  M_nofunc(int file_des){

	int ret=FAIL;
	sprintf(mess,"Unrecognized Function\n");
	printf(mess);

	sendDataOnly(file_des,&ret,sizeof(ret));
	sendDataOnly(file_des,mess,sizeof(mess));
	return GOODBYE;
}


int exit_server(int file_des) {
	int retval=FAIL;
	sendDataOnly(file_des,&retval,sizeof(retval));
	printf("closing server.");
	sprintf(mess,"closing server");
	sendDataOnly(file_des,mess,sizeof(mess));
	return GOODBYE;
}

int exec_command(int file_des) {
	char cmd[MAX_STR_LENGTH];
	char answer[MAX_STR_LENGTH];
	int retval=OK;
	int sysret=0;
	int n=0;

	/* receive arguments */
	n = receiveDataOnly(file_des,cmd,MAX_STR_LENGTH);
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		retval=FAIL;
	}

	/* execute action if the arguments correctly arrived*/
	if (retval==OK) {
#ifdef VERBOSE
		printf("executing command %s\n", cmd);
#endif
		if (lockStatus==0 || differentClients==0)
			sysret=system(cmd);

		//should be replaced by popen
		if (sysret==0) {
			sprintf(answer,"Succeeded\n");
			if (lockStatus==1 && differentClients==1)
				sprintf(answer,"Detector locked by %s\n", lastClientIP);
		} else {
			sprintf(answer,"Failed\n");
			retval=FAIL;
		}
	} else {
		sprintf(answer,"Could not receive the command\n");
	}

	/* send answer */
	n = sendDataOnly(file_des,&retval,sizeof(retval));
	n = sendDataOnly(file_des,answer,MAX_STR_LENGTH);
	if (n < 0) {
		sprintf(mess,"Error writing to socket");
		retval=FAIL;
	}


	/*return ok/fail*/
	return retval;

}





int get_detector_type(int file_des) {
	int n=0;
	enum detectorType ret;
	int retval=OK;

	sprintf(mess,"Can't return detector type\n");


	/* receive arguments */
	/* execute action */
	ret=myDetectorType;

#ifdef VERBOSE
	printf("Returning detector type %d\n",ret);
#endif

	/* send answer */
	/* send OK/failed */
	if (differentClients==1)
		retval=FORCE_UPDATE;

	n += sendDataOnly(file_des,&retval,sizeof(retval));
	if (retval!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&ret,sizeof(ret));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}
	/*return ok/fail*/
	return retval;


}


int set_number_of_modules(int file_des) {
	int n;
	int arg[2], ret=0;
	int retval=OK;
	int dim, nm;

	sprintf(mess,"Can't set number of modules\n");

	/* receive arguments */
	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket %d", n);
		retval=GOODBYE;
	}
	if (retval==OK) {
		dim=arg[0];
		nm=arg[1];

		/* execute action */
#ifdef VERBOSE
		printf("Setting the number of modules in dimension %d to %d\n",dim,nm );
#endif

		//if (nm!=GET_FLAG) {
		if (dim!=X && nm!=GET_FLAG) {
			retval=FAIL;
			sprintf(mess,"Can't change module number in dimension %d\n",dim);
		} else  {
			if (lockStatus==1 && differentClients==1 && nm!=GET_FLAG) {
				sprintf(mess,"Detector locked by %s\n", lastClientIP);
				retval=FAIL;
			} else {
				ret=setNMod(nm);
				if (NMODX==nm || nm==GET_FLAG) {
					retval=OK;
					if (differentClients==1)
						retval=FORCE_UPDATE;
				} else
					retval=FAIL;
			}
		}
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&retval,sizeof(retval));
	if (retval!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&ret,sizeof(ret));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}
	/*return ok/fail*/
	return retval;

}


int get_max_number_of_modules(int file_des) {
	int n;
	int ret;
	int retval=OK;
	enum dimension arg;

	sprintf(mess,"Can't get max number of modules\n");
	/* receive arguments */
	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		retval=FAIL;
	}
	/* execute action */
#ifdef VERBOSE
	printf("Getting the max number of modules in dimension %d \n",arg);
#endif


	switch (arg) {
	case X:
		ret=getNModBoard();
		break;
	case Y:
		ret=NMAXMODY;
		break;
	default:
		ret=FAIL;
		retval=FAIL;
		break;
	}
#ifdef VERBOSE
	printf("Max number of module in dimension %d is %d\n",arg,ret );
#endif



	if (differentClients==1 && retval==OK) {
		retval=FORCE_UPDATE;
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&retval,sizeof(retval));
	if (retval!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&ret,sizeof(ret));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}



	/*return ok/fail*/
	return retval;
}


//index 0 is in gate
//index 1 is in trigger
//index 2 is out gate
//index 3 is out trigger

int set_external_signal_flag(int file_des) {
	int n;
	int arg[2];
	int ret=OK;
	int signalindex;
	enum externalSignalFlag flag, retval;

	sprintf(mess,"Can't set external signal flag\n");

	/* receive arguments */
	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	retval=SIGNAL_OFF;
	if (ret==OK) {
		signalindex=arg[0];
		flag=arg[1];
		/* execute action */
		switch (flag) {
		case GET_EXTERNAL_SIGNAL_FLAG:
			retval=getExtSignal(signalindex);
			break;

		default:
			if (differentClients==0 || lockStatus==0) {
				retval=setExtSignal(signalindex,flag);
			} else {
				if (lockStatus!=0) {
					ret=FAIL;
					sprintf(mess,"Detector locked by %s\n", lastClientIP);
				}
			}
		}
#ifdef VERBOSE
		printf("Setting external signal %d to flag %d\n",signalindex,flag );
		printf("Set to flag %d\n",retval);
#endif
	} else
		ret=FAIL;


	if (ret==OK && differentClients!=0)
		ret=FORCE_UPDATE;


	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}


	/*return ok/fail*/
	return ret;

}


int set_external_communication_mode(int file_des) {
	int n;
	enum externalCommunicationMode arg, ret=GET_EXTERNAL_COMMUNICATION_MODE;
	int retval=OK;

	sprintf(mess,"Can't set external communication mode\n");

	/* receive arguments */
	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		retval=FAIL;
	}
	/*
	enum externalCommunicationMode{
	  GET_EXTERNAL_COMMUNICATION_MODE,
	  AUTO,
	  TRIGGER_EXPOSURE_SERIES,
	  TRIGGER_EXPOSURE_BURST,
	  TRIGGER_READOUT,
	  TRIGGER_COINCIDENCE_WITH_INTERNAL_ENABLE,
	  GATE_FIX_NUMBER,
	  GATE_FIX_DURATION,
	  GATE_WITH_START_TRIGGER,
	  GATE_COINCIDENCE_WITH_INTERNAL_ENABLE
	};
	 */
	if (retval==OK) {
		/* execute action */
		ret=setTiming(arg);
#ifdef VERBOSE
		printf("Setting external communication mode to %d\n", arg);
#endif
	} else
		ret=FAIL;

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&retval,sizeof(retval));
	if (retval!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&ret,sizeof(ret));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return retval;
}



int get_id(int file_des) {
	// sends back 64 bits!
	int64_t retval=-1;
	int ret=OK;
	int n=0;
	enum idMode arg;

	sprintf(mess,"Can't return id\n");

	/* receive arguments */
	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

#ifdef VERBOSE
	printf("Getting id %d\n", arg);
#endif  

	switch (arg) {
	case DETECTOR_SERIAL_NUMBER:
	case DETECTOR_FIRMWARE_VERSION:
	case DETECTOR_SOFTWARE_VERSION:
		retval = getId(arg);
		break;
	default:
		printf("Required unknown id %d \n", arg);
		ret=FAIL;
		retval=FAIL;
		break;
	}

#ifdef VERBOSE
	printf("Id is %llx\n", retval);
#endif  

	if (differentClients==1)
		ret=FORCE_UPDATE;

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;

}

int digital_test(int file_des) {

	int retval;
	int ret=OK;
	int imod=-1;
	int n=0;
	int ival;
	enum digitalTestMode arg;

	sprintf(mess,"Can't send digital test\n");

	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

#ifdef VERBOSE
	printf("Digital test mode %d\n",arg );
#endif  

	switch (arg) {
	case  CHIP_TEST:
		n = receiveDataOnly(file_des,&imod,sizeof(imod));
		if (n < 0) {
			sprintf(mess,"Error reading from socket\n");
			retval=FAIL;
		}
		ret = FAIL;
		strcpy(mess, "Not implemented for this detector\n");
		break;
	case MODULE_FIRMWARE_TEST:
		retval=0x2;
		break;
	case DETECTOR_FIRMWARE_TEST:
		retval=testFpga();
		break;
	case DETECTOR_MEMORY_TEST:
		ret=testRAM();
		break;
	case DETECTOR_BUS_TEST:
		retval=testBus();
		break;
	case DETECTOR_SOFTWARE_TEST:
		retval=testFpga();
		break;
	case DIGITAL_BIT_TEST:
		n = receiveDataOnly(file_des,&ival,sizeof(ival));
		if (n < 0) {
			sprintf(mess,"Error reading from socket\n");
			retval=FAIL;
		}
#ifdef VERBOSE
		printf("with value %d\n", ival);
#endif
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
			break;
		}
		digitalTestBit = ival;
		retval=digitalTestBit;
		break;
	default:
		printf("Unknown digital test required %d\n",arg);
		ret=FAIL;
		retval=FAIL;
		break;
	}

#ifdef VERBOSE
	printf("digital test result is 0x%x\n", retval);
#endif  
	//Always returns force update such that the dynamic range is always updated on the client

	// if (differentClients==1 && ret==OK)
	ret=FORCE_UPDATE;

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;

}

int write_register(int file_des) {

	int retval;
	int ret=OK;
	int arg[2];
	int addr, val;
	int n;
	u_int32_t address;

	sprintf(mess,"Can't write to register\n");

	n = receiveDataOnly(file_des,arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	addr=arg[0];
	val=arg[1];

#ifdef VERBOSE
	printf("writing to register 0x%x data 0x%x\n", addr, val);
#endif  

	if (differentClients==1 && lockStatus==1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	}


	if(ret!=FAIL){
		address=(addr<<11);
		/*if((address==FIFO_DATA_REG_OFF)||(address==CONTROL_REG)) ask Carlos
			ret = bus_w16(address,val);
		else*/
			ret=bus_w(address,val);
		if(ret==OK){
			/*if((address==FIFO_DATA_REG_OFF)||(address==CONTROL_REG)) ask Carlos
				retval=bus_r16(address);
			else*/
				retval=bus_r(address);
		}
	}


#ifdef VERBOSE
	printf("Data set to 0x%x\n",  retval);
#endif  
	if (retval==val) {
		ret=OK;
		if (differentClients)
			ret=FORCE_UPDATE;
	} else {
		ret=FAIL;
		sprintf(mess,"Writing to register 0x%x failed: wrote 0x%x but read 0x%x\n", addr, val, retval);
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;

}

int read_register(int file_des) {

	int retval;
	int ret=OK;
	int arg;
	int addr;
	int n;
	u_int32_t address;

	sprintf(mess,"Can't read register\n");

	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	addr=arg;



	//#ifdef VERBOSE
	printf("reading  register 0x%x\n", addr);
	//#endif

	if(ret!=FAIL){
		address=(addr<<11);
		/*if((address==FIFO_DATA_REG_OFF)||(address==CONTROL_REG)) ask Carlos
			retval=bus_r16(address);
		else*/
			retval=bus_r(address);
	}



#ifdef VERBOSE
	printf("Returned value 0x%x\n",  retval);
#endif  
	if (ret==FAIL) {
		ret=FAIL;
		printf("Reading register 0x%x failed\n", addr);
	} else if (differentClients)
		ret=FORCE_UPDATE;


	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;

}

int set_dac(int file_des) {
	//default:all mods
	int retval, retval1;
	int ret=OK;
	int arg[3];
	enum dacIndex ind;
	int imod;
	int n;
	int val;
	int mV=0;
	sprintf(mess,"Can't set DAC\n");

	n = receiveDataOnly(file_des,arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	ind=arg[0];
	imod=arg[1];
	mV=arg[3];
	n = receiveDataOnly(file_des,&val,sizeof(val));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	//#ifdef VERBOSE
	printf("Setting DAC %d of module %d to %d , mode %d\n", ind, imod, val, mV);
	//#endif 

	if (imod>=getNModBoard())
		ret=FAIL;
	if (imod<0)
		imod=ALLMOD;




#ifdef MCB_FUNCS

	if (ret==OK) {
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		} else{		  
			if (ind<16) {

				if (mV) {
					if (val>2500)
						val=-1;
					printf("%d mV is ",val);
					if (val>0)
						val=4095*val/2500;
					printf("%d DACu\n", val);
				} else if (val>4095)
					val=-1;


				retval=setDac(ind,val);
			}
			else if (ind==ADC_VPP) {
				printf("Setting ADC VPP to %d\n",val);
				if (val>4 || val<0)
					printf("Cannot set ADC VPP to %d\n",val);
				else {
					setAdc(0x18,val);
					adcvpp=val;
				}
				retval=adcvpp;;

			} else if (ind==HV_NEW )
				retval=setHighVoltage(val,0);
			else
				printf("**********No dac with index %d\n",ind);
		}
	}
	if(ret==OK){
		if (ind<16) {
			if (mV) {

				printf("%d DACu is ",retval);
				retval1=2500*retval/16535;
				printf("%d mV \n",retval1);
			} else
				retval1=retval;
		} else
			retval1=retval;
	}
#endif

#ifdef VERBOSE
	printf("DAC set to %d V\n",  retval);
#endif  

	if(ret==FAIL)
		printf("Setting dac %d of module %d: wrote %d but read %d\n", ind, imod, val, retval);
	else{
		if (differentClients)
			ret=FORCE_UPDATE;
	}


	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
		n += sendDataOnly(file_des,&retval1,sizeof(retval1));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}


	/*return ok/fail*/
	return ret;

}



int get_adc(int file_des) {
	//default: mod 0
	int retval;
	int ret=OK;
	int arg[2];
	enum dacIndex ind;
	int imod;
	int n;
	int idac=0;

	sprintf(mess,"Can't read ADC\n");


	n = receiveDataOnly(file_des,arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	ind=arg[0];
	imod=arg[1];

#ifdef VERBOSE
	printf("Getting ADC %d of module %d\n", ind, imod);
#endif

	if (imod>=getNModBoard() || imod<0)
		ret=FAIL;

#ifdef MCB_FUNCS
	switch (ind) {
	case TEMPERATURE_FPGA:
		idac=TEMP_FPGA;
		break;
	case TEMPERATURE_ADC:
		idac=TEMP_ADC;
		break;
	default:
		printf("Unknown DAC index %d\n",ind);
		sprintf(mess,"Unknown DAC index %d\n",ind);
		ret=FAIL;
		break;
	}

	if (ret==OK)
		retval=getTemperature(idac,imod);
#endif

#ifdef VERBOSE
	printf("ADC is %d V\n",  retval);
#endif  
	if (ret==FAIL) {
		printf("Getting adc %d of module %d failed\n", ind, imod);
	}

	if (differentClients)
		ret=FORCE_UPDATE;

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;

}

int set_channel(int file_des) {
	int ret=OK;
	sls_detector_channel myChan;
	int retval;
	int n;


	sprintf(mess,"Can't set channel\n");

#ifdef VERBOSE
	printf("Setting channel\n");
#endif
	ret=receiveChannel(file_des, &myChan);
	if (ret>=0)
		ret=OK;
	else
		ret=FAIL;
#ifdef VERBOSE
	printf("channel number is %d, chip number is %d, module number is %d, register is %lld\n", myChan.chan,myChan.chip, myChan.module, myChan.reg);
#endif

	if (ret==OK) {
		if (myChan.module>=getNModBoard())
			ret=FAIL;
		if (myChan.chip>=NCHIP)
			ret=FAIL;
		if (myChan.chan>=NCHAN)
			ret=FAIL;
		if (myChan.module<0)
			myChan.module=ALLMOD;
	}


	if (ret==OK) {
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		} else {
#ifdef MCB_FUNCS
			retval=initChannelbyNumber(myChan);
#endif
		}
	}
	/* Maybe this is done inside the initialization funcs */
	//copyChannel(detectorChans[myChan.module][myChan.chip]+(myChan.chan), &myChan);



	if (differentClients==1 && ret==OK)
		ret=FORCE_UPDATE;

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}


	/*return ok/fail*/
	return ret;

}




int get_channel(int file_des) {

	int ret=OK;
	sls_detector_channel retval;

	int arg[3];
	int ichan, ichip, imod;
	int n;

	sprintf(mess,"Can't get channel\n");



	n = receiveDataOnly(file_des,arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	ichan=arg[0];
	ichip=arg[1];
	imod=arg[2];

	if (ret==OK) {
		ret=FAIL;
		if (imod>=0 && imod<getNModBoard()) {
			ret=OK;
		}
	}
	if (ret==OK) {
		ret=FAIL;
		if (ichip>=0 && ichip<NCHIP) {
			ret=OK;
		}
	}
	if (ret==OK) {
		ret=FAIL;
		if (ichan>=0 && ichan<NCHAN) {
			ret=OK;
		}
	}


	if (ret==OK) {
#ifdef MCB_FUNCS
		ret=getChannelbyNumber(&retval);
#endif
		if (differentClients && ret==OK)
			ret=FORCE_UPDATE;
	}

#ifdef VERBOSE
	printf("Returning channel %d %d %d, 0x%llx\n", ichan, ichip, imod, (retval.reg));
#endif 

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		ret=sendChannel(file_des, &retval);
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}



	/*return ok/fail*/
	return ret;


}


int set_chip(int file_des) {

	sls_detector_chip myChip;
	int ch[NCHAN];
	int n, retval;
	int ret=OK;
	myChip.nchan=NCHAN;
	myChip.chanregs=ch;

#ifdef VERBOSE
	printf("Setting chip\n");
#endif
	ret=receiveChip(file_des, &myChip);
#ifdef VERBOSE
	printf("Chip received\n");
#endif
	if (ret>=0)
		ret=OK;
	else
		ret=FAIL;
#ifdef VERBOSE
	printf("chip number is %d, module number is %d, register is %d, nchan %d\n",myChip.chip, myChip.module, myChip.reg, myChip.nchan);
#endif

	if (ret==OK) {
		if (myChip.module>=getNModBoard())
			ret=FAIL;
		if  (myChip.module<0)
			myChip.module=ALLMOD;
		if (myChip.chip>=NCHIP)
			ret=FAIL;
	}
	if (differentClients==1 && lockStatus==1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	} else {
#ifdef MCB_FUNCS
		retval=initChipbyNumber(myChip);
#endif
	}
	/* Maybe this is done inside the initialization funcs */
	//copyChip(detectorChips[myChip.module]+(myChip.chip), &myChip);

	if (differentClients && ret==OK)
		ret=FORCE_UPDATE;
	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	return ret;
}


int get_chip(int file_des) {


	int ret=OK;
	sls_detector_chip retval;
	int arg[2];
	int  ichip, imod;
	int n;



	n = receiveDataOnly(file_des,arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	ichip=arg[0];
	imod=arg[1];
	if (ret==OK) {
		ret=FAIL;
		if (imod>=0 && imod<getNModBoard()) {
			ret=OK;
		}
	}
	if (ret==OK) {
		ret=FAIL;
		if (ichip>=0 && ichip<NCHIP) {
			ret=OK;
		}
	}

	if (ret==OK) {
#ifdef MCB_FUNCS
		ret=getChipbyNumber(&retval);
#endif
		if (differentClients && ret==OK)
			ret=FORCE_UPDATE;
	}

#ifdef VERBOSE
	printf("Returning chip %d %d\n",  ichip, imod);
#endif 


	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		ret=sendChip(file_des, &retval);
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}



	/*return ok/fail*/
	return ret;
}


int set_module(int file_des) {

	int retval, n;
	int ret=OK;
	int dr;
	sls_detector_module myModule;
	int *myDac=malloc(NDAC*sizeof(int));
	int *myAdc=malloc(NADC*sizeof(int));
	int *myChip=NULL;
	int *myChan=NULL;
	dr=setDynamicRange(-1);

	//initialize myModule values
	if (myDac)
		myModule.dacs=myDac;
	else {
		sprintf(mess,"could not allocate dacs\n");
		ret=FAIL;
	}
	if (myAdc)
		myModule.adcs=myAdc;
	else {
		sprintf(mess,"could not allocate adcs\n");
		ret=FAIL;
	}

	myModule.chipregs=NULL;
	myModule.chanregs=NULL;
	myModule.ndac=NDAC;
	myModule.nchip=NCHIP;
	myModule.nchan=NCHAN*NCHIP;
	myModule.nadc=NADC;

	//#ifdef VERBOSE
	printf("Setting module\n");
	//#endif

	ret=receiveModuleGeneral(file_des, &myModule, 0); //0 is to receive partially (without trimbits etc.)

	if (ret>=0)
		ret=OK;
	else
		ret=FAIL;


	//#ifdef VERBOSE
	printf("module number is %d,register is %d, nchan %d, nchip %d, ndac %d, nadc %d, gain %f, offset %f\n",myModule.module, myModule.reg, myModule.nchan, myModule.nchip, myModule.ndac,  myModule.nadc, myModule.gain,myModule.offset);
	//#endif

	if (ret==OK) {
		if (myModule.module>=getNModBoard()) {
			ret=FAIL;
			printf("Module number is too large %d\n",myModule.module);
		}
		if (myModule.module<0)
			myModule.module=ALLMOD;
	}

	if (ret==OK) {
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		} else {
#ifdef MCB_FUNCS
			retval=initModulebyNumber(myModule);
			if(retval != myModule.reg)
				ret = FAIL;
#endif
		}
	}

	if (differentClients==1 && ret==OK)
		ret=FORCE_UPDATE;

	/* Maybe this is done inside the initialization funcs */
	//copyChip(detectorChips[myChip.module]+(myChip.chip), &myChip);

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	free(myDac);
	if(myAdc != NULL) 	free(myAdc);
	if(myChip != NULL) 	free(myChip);
	if(myChan != NULL) 	free(myChan);


	//setDynamicRange(dr);  always 16 commented out

	return ret;
}




int get_module(int file_des) {

	int ret=OK;
	int arg;
	int   imod;
	int n;
	sls_detector_module myModule;
	int *myDac=malloc(NDAC*sizeof(int));
	int *myChip=NULL;
	int *myChan=NULL;
	int *myAdc=NULL;
	//not allocating mychip,mychan for jungfrau to sace memory

	if (myDac)
		myModule.dacs=myDac;
	else {
		sprintf(mess,"could not allocate dacs\n");
		ret=FAIL;
	}


	myModule.adcs=NULL;
	myModule.chipregs=NULL;
	myModule.chanregs=NULL;
	myModule.ndac=NDAC;
	myModule.nchip=NCHIP;
	myModule.nchan=NCHAN*NCHIP;
	myModule.nadc=NADC;



	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	imod=arg;

	if (ret==OK) {
		ret=FAIL;
		if (imod>=0 && imod<getNModBoard()) {
			ret=OK;
			myModule.module=imod;
#ifdef MCB_FUNCS
			getModulebyNumber(&myModule);
#endif

#ifdef VERBOSE
			printf("Returning module %d of register %x\n",  imod, myModule.reg);
#endif 
		}
	}

	if (differentClients==1 && ret==OK)
		ret=FORCE_UPDATE;

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		ret=sendModuleGeneral(file_des, &myModule,0);  //0 is to send partially (without trimbits etc.)
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	free(myDac);
	if(myChip != NULL) 	free(myChip);
	if(myChan != NULL) 	free(myChan);
	if(myAdc != NULL) 	free(myAdc);


	/*return ok/fail*/
	return ret;

}


int get_threshold_energy(int file_des) { 
	int ret=FAIL;
	int n;
	int  imod;

	strcpy(mess,"cannot set threshold for this detector");

	n = receiveDataOnly(file_des,&imod,sizeof(imod));
	if (n < 0)
		sprintf(mess,"Error reading from socket\n");

	/* send answer */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	n += sendDataOnly(file_des,mess,sizeof(mess));

	/*return ok/fail*/
	return OK;

}

int set_threshold_energy(int file_des) { 
	int ret=FAIL;
	int arg[3];
	int n;

	strcpy(mess,"cannot set threshold for this detector");

	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0)
		sprintf(mess,"Error reading from socket\n");


	/* send answer */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	n += sendDataOnly(file_des,mess,sizeof(mess));

	/*return ok/fail*/
	return OK;

}



int set_settings(int file_des) {

	int retval;
	int ret=OK;
	int arg[2];
	int n;
	int  imod;
	enum detectorSettings isett;


	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	imod=arg[1];
	isett=arg[0];


#ifdef VERBOSE
	printf("Changing settings of module %d to %d\n", imod,  isett);
#endif 

	if (differentClients==1 && lockStatus==1 && arg[0]!=GET_SETTINGS) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	} else {
#ifdef MCB_FUNCS
		retval=setSettings(arg[0],imod);
#endif
#ifdef VERBOSE
		printf("Settings changed to %d\n",retval);
#endif  

		if (retval==isett || isett<0) {
			ret=OK;
		} else {
			ret=FAIL;
			printf("Changing settings of module %d: wrote %d but read %d\n", imod, isett, retval);
		}

	}
	if (ret==OK && differentClients==1)
		ret=FORCE_UPDATE;

	/* send answer */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	} else
		n += sendDataOnly(file_des,&retval,sizeof(retval));



	return ret;


}

int start_acquisition(int file_des) {

	int ret=OK;
	int n;


	sprintf(mess,"can't start acquisition\n");

#ifdef VERBOSE
	printf("Starting acquisition\n");
#endif

	if (differentClients==1 && lockStatus==1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	} else if (setPeriod(-1)==0){
		ret=FAIL;
		sprintf(mess,"Frame Period is 0. Set it to start acquisition\n");
		cprintf(RED,"%s",mess);
	}
	else {
		ret=startStateMachine();
	}
	if (ret==FAIL)
		sprintf(mess,"Start acquisition failed\n");
	else if (differentClients)
		ret=FORCE_UPDATE;

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}
	return ret;

}

int stop_acquisition(int file_des) {

	int ret=OK;
	int n;


	sprintf(mess,"can't stop acquisition\n");

#ifdef VERBOSE
	printf("Stopping acquisition\n");
#endif 


	if (differentClients==1 && lockStatus==1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	} else {
		ret=stopStateMachine();
	}

	if (ret==FAIL)
		sprintf(mess,"Stop acquisition failed\n");
	else if (differentClients)
		ret=FORCE_UPDATE;

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}
	return ret;


}

int start_readout(int file_des) {


	int ret=OK;
	int n;


	sprintf(mess,"can't start readout\n");

#ifdef VERBOSE
	printf("Starting readout\n");
#endif     
	if (differentClients==1 && lockStatus==1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	} else {
		ret=startReadOut();
	}
	if (ret==FAIL)
		sprintf(mess,"Start readout failed\n");
	else if (differentClients)
		ret=FORCE_UPDATE;

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}
	return ret;



}

int get_run_status(int file_des) {  

	int ret=OK;
	int n;

	int retval;
	enum runStatus s;
	sprintf(mess,"getting run status\n");

	s = getStatus();
	if (s == ERROR)
		ret = FAIL;

	if (ret!=OK) {
		printf("get status failed %04x\n",retval);
		sprintf(mess, "get status failed %08x\n", retval);

	} else if (differentClients)
		ret=FORCE_UPDATE;

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	} else {
		n += sendDataOnly(file_des,&s,sizeof(s));
	}
	return ret;



}

int read_frame(int file_des) {
	int dataret=FAIL;
	if (differentClients==1 && lockStatus==1) {
		dataret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
		cprintf(RED,"%s",mess);
		sendDataOnly(file_des,&dataret,sizeof(dataret));
		sendDataOnly(file_des,mess,sizeof(mess));
#ifdef VERBOSE
		printf("dataret %d\n",dataret);
#endif
		return dataret;
	}

#ifdef MCB_FUNCS
	waitForAcquisitionEnd();
	if (getFrames()+1>0) {
		dataret=FAIL;
		sprintf(mess,"no data and run stopped: %d frames left\n",(int)(getFrames()+1));
		cprintf(RED,"%s\n",mess);
	} else {
		dataret=FINISHED;
		sprintf(mess,"acquisition successfully finished\n");
		printf("%s",mess);
		if (differentClients)
			dataret=FORCE_UPDATE;
	}
#endif
	sendDataOnly(file_des,&dataret,sizeof(dataret));
	sendDataOnly(file_des,mess,sizeof(mess));
	return dataret;
}




int read_all(int file_des) {
	while(read_frame(file_des)==OK) {
#ifdef VERBOSE
		printf("frame read\n");
#endif   
		;
	}
#ifdef VERBOSE
	printf("Frames finished\n");
#endif   
	return OK;
}



int start_and_read_all(int file_des) {
#ifdef VERBOSE
	printf("Starting and reading all frames\n");
#endif 
	if (differentClients==1 && lockStatus==1) {
		int dataret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
		sendDataOnly(file_des,&dataret,sizeof(dataret));
		sendDataOnly(file_des,mess,sizeof(mess));
		return dataret;

	}
	startStateMachine();
	read_all(file_des);
#ifdef VERBOSE
	printf("Frames finished\n");
#endif   
	return OK;
}

int set_timer(int file_des) {
	enum timerIndex ind;
	int64_t tns;
	int n;
	int64_t retval;
	int ret=OK;

	sprintf(mess,"can't set timer\n");

	n = receiveDataOnly(file_des,&ind,sizeof(ind));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	n = receiveDataOnly(file_des,&tns,sizeof(tns));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	if (ret!=OK) {
		printf(mess);
	}


#ifdef VERBOSE
	printf("setting timer %d to %lld ns\n",ind,tns);
#endif 

	if (ret==OK) {

		if (differentClients==1 && lockStatus==1 && tns!=-1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		}  else {
			switch(ind) {
			case FRAME_NUMBER:
				retval=setFrames(tns);
				break;
			case ACQUISITION_TIME:
				retval=setExposureTime(tns);
				break;
			case FRAME_PERIOD:
				retval=setPeriod(tns);
				break;
			case DELAY_AFTER_TRIGGER:
				retval=setDelay(tns);
				break;
			case GATES_NUMBER:
				retval=setGates(tns);
				break;
			case PROBES_NUMBER:
				sprintf(mess,"can't set timer for this detector\n");
				ret=FAIL;
				break;
			case CYCLES_NUMBER:
				retval=setTrains(tns);
				break;
			default:
				ret=FAIL;
				sprintf(mess,"timer index unknown %d\n",ind);
				break;
			}
		}
	}

	if(ret == OK && (retval!=tns) && (tns != -1)){
		ret = FAIL;
		sprintf(mess,"Setting timer %d of failed: wrote %lld but read %lld\n", ind, (long long int)tns, (long long int)retval);
		cprintf(RED,"%s",mess);
	}else if (ret!=OK) {
		cprintf(RED,"%s",mess);
		cprintf(RED,"set timer failed\n");
	}

	if (ret==OK && differentClients)
		ret=FORCE_UPDATE;


	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n = sendDataOnly(file_des,mess,sizeof(mess));
	} else {
#ifdef VERBOSE
		printf("returning ok %d\n",(int)(sizeof(retval)));
#endif 

		n = sendDataOnly(file_des,&retval,sizeof(retval));
	}

	return ret;

}








int get_time_left(int file_des) {

	enum timerIndex ind;
	int n;
	int64_t retval;
	int ret=OK;

	sprintf(mess,"can't get timer\n");
	n = receiveDataOnly(file_des,&ind,sizeof(ind));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}


	//#ifdef VERBOSE

	printf("getting time left on timer %d \n",ind);
	//#endif

	if (ret==OK) {
		switch(ind) {
		case FRAME_NUMBER:
			printf("getting frames \n");
			retval=getFrames();
			break;
		case ACQUISITION_TIME:
			retval=getExposureTime();
			break;
		case FRAME_PERIOD:
			retval=getPeriod();
			break;
		case DELAY_AFTER_TRIGGER:
			retval=getDelay();
			break;
		case GATES_NUMBER:
			retval=getGates();
			break;
		case PROBES_NUMBER:
			retval=getProbes();
			break;
		case CYCLES_NUMBER:
			retval=getTrains();
			break;
		case PROGRESS:
			retval=getProgress();
			break;
		case ACTUAL_TIME:
			retval=getActualTime();
			break;
		case MEASUREMENT_TIME:
			retval=getMeasurementTime();
			break;
		case FRAMES_FROM_START:
		case FRAMES_FROM_START_PG:
			retval=getFramesFromStart();
			break;
		default:
			ret=FAIL;
			sprintf(mess,"timer index unknown %d\n",ind);
			break;
		}
	}


	if (ret!=OK) {
		printf("get time left failed\n");
	} else if (differentClients)
		ret=FORCE_UPDATE;

	//#ifdef VERBOSE

	printf("time left on timer %d is %lld\n",ind, retval);
	//#endif

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	} else
		n = sendDataOnly(file_des,&retval,sizeof(retval));

#ifdef VERBOSE

	printf("data sent\n");
#endif 

	return ret;


}

int set_dynamic_range(int file_des) {



	int dr;
	int n;
	int retval;
	int ret=OK;

	sprintf(mess,"can't set dynamic range\n");


	n = receiveDataOnly(file_des,&dr,sizeof(dr));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}


	if (differentClients==1 && lockStatus==1 && dr>=0) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	}  else {
		retval=setDynamicRange(dr);
	}

	//if (dr>=0 && retval!=dr)   ret=FAIL;
	if (ret!=OK) {
		sprintf(mess,"set dynamic range failed\n");
	} else {
		if (differentClients)
			ret=FORCE_UPDATE;
	}

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n = sendDataOnly(file_des,mess,sizeof(mess));
	} else {
		n = sendDataOnly(file_des,&retval,sizeof(retval));
	}
	return ret;
}

int set_roi(int file_des) {

	int ret=OK;
	int nroi=-1;
	int n=0;
	strcpy(mess,"Could not set/get roi\n");

	n = receiveDataOnly(file_des,&nroi,sizeof(nroi));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	ret = FAIL;
	strcpy(mess,"Not applicable/implemented for this detector\n");
	printf("Error:Set ROI-%s",mess);


	if(ret==OK && differentClients){
		printf("Force update\n");
		ret=FORCE_UPDATE;
	}

	/* send answer */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	n = sendDataOnly(file_des,mess,sizeof(mess));
	/*return ok/fail*/
	return ret;
}

int get_roi(int file_des) {
	return FAIL;
}

int set_speed(int file_des) {

	enum speedVariable arg;
	int val,n;
	int ret=OK;
	int retval=-1;

	n=receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	n=receiveDataOnly(file_des,&val,sizeof(val));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	if (ret==OK) {


		if (val!=-1) {
			if (differentClients==1 && lockStatus==1 && val>=0) {
				ret=FAIL;
				sprintf(mess,"Detector locked by %s\n",lastClientIP);
			}  else {
				switch (arg) {
				case CLOCK_DIVIDER:
					switch(val){
					case 0:
						ret=FAIL;
						sprintf(mess,"Full speed not implemented yet. Available options: 1 for half speed and 2 for quarter speed");
						break;
					case 1:
					case 2:
						break;
					default:
						ret=FAIL;
						sprintf(mess,"Unknown clock options %d. Available options: 1 for half speed and 2 for quarter speed",arg);
						break;
					}
					setClockDivider(val);
					break;

					case ADC_PHASE:
						adcPhase(val);
						break;

					default:
						ret=FAIL;
						sprintf(mess,"Unknown speed parameter %d",arg);
				}
			}
		}
	}



	if (ret==OK) {
		switch (arg) {
		case CLOCK_DIVIDER:
			retval=getClockDivider();
			break;

		case ADC_PHASE:
			retval=getPhase();
			break;

		default:
			ret=FAIL;
			sprintf(mess,"Unknown speed parameter %d",arg);
		}
	}


	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n = sendDataOnly(file_des,mess,sizeof(mess));
	} else {
		n = sendDataOnly(file_des,&retval,sizeof(retval));
	}
	return ret;
}



int set_readout_flags(int file_des) {

	enum readOutFlags arg;
	int ret=FAIL;


	receiveDataOnly(file_des,&arg,sizeof(arg));

	sprintf(mess,"can't set readout flags for this detector\n");

	sendDataOnly(file_des,&ret,sizeof(ret));
	sendDataOnly(file_des,mess,sizeof(mess));

	return ret;
}





int execute_trimming(int file_des) {

	int arg[3];
	int ret=FAIL;
	enum trimMode mode;

	sprintf(mess,"can't set execute trimming for this detector\n");

	receiveDataOnly(file_des,&mode,sizeof(mode));
	receiveDataOnly(file_des,arg,sizeof(arg));


	sendDataOnly(file_des,&ret,sizeof(ret));
	sendDataOnly(file_des,mess,sizeof(mess));

	return ret;
}


int lock_server(int file_des) {


	int n;
	int ret=OK;

	int lock;
	n = receiveDataOnly(file_des,&lock,sizeof(lock));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		printf("Error reading from socket (lock)\n");
		ret=FAIL;
	}
	if (lock>=0) {
		if (lockStatus==0 || strcmp(lastClientIP,thisClientIP)==0 || strcmp(lastClientIP,"none")==0)
			lockStatus=lock;
		else {
			ret=FAIL;
			sprintf(mess,"Server already locked by %s\n", lastClientIP);
		}
	}
	if (differentClients && ret==OK)
		ret=FORCE_UPDATE;

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n = sendDataOnly(file_des,mess,sizeof(mess));
	}  else
		n = sendDataOnly(file_des,&lockStatus,sizeof(lockStatus));

	return ret;

}

int set_port(int file_des) {
	int n;
	int ret=OK;
	int sd=-1;

	enum portType p_type; /** data? control? stop? Unused! */
	int p_number; /** new port number */

	n = receiveDataOnly(file_des,&p_type,sizeof(p_type));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		printf("Error reading from socket (ptype)\n");
		ret=FAIL;
	}

	n = receiveDataOnly(file_des,&p_number,sizeof(p_number));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		printf("Error reading from socket (pnum)\n");
		ret=FAIL;
	}
	if (differentClients==1 && lockStatus==1 ) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	}  else {
		if (p_number<1024) {
			sprintf(mess,"Too low port number %d\n", p_number);
			printf("\n");
			ret=FAIL;
		}

		printf("set port %d to %d\n",p_type, p_number);

		sd=bindSocket(p_number);
	}
	if (sd>=0) {
		ret=OK;
		if (differentClients )
			ret=FORCE_UPDATE;
	} else {
		ret=FAIL;
		sprintf(mess,"Could not bind port %d\n", p_number);
		printf("Could not bind port %d\n", p_number);
		if (sd==-10) {
			sprintf(mess,"Port %d already set\n", p_number);
			printf("Port %d already set\n", p_number);

		}
	}

	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n = sendDataOnly(file_des,mess,sizeof(mess));
	} else {
		n = sendDataOnly(file_des,&p_number,sizeof(p_number));
		closeConnection(file_des);
		exitServer(sockfd);
		sockfd=sd;

	}

	return ret;

}

int get_last_client_ip(int file_des) {
	int ret=OK;
	int n;
	if (differentClients )
		ret=FORCE_UPDATE;
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	n = sendDataOnly(file_des,lastClientIP,sizeof(lastClientIP));

	return ret;

}


int send_update(int file_des) {

	int ret=OK;
	enum detectorSettings t;
	int n;
	int val;
	size_t s;
	int64_t retval, tns=-1;
	n = sendDataOnly(file_des,lastClientIP,sizeof(lastClientIP));
	val = NMODX;
	n = sendDataOnly(file_des,&val,sizeof(val));
	val = NMODY;
	n = sendDataOnly(file_des,&val,sizeof(val));
	val = DYNAMIC_RANGE;
	n = sendDataOnly(file_des,&val,sizeof(val));
	s = DATA_BYTES;
	n = sendDataOnly(file_des,&s,sizeof(s));
	t=setSettings(GET_SETTINGS,-1);
	n = sendDataOnly(file_des,&t,sizeof(t));
	/*  thr=getThresholdEnergy();
  n = sendDataOnly(file_des,&thr,sizeof(thr));*/
	retval=setFrames(tns);
	n = sendDataOnly(file_des,&retval,sizeof(int64_t));
	retval=setExposureTime(tns);
	n = sendDataOnly(file_des,&retval,sizeof(int64_t));
	retval=setPeriod(tns);
	n = sendDataOnly(file_des,&retval,sizeof(int64_t));
	retval=setDelay(tns);
	n = sendDataOnly(file_des,&retval,sizeof(int64_t));
	retval=setGates(tns);
	n = sendDataOnly(file_des,&retval,sizeof(int64_t));
	/*  retval=setProbes(tns);
  n = sendDataOnly(file_des,&retval,sizeof(int64_t));*/
	retval=setTrains(tns);
	n = sendDataOnly(file_des,&retval,sizeof(int64_t));

	if (lockStatus==0) {
		strcpy(lastClientIP,thisClientIP);
	}

	return ret;


}
int update_client(int file_des) {

	int ret=OK;

	sendDataOnly(file_des,&ret,sizeof(ret));
	return send_update(file_des);



}


int configure_mac(int file_des) {

	int ret=OK;
	char arg[5][50];
	int n;

	int imod=0;//should be in future sent from client as -1, arg[2]
	int ipad;
	long long int imacadd;
	long long int idetectormacadd;
	int udpport;
	int detipad;
	int retval=-100;

	sprintf(mess,"Can't configure MAC\n");


	n = receiveDataOnly(file_des,arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	sscanf(arg[0], "%x", 		&ipad);
	sscanf(arg[1], "%llx", 	&imacadd);
	sscanf(arg[2], "%x", 		&udpport);
	sscanf(arg[3], "%llx",	&idetectormacadd);
	sscanf(arg[4], "%x",		&detipad);

	//#ifdef VERBOSE
	int i;
	printf("\nipadd %x\t",ipad);
	printf("destination ip is %d.%d.%d.%d = 0x%x \n",(ipad>>24)&0xff,(ipad>>16)&0xff,(ipad>>8)&0xff,(ipad)&0xff,ipad);
	printf("macad:%llx\n",imacadd);
	for (i=0;i<6;i++)
		printf("mac adress %d is 0x%x \n",6-i,(unsigned int)(((imacadd>>(8*i))&0xFF)));
	printf("udp port:0x%x\n",udpport);
	printf("detector macad:%llx\n",idetectormacadd);
	for (i=0;i<6;i++)
		printf("detector mac adress %d is 0x%x \n",6-i,(unsigned int)(((idetectormacadd>>(8*i))&0xFF)));
	printf("detipad %x\n",detipad);
	printf("\n");
	//#endif



	if (imod>=getNModBoard())
		ret=FAIL;
	if (imod<0)
		imod=ALLMOD;

	//#ifdef VERBOSE
	printf("Configuring MAC of module %d at port %x\n", imod, udpport);
	//#endif
#ifdef MCB_FUNCS
	if (ret==OK){
		if(runBusy()){
			ret=stopStateMachine();
			if(ret==FAIL)
				strcpy(mess,"could not stop detector acquisition to configure mac");
		}

		if(ret==OK)
			configureMAC(ipad,imacadd,idetectormacadd,detipad,udpport);
	}
#endif
	if (ret==FAIL)
		printf("configuring MAC of mod %d failed\n", imod);
	else
		printf("Configuremac successful of mod %d\n",imod);

	if (differentClients)
		ret=FORCE_UPDATE;

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL)
		n += sendDataOnly(file_des,mess,sizeof(mess));
	else
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	/*return ok/fail*/
	return ret;

}



int load_image(int file_des) {
	int retval;
	int ret=OK;
	int n;
	enum imageType index;
	short int ImageVals[NCHAN*NCHIP];

	sprintf(mess,"Loading image failed\n");

	n = receiveDataOnly(file_des,&index,sizeof(index));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	n = receiveDataOnly(file_des,ImageVals,DATA_BYTES);
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	switch (index) {
	case DARK_IMAGE :
#ifdef VERBOSE
		printf("Loading Dark image\n");
#endif
		break;
	case GAIN_IMAGE :
#ifdef VERBOSE
		printf("Loading Gain image\n");
#endif
		break;
	default:
		printf("Unknown index %d\n",index);
		sprintf(mess,"Unknown index %d\n",index);
		ret=FAIL;
		break;
	}

	if (ret==OK) {
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		} else{
			retval=loadImage(index,ImageVals);
			if (retval==-1)
				ret = FAIL;
		}
	}

	if(ret==OK){
		if (differentClients)
			ret=FORCE_UPDATE;
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;
}



int set_master(int file_des) {

	enum masterFlags retval=GET_MASTER;
	enum masterFlags arg;
	int n;
	int ret=OK;
	// int regret=OK;


	sprintf(mess,"can't set master flags\n");


	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}


#ifdef VERBOSE
	printf("setting master flags  to %d\n",arg);
#endif

	if (differentClients==1 && lockStatus==1 && arg!=GET_READOUT_FLAGS) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	}  else {
		retval=setMaster(arg);

	}
	if (retval==GET_MASTER) {
		ret=FAIL;
	}
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n = sendDataOnly(file_des,mess,sizeof(mess));
	} else {
		n = sendDataOnly(file_des,&retval,sizeof(retval));
	}
	return ret;
}






int set_synchronization(int file_des) {

	enum synchronizationMode retval=GET_MASTER;
	enum synchronizationMode arg;
	int n;
	int ret=OK;
	//int regret=OK;


	sprintf(mess,"can't set synchronization mode\n");


	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
#ifdef VERBOSE
	printf("setting master flags  to %d\n",arg);
#endif

	if (differentClients==1 && lockStatus==1 && arg!=GET_READOUT_FLAGS) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	}  else {
		//ret=setStoreInRAM(0);
		// initChipWithProbes(0,0,0, ALLMOD);
		retval=setSynchronization(arg);
	}
	if (retval==GET_SYNCHRONIZATION_MODE) {
		ret=FAIL;
	}
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n = sendDataOnly(file_des,mess,sizeof(mess));
	} else {
		n = sendDataOnly(file_des,&retval,sizeof(retval));
	}
	return ret;
}






int read_counter_block(int file_des) {

	int ret=OK;
	int n;
	int startACQ;
	//char *retval=NULL;
	short int CounterVals[NCHAN*NCHIP];

	sprintf(mess,"Read counter block failed\n");

	n = receiveDataOnly(file_des,&startACQ,sizeof(startACQ));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	if (ret==OK) {
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		} else{
			ret=readCounterBlock(startACQ,CounterVals);
#ifdef VERBOSE
			int i;
			for(i=0;i<6;i++)
				printf("%d:%d\t",i,CounterVals[i]);
#endif
		}
	}

	if(ret!=FAIL){
		if (differentClients)
			ret=FORCE_UPDATE;
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,CounterVals,DATA_BYTES);
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;
}





int reset_counter_block(int file_des) {

	int ret=OK;
	int n;
	int startACQ;

	sprintf(mess,"Reset counter block failed\n");

	n = receiveDataOnly(file_des,&startACQ,sizeof(startACQ));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	if (ret==OK) {
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		} else
			ret=resetCounterBlock(startACQ);
	}

	if(ret==OK){
		if (differentClients)
			ret=FORCE_UPDATE;
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL)
		n += sendDataOnly(file_des,mess,sizeof(mess));

	/*return ok/fail*/
	return ret;
}






int start_receiver(int file_des) {
	int ret=FAIL;
	int n;
	strcpy(mess,"Not implemented for this detector\n");

	/* send answer */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	n = sendDataOnly(file_des,mess,sizeof(mess));
	/*return ok/fail*/
	return ret;
}






int stop_receiver(int file_des) {
	int ret = FAIL;
	int n;
	strcpy(mess,"Not implemented for this detector\n");

	/* send answer */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	n = sendDataOnly(file_des,mess,sizeof(mess));
	/*return ok/fail*/
	return ret;
}





int calibrate_pedestal(int file_des){

	int ret=OK;
	int retval=-1;
	int n;
	int frames;

	sprintf(mess,"Could not calibrate pedestal\n");

	n = receiveDataOnly(file_des,&frames,sizeof(frames));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}

	if (ret==OK) {
		if (differentClients==1 && lockStatus==1) {
			ret=FAIL;
			sprintf(mess,"Detector locked by %s\n",lastClientIP);
		} else
			ret=calibratePedestal(frames);
	}

	if(ret==OK){
		if (differentClients)
			ret=FORCE_UPDATE;
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL)
		n += sendDataOnly(file_des,mess,sizeof(mess));
	else
		n += sendDataOnly(file_des,&retval,sizeof(retval));

	/*return ok/fail*/
	return ret;
}


int write_adc_register(int file_des) {

	int retval;
	int ret=OK;
	int arg[2];
	int addr, val;
	int n;

	sprintf(mess,"Can't write to register\n");

	n = receiveDataOnly(file_des,arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	addr=arg[0];
	val=arg[1];

#ifdef VERBOSE
	printf("writing to register 0x%x data 0x%x\n", addr, val);
#endif  

	if (differentClients==1 && lockStatus==1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	}


	if(ret!=FAIL){
		ret=setAdc(addr,val);
		if (ret==OK)
			retval=val;
	}


#ifdef VERBOSE
	printf("Data set to 0x%x\n",  retval);
#endif  
	if (retval==val) {
		ret=OK;
		if (differentClients)
			ret=FORCE_UPDATE;
	} else {
		ret=FAIL;
		sprintf(mess,"Writing to register 0x%x failed: wrote 0x%x but read 0x%x\n", addr, val, retval);
	}

	/* send answer */
	/* send OK/failed */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret!=FAIL) {
		/* send return argument */
		n += sendDataOnly(file_des,&retval,sizeof(retval));
	} else {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	}

	/*return ok/fail*/
	return ret;

}





int program_fpga(int file_des) {
	int ret=OK;
	int n;
	sprintf(mess,"Program FPGA unsuccessful\n");
	char* fpgasrc = NULL;
	FILE* fp = NULL;
	size_t filesize = 0;
	size_t unitprogramsize = 0;
	size_t totalsize = 0;


	//filesize
	n = receiveDataOnly(file_des,&filesize,sizeof(filesize));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}
	totalsize = filesize;
#ifdef VERY_VERBOSE
	printf("\n\n Total size is:%d\n",totalsize);
#endif

	//lock
	if (ret==OK && differentClients==1 && lockStatus==1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
		filesize = 0;
	}

	//opening file pointer to flash and telling FPGA to not touch flash
	if(ret == OK && startWritingFPGAprogram(&fp) != OK){
		sprintf(mess,"Could not write to flash. Error at startup.\n");
		cprintf(RED,"%s",mess);
		ret=FAIL;
		filesize = 0;
	}

	//---------------- first ret ----------------
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL)
		n += sendDataOnly(file_des,mess,sizeof(mess));
	//---------------- first ret ----------------


	//erasing flash
	if(ret != FAIL){
		eraseFlash();
		fpgasrc = (char*)malloc(MAX_FPGAPROGRAMSIZE);
	}



	//writing to flash part by part
	while(ret != FAIL && filesize){

		unitprogramsize = MAX_FPGAPROGRAMSIZE;  //2mb
		if(unitprogramsize > filesize) //less than 2mb
			unitprogramsize = filesize;
#ifdef VERY_VERBOSE
		printf("unit size to receive is:%d\n",unitprogramsize);
		printf("filesize:%d currentpointer:%d\n",filesize,currentPointer);
#endif


		//receive
		n = receiveDataOnly(file_des,fpgasrc,unitprogramsize);
		if (n < 0) {
			sprintf(mess,"Error reading from socket\n");
			ret=FAIL;
		}


		if (ret==OK) {
			if(!(unitprogramsize - filesize)){
				fpgasrc[unitprogramsize]='\0';
				filesize-=unitprogramsize;
				unitprogramsize++;
			}else
				filesize-=unitprogramsize;

			ret = writeFPGAProgram(fpgasrc,unitprogramsize,fp);
		}


		//---------------- middle rets ----------------
		n = sendDataOnly(file_des,&ret,sizeof(ret));
		if (ret==FAIL) {
			n += sendDataOnly(file_des,mess,sizeof(mess));
			cprintf(RED,"Failure: Breaking out of program receiving\n");
		}
		//---------------- middle rets ----------------


		if(ret != FAIL){
			//print progress
			printf("Writing to Flash:%d%%\r",(int) (((double)(totalsize-filesize)/totalsize)*100) );
			fflush(stdout);
		}

	}



	printf("\n");

	//closing file pointer to flash and informing FPGA
	if(stopWritingFPGAprogram(fp) == FAIL){
		sprintf(mess,"Could not write to flash. Error at end.\n");
		cprintf(RED,"%s",mess);
		ret=FAIL;
	}

	if(ret!=FAIL){
		ret=FORCE_UPDATE;
	}


	//---------------- last ret ----------------
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL)
		n += sendDataOnly(file_des,mess,sizeof(mess));
	//---------------- last ret ----------------


	//free resources
	if(fpgasrc != NULL)
		free(fpgasrc);
	if(fp!=NULL)
		fclose(fp);
#ifdef VERY_VERBOSE
	printf("Done with program receiving command\n");
#endif
	/*return ok/fail*/
	return ret;
}



int reset_fpga(int file_des) {
	int ret=OK;
	int n;
	sprintf(mess,"Reset FPGA unsuccessful\n");

	resetFPGA();
	initializeDetector();

	ret = FORCE_UPDATE;
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL)
		n += sendDataOnly(file_des,mess,sizeof(mess));

	/*return ok/fail*/
	return ret;
}



int power_chip(int file_des) {

	int retval=-1;
	int ret=OK;
	int arg=-1;
	int n;

	n = receiveDataOnly(file_des,&arg,sizeof(arg));
	if (n < 0) {
		sprintf(mess,"Error reading from socket\n");
		ret=FAIL;
	}


#ifdef VERBOSE
	printf("Power chip to %d\n", arg);
#endif

	if (differentClients==1 && lockStatus==1 && arg!=-1) {
		ret=FAIL;
		sprintf(mess,"Detector locked by %s\n",lastClientIP);
	} else {
		retval=powerChip(arg);
#ifdef VERBOSE
		printf("Chip powered: %d\n",retval);
#endif

		if (retval==arg || arg<0) {
			ret=OK;
		} else {
			ret=FAIL;
			printf("Powering chip failed, wrote %d but read %d\n", arg, retval);
		}

	}
	if (ret==OK && differentClients==1)
		ret=FORCE_UPDATE;

	/* send answer */
	n = sendDataOnly(file_des,&ret,sizeof(ret));
	if (ret==FAIL) {
		n += sendDataOnly(file_des,mess,sizeof(mess));
	} else
		n += sendDataOnly(file_des,&retval,sizeof(retval));

	return ret;
}
