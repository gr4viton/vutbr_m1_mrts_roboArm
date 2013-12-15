#include "roboArm.h"


/****************************************************************************
@function   CREATE_thread
@brief      this function creates thread with inputted parameters 
			and gives created thread handle and threadID
@param[in]  int iTh | index of a thread - to write in logs
			int wanted_priority | wanted thread priority
			LPTHREAD_START_ROUTINE a_threadRoutine | pointer to thread routine
			void* a_threadParam | input parameter for the thread function
@param[out] 	HANDLE& a_threadHandle | handle to created thread
			DWORD* a_threadID | created thread id
@return     DWORD error_sum
************/
DWORD CREATE_thread(int iTh, HANDLE& a_threadHandle, DWORD* a_threadID, 
	int wanted_priority, LPTHREAD_START_ROUTINE a_threadRoutine, void* a_threadParam)
{
	//____________________________________________________
	// priorities - changed in switch case
	int thread_priority = RT_PRIORITY_MIN;
	
	char textMsg[MAX_MESSAGE_LENGTH];	// char array for log messages
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// create & priority & unsuspend 
	//____________________________________________________
	// thread handle creation 
	a_threadHandle = CreateThread(NULL, NORMAL_THREAD_STACK_SIZE, 
		(LPTHREAD_START_ROUTINE) a_threadRoutine, 
		(VOID*) a_threadParam, CREATE_SUSPENDED, a_threadID);
	
	if(a_threadHandle == NULL){
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tCannot create thread[%i].\n", iTh);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
		return(ERROR_COULD_NOT_CREATE_THREAD);
	}

	RtPrintf("Thread[%i] created and suspended with priority %i.\n", iTh, RtGetThreadPriority(a_threadHandle) );

	// ____________________________________________________
	// set thread priority to wanted_priority
	BOOL ret_val = RtSetThreadPriority(a_threadHandle, wanted_priority);
	thread_priority = RtGetThreadPriority(a_threadHandle);

	if( !ret_val || (thread_priority != wanted_priority) )
	{	
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, 
			"ERROR:\tCannot set thread[%i] priority to %i! It currently has priority %i.\n", 
			iTh, wanted_priority , thread_priority);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
		return(ERROR_COULD_NOT_CHANGE_PRIORITY);
	}

	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Priority of thread[%i] sucessfully set to %i\n", iTh, wanted_priority );
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);

	//____________________________________________________
	// RtResumeThread - un-suspend 
	if( RtResumeThread(a_threadHandle) == 0xFFFFFFFF ){
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Could not resume thread[%i].\n", iTh);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
		return(ERROR_COULD_NOT_RESUME_THREAD);
	}

	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Succesfully resumed thread[%i].\n", iTh);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);

	return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function	CREATE_threads
