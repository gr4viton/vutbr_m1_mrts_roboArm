/***************
@project  roboArm
@filename roboArm.cpp
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_07
@brief    main file
		Program for communication and control of PIO821 card, connected to a robotic manipulator ROB2-6.
		*	It reads the instructions from text file and sets the servo-mechanisms' angles respectively with
			predefined interval of halt time.
		*	For three of total six servos, there is time-ramp control implemented as there are feedback loop
			conected to the module.
***************/

#include "roboArm.h"

//____________________________________________________ 
// global Variables

unsigned int baseAddress = 0;
HMODULE hLibModule = NULL;
char* G_controlString = NULL;
C_LogMessageA logMsg; 

#ifdef RUNNING_ON_1CPU
	LARGE_INTEGER preemptive_interval; 
#endif

/****************************************************************************
@function		MEAN_adc
@brief			mean of c measured values
@param[in]		
***************/
DWORD MEAN_adc(UCHAR channel, UCHAR gain, int c)
{
	if(c==0) return(0);
	DWORD sum=0;
	//DWORD *vals;
	//vals = (DWORD*)malloc(sizeof(DWORD));
	int i = c;
	for(i;i>=0;i--){
		//vals[c-1] = GET_ADC(channel);
		sum += GET_ADC(channel, gain);
	}
	//free(vals);
	return(sum/c);
}

/****************************************************************************
@function	EXIT_process
@brief		exit process and log it
@param[in]
@param[out]
@return
***************/
void EXIT_process(DWORD error_sum){
	char textMsg[MAX_MESSAGE_LENGTH];
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Exiting process with error_sum %lu\n", error_sum);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_EXITING_PROCESS);
	ExitProcess(error_sum);
}
/**
 * @name 	LogMessage
 * @brief	function logs a message into the application log stream in the format: DD.MM.YYYY HH:MM:SS:MSS Message
 * @param [in]	iSeverity - an importance of the message in the range <SEVERITY_MIN, SEVERITY_MAX>. Messages lower than SEVERITY_LEVEL are not being logged
 * @param [in]	cMessage - a message to be logged
 * @param [in]	bBlocking - if, 0 the function is blocking (synchronous write operation), otherwise function is non-blocking (asynchronous write operation with buffering)
 * @retval	0 - function succeeded
 * @retval	otherwise - an error occurred
*/
int LogMessage(int iSeverity, char *cMessage, int bBlocking)
{
	DWORD error_sum = logMsg.PushMessage(cMessage, iSeverity, bBlocking);
	return(error_sum);
}


/****************************************************************************
@function	main
@brief		the function which is runned first, 
			Calls init procedures and all other needed functions
@param[in]	(int)argc - number of text input program parameters
			(char)**argv - individual text input program parameters
@param[out]	-
@return		-
***************/
//void _cdecl main(int  argc, char **argv, char **envp)
void _cdecl main(int  argc, char **argv)
{
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// program parameters aquisition
	char textMsg[MAX_MESSAGE_LENGTH];	// char array for log messages
	logMsg.PushMessage(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> roboArm started <<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", LOG_SEVERITY_MAIN_FUNCTION);
	logMsg.PushMessage("function main()\n", LOG_SEVERITY_MAIN_FUNCTION);
#ifdef RUNNING_ON_1CPU
	logMsg.PushMessage("SET preemptive_interval\n", LOG_SEVERITY_MAIN_FUNCTION);
	preemptive_interval.QuadPart = DEFAULT_PREEMPTIVE_INTERVAL;	
#endif
	DWORD error_sum = 0;
	if ( argc != 2 )	 
	{// argc should be 2 for correct execution
		logMsg.PushMessage("You must specify the control txt file! Run:\n", LOG_SEVERITY_MAIN_FUNCTION);
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "$ %s <control_file_path>\n", argv[0]);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);
		EXIT_process(ERROR_CONTROLFILE_PATH_NOT_SPECIFIED);
	}
	// Assume argv[1] is a filename to open - try lenght of string file_path 
	if( GET_stringLength(argv[1], MAX_PATH, &error_sum) == 0)
	{
		if( error_sum == ERROR_STRING_LENGHT_LARGER_THAN_TRESHOLD )
		{		
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, 
				"ERROR - control file path is too long (max=%u) = program parameter [%s]\n", 
				MAX_PATH, argv[1]
			);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);
			EXIT_process(ERROR_FILE_PATH_STRING_TOO_LONG);
		}
	}
	// the filename lenght is short enaugh
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Control-file: %s\n", argv[1]);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);


	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// INITIALIZATIONS
	logMsg.PushMessage("Starting initialization process.\n", LOG_SEVERITY_MAIN_FUNCTION);
	// ____________________________________________________
	// init HW
	logMsg.PushMessage("> Try to initialize hardware\n", LOG_SEVERITY_MAIN_FUNCTION);
	error_sum = INIT_HW();
	if(error_sum != FLAWLESS_EXECUTION){
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, 
			"Initialization process failed with error_sum %lu\n", error_sum);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);
		EXIT_process(error_sum);
	}	
	//____________________________________________________
	// init classes for the manipulator
	logMsg.PushMessage("> Try to initialize robotic manipulator\n", LOG_SEVERITY_MAIN_FUNCTION);
	C_roboticManipulator ROB(error_sum);
	if(error_sum != FLAWLESS_EXECUTION)
	{
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, 
			"Initialization of robotic manipulator failed with error_sum %lu\n", error_sum);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);
		EXIT_process(error_sum);
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// read phases from file
#ifdef READ_SPATIAL_CONFIGURATION
	READ_spatialConfigurationFromFile(&ROB, argv[1]);
