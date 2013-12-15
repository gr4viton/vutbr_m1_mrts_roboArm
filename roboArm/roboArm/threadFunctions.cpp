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
void LOAD_actualPhase(C_roboticManipulator* a_ROB, 
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
		intervalZero.QuadPart = a_ROB->PWMperiod_interval.QuadPart - (*a_actualPhase)->serv_intervalOne[i_serv].QuadPart;
		// DEBUG
		//intervalZero.QuadPart = PWMperiod_interval->QuadPart - 1750 * NS100_1US;
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
				in the main loop there are PWMtic waitings
				if the number of them is same as PWMperiod_interval -> new period starts
				-> every new period zeros are written on all ports
				-> when the number of tics is the same as intervalZero of serv[x]
				---> one is written to its bit in register
				Servo motors have their position regulated by pulses of different width.
@param[in]		void *a_struct
				- i will not be needed
***************/
void RTFCNDCL TIM_PWMfunction(void *a_ROB)
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

	
	printf("RtGetClockTimerPeriod = %I64d\n", PWMtic_interval);


	// here there will be some mutexed variable for control of this thread termination ??
	bool generateTics = true;
	bool allPhasesEnded = false;
//	DWORD error_sum = 0;
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// main thread loop
	ROB->RESET_DOport();

	
	LONGLONG PWMperiod_sum_max =0;
	LARGE_INTEGER lastIntervalOne;
	lastIntervalOne.QuadPart = 0;
	DWORD PWMperiod_sum = 0; // counter of periods
#ifdef DEBUG
	DWORD PWMperiod_sum_last = 0;
#endif
	while(!allPhasesEnded)
	{
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// LOAD this phase
		if(ROB->phase_act != ROB->phases.end())
		{ //	 if iterator is not past-the-end element in the list container
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Load phase[%i/%i] values\n", ROB->phase_act->i_phase, ROB->phase_act->i_phase_max);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PHASE);
			LOAD_actualPhase(ROB, &ROB->phase_act);
		}
		
		if(ROB->phase_act != ROB->phases.begin())
		{
			ROB->phase_prev = ROB->phase_act;
			ROB->phase_prev--;
		}

		PWMperiod_sum_max = ROB->phase_act->phaseInterval.QuadPart / ROB->PWMperiod_interval.QuadPart ;

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// tics loop 
		while(generateTics)	
		{
			//____________________________________________________
			// ask each servo if this PWMtic the interval zero has passed = time [to write 1]
			for(i_serv=0; i_serv<SUM_SERVOMOTORS; i_serv++)
			{ // iterate through all servos

				// get the pointer of ROB->serv[i_serv] into serv
				ROB->GET_servoMotor(i_serv, &serv);

				if(PWMtic_sum.QuadPart >= serv->intervalZero.QuadPart)
				{ // time for writing 1 has come
					
					// serv, ROB, 
					if(ROB->phase_act->serv_fixedPositioning)
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
							if(ROB->phase_act != ROB->phases.begin())
							{ // this is not a first phase - we have previous phase
								if( ROB->phase_act->serv_intervalOne[i_serv].QuadPart 
									<= ROB->phase_prev->serv_intervalOne[i_serv].QuadPart)
								{ // intOne counting up
								// seky
									//functional
									intervalOneDif.QuadPart = 
										ROB->phase_act->serv_intervalOne[i_serv].QuadPart 
										- ROB->phase_prev->serv_intervalOne[i_serv].QuadPart;
								}
								else
								{ // intOne counting down
									intervalOneDif.QuadPart = 
										ROB->phase_prev->serv_intervalOne[i_serv].QuadPart 
										- ROB->phase_act->serv_intervalOne[i_serv].QuadPart;
								}
							} // end - this is not a first phase - we have previous phase
						} // end - we do not have a feedback
						else
						{ // we have a feedback

						} // end - we have a feedback

						LARGE_INTEGER actIntervalOne;
						// evaluate new value of angle
						if( ROB->phase_act->serv_intervalOne[i_serv].QuadPart 
							<= ROB->phase_prev->serv_intervalOne[i_serv].QuadPart)
						{ // intOne counting up
								//functional
							actIntervalOne.QuadPart = ROB->phase_prev->serv_intervalOne[i_serv].QuadPart 
								+ LONGLONG((   ((double)PWMperiod_sum) * ((double)intervalOneDif.QuadPart)  ) / ( (double)PWMperiod_sum_max) );
						}
						else
						{ // intOne counting down
							// seky
							actIntervalOne.QuadPart = ROB->phase_prev->serv_intervalOne[i_serv].QuadPart 
								- LONGLONG((   ((double)PWMperiod_sum) * ((double)intervalOneDif.QuadPart)  ) / ( (double)PWMperiod_sum_max) );
						}
				
						//____________________________________________________
						// the time for writing 1 has really come - with linear positioning
						if(PWMtic_sum.QuadPart >= (ROB->PWMperiod_interval.QuadPart - actIntervalOne.QuadPart))
						{
							// write one to this servo bit
							ROB->SET_DOportBitUchar(serv->servoMotorDigit);
						}
#ifdef DEBUG // debuging breakpoint 
						if( PWMperiod_sum != PWMperiod_sum_last){PWMperiod_sum_last = PWMperiod_sum;}
#endif
					} // ramp - linear position change
				} // end - time for writing 1 has come
			} // end - iterate through all servos
			//____________________________________________________
			// write new DO port value from this period
			ROB->WRITE_DOport_thisPeriodNewValue();
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// iteration & PWMtic-waiting
			RtSleepFt(&PWMtic_interval);

			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "PWMtic_sum=%I64d/%I64d\n", PWMtic_sum, DEFAULT_PWM_PERIOD);
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
				PWMperiod_sum++;
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
			ROB->phase_act++;
			PWMperiod_sum = 0;
			if(ROB->phase_act == ROB->phases.end())
			{
				logMsg.PushMessage("All phases are done!\n", LOG_SEVERITY_PWM_PHASE);
				allPhasesEnded = true;
				break;
			}
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Continuing with next phase[%i/%i].\n", ROB->phase_act->i_phase, ROB->phase_act->i_phase_max);
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
