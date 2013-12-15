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
@function   LOAD_actualPhase
@brief      
@param[in]  
@param[out] 
@return     
************/
void LOAD_actualPhase(C_roboticManipulator* a_ROB, LARGE_INTEGER* PWM_period, 
	std::list<C_spatialConfiguration>::iterator * a_actualPhase)
{	
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
	DWORD error_sum = FLAWLESS_EXECUTION;
	
	LARGE_INTEGER intervalZero;		// tics for holding one on defined pin
	intervalZero.QuadPart = 0;
	C_servoMotor* serv = NULL;
	for(int i_serv=0 ; i_serv < SUM_SERVOMOTORS ; i_serv++)
	{
		error_sum = a_ROB->GET_servoMotor(i_serv, &serv);
		if(error_sum != FLAWLESS_EXECUTION)
		{
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Could not get servoMotor[%i] pointer\n", i_serv);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Terminating thread with error_sum %lu\n", error_sum);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
			ExitThread(error_sum);
		}
		// count the [zero interval] from [pwm period - one interval]
		intervalZero.QuadPart = PWM_period->QuadPart - (*a_actualPhase)->serv_intervalOne[i_serv].QuadPart;
		// DEBUG
		//intervalZero.QuadPart = PWM_period->QuadPart - 1750 * NS100_1US;
		// write it to actual
		serv->SET_intervalZero( intervalZero );
	}

	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Actual phase[%i/%i] interval = %I64d[ms]\n", 
		(*a_actualPhase)->i_phase, (*a_actualPhase)->i_phase_max, 
		(*a_actualPhase)->phaseInterval.QuadPart / NS100_1MS );
	logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
}