#endif
	logMsg.PushMessage("> Insert testing phases\n", LOG_SEVERITY_MAIN_FUNCTION);
	ROB.DEBUG_fillPhases();

	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// thread creation
	
	HANDLE hTh[NUM_OF_THREADS];			// array of handles to the threads

	int iTh = 0;							// thread handler index
	const int iTh_max = NUM_OF_THREADS; 
	DWORD thID[NUM_OF_THREADS];			// thread id input param

	//____________________________________________________
	// CREATE logging thread
	logMsg.LoggingStart(); // Before log thread started, LoggingStart() must be called
	error_sum = CREATE_thread(iTh, hTh[iTh], &(thID[iTh]), TH_LOG_PRIORITY, 
		(LPTHREAD_START_ROUTINE)LogMessageThread, NULL);
	
	if(error_sum != FLAWLESS_EXECUTION) EXIT_process(error_sum);
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Logging started! writing into [%s], SEVERITY_LEVEL = %i\n", LOG_FILE_PATH, SEVERITY_LEVEL);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_LOGGING_STARTED);	
	
	//____________________________________________________
	// CREATE PWM thread
	iTh++;
	error_sum = CREATE_thread(iTh, hTh[iTh], &(thID[iTh]), TH_PWM_PRIORITY, 
		(LPTHREAD_START_ROUTINE)PWMthread, &ROB);
	if(error_sum != FLAWLESS_EXECUTION) EXIT_process(error_sum);
	//CREATE_threads(&ROB, hTh, thID);
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main thread-controlling super-loop

	int still_active_threads;
	DWORD thExitCode[NUM_OF_THREADS];		// exit code from thread

	//____________________________________________________
	// waiting for the termination of all threads but the Logging one [TH_LOG_I = 0]
	iTh = TH_LOG_I + 1;
	logMsg.PushMessage("main() - Waiting for all but the Logging thread to terminate.\n", LOG_SEVERITY_MAIN_FUNCTION);
	do{ 
		still_active_threads = 0;
		iTh = iTh;
		// get the exit code of a thread
		if(GetExitCodeThread(hTh[iTh], &(thExitCode[iTh]) ) == FALSE)
		{ // the function failed 
			error_sum = GetLastError();
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function GetExitCodeThread called with thread[%i] failed, returned FALSE with error %lu\n", iTh, error_sum);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);
			break;
		}
		// is this thread still active?
		if( thExitCode[iTh] == STILL_ACTIVE )
		{ // this thread is still active
			still_active_threads++;
		}
		// try next thread
		iTh++;
		if(iTh >= iTh_max) iTh = 1; 
#ifdef RUNNING_ON_1CPU
		RtSleepFt(&preemptive_interval);  // preemption
#endif
	}while(still_active_threads);
	
	for(iTh = 1; iTh<iTh_max; iTh++){
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Thread[%i] terminated with exit code %lu\n", iTh, thExitCode[iTh]);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);		
	}			

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// End of logging -> ends thread hTh[TH_LOG_I],  TH_LOG_I = 0
	logMsg.PushMessage("main() - Try to stop Loggign function.\n", LOG_SEVERITY_MAIN_FUNCTION);
				
	logMsg.LoggingStop();
	logMsg.PushMessage("main() - Waiting for the Logging thread to terminate.\n", LOG_SEVERITY_MAIN_FUNCTION);
	// Wait to end of thread hTh[TH_LOG_I]
	do{
		if(GetExitCodeThread( hTh[TH_LOG_I], &(thExitCode[TH_LOG_I]) ) == FALSE){
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function of thread[%i] failed, returned FALSE with exit-code %lu\n", TH_LOG_I, thExitCode[TH_LOG_I]);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);
			break;
		}
		if( thExitCode[TH_LOG_I] != STILL_ACTIVE ) 	break;
#ifdef RUNNING_ON_1CPU
		RtSleepFt(&preemptive_interval);  // preemption
#endif
	}while(still_active_threads);

	

	//    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// for debugging purposes only
	

