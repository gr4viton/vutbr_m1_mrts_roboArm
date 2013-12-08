#include "roboArm.h"



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

	tic.QuadPart = 0;
	//PWM_period.QuadPart = NS100_1S / 100; // 1/100 s = 100 Hz
	PWM_period.QuadPart = NS100_1S / 1;				// 1/1 s = 1 Hz
	RtGetClockTimerPeriod(CLOCK_X, &tic_interval);	// time to wait between individual tics
	
	int i_serv = 0;
	C_roboticManipulator* ROB = (C_roboticManipulator*)a_manip;
	C_servoMotor* serv = NULL;

	// here there will be some mutexed variable for control of this thread termination ??
	bool done = false;
	int error_sum = 0;
	//____________________________________________________
	// main thread loop
	ROB->RESET_DOport();
	while(!done)
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
				printf("Could not get servoMotor[%i] pointer\n", i_serv);
				printf("Terminating thread with error_sum %i\n", error_sum);
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
		// ____________________________________________________
		// end of each period
		if(tic.QuadPart >= PWM_period.QuadPart)
		{ // end of one period
			ROB->RESET_DOport();
			tic.QuadPart = 0;
		
			RtGetClockTime(CLOCK_X,&tim2);
			tim2.QuadPart = tim2.QuadPart-tim1.QuadPart;
			printf("PWM_period = %I64d [100ns] = %I64d [1s]  \n", tim2.QuadPart, tim2.QuadPart / NS100_1S);
			RtGetClockTime(CLOCK_X,&tim1);
		// stop after first period (for debug - to terminate threads)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			//done = true;
		}
	}

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
	error_sum = CLOSE_handleAndReturn(handle,error_sum);
	printf("Exiting thread with error_sum %8i\n", error_sum);
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