@brief		Create all needed threads and returns a handle to an array of them
@param[in]	ROB robot definitions, 
@param[out] hTh thread handlers array, thExitCode exit code array, thread_id ids
@return
***************/
DWORD CREATE_threads(C_roboticManipulator* a_ROB, HANDLE *a_hTh, DWORD* a_threadID)
{
	/*
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// thread creation
	int iTh = 0;							// thread handler index
	const int iTh_max = NUM_OF_THREADS; 
	//HANDLE hTh = *a_hTh;
	
	//printf("&(a_hTh[0]) = %i; &(a_hTh) = %i; a_hTh[0] = %i; a_hTh = %i; \n", &(a_hTh[0]), &(a_hTh), a_hTh[0], a_hTh );

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
				a_hTh[iTh] = RtCreateThread(NULL, 0, 
					(LPTHREAD_START_ROUTINE) LogMessageThread, 
					NULL, CREATE_SUSPENDED, &(a_threadID[0]));
				break;
			//____________________________________________________
			case(TH_PWM_I): // PWM controllign thread
				wanted_priority = TH_PWM_PRIORITY;
				// create thread
				a_hTh[iTh] = RtCreateThread(NULL, 0, 
					(LPTHREAD_START_ROUTINE) PWMthread, 
					(VOID*)a_ROB, CREATE_SUSPENDED, &(a_threadID[1]));
				break;

		}

		//____________________________________________________
		// the other rutines are same for all types of threads 
		if(a_hTh[iTh] == NULL){
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tCannot create thread[%i].\n",iTh);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			TERMINATE_allThreadsAndExitProcess(a_hTh, iTh_max, ERROR_COULD_NOT_CREATE_THREAD);
		}
		RtPrintf("Thread[%i] created and suspended with priority %i.\n", iTh, RtGetThreadPriority(a_hTh[iTh]) );

		// ____________________________________________________
		// RtSetThreadPriority - set thread priority to wanted_priority
		if( RtSetThreadPriority( a_hTh[iTh], wanted_priority) ){
			thread_priority = RtGetThreadPriority(a_hTh[iTh]);
			if( thread_priority == wanted_priority ){
				sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Priority of thread[%i] sucessfully set to %i\n", iTh, wanted_priority );
				logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			}
			else{
				sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tCannot set thread[%i] priority to %i! It currently has priority %i.\n", 
					iTh, wanted_priority , thread_priority);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
				TERMINATE_allThreadsAndExitProcess(a_hTh, iTh_max, ERROR_COULD_NOT_CHANGE_PRIORITY);
			}
		}
		else{
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "ERROR:\tCannot set thread[%i] priority to %i! It currently has priority %i.\n", 
				iTh, wanted_priority , GetThreadPriority(a_hTh[iTh]) );
			TERMINATE_allThreadsAndExitProcess(a_hTh, iTh_max, ERROR_COULD_NOT_CHANGE_PRIORITY);
		}

		//____________________________________________________
		// RtResumeThread - unsuspend 
		if( RtResumeThread(a_hTh[iTh]) != 0xFFFFFFFF ){
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
			TERMINATE_allThreadsAndExitProcess(a_hTh, iTh_max, ERROR_COULD_NOT_RESUME_THREAD);
		}
	}
	*/
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function		LOGGING
@brief			Function function periodically call method
				WriteBuffToFile in logMsg instance of C_LogMessageA. If the buffer
				contains message and mutex is free, then it writes message to Log file.
@param[in]		(void*) - 
***************/
void RTFCNDCL LogMessageThread(void *)
{
	// logging - do NOT modify
	while(logMsg.GetState())
	{
		logMsg.WriteBuffToFile();
#ifdef RUNNING_ON_1CPU
		RtSleepFt(&preemptive_interval);
#endif
	}
	ExitThread(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function		PWMthread
@brief			Function of thread writing into the DO register 
				in the main loop there are PWMtic waitings
				if the number of them is same as PWMperiod_interval -> new period starts
				-> every new period zeros are written on all ports
				-> when the number of tics is the same as intervalZero of serv[x]
				---> one is written to its bit in register
				Servo motors have their position regulated by pulses of different width.
@param[in]		void *a_struct
				- i will not be needed
***************/
void RTFCNDCL PWMthread(void *a_ROB)
{
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
	// robotic manipulator pointer
	C_roboticManipulator* ROB = (C_roboticManipulator*)a_ROB;

	// smallest tic interval
	printf("RtGetClockTimerPeriod = %I64d [100ns]\n", ROB->PWMtic_interval);
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main PWMthread loop init
	DWORD error_sum = 0;
	bool generateTics = true;
	bool allPhasesEnded = false;

	ROB->RESET_DOport();
	
	ROB->PWMperiod_sum = 0; 
	ROB->PWMperiod_sum_max = 0;
	
	ROB->phase_act = ROB->phases.begin();
	ROB->PWMperiod_interval.QuadPart = DEFAULT_PWM_PERIOD;
	//____________________________________________________
	// time measurement
	RtGetClockTime(CLOCK_MEASUREMENT, &(ROB->tim_startPWMperiod));
		
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main PWMthread loop start
	while(!allPhasesEnded)
	{
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// LOAD this phase
		error_sum = ROB->LOAD_actualPhase();
		if(error_sum != FLAWLESS_EXECUTION) ExitThread(EXIT_threadPWM(error_sum, &ROB));
		
#ifdef DEBUGGING_WITHOUT_HW
		logMsg.PushMessage("DEBUGING_WITHOUT_HW - not writing to any register!", LOG_SEVERITY_PWM_PHASE);
#endif
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// tics loop 
		while(generateTics)	
		{
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "PWMtic_sum=%I64d/%I64d\n", ROB->PWMtic_sum, ROB->PWMperiod_interval);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_TIC);

			// calculate newDO port 
			ROB->CALC_DOport_thisPeriodNewValue();

			// write new DO port value from this period
			ROB->WRITE_DOport_thisPeriodNewValue();

			// PWMtic - waiting
			RtSleepFt(&(ROB->PWMtic_interval));
			
			// increment tic sums
			ROB->PWMtic_sum += ROB->PWMtic_interval.QuadPart;
			ROB->phaseTic_sum += ROB->PWMtic_interval.QuadPart;

			// End of phase = exit generateTics loop
			if( ROB->IS_endOfPhase() ) break;
			// ____________________________________________________
			// end of each period = reset PWMtic_sum etc.
			if( ROB->IS_endOfPeriod() ) 
			{
				ROB->FINISH_period();
			}
		} //generateTics loop
			
		//____________________________________________________
		// iteration to next phase
		if(ROB->SET_NextPhase() != FLAWLESS_EXECUTION)
		{ // end of all phases 
			logMsg.PushMessage("All phases are done!\n", LOG_SEVERITY_PWM_PHASE);
			allPhasesEnded = true;
			break;
		}
	} // end - allPhasesEnded - phase loop END

	ExitThread(EXIT_threadPWM(FLAWLESS_EXECUTION, &ROB));
}

/****************************************************************************
@function   EXIT_threadPWM
@brief      unitializises threadPWM variables and returns error_sum
@param[in]  
@param[out] 
@return     error_sum
************/
DWORD EXIT_threadPWM(DWORD error_sum, C_roboticManipulator** a_ROB)
{	
	logMsg.PushMessage("Deinitializing ROB pointer\n", LOG_SEVERITY_EXITING_THREAD);
	(*a_ROB) = NULL;	

	char textMsg[MAX_MESSAGE_LENGTH];
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Exitting thread PWM with error_sum %lu\n", 
		error_sum);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_EXITING_THREAD);
	return(error_sum);
}

