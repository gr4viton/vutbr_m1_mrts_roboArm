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
//char str[FILE_MAX_CHARS+CZERO] = ""; 
// Pointer of C_LogMessageA, used for all logging
C_LogMessageA logMsg;
LARGE_INTEGER preemptive_interval; 
	

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
	printf("Exiting process with error_sum %lu\n",error_sum);
	ExitProcess(error_sum);
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
	//printf("_________________________(: Clean start :)___________________________\n");
	//logMsg.PushMessage("cokoliv",10);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> roboArm started <<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	printf("SET preemptive_interval\n");
	preemptive_interval.QuadPart = 100;	
	printf("function main()\n");
	DWORD error_sum = 0;
	/*
	if ( argc != 2 )	 
	{// argc should be 2 for correct execution
		printf("You must specify the control txt file! Run:\n");
		printf("$ %s <control_file_path>\n", argv[0]);
		EXIT_process(ERROR_CONTROLFILE_PATH_NOT_SPECIFIED);
	}
	// We assume argv[1] is a filename to open - try lenght of string file_path 
	if( GET_stringLength(argv[1], MAX_PATH, &error_sum) == 0)
	{
		if( error_sum == ERROR_STRING_LENGHT_LARGER_THAN_TRESHOLD )
		{
			printf("ERROR - control file path is too long (max=%u) = program parameter [%s]\n", MAX_PATH, argv[1]);
			EXIT_process(ERROR_FILE_PATH_STRING_TOO_LONG);
		}
	}
	// the filename lenght is short enaght
	printf("Control-file: %s\n", argv[1]);
		*/
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// INITIALIZATIONS
	printf("Starting initialization process.\n");
	// ____________________________________________________
	// init logMsg = global initialized before main
	// printf("> Try to logging class\n");
	//logMsg = new C_LogMessageA();
	// ____________________________________________________
	// init HW
	printf("> Try to initialize hardware\n");
	error_sum = INIT_HW();
	if(error_sum != FLAWLESS_EXECUTION){
		printf("Initialization process failed with error_sum %lu\n", error_sum);
		EXIT_process(error_sum);
	}	
	//____________________________________________________
	// init classes for the manipulator
	printf("> Try to initialize robotic manipulator\n");
	C_roboticManipulator ROB(error_sum);
	if(error_sum != FLAWLESS_EXECUTION)
	{
		printf("Initialization of robotic manipulator failed with error_sum %lu\n", error_sum);
		EXIT_process(error_sum);
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// read phases from file
	
	//printf("> Try to read phases from Configuration file\n");
	// READ_spatialConfigurationFromFile(&ROB, argv[1]);
	printf("> Try to fill phases with testing positions\n");
	ROB.DEBUG_fillPhases();


// ____________________________________________________
// will be in the function CREATE_threads

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// thread creation

	const int iTh_max = NUM_OF_THREADS; 

	HANDLE		hTh[iTh_max];			// array of handles to the threads
	DWORD		thExitCode[iTh_max];		// exit code from thread

	int iTh = 0;							// handler iterator
	DWORD thread_id = 0;					// thread id input param

	//____________________________________________________
	// priorities - changed in switch case
	int wanted_priority = RT_PRIORITY_MIN;
	int thread_priority = RT_PRIORITY_MIN;
	
	//____________________________________________________
	char textMsg[MAX_MESSAGE_LENGTH];	// char array for log messages

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// will be in separate function CREATE THREAD?
	// create & priority & unsuspend all needed threads
	for(iTh = 0; iTh<iTh_max; iTh++)
	{
		//____________________________________________________
		// RtCreateThread - handle creation 
		RtPrintf("> Try to create thread[%i].\n", iTh);
		switch(iTh){
			//____________________________________________________
			case(TH_LOG_I): // Logging thread				
				logMsg.LoggingStart();	// Before log thread started, LoggingStart() must be called
				wanted_priority = TH_LOG_PRIORITY;
				// create thread
				hTh[iTh] = RtCreateThread(NULL, 0, 
					(LPTHREAD_START_ROUTINE) LogMessageThread, 
					NULL, CREATE_SUSPENDED, &thread_id);
				break;
			//____________________________________________________
			case(TH_PWM_I): // PWM controllign thread
				wanted_priority = TH_PWM_PRIORITY;
				// create thread
				hTh[iTh] = RtCreateThread(NULL, 0, 
					(LPTHREAD_START_ROUTINE) PWMthread, 
					(VOID*)&ROB, CREATE_SUSPENDED, &thread_id);
				break;

		}

		//____________________________________________________
		// the other rutines are same for all types of threads 
		if(hTh[iTh] == NULL){
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tCannot create thread[%i].\n",iTh);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_CREATE_THREAD);
		}
		RtPrintf("Thread[%i] created and suspended with priority %i.\n", iTh, RtGetThreadPriority(hTh[iTh]) );

		// ____________________________________________________
		// RtSetThreadPriority - set thread priority to wanted_priority
		if( RtSetThreadPriority( hTh[iTh], wanted_priority) ){
			thread_priority = RtGetThreadPriority(hTh[iTh]);
			if( thread_priority == wanted_priority ){
				sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Priority of thread[%i] sucessfully set to %i\n", iTh, wanted_priority );
				logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			}
			else{
				sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tCannot set thread[%i] priority to %i! It currently has priority %i.\n", 
					iTh, wanted_priority , thread_priority);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
				TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_CHANGE_PRIORITY);
			}
		}
		else{
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tCannot set thread[%i] priority to %i! It currently has priority %i.\n", 
				iTh, wanted_priority , GetThreadPriority(hTh[iTh]) );
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_CHANGE_PRIORITY);
		}

		//____________________________________________________
		// RtResumeThread - unsuspend 
		if( RtResumeThread(hTh[iTh]) != 0xFFFFFFFF ){
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Succesfully resumed thread[%i].\n", iTh);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			if(iTh == 0)
			{
				logMsg.PushMessage("Logging started.\n", SEVERITY_MAX - 5);
			}
		}
		else{
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Could not resume thread[%i].\n", iTh);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_RESUME_THREAD);
		}
	}