#ifndef DEBUG_PRINT_FDBACK_ADC_DATA_AFTER_PHASES_END
	int num = 100; // number_of_mean_values
	// Reading Data 
	while(num--)
	{
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, 
			"ADC_feedBack[1,2,3] = %5u;\t\t%5u;\t\t%5u;\n",
			MEAN_adc(0,0,num),
			MEAN_adc(1,0,num),
			MEAN_adc(2,0,num)
			);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_MAIN_FUNCTION);
		RtSleep(100);
	}
#endif
	//____________________________________________________
	// everything should be unallocated and closed
	logMsg.PushMessage(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> roboArm ended <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", 
		LOG_SEVERITY_MAIN_FUNCTION);
	//____________________________________________________
	// write all messages 
	while( ERROR_BUFFER_IS_EMPTY != logMsg.WriteBuffToFile());
	logMsg.PushMessage("Exiting process.\n", LOG_SEVERITY_MAIN_FUNCTION);
    EXIT_process(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function	GET_ADC
@brief
@param[in]
@param[out]
@return
***************/
DWORD GET_ADC(UCHAR channel, UCHAR gain)
{
	UCHAR ready;	
	UCHAR mp;
	UCHAR ADlow;								
	UCHAR ADhigh;								
	DWORD val;									
	unsigned short c;

	// set gain and mux
	// x|x|MUX3|MUX2|MUX1|MUX0|GAIN1|GAIN0
	// MUX[3-0] = binary number selecting from 0to15 shifted left 2 
	//	0b0000 0000<<2 = 0x00<<2 = 0x00 = 0<<2 = AI0
	//	0b0000 0001<<2 = 0x01<<2 = 0x04 = 1<<2 = AI1
	//	0b0000 0010<<2 = 0x02<<2 = 0x08 = 2<<2 = AI2
	//	0b0000 0100<<2 = 0x03<<2 = 0x0C = 3<<2 = AI3
	// etc
	gain = 0;
	mp = channel<<2 | gain;

	RtWritePortUchar((PUCHAR)(baseAddress+AD_GAIN), mp);

	RtSleep(1);

	RtWritePortUchar((PUCHAR)(baseAddress+AD_SOFT_TRIGGER_START), 0x01);

	for ( c = 0, ready = 0x00 ; (ready == 0x00) && (c <= 10) ; c++ ) {
		ready = RtReadPortUchar((PUCHAR)(baseAddress+AD_STATUS));		
		ready = ready & 0x01;
	}

	if ( c > 10 ) {
		logMsg.PushMessage("GetADC: Reading timeout\n", LOG_SEVERITY_NORMAL);
		return 0;		
	}

	ADlow = RtReadPortUchar((PUCHAR)(baseAddress+AD_LOWBYTE));
	ADhigh = RtReadPortUchar((PUCHAR)(baseAddress+AD_HIGHBYTE));
	val = ADhigh;
	val = val << 8;
	val = val | ADlow;
	/*
	RtPrintf("control byte = 0x%08x;\t\tADlow = %i;\t\tADhigh = %i;\t\tval = %lu\n",
		channel, ADlow, ADhigh, val);
	*/	
	return val;
}






//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// do not delete until last release

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// TODO
	//____________________________________________________
	// 0) find out initial configurations for each servo in C_roboticManipulator constructor!
	// # done
	// 1) thread creation for each servo (?in C_roboticManipulator constructor?)
	// --> REWRITE it as I thought it will be handled with timers, but
	// thread handling will be better
	
	// writing to a critical section should be treated wisely ! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// is the register critical section? I think yess
	// a) mutex for individual bytes / bites of shadow-register
	//		and one core thread reading shadow-register and writing it to the true byteAdress 
	// b) in each instance of class C_servoMotor there will be mutex for writing the intervals
	//		this mutex will be set/unset with the individual servoMotors threads
	//		and it will be read by the main thread every min_period of setting the register
	//		-> if it is closed the thread waits to write into it until main thread opens it agein
	// c) in each thread will add event when it wants to set/unset the byte
	//		main thread will treat this events and write to the critical-section = register
	// --- possibly event driven ---
	//

	// 2) member function PWM_dutyCycle -> periodically executed in each thread
	// 3) find out if writing to register is criticall section

	
	/*
	// is this secure? 
	// or I should be working only in C_roboticManipulator class to avoid encapsulation problems??
	C_servoMotor* pServo = NULL;
	ROB.GET_servoMotor(1, pServo);
	pServo->PWM_dutyCycle();
	*/