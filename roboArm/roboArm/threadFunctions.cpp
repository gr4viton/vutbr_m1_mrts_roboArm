#include "roboArm.h"


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
	// robotic manipulator and servo pointers
	int i_serv = 0;
	C_roboticManipulator* ROB = (C_roboticManipulator*)a_ROB; 
	C_servoMotor* serv = NULL;


	//____________________________________________________
	// time measurement
	LARGE_INTEGER tim1; tim1.QuadPart = 0;
	LARGE_INTEGER tim2; tim2.QuadPart = 0;
	RtGetClockTime(CLOCK_X,&tim1);

	//____________________________________________________
	// PWM tics creation
	//LARGE_INTEGER PWMperiod_interval;// one period of PWM - how often to rewrite DO port
	LARGE_INTEGER PWMtic_sum;		// iterating variable
	LARGE_INTEGER phaseTic_sum;		// counting phase time
	
	PWMtic_sum.QuadPart = 0;
	phaseTic_sum.QuadPart = 0;
	//ROB->PWMperiod_interval.QuadPart = DEFAULT_PWM_PERIOD; //-- in constructor
	
	LARGE_INTEGER PWMtic_interval;	// how long does one PWMtic take
	RtGetClockTimerPeriod(CLOCK_X, &PWMtic_interval);	// time to wait between individual PWMtics

	
	printf("RtGetClockTimerPeriod = %I64d [100ns]\n", PWMtic_interval);


	// here there will be some mutexed variable for control of this thread termination ??
	bool generateTics = true;
	bool allPhasesEnded = false;
	DWORD error_sum = 0;
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main PWMthread loop
	ROB->RESET_DOport();

	ROB->PWMperiod_sum_max = 0;
	LARGE_INTEGER lastIntervalOne;
	lastIntervalOne.QuadPart = 0;

	ROB->PWMperiod_sum = 0; // counter of periods
#ifdef DEBUG
	LONGLONG PWMperiod_sum_last = 0;
#endif
	while(!allPhasesEnded)
	{
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// LOAD this phase
		error_sum = ROB->LOAD_actualPhase();
		if(error_sum != FLAWLESS_EXECUTION) ExitThread(EXIT_threadPWM(error_sum, &ROB));

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// tics loop 
		while(generateTics)	
		{
			//____________________________________________________
			// calculate newDO port...
			ROB->CALC_DOport_thisPeriodNewValue();
			//____________________________________________________
			// write new DO port value from this period
			ROB->WRITE_DOport_thisPeriodNewValue();
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// iteration & PWMtic-waiting
			RtSleepFt(&PWMtic_interval);

			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "PWMtic_sum=%I64d/%I64d\n", PWMtic_sum, ROB->PWMperiod_interval);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_TIC);

			PWMtic_sum.QuadPart += PWMtic_interval.QuadPart;
			phaseTic_sum.QuadPart += PWMtic_interval.QuadPart;
			//____________________________________________________
			// End of phase = EXIT PERIOD-PWM LOOP 
			if(phaseTic_sum.QuadPart >= ROB->phase_act->phaseInterval.QuadPart)
			{
				PWMtic_sum.QuadPart = 0;
				phaseTic_sum.QuadPart = 0;
				break;
			}
			// ____________________________________________________
			// end of each period = PWM END
			if(PWMtic_sum.QuadPart >= ROB->PWMperiod_interval.QuadPart)
			{
				ROB->PWMperiod_sum++;
				sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "period ended - phase[%i/%i] - PWMperiod_interval = %I64d [100ns] = %I64d [1s]\n", 
					ROB->phase_act->i_phase, ROB->phase_act->i_phase_max,
					tim2.QuadPart, tim2.QuadPart / NS100_1S);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PERIOD);

				ROB->RESET_DOport();
				PWMtic_sum.QuadPart = 0;

				/*
				// READ ADC VALUES
				for(int i_serv = 0; i_serv<
					GET_ADC(UCHAR channel, UCHAR gain)
					*/

				RtGetClockTime(CLOCK_X, &tim2);
				tim2.QuadPart = tim2.QuadPart-tim1.QuadPart;
				//printf("PWMperiod_interval = %I64d [100ns] = %I64d [1s]  \n", tim2.QuadPart, tim2.QuadPart / NS100_1S);
				RtGetClockTime(CLOCK_X, &tim1);
			}
		} //PWMtic loop == PHASE END

		//____________________________________________________
		{ // iteration to next phase
			if(ROB->SET_NextPhase() == ERROR_CANNOT_SET_NEXTPHASE)
			{
			logMsg.PushMessage("All phases are done!\n", LOG_SEVERITY_PWM_PHASE);
			allPhasesEnded = true;
			break;
			}
#ifdef DEBUGGING_WITHOUT_HW
			logMsg.PushMessage("DEBUGING_WITHOUT_HW - not writing to any register!", LOG_SEVERITY_PWM_PHASE);
#endif
		} 
	} // end - allPhasesEnded - phase loop END

	ExitThread(EXIT_threadPWM(FLAWLESS_EXECUTION, &ROB));
}

/****************************************************************************
@function   EXIT_threadPWM
@brief      unitializises threadPWM variables
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
	logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);

	for(int iTh = 0; iTh<iTh_max; iTh++){
		if(FALSE == TerminateThread(hTh[iTh], EXITCODE_TERMINATED_BY_MAIN)){
			error_sum += ERROR_COULD_NOT_TERMINATE_THREAD;
		}
		CLOSE_handleAndExitThread(hTh[iTh],error_sum);
	}
}