/****************************************************************************
@function		TIM_PWMfunction
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
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
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
	
	printf("RtGetClockTimerPeriod = %I64d\n", tic_interval);

	int i_serv = 0;
	C_roboticManipulator* ROB = (C_roboticManipulator*)a_manip;
	C_servoMotor* serv = NULL;

	// here there will be some mutexed variable for control of this thread termination ??
	bool generateTics = true;
	bool allPhasesEnded = false;
	DWORD error_sum = 0;
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main thread loop
	ROB->RESET_DOport();
	std::list<C_spatialConfiguration>::iterator actPhase = ROB->phases.begin();
	std::list<C_spatialConfiguration>::iterator actPhasePrev = ROB->phases.begin();

	
	LARGE_INTEGER lastIntervalOne;
	lastIntervalOne.QuadPart = 0;
	DWORD nPWM = 0; // counter of periods
	DWORD nPWM_last = 0;
	while(!allPhasesEnded)
	{
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// LOAD this phase
		if(actPhase != ROB->phases.end())
		{ //	 if iterator is not past-the-end element in the list container
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Load phase[%i/%i] values\n", actPhase->i_phase, actPhase->i_phase_max);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PHASE);
			LOAD_actualPhase(ROB,&PWM_period, &actPhase);
		}

		
		LONGLONG nKrok = actPhase->phaseInterval.QuadPart/PWM_period.QuadPart ;
		if(actPhase != ROB->phases.begin())
		{
			actPhasePrev = actPhase;
			actPhasePrev--;
		}
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// tics loop 
		while(generateTics)	
		{
			//____________________________________________________
			// ask each servo if this tic the interval zero has passed = time [to write 1]
			for(i_serv=0; i_serv<SUM_SERVOMOTORS; i_serv++)
			{ // iterate through all servos

				// get the pointer of ROB->serv[i_serv] into serv
				ROB->GET_servoMotor(i_serv, &serv);

				if(tic.QuadPart >= serv->intervalZero.QuadPart)
				{ // time for writing 1 has come
					
					// serv, ROB, 
					if(actPhase->serv_fixedPositioning)
					{ // not ramp - square position change
						// write one to this servo bit
						ROB->SET_DOportBitUchar(serv->servoMotorDigit);
					}
					else
					{ // ramp - linear position change
						LARGE_INTEGER intervalOneDif;
						// Get last and actual interval one (angle) differention
						if(serv->ADC_feedBack == false)
						{ // we do not have a feedback
							if(actPhase != ROB->phases.begin())
							{ // this is not a first phase - we have previous phase
								if( actPhase->serv_intervalOne[i_serv].QuadPart 
									<= actPhasePrev->serv_intervalOne[i_serv].QuadPart)
								{ // intOne counting up
								// seky
									//functional
									intervalOneDif.QuadPart = 
										actPhase->serv_intervalOne[i_serv].QuadPart 
										- actPhasePrev->serv_intervalOne[i_serv].QuadPart;
								}
								else
								{ // intOne counting down
									intervalOneDif.QuadPart = 
										actPhasePrev->serv_intervalOne[i_serv].QuadPart 
										- actPhase->serv_intervalOne[i_serv].QuadPart;
								}
							} // end - this is not a first phase - we have previous phase
						} // end - we do not have a feedback
						else
						{ // we have a feedback

						} // end - we have a feedback

						LARGE_INTEGER actIntervalOne;
						// evaluate new value of angle
						if( actPhase->serv_intervalOne[i_serv].QuadPart 
							<= actPhasePrev->serv_intervalOne[i_serv].QuadPart)
						{ // intOne counting up
								//functional
							actIntervalOne.QuadPart = actPhasePrev->serv_intervalOne[i_serv].QuadPart 
								+ LONGLONG((   ((double)nPWM) * ((double)intervalOneDif.QuadPart)  ) / ( (double)nKrok) );
						}
						else
						{ // intOne counting down
							// seky
							actIntervalOne.QuadPart = actPhasePrev->serv_intervalOne[i_serv].QuadPart 
								- LONGLONG((   ((double)nPWM) * ((double)intervalOneDif.QuadPart)  ) / ( (double)nKrok) );
						}
				
						//____________________________________________________
						// the time for writing 1 has really come - with linear positioning
						if(tic.QuadPart >= (PWM_period.QuadPart - actIntervalOne.QuadPart))
						{
							// write one to this servo bit
							ROB->SET_DOportBitUchar(serv->servoMotorDigit);
						}
						if( nPWM != nPWM_last){
							// ONLY for debugging
							nPWM_last = nPWM;
						}
					} // ramp - linear position change
				} // end - time for writing 1 has come
			} // end - iterate through all servos
			//____________________________________________________
			// write new DO port value from this period
			ROB->WRITE_DOport_thisPeriodNewValue();
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// iteration & tic-waiting
			RtSleepFt(&tic_interval);

			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "tic=%I64d/%I64d\n", tic, DEFAULT_PWM_PERIOD);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_TIC);

			tic.QuadPart += tic_interval.QuadPart;
			tic_phase.QuadPart += tic_interval.QuadPart;
			//____________________________________________________
			// End of phase = EXIT PERIOD-PWM LOOP 
			if(tic_phase.QuadPart >= actPhase->phaseInterval.QuadPart)
			{
				tic.QuadPart = 0;
				tic_phase.QuadPart = 0;
				break;
			}
			// ____________________________________________________
			// end of each period = PWM END
			if(tic.QuadPart >= PWM_period.QuadPart)
			{
				nPWM++;
				sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "period ended - phase[%i/%i] - PWM_period = %I64d [100ns] = %I64d [1s]\n", 
					actPhase->i_phase, actPhase->i_phase_max,
					tim2.QuadPart, tim2.QuadPart / NS100_1S);
				logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PERIOD);

				ROB->RESET_DOport();
				tic.QuadPart = 0;

				/*
				// READ ADC VALUES
				for(int i_serv = 0; i_serv<
					GET_ADC(UCHAR channel, UCHAR gain)
					*/

				RtGetClockTime(CLOCK_X, &tim2);
				tim2.QuadPart = tim2.QuadPart-tim1.QuadPart;
				//printf("PWM_period = %I64d [100ns] = %I64d [1s]  \n", tim2.QuadPart, tim2.QuadPart / NS100_1S);
				RtGetClockTime(CLOCK_X, &tim1);
			}
		} //tic loop == PHASE END

		//____________________________________________________
		{ // iteration to next phase
			actPhase++;
			nPWM = 0;
			if(actPhase == ROB->phases.end())
			{
				logMsg.PushMessage("All phases are done!\n", LOG_SEVERITY_PWM_PHASE);
				allPhasesEnded = true;
				break;
			}
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Continuing with next phase[%i/%i].\n", actPhase->i_phase, actPhase->i_phase_max);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PHASE);
#ifdef DEBUGGING_WITHOUT_HW
			logMsg.PushMessage("DEBUGING_WITHOUT_HW - not writing to any register!", LOG_SEVERITY_PWM_PHASE);
#endif
		} 
	} // end - allPhasesEnded - phase loop END

	ROB = NULL;	
	logMsg.PushMessage("Exitting thread PWM\n", LOG_SEVERITY_EXITING_THREAD);
	ExitThread(FLAWLESS_EXECUTION);
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