//
//// convert a_serv
//UCHAR GET_channel(C_servoMotor* a_serv){
//
//}
/*
LONGLONG CONVERT_ADCRead(C_servoMotor* a_serv){
	LARGE_INTEGER intervalOne;
	intervalOne.QuadPart = 0;
	int value = GET_ADC(GET_channel(i_serv),DEFAULT_ADC_GAIN);
	//serv[i_serv]
	// min = 500
	intervalOne = 500 + ((DWORD)-min_ADC + intervalOne.QuadPart)
		
	return((LONGLONG)intervalOne);
}
*/

/****************************************************************************
@function	CLOSE_handleAndExitThread
@brief		
@param[in]	
@param[out]	
@return		
***************/
void CLOSE_handleAndExitThread(HANDLE handle, DWORD error_sum)
{
	// char array for printing messages
	char textMsg[MAX_MESSAGE_LENGTH];
	error_sum = CLOSE_handleAndReturn(handle,error_sum);
	//printf("Exiting thread with error_sum %lu\n", error_sum);
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Exiting thread with error_sum %lu\n", error_sum);
	logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
	ExitThread(error_sum);
}


/****************************************************************************
@function	TERMINATE_allThreadsAndExitProcess
@brief
@param[in]
@param[out]
@return
***************/
void TERMINATE_allThreadsAndExitProcess(HANDLE *hTh, int iTh_max, DWORD error_sum)
{
	char textMsg[MAX_MESSAGE_LENGTH];
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Starting to terminate all threads with error_sum %lu\n", error_sum);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_EXITING_PROCESS);

	for(int iTh = 0; iTh<iTh_max; iTh++)
	{
		if(FALSE == TerminateThread(hTh[iTh], EXITCODE_TERMINATED_BY_MAIN))
		{
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Thread[%i] cannot be terminated\n", iTh);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_EXITING_PROCESS);
			error_sum += ERROR_COULD_NOT_TERMINATE_THREAD<<iTh;
		}
		CLOSE_handleAndReturn(hTh[iTh], error_sum);
	}

	EXIT_process(ERROR_COULD_NOT_TERMINATE_THREAD_OFFSET	 + error_sum);
}
