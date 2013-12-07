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

DWORD baseAddress = 0;
HMODULE hLibModule = NULL;
char* Gstr = NULL;
//char str[FILE_MAX_CHARS+CZERO] = ""; 




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
@function	CREATE_threads
@brief		Create all needed threads and returns a handle to an array of them
@param[in]
@param[out]
@return
***************/
HANDLE* CREATE_threads(void)
{
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function	main
@brief
@param[in]
@param[out]
@return
***************/
//void _cdecl main(int  argc, char **argv, char **envp)
void _cdecl main(int  argc, char **argv)
{
	printf("_________________________(: Clean start :)___________________________\n");
	if ( argc != 2 )	 
	{// argc should be 2 for correct execution
		printf("You must specify the control txt file! Run:\n");
		printf("$ %s <control_file_path>\n", argv[0]);
		ExitProcess(ERROR_CONTROLFILE_PATH_NOT_SPECIFIED);
	}
	// We assume argv[1] is a filename to open
	
	// try lenght of string file_path 
	for(int i=0; argv[1][i] != '\0'; i++)
	{
		if( i>=MAX_PATH ) 
		{
			ExitProcess(ERROR_FILE_PATH_STRING_TOO_LONG);
		}
	}
	printf("Control-file: %s\n",argv[1]);

	int error_sum = 0;
	// ____________________________________________________
	// init HW
	printf("Starting initialization process.");
	error_sum = INIT_All();
	if(error_sum!=FLAWLESS_EXECUTION){
		//log
		ExitProcess(error_sum);
	}	
	//____________________________________________________
	// init classes for the manipulator
	printf("> Try to initialize robotic manipulator\m");
	C_roboticManipulator ROB(error_sum);
	if(error_sum != FLAWLESS_EXECUTION)
	{
		printf("Initialization of robotic manipulator failed with error_sum %i\n", error_sum);
		ExitProcess(error_sum);
	}

	//____________________________________________________
	// read phases from file
	READ_spatialConfigurationFromFile(&ROB, argv[1]);


// ____________________________________________________
// will be in the function CREATE_threads

	//____________________________________________________
	// thread creation
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	const int iTh_max = 1; // number of threads = for now only one
	HANDLE hTh[iTh_max]; // array of handles to the threads
	//DWORD this_loop_ExitCode_sum = 0;

	LPDWORD thExitCode[iTh_max];
//	void* thread_argument[iTh_max];

	int default_priority = RT_PRIORITY_MAX - 1;
	int wanted_priority = default_priority;
	int thread_priority = 0;
	int iTh = 0;
	
	DWORD thread_id = 0;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// RtCreateThread - will be in the separate function
	for(iTh = 0; iTh<iTh_max; iTh++){
		//____________________________________________________
		// RtCreateThread - handle creation 
		RtPrintf("> Try to create multi thread %i.\n", iTh);
		hTh[iTh] = RtCreateThread(NULL, 0, 
			(LPTHREAD_START_ROUTINE) TIM_PWMfunction, (VOID*)&ROB, CREATE_SUSPENDED, &thread_id);
		if(hTh[iTh] == NULL){
			RtPrintf("ERROR:\tCannot create thread %i.\n",iTh);
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_CREATE_THREAD);
		}
		RtPrintf("Thread %i created and suspended with priority %i.\n", iTh, RtGetThreadPriority(hTh[iTh]) );

		// ____________________________________________________
		// RtSetThreadPriority
		wanted_priority = default_priority - iTh;
		if( RtSetThreadPriority( hTh[iTh], wanted_priority) ){
			thread_priority = RtGetThreadPriority(hTh[iTh]);
			if( thread_priority == wanted_priority ){
				RtPrintf("Priority of thread %i sucessfully set to %i\n", iTh, wanted_priority );
			}
			else{
				RtPrintf("ERROR:\tCannot set thread %i priority to %i! It currently has priority %i.\n", 
					iTh, wanted_priority , thread_priority);
				TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_CHANGE_PRIORITY);
			}
		}
		else{
			RtPrintf("ERROR:\tCannot set thread %i priority to %i! It currently has priority %i.\n", 
				iTh, wanted_priority , GetThreadPriority(hTh[iTh]) );
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_CHANGE_PRIORITY);
		}
		//____________________________________________________
		// RtResumeThread
		if( RtResumeThread(hTh[iTh]) != 0xFFFFFFFF ){
			RtPrintf("Succesfully resumed thread %i.\n", iTh);
		}
		else{
			RtPrintf("Could not resume thread %i.\n", iTh);
			TERMINATE_allThreadsAndExitProcess(hTh, iTh_max, ERROR_COULD_NOT_RESUME_THREAD);
		}
	}


//____________________________________________________
// will be in main
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main thread-controlling super-loop

	int still_active_threads;
	LARGE_INTEGER preemptive_interval; 
	preemptive_interval.QuadPart = 100;
	iTh = 0;
	do{
		still_active_threads = 0;
		//BOOL GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode);
		if(GetExitCodeThread(hTh[iTh], (thExitCode[iTh]) ) == FALSE){
			RtPrintf("Function of thread %i failed, returned FALSE with exit-code %lu\n", iTh, *thExitCode);
			break;
		}
		if( *thExitCode[iTh] == STILL_ACTIVE ){
			still_active_threads++;
		}
		else{
			
		}
		//____________________________________________________
		// could not be executed if CPU>1 
		RtSleepFt(&preemptive_interval);
	}while(still_active_threads);
	
	for(iTh = 0; iTh<iTh_max; iTh++){
		RtPrintf("Thread %i terminated with exit code %lu\n", iTh, *thExitCode[iTh]);
		//printf("Thread %i sum = %f\n", iTh, static_cast<double *>(thread_argument[iTh]));
	}


	
			
	
	LARGE_INTEGER interval_one; 
	LARGE_INTEGER interval_zero; 
	LARGE_INTEGER interval_wait; 
	
	interval_one.QuadPart = 10000; // 1000us
	interval_zero.QuadPart = 200000 - 10000; // 0.02s = 50Hz
	interval_wait.QuadPart = 10000000; // 1s
	
	int i = 0;
//	int j = 0;
//	int max_j = 8;

	E_servos testedServo = S5;
	int period = 200;	//Hz
	interval_one.QuadPart = NS100_1US * 500;	// 500 us
	interval_zero.QuadPart = NS100_1MS * 1000/period - interval_one.QuadPart; // 0.01s = 100Hz
	interval_wait.QuadPart = NS100_1MS * 300; // 0.1s
	for( i = 0 ; i < 200 ; i++)
	{
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 1<<testedServo);
		RtSleepFt(&interval_one);
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x00);
		RtSleepFt(&interval_zero);
	}

	interval_one.QuadPart = NS100_1US * 2500;	// 2500 us
	interval_zero.QuadPart = NS100_1MS * 1000/period - interval_one.QuadPart; // 0.01s = 100Hz
	interval_wait.QuadPart = NS100_1MS * 300; // 0.1s
	for( i = 0 ; i < 200 ; i++)
	{
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 1<<testedServo);
		RtSleepFt(&interval_one);
		RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x00);
		RtSleepFt(&interval_zero);
	}
	int num = 5; // number_of_mean_values
	// Reading Data 
	while(num)
	{
		RtPrintf("FDBACK[1,2,3] = %5u;\t\t%5u;\t\t%5u;\n",
			MEAN_adc(0,0,num),
			MEAN_adc(1,0,num),
			MEAN_adc(2,0,num)
			);
		RtSleep(100);
	}

    ExitProcess(0);
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
		RtPrintf("GetADC: Reading timeout\n");
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