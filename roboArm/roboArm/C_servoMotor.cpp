/***************
* @filename		C_servoMotor.cpp
* @author 		xdavid10, xslizj00 @ FEEC-VUTBR 
* @date			2013_12_02
* @brief			file containing member function definitions of class C_servoMotor
***************/

#include "roboArm.h"
//#include "C_servoMotor.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// constructor

/****************************************************************************
* @function		
* @brief			
				Servo motors have their position regulated by pulses of different width.
* @param[in]		
***************/
C_servoMotor::C_servoMotor(void)
	:min_val(0),max_val(255),mean_vals(1),servo_index(-1) //, ...
{return;}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// SET

/****************************************************************************
* @function		
* @brief			
				Servo motors have their position regulated by pulses of different width.
* @param[in]		
***************/
int C_servoMotor::SET_constants(
	int a_servo_index,
	bool a_addressHighByte,
	UCHAR servoMotorBit,
	bool a_FDBACK,
	DWORD a_min_val, DWORD a_max_val, DWORD a_mean_vals
	)
{
	interval_one.QuadPart = 0;
	interval_zero.QuadPart = 0;
	// stop the PWM function and timers!!


	addressHighByte = a_addressHighByte,
	FDBACK = a_FDBACK;
	min_val = a_min_val;
	max_val = a_max_val;
	mean_vals = a_mean_vals;
	servo_index = a_servo_index;


	if(addressHighByte == SERVOMOTOR_ADDRESS_HIGHBYTE)
		servoMotorByteAddress = (PUCHAR)(baseAddress + DO_High_Byte);
	else
		servoMotorByteAddress = (PUCHAR)(baseAddress + DO_Low_Byte);

	return(servo_index);
}

/****************************************************************************
* @function		
* @brief			
				Servo motors have their position regulated by pulses of different width.
* @param[in]		
***************/
int C_servoMotor::SET_dutyCycleIntervals(LARGE_INTEGER a_interval_one, LARGE_INTEGER a_interval_zero)
{
	interval_one.QuadPart = a_interval_one.QuadPart;
	interval_zero.QuadPart = a_interval_zero.QuadPart;
	return(FLAWLESS_EXECUTION);
}
	
// ____________________________________________________
// GET_ADC..

/****************************************************************************
* @function		PWM_dutyCycle
* @brief			periodically executed function for PWM on predefined DO port .
				Servo motors have their position regulated by pulses of different width.
* @param[in]		void *a_struct
				- i will not be needed
***************/
void RTFCNDCL C_servoMotor::PWM_dutyCycle(void *a_struct)
{
	// writing to a critical section should be treated wisely ! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// is the register critical section? I think yess
	// mutex for individual bytes / bites of shadow-register
	// and one core thread reading shadow-register and writing it to the true byteAdress 
	// --- possibly event driven ---

	// so RtWritePortUchar -> will become something like WriteIntentionBit which will handle the mutexes right
	RtWritePortUchar(servoMotorByteAddress, 1<<servoMotorDigit);
	RtSleepFt(&interval_one);
	// You cannot write zeros everywhere
	RtWritePortUchar((PUCHAR)(baseAddress+DO_Low_Byte), 0x00);
	RtSleepFt(&interval_zero);
}




/****************************************************************************
* @function		CREATE_timer
* @brief			
				Servo motors have their position regulated by pulses of different width.
* @param[in]		
***************/
int C_servoMotor::CREATE_timer(void){
			
	LARGE_INTEGER min_tim_period;

	LARGE_INTEGER expir_interval; // interval before first calling of routine
	LARGE_INTEGER periodic_interval; // interval between routines

	periodic_interval.QuadPart = NS100_50HZ;
	expir_interval.QuadPart = NS100_1US;
		
	// periodic_interval must be dividable by a min_tim_period without any "rest"
	RtGetClockTimerPeriod(CLOCK_X, &min_tim_period);
	if ( !(periodic_interval.QuadPart % min_tim_period.QuadPart) ) 
	{
		// periodic interval is not dividable by a min_tim_period
		// log it and exit
		return(DESTRUCT_EVERYTHING); 
	}
	periodic_input = 0;
	hTimer = NULL;
	
	// !IT IS! possible to point at a class member function in RtCreateTimer, but how?
	/*
	hTimer = RtCreateTimer( NULL, 0, PWM_dutyCycle, (PVOID)&periodic_input, 63, CLOCK_X );
	//hTimer = RtCreateTimer( NULL, 0, static_cast<VOID>(PWM_dutyCycle), (PVOID)&periodic_input, 63, CLOCK_X );
	
	if(hTimer == NULL)
		return(C_SERVOMOTOR_TIMER_INVALID_HANDLE);
	// starts the timer
	if(!RtSetTimerRelative( hTimer, &expir_interval, &periodic_interval ) )
	{
		return(C_SERVOMOTOR_SETTIMERREL_ERROR);
	}
	*/

	return(FLAWLESS_EXECUTION);
}
	
/****************************************************************************
* @function		~C_servoMotor - destructor
* @brief			
				Servo motors have their position regulated by pulses of different width.
* @param[in]		-
***************/
C_servoMotor::~C_servoMotor(void)
{
	if(!RtDeleteTimer(hTimer))
	{
		//log
		return;
	}
	if(!RtCloseHandle(hTimer))
	{
		//log
		return;
	}
	// log
}