//____________________________________________________
// will be in main
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main thread-controlling super-loop

	int still_active_threads;

	//____________________________________________________
	// waiting for the termination of all threads but the Logging one [TH_LOG_I = 0]
	iTh = TH_LOG_I + 1;
	printf("main() - Waiting for all but the Logging thread to terminate.\n");
	do{ 
		still_active_threads = 0;
		iTh = iTh;
		// get the exit code of a thread
		if(GetExitCodeThread(hTh[iTh], &(thExitCode[iTh]) ) == FALSE)
		{ // the function failed 
			error_sum = GetLastError();
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function GetExitCodeThread called with thread[%i] failed, returned FALSE with error %lu\n", iTh, error_sum);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
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
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);		
	}			

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// End of logging -> ends thread hTh[TH_LOG_I],  TH_LOG_I = 0
	printf("main() - Try to stop Loggign function.\n");
	logMsg.LoggingStop();
	printf("main() - Waiting for the Logging thread to terminate.\n");
	// Wait to end of thread hTh[TH_LOG_I]
	do{
		if(GetExitCodeThread( hTh[TH_LOG_I], &(thExitCode[TH_LOG_I]) ) == FALSE){
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Function of thread[%i] failed, returned FALSE with exit-code %lu\n", TH_LOG_I, thExitCode[TH_LOG_I]);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
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
	

#ifndef HIDE_TEST_CODES
	
	int num = 5; // number_of_mean_values
	// Reading Data 
	while(num)
	{
		RtPrintf("ADC_feedBack[1,2,3] = %5u;\t\t%5u;\t\t%5u;\n",
			MEAN_adc(0,0,num),
			MEAN_adc(1,0,num),
			MEAN_adc(2,0,num)
			);
		RtSleep(100);
	}

	LARGE_INTEGER interval_one; 
	LARGE_INTEGER intervalZero; 
	LARGE_INTEGER interval_wait; 
	
	interval_one.QuadPart = 10000; // 1000us
	intervalZero.QuadPart = 200000 - 10000; // 0.02s = 50Hz
	interval_wait.QuadPart = 10000000; // 1s
	
	int i = 0;
//	int j = 0;
//	int max_j = 8;

	E_servos testedServo = S5;
	int period = 200;	//Hz
	interval_one.QuadPart = NS100_1US * 500;	// 500 us
	intervalZero.QuadPart = NS100_1MS * 1000/period - interval_one.QuadPart; // 0.01s = 100Hz
	interval_wait.QuadPart = NS100_1MS * 300; // 0.1s
	for( i = 0 ; i < 200 ; i++)
	{
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 1<<testedServo);
		RtSleepFt(&interval_one);
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x00);
		RtSleepFt(&intervalZero);
	}

	interval_one.QuadPart = NS100_1US * 2500;	// 2500 us
	intervalZero.QuadPart = NS100_1MS * 1000/period - interval_one.QuadPart; // 0.01s = 100Hz
	interval_wait.QuadPart = NS100_1MS * 300; // 0.1s
	for( i = 0 ; i < 200 ; i++)
	{
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 1<<testedServo);
		RtSleepFt(&interval_one);
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x00);
		RtSleepFt(&intervalZero);
	}
#endif
	//____________________________________________________
	// everything should be unallocated and closed
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> roboArm ended <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	//____________________________________________________
	// write all messages 
	while( ERROR_BUFFER_IS_EMPTY != logMsg.WriteBuffToFile());
	printf("Exiting process.\n");
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
		//RtPrintf("GetADC: Reading timeout\n");
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