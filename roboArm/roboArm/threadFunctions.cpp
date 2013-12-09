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
@param[in]		void *a_struct - not used
***************/
void RTFCNDCL LogMessageThread(void *a_manip)
{
	// logging - do NOT modify
	while(logMsg->GetState())
	{
		logMsg->WriteBuffToFile();
		Sleep(10);
	}

	ExitThread(0);
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
	// char array for printing messages
	char textMsg[LENGTH_OF_BUFFER];
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
	LARGE_INTEGER tic_phase;	// counting phase time
	LARGE_INTEGER intervalOne;	// counting phase time

	intervalOne.QuadPart = 0;
	tic.QuadPart = 0;
	tic_phase.QuadPart = 0;
	//PWM_period.QuadPart = NS100_1S / 100; // 1/100 s = 100 Hz
	PWM_period.QuadPart = NS100_1S / 1;				// 1/1 s = 1 Hz
	RtGetClockTimerPeriod(CLOCK_X, &tic_interval);	// time to wait between individual tics
	
	int i_serv = 0;
	C_roboticManipulator* ROB = (C_roboticManipulator*)a_manip;
	C_servoMotor* serv = NULL;

	// here there will be some mutexed variable for control of this thread termination ??
	bool ticDone = false;
	bool phaseDone = false;
	int error_sum = 0;
	//____________________________________________________
	// main thread loop
	ROB->RESET_DOport();
	std::list<C_spatialConfiguration>::iterator it =ROB->phases.begin();
	while(phaseDone)
	{
		if(it != ROB->phases.end())
		{			
			// read phase
			for(int i_serv=0 ; i_serv < SUM_SERVOMOTORS ; i_serv++)
			{
				error_sum = ROB->GET_servoMotor(i_serv, &serv);
				if(error_sum != FLAWLESS_EXECUTION)
				{
					printf_s(textMsg, LENGTH_OF_BUFFER, "Could not get servoMotor[%i] pointer\n", i_serv);
					logMsg->PushMessage(textMsg, SEVERITY_MAX - 1);
					//printf("Terminating thread with error_sum %i\n", error_sum);
					printf_s(textMsg, LENGTH_OF_BUFFER, "Terminating thread with error_sum %i\n", error_sum);
					logMsg->PushMessage(textMsg, SEVERITY_MAX - 1);
					ExitThread(error_sum);
				}
				intervalOne.QuadPart = PWM_period.QuadPart - it->servIntervalZero[i_serv].QuadPart;
				serv->SET_intervalZero( intervalOne );
			}
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
					printf_s(textMsg, LENGTH_OF_BUFFER, "Could not get servoMotor[%i] pointer\n", i_serv);
					logMsg->PushMessage(textMsg, SEVERITY_MAX - 1);
					//printf("Terminating thread with error_sum %i\n", error_sum);
					printf_s(textMsg, LENGTH_OF_BUFFER, "Terminating thread with error_sum %i\n", error_sum);
					logMsg->PushMessage(textMsg, SEVERITY_MAX - 1);
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
			//RtPrintf("tic=%I64d/%I64d\n",tic,PWM_period);
			tic.QuadPart += tic_interval.QuadPart;
			tic_phase.QuadPart += tic_interval.QuadPart;
			if(tic_phase.QuadPart >= it->phaseInterval.QuadPart)
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
				printf_s(textMsg, LENGTH_OF_BUFFER, "PWM_period = %I64d [100ns] = %I64d [1s]  \n", tim2.QuadPart, tim2.QuadPart / NS100_1S);
				logMsg->PushMessage(textMsg, SEVERITY_MAX - 1);
				RtGetClockTime(CLOCK_X,&tim1);
			// stop after first period (for debug - to terminate threads)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
				//done = true;
			}
		}//tic loop
		if(it != ROB->phases.end()) it++;
	}// phase loop
	ROB = NULL;
}


/****************************************************************************
@function	CLOSE_handleAndExitThread
@brief		
@param[in]	
@param[out]	
@return		
***************/
void CLOSE_handleAndExitThread(HANDLE handle, int error_sum)
{
	// char array for printing messages
	char textMsg[LENGTH_OF_BUFFER];
	error_sum = CLOSE_handleAndReturn(handle,error_sum);
	//printf("Exiting thread with error_sum %8i\n", error_sum);
	printf_s(textMsg, LENGTH_OF_BUFFER, "Exiting thread with error_sum %8i\n", error_sum);
	logMsg->PushMessage(textMsg, SEVERITY_MAX - 1);
	ExitThread(error_sum);
}


/****************************************************************************
@function	TERMINATE_allThreadsAndExitProcess
@brief
@param[in]
@param[out]
@return
***************/
void TERMINATE_allThreadsAndExitProcess(HANDLE *hTh, int iTh_max, int error_sum)
{
	for(int iTh = 0; iTh<iTh_max; iTh++){
		if(FALSE == TerminateThread(hTh[iTh], EXITCODE_TERMINATED_BY_MAIN)){
			error_sum += ERROR_COULD_NOT_TERMINATE_THREAD;
		}
		CLOSE_handleAndExitThread(hTh[iTh],error_sum);
	}
}
