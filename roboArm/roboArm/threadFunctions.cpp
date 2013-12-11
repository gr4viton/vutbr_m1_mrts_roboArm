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
	while(logMsg->GetState())
	{
		logMsg->WriteBuffToFile();
		Sleep(10);
	}
	ExitThread(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function   LOAD_actualPhase
@brief      
@param[in]  
@param[out] 
@return     
************/
void LOAD_actualPhase(C_roboticManipulator* a_ROB, LARGE_INTEGER* PWM_period, 
	std::list<C_spatialConfiguration>::iterator * a_actualPhase)
{	
	char textMsg[LENGTH_OF_BUFFER]; // char array for printing messages
	DWORD error_sum = FLAWLESS_EXECUTION;
	
	LARGE_INTEGER intervalZero;		// tics for holding one on defined pin
	intervalZero.QuadPart = 0;
	C_servoMotor* serv = NULL;
	for(int i_serv=0 ; i_serv < SUM_SERVOMOTORS ; i_serv++)
	{
		error_sum = a_ROB->GET_servoMotor(i_serv, &serv);
		if(error_sum != FLAWLESS_EXECUTION)
		{
			sprintf_s(textMsg, LENGTH_OF_BUFFER, "Could not get servoMotor[%i] pointer\n", i_serv);
			logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
			//printf("Terminating thread with error_sum %lu\n", error_sum);
			sprintf_s(textMsg, LENGTH_OF_BUFFER, "Terminating thread with error_sum %lu\n", error_sum);
			logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
			ExitThread(error_sum);
		}
		// count the [zero interval] from [pwm period - one interval]
		intervalZero.QuadPart = PWM_period->QuadPart - (*a_actualPhase)->servIntervalOne[i_serv].QuadPart;
		// write it to actual
		serv->SET_intervalZero( intervalZero );
	}
}

/****************************************************************************
@function		PWM_dutyCycle
@brief			Function of thread writing into the DO register 
				in the main loop there are tic waitings
				if the number of them is same as PWM_period -> new period starts
				-> every new period zeros are written on all ports
				-> when the number of tics is the same as intervalZero of serv[x]
				---> one is written to its bit in register
				Servo motors have their position regulated by pulses of different width.
@param[in]		void *a_struct
				- i will not be needed
***************/
void RTFCNDCL TIM_PWMfunction(void *a_manip)
{
	char textMsg[LENGTH_OF_BUFFER]; // char array for printing messages
	//____________________________________________________
	// time measurement
	LARGE_INTEGER tim1; tim1.QuadPart = 0;
	LARGE_INTEGER tim2; tim2.QuadPart = 0;
	RtGetClockTime(CLOCK_X,&tim1);

	//____________________________________________________
	// PWM tics creation
	LARGE_INTEGER PWM_period;	// how often to write position - restart tic
	LARGE_INTEGER tic;			// iterating variable
	LARGE_INTEGER tic_interval;	// how long does one tic take
	LARGE_INTEGER tic_phase;		// counting phase time

	tic.QuadPart = 0;
	tic_phase.QuadPart = 0;
	PWM_period.QuadPart = DEFAULT_PWM_PERIOD;
	RtGetClockTimerPeriod(CLOCK_X, &tic_interval);	// time to wait between individual tics
	
	int i_serv = 0;
	C_roboticManipulator* ROB = (C_roboticManipulator*)a_manip;
	C_servoMotor* serv = NULL;

	// here there will be some mutexed variable for control of this thread termination ??
	bool ticDone = false;
	bool phaseDone = false;
	DWORD error_sum = 0;
	//____________________________________________________
	// main thread loop
	ROB->RESET_DOport();
	std::list<C_spatialConfiguration>::iterator actPhase = ROB->phases.begin();
	while(!phaseDone)
	{
		try{
			if(actPhase != ROB->phases.end())
			{ //	 if iterator is not past-the-end element in the list container
				// load next phase
				printf("Load phase[%i] values\n", actPhase->i_phase);
			
					LOAD_actualPhase(ROB,&PWM_period,&actPhase);
			}
			while(!ticDone)	// tics loop
			{
				// ask each servo if this tic the interval zero has passed = time [to write 1]
				for(i_serv=0; i_serv<SUM_SERVOMOTORS; i_serv++)
				{
					// get the pointer of ROB->serv[i_serv] into serv
					error_sum = ROB->GET_servoMotor(i_serv, &serv);
					if(error_sum != FLAWLESS_EXECUTION)
					{
						serv = NULL;
						ROB = NULL;
						//printf("Could not get servoMotor[%i] pointer\n", i_serv);
						sprintf_s(textMsg, LENGTH_OF_BUFFER, "Could not get servoMotor[%i] pointer\n", i_serv);
						logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
						//printf("Terminating thread with error_sum %lu\n", error_sum);
						sprintf_s(textMsg, LENGTH_OF_BUFFER, "Terminating thread with error_sum %lu\n", error_sum);
						logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
						ExitThread(error_sum);
					}
					if(tic.QuadPart >= serv->intervalZero.QuadPart)
					{ // time for writing 1 has come
						// write to the right digit
						ROB->SET_DOportBitUchar(serv->servoMotorDigit);
					}
				}
				// ____________________________________________________
				// iteration & tic-waiting
				RtSleepFt(&tic_interval);
				//RtPrintf("tic=%I64d/%I64d\n",tic,DEFAULT_PWM_PERIOD);
				tic.QuadPart += tic_interval.QuadPart;
				tic_phase.QuadPart += tic_interval.QuadPart;
				if(tic_phase.QuadPart >= actPhase->phaseInterval.QuadPart)
				{
					tic.QuadPart = 0;
					tic_phase.QuadPart = 0;
					break;
				}
				// ____________________________________________________
				// end of each period
				if(tic.QuadPart >= PWM_period.QuadPart)
				{ // end of one period
					ROB->RESET_DOport();
					tic.QuadPart = 0;
		
					RtGetClockTime(CLOCK_X,&tim2);
					tim2.QuadPart = tim2.QuadPart-tim1.QuadPart;
					//printf("PWM_period = %I64d [100ns] = %I64d [1s]  \n", tim2.QuadPart, tim2.QuadPart / NS100_1S);
					sprintf_s(textMsg, LENGTH_OF_BUFFER, "End of one period - PWM_period = %I64d [100ns] = %I64d [1s]  \n", tim2.QuadPart, tim2.QuadPart / NS100_1S);
					logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
					RtGetClockTime(CLOCK_X,&tim1);
				// stop after first period (for debug - to terminate threads)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
					//done = true;
				}
			}//tic loop

			actPhase++;
			if(actPhase == ROB->phases.end())
			{
				//actPhase--;
				phaseDone = true;
			}

		}
		catch (std::exception & e) {
			printf("ERR-exception:\n%s\n", e.what());
			ExitThread(10000);
		}

	}// phase loop
	ROB = NULL;
	ExitThread(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function	CLOSE_handleAndExitThread
@brief		
@param[in]	
@param[out]	
@return		
***************/
void CLOSE_handleAndExitThread(HANDLE handle, DWORD error_sum)
{
	//PUSHMSG_SEVERITY_HIGH
	// char array for printing messages
	char textMsg[LENGTH_OF_BUFFER];
	error_sum = CLOSE_handleAndReturn(handle,error_sum);
	//printf("Exiting thread with error_sum %lu\n", error_sum);
	sprintf_s(textMsg, LENGTH_OF_BUFFER, "Exiting thread with error_sum %lu\n", error_sum);
	logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);
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
	char textMsg[LENGTH_OF_BUFFER];
	sprintf_s(textMsg, LENGTH_OF_BUFFER, "Starting to terminate all threads with error_sum %lu\n", error_sum);
	logMsg->PushMessage(textMsg, PUSHMSG_SEVERITY_NORMAL);

	for(int iTh = 0; iTh<iTh_max; iTh++){
		if(FALSE == TerminateThread(hTh[iTh], EXITCODE_TERMINATED_BY_MAIN)){
			error_sum += ERROR_COULD_NOT_TERMINATE_THREAD;
		}
		CLOSE_handleAndExitThread(hTh[iTh],error_sum);
	}
}
