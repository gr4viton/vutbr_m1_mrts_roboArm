/***********
@project		roboArm
@filename	C_roboticManipulator.cpp
@author		xdavid10, xslizj00 @ FEEC-VUTBR 
@date		2013_12_02
@brief		file containing member function definitions 
			of classes C_roboticManipulator 
***********/

#include "roboArm.h"
//#include "C_roboticManipulator.h"

/****************************************************************************
@function   DEBUG_fillPhases
@class		C_roboticManipulator
@brief      add some debug phases for testing the servos 
@return     DWORD error_sum
************/
DWORD C_roboticManipulator::DEBUG_fillPhases(void)
{
	// add some phases
	LONGLONG addVal = 0;
	LONGLONG phaseInterval = 1000 * NS100_1MS;
	int i_phase_max = 11;
	int i_serv_min = 5;
	C_spatialConfiguration new_phase;

	new_phase.phaseInterval.QuadPart = phaseInterval;	
	// set initial phase servos position
	
	//____________________________________________________
	// go 0-180°
	for(int i_phase = 0; i_phase < i_phase_max ; i_phase++)
	{ // phases
		for(int i_serv=i_serv_min; i_serv<SUM_SERVOMOTORS; i_serv++)
		{
			new_phase.serv_fixedPositioning[i_serv] = false;
			addVal = serv[i_serv].intervalOne_max.QuadPart 
				- serv[i_serv].intervalOne_min.QuadPart;
			addVal = LONGLONG( double(addVal) / double(i_phase_max) );
			new_phase.serv_intervalOne[i_serv].QuadPart = 
				serv[i_serv].intervalOne_min.QuadPart 
				+ LONGLONG(i_phase) * addVal;
		}
		PUSHBACK_newPhase(&new_phase); 
	}
	//____________________________________________________
	// and back	(180-0°)
	for(int i_phase = 0; i_phase < i_phase_max ; i_phase++)
	{ // phases
		for(int i_serv=i_serv_min; i_serv<SUM_SERVOMOTORS; i_serv++)
		{
			new_phase.serv_fixedPositioning[i_serv] = false;
			addVal = serv[i_serv].intervalOne_max.QuadPart 
				- serv[i_serv].intervalOne_min.QuadPart;
			addVal = LONGLONG( double(addVal) / double(i_phase_max) );
			new_phase.serv_intervalOne[i_serv].QuadPart = 
				serv[i_serv].intervalOne_max.QuadPart 
				- LONGLONG(i_phase) * addVal;
		}
		PUSHBACK_newPhase(&new_phase); 
	}
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function   CALC_DOport_thisPeriodNewValue
@class		C_roboticManipulator
@brief      calculate new value of DO port for this period
************/
void C_roboticManipulator::CALC_DOport_thisPeriodNewValue()
{
	// ask each servo if this PWMtic the interval zero has passed = time [to write 1]
	for(int i_serv=0; i_serv<SUM_SERVOMOTORS; i_serv++)
	{ // iterate through all servos
		if( IS_timeToWriteOne(i_serv) == true)
		{ // time for writing 1 has come
			if( IS_reallyTimeToWriteOne(i_serv) == true)
			{
				// write one to serv[i_serv] bit in thisPeriodNewValue
				SET_DOport_thisPeriodNewValue(serv[i_serv].servoMotorDigit);
			}
		} // end - time for writing 1 has come
	} // end - iterate through all servos

}
/****************************************************************************
@function   FINISH_period
@class		C_roboticManipulator
@brief      is called on end of each period in PWM thread
			increments period counter, resets tic counter, resets DOport
			finishes period time measurements and logs it
************/
void C_roboticManipulator::FINISH_period()
{
	PWMtic_sum = 0;
	PWMperiod_sum++;	
	RESET_DOport();	

	//finish period measurement
	RtGetClockTime(CLOCK_MEASUREMENT, &tim_endPWMperiod);
	tim_endPWMperiod.QuadPart = tim_endPWMperiod.QuadPart - tim_startPWMperiod.QuadPart;
	RtGetClockTime(CLOCK_MEASUREMENT, &tim_startPWMperiod);
	// log
	char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
	sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "period ended - phase[%i/%i] - PWMperiod_interval = %I64d [100ns] = %I64d [1s]\n", 
		phase_act->i_phase, phase_act->i_phase_max,
		tim_endPWMperiod.QuadPart, tim_endPWMperiod.QuadPart / NS100_1S
		);
		logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PERIOD);
}

/****************************************************************************
@function   SET_NextPhase
@class		C_roboticManipulator
@brief      increments phase_act and phase_prev
			set PWMperiod_sum to zero
@return     DWORD error_sum
************/
DWORD C_roboticManipulator::SET_NextPhase()
{
	phase_act++;
	if(phase_act == phases.end())
	{
		return(NEXT_PHASE_IS_VOID);
	}
	else
	{
		PWMperiod_sum = 0;
		if(phase_act != phases.begin())
		{
			phase_prev = phase_act;
			phase_prev--;
		}
	}
	return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function   LOAD_actualPhase
@class		C_roboticManipulator
@brief      copies the needed data from phase_act into individual C_servoMotor instances 
@param[in]  
@param[out] 
@return     error_sum
************/
DWORD C_roboticManipulator::LOAD_actualPhase(void)
{		
	PWMtic_sum = 0;
	phaseTic_sum = 0;

	if(phase_act == phases.end())
	{ // end of all phases 
		return(ERROR_THIS_PHASE_IS_VOID);
	}
	else
	{ // the iterator is not past-the-end element in the list container
		char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
			sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Load phase[%i/%i] values\n", phase_act->i_phase, phase_act->i_phase_max);
			logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PHASE);
	
		LARGE_INTEGER intervalZero;		// tics for holding one on defined pin
		intervalZero.QuadPart = 0;
		for(int i_serv=0 ; i_serv < SUM_SERVOMOTORS ; i_serv++)
		{
			// count the [zero interval] from [pwm period - one interval]
			//intervalZero.QuadPart = PWMperiod_interval->QuadPart - 1750 * NS100_1US;
			intervalZero.QuadPart = PWMperiod_interval.QuadPart - phase_act->serv_intervalOne[i_serv].QuadPart;
			// write it to actual
			serv[i_serv].SET_intervalZero( intervalZero );
			serv[i_serv].fixedPositioning = phase_act->serv_fixedPositioning[i_serv];
		}
		//____________________________________________________
		// get sum of periods in this phase
		PWMperiod_sum_max = phase_act->phaseInterval.QuadPart / PWMperiod_interval.QuadPart ;

		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Actual phase[%i/%i] interval = %I64d[ms]\n", 
			phase_act->i_phase, phase_act->i_phase_max, 
			phase_act->phaseInterval.QuadPart / NS100_1MS );
		logMsg.PushMessage(textMsg, LOG_SEVERITY_NORMAL);
	}
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@function   IS_endOfPhase
@class		C_roboticManipulator
@return     bool | [true] if the phaseTic_sum is greater than this phase interval
************/
bool C_roboticManipulator::IS_endOfPhase() 
{
	return(phaseTic_sum >= phase_act->phaseInterval.QuadPart);
}
/****************************************************************************
@function   IS_endOfPeriod
@class		C_roboticManipulator
@return     bool | [true] if the PWMtic_sum is greater than period interval
************/
bool C_roboticManipulator::IS_endOfPeriod() 		
{
	return(PWMtic_sum >= PWMperiod_interval.QuadPart);
}

/****************************************************************************
@function   IS_timeToWriteOne
@class		C_roboticManipulator
@param[in]	int a_i_serv | on which servo from the array serv we count
@return     bool | [true] if the PWMtic_sum is greater this servo intervalZero
************/
bool C_roboticManipulator::IS_timeToWriteOne(int a_i_serv)
{
	return(PWMtic_sum >= serv[a_i_serv].intervalZero.QuadPart);
}

/****************************************************************************
@function   IS_reallyTimeToWriteOne
@class		C_roboticManipulator
@brief      
@param[in]	int a_i_serv | on which servo from the array serv we count
@return     bool 
			| if fixedPositioning - always [true]
			| if not fixedPositioning = ramp
				| if the number of tics is greater 
				than counted tic treshold [true]
			| else [false]

************/
bool C_roboticManipulator::IS_reallyTimeToWriteOne(int i_serv)
{
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// not ramp - square position change
	if(phase_act->serv_fixedPositioning)
	{ 
		return(true);
	}
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// ramp - linear position change
	else
	{ 
		// the previous phase has different intervalOne value
		if( serv[i_serv].intervalOne_difference.QuadPart != 0)
		{
			//____________________________________________________
			// evaluate new value of intOne
			LONGLONG intervalOne_thisPeriodDiff = LONGLONG(
					(double)PWMperiod_sum 
					* (double)(serv[i_serv].intervalOne_difference.QuadPart)  
					/ (double)PWMperiod_sum_max
					);
			// (add || subtract) thisPeriodDiff to the previous period intOne
			serv[i_serv].intervalOne_actual.QuadPart = phase_prev->serv_intervalOne[i_serv].QuadPart;

			// intOne is growing -> intOne = previous + thisPeriodDiff
			if( serv[i_serv].intervalOne_growing )
			{						
				serv[i_serv].intervalOne_actual.QuadPart += intervalOne_thisPeriodDiff;
			}
			// intOne is not growing -> intOne = previous - thisPeriodDiff
			else
			{
				serv[i_serv].intervalOne_actual.QuadPart -= intervalOne_thisPeriodDiff;
			}
				
			//____________________________________________________
			// the time for writing 1 has really come - with linear positioning
			if(PWMtic_sum >= (PWMperiod_interval.QuadPart - serv[i_serv].intervalOne_actual.QuadPart))
			{
				// write one to this servo bit
				return(true);
			}
#ifdef DEBUG // debuging breakpoint 
			if( PWMperiod_sum != PWMperiod_sum_last){PWMperiod_sum_last = PWMperiod_sum;}
#endif
		}// end - for all the servos
	} // ramp - linear position change
	// it is not the time
	return(false);
}

/****************************************************************************
@function   CONVERT_angle2intervalOne
@class		C_roboticManipulator
@brief      converts the value of angle of bounds [angle_min, angle_max]
			into bounds of servo [a_serv] intervals [ADC_min, ADC_max]
				can be rewriten to be faster with external pre-counted variables
@param[in]  
@param[out] (LARGE_INTEGER*) a_intervalOne
@return     error_sum
************/
int C_roboticManipulator::CONVERT_angle2intervalOne(int a_angle, int a_i_serv, LARGE_INTEGER* a_intervalOne)
{
	float relative = 0 ; // from f0.0 to f1.0
	relative = (a_angle - angle_min)/((float)(angle_max - angle_min));
	if(relative>1)
	{
#ifdef CUT_OFF_OUT_OF_BOUNDS_ANGLE_IN_CONTROL_FILE //IF NOT DEFINED
		relative = 1; 
#else
		return(ERROR_ANGLE_OUT_OF_BOUNDS);
#endif
	}
	else if(relative <0)
	{
#ifdef CUT_OFF_OUT_OF_BOUNDS_ANGLE_IN_CONTROL_FILE //IF NOT DEFINED
		relative = 0; 
#else
		return(ERROR_ANGLE_OUT_OF_BOUNDS);
#endif
	}
	a_intervalOne->QuadPart = (LONGLONG)( 
		serv[a_i_serv].intervalOne_min.QuadPart + relative 
		* (serv[a_i_serv].intervalOne_max.QuadPart - serv[a_i_serv].intervalOne_min.QuadPart)
		);
	return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function   PUSHFRONT_InitialPhases
@class		C_roboticManipulator
@brief      appends initial phases to the begining of list
@return     DWORD error_sum
************/
DWORD C_roboticManipulator::PUSHFRONT_InitialPhases(void)
{
	C_spatialConfiguration new_phase;

	// initial phase interval
	new_phase.phaseInterval.QuadPart = DEFAULT_INITIAL_PHASE_INTERVAL;
	
	// set initial phase servos position
	for(int i_serv=0; i_serv<SUM_SERVOMOTORS; i_serv++)
	{
		new_phase.serv_fixedPositioning[i_serv] = false;
		new_phase.serv_intervalOne[i_serv].QuadPart = serv[i_serv].ADC_min;
	}
	
	PUSHBACK_newPhase(&new_phase); 
	return(FLAWLESS_EXECUTION);
 }

/****************************************************************************
@function   PUSHBACK_newPhase
@class      C_roboticManipulator
@brief      copy-constructor alike, 
			if [a_phase.intervalOne_change[x]==false] make sure 
				that the intervalOne value is copied from previous phase
			also count differences of intervalOne between a_phase and previous phase
@param[in]  
@param[out] 
@return     
************/
void C_roboticManipulator::PUSHBACK_newPhase(C_spatialConfiguration* a_phase){
	phases.push_back(C_spatialConfiguration());
	
	//get pointers to last and previous-to-last phases in list phases
	phase_act = phases.end(); phase_act--;
	if(phase_act->i_phase_max > 1)
	{// not the first phase 
		phase_prev = phase_act; phase_prev--;
	}

	// copy phase interval
	phase_act->phaseInterval.QuadPart = a_phase->phaseInterval.QuadPart;

	// servos variables
	for(int i_serv=0; i_serv<SUM_SERVOMOTORS; i_serv++)
	{
		phase_act->serv_fixedPositioning[i_serv] = a_phase->serv_fixedPositioning[i_serv];

		//phases.back().serv_intervalOne_changed =;
		if(a_phase->serv_intervalOne_changed[i_serv] == true)
		{ // intervalOne changed after calling constructor --> a_phase has a new value
			phase_act->serv_intervalOne[i_serv].QuadPart = a_phase->serv_intervalOne[i_serv].QuadPart;
			
			// this is first phase || interval was not defined after constructor [intervalOne_change==false]
			phase_act->serv_intervalOne_difference[i_serv].QuadPart = 0;

			// ramp - linear positioning
			if(a_phase->serv_fixedPositioning[i_serv] == false)
			{ 
				// not the first phase 
				if(phase_act->i_phase_max > 1)
				{
					// count the difference of intervalOne between last and previous-to-last phases
					// growing
					if( phase_act->serv_intervalOne[i_serv].QuadPart 
						> phase_prev->serv_intervalOne[i_serv].QuadPart
						)
					{ 
						phase_act->serv_intervalOne_growing[i_serv] = true;
						phase_act->serv_intervalOne_difference[i_serv].QuadPart 
							= phase_act->serv_intervalOne[i_serv].QuadPart 
							- phase_prev->serv_intervalOne[i_serv].QuadPart;
					}
					// not - growing	
					else
					{ 
						phase_act->serv_intervalOne_growing[i_serv] = false;
						phase_act->serv_intervalOne_difference[i_serv].QuadPart 
							= phase_prev->serv_intervalOne[i_serv].QuadPart 
							- phase_act->serv_intervalOne[i_serv].QuadPart;
					}
				} // end - not the first phase
			} // end - ramp - linear positioning
		}
		else
		{ // intervalOne was not defined after constructor --> copy a previous phase value
			// not the first phase 
			if(phase_act->i_phase_max > 1)
			{
				// copy not changed intervalOnes of a_phase from previous phase to actual phase
				phase_act->serv_intervalOne[i_serv] = phase_prev->serv_intervalOne[i_serv];
			} // end - not the first phase
		}
	}
	
}

/****************************************************************************
@function   C_roboticManipulator
@class		C_roboticManipulator
@brief      constructor
@param[out] DWORD& error_sum
************/
C_roboticManipulator::C_roboticManipulator(DWORD &error_sum)
{
	// set interval and counters to zeros
	phaseTic_sum = 0;
	PWMtic_sum = 0;
	PWMperiod_sum = 0;
	PWMperiod_sum_max = 0;
#ifdef DEBUG
	PWMperiod_sum_last = 0;		// only for DEBUG breakpointing after every new period
#endif
	
	tim_startPWMperiod.QuadPart = 0;
	tim_endPWMperiod.QuadPart = 0;
	PWMtic_interval.QuadPart = 0;

	// tic time interval - should be the smallest possible - HAL timer length
	RtGetClockTimerPeriod(CLOCK_TIC_INTERVAL, &PWMtic_interval);	// time to wait between individual PWMtics

	// init phases - set the default one on the beginning
	error_sum = PUSHFRONT_InitialPhases();
	if(error_sum != FLAWLESS_EXECUTION) return;
	phase_act = phases.begin();
	
	// init addresses 
	DOport_ByteAddress = (PUCHAR)(baseAddress + DO_High_Byte);

	// write zeros to register
	DOport_lastPeriodValue = 1; // to work-around WRITE_portUchar not writing the addres if it is the same
	WRITE_portUchar(DOport_ByteAddress, 0);
	
	// the min, max of angle from control-file 
	angle_min = 0;
	angle_max = 1800;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//set constants for individual servos
	int ret_i;
	LONGLONG min_intervalZero = 500;
	LONGLONG max_intervalZero = 2500;

	// debug values -> would be values from empirical measurement for each servo
	DWORD ADC_min = 1700;
	DWORD ADC_max = 2300;

	//____________________________________________________
	// SET constants of servos
	int i=0;		// servo[0]
	ret_i = serv[i].SET_constants( i, S1, min_intervalZero, max_intervalZero, true, ADC_min, ADC_max,5 );
	if(ret_i != i){ 	error_sum = ERROR_SERVO_SET_CONSTANTS_FAIL; 	return;	} 	
	i++;			// servo[1]
	ret_i = serv[i].SET_constants( i, S2, min_intervalZero, max_intervalZero, true, ADC_min, ADC_max,5 );
	if(ret_i != i){ 	error_sum = ERROR_SERVO_SET_CONSTANTS_FAIL; 	return; } 
	i++; 		// servo[2]
	ret_i = serv[i].SET_constants( i, S3, min_intervalZero, max_intervalZero, true, ADC_min, ADC_max,5 );
	if(ret_i != i){ 	error_sum = ERROR_SERVO_SET_CONSTANTS_FAIL; 	return;	} 
	i++;			// servo[3]
	ret_i = serv[i].SET_constants( i, S4, min_intervalZero, max_intervalZero );
	if(ret_i != i){ 	error_sum = ERROR_SERVO_SET_CONSTANTS_FAIL; 	return;	} 
	i++;			// servo[4]
	ret_i = serv[i].SET_constants( i, S5, min_intervalZero, max_intervalZero );
	if(ret_i != i){ 	error_sum = ERROR_SERVO_SET_CONSTANTS_FAIL; 	return;	} 
	i++;			// servo[5]
	ret_i = serv[i].SET_constants( i, S6, min_intervalZero, max_intervalZero );
	if(ret_i != i){ 	error_sum = ERROR_SERVO_SET_CONSTANTS_FAIL; 	return;	} 
	//____________________________________________________
	// end
	error_sum = FLAWLESS_EXECUTION;
	return;
}

/****************************************************************************
@function	IS_in_bounds
@class		C_roboticManipulator
@brief		in bounds of 0 to max_servo_i
@param[in]
@param[out]
@return
***************/
DWORD C_roboticManipulator::IS_in_bounds(int servo_i){
	if(servo_i<0 || servo_i>=SUM_SERVOMOTORS) return(ERR_SERVO_INDEX_OUT_OF_BOUNDS);
	else return(FLAWLESS_EXECUTION);
}


/****************************************************************************
@function	GET_servoMotor
@class		C_roboticManipulator
@brief
@param[in]
@param[out]
@return
***************/
DWORD C_roboticManipulator::GET_servoMotor(int a_servo_i, C_servoMotor** servoMotor)
//DWORD C_roboticManipulator::GET_servoMotor(int a_servo_i, C_servoMotor* servoMotor)
{
	DWORD error_sum = IS_in_bounds(a_servo_i);
	if(error_sum != FLAWLESS_EXECUTION) 
	{
		char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "Could not get servoMotor[%i] pointer - index out of bounds\n", a_servo_i);
		logMsg.PushMessage(textMsg, SEVERITY_ERROR);
		return(error_sum);
	}
	else 
		*servoMotor = &serv[a_servo_i];
	return(error_sum);
}

/****************************************************************************
@function	SET_portUchar
@class		C_roboticManipulator
@brief		if DEBUGGING_WITHOUT_HW is set 
			-> do not call RTX RtWritePortUchar function
			-> rtPrintf byte instead
			else call RtWritePortUchar with the same input parameters			
@param[in]	
@param[out]	
@return		
***************/
void C_roboticManipulator::WRITE_portUchar(PUCHAR a_port_address, UCHAR a_port_data)
{	
	if(DOport_lastPeriodValue == a_port_data)
	{ // register byte did not change
		logMsg.PushMessage("Do not have to write byte - port has not changed.\n", LOG_SEVERITY_PWM_PERIOD);		
	}
	else
	{ // write new byte
		char textMsg[MAX_MESSAGE_LENGTH]; // char array for printing messages
		DOport_lastPeriodValue = a_port_data; // actualize - last from the view of the next period
		DOport_thisPeriodNewValue = DOport_lastPeriodValue; // actualize - this from the view of the next period

#ifndef DEBUGGING_WITHOUT_HW // if NOT defined
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "WRITE > address= 0x%02x | data= 0x%02x\n", a_port_address, a_port_data);
		RtWritePortUchar(a_port_address, a_port_data);
#else	
		// not writing anywhere
		sprintf_s(textMsg, MAX_MESSAGE_LENGTH, "DEBUG > address= 0x%02x | data= 0x%02x\n", a_port_address, a_port_data);
	
#endif
		logMsg.PushMessage(textMsg, LOG_SEVERITY_PWM_PERIOD);
	}
}

/****************************************************************************
@function	RESET_DOport
@class		C_roboticManipulator
@brief		Writes zeros into DO port
@param[in]	
@param[out]	
***************/
void C_roboticManipulator::RESET_DOport()
{	
	WRITE_portUchar(DOport_ByteAddress,(UCHAR)0);
}

/****************************************************************************
@function	SET_bitUchar
@class		C_roboticManipulator
@brief		if DEBUGGING_WITHOUT_HW is set 
			-> do not call RTX RtWritePortUchar function
			-> rtPrintf byte instead
			else call RtWritePortUchar with the same input parameters			
@param[in]	
@param[out]	
@return		
***************/
void C_roboticManipulator::SET_DOport_thisPeriodNewValue(UCHAR a_port_bit)
{	
	if(DOport_thisPeriodNewValue & 1<<a_port_bit)
	{ // the port bit is already SET - no change
		return;
	}
	else
	{ // write changed bit to DOport_thisPeriodNewValue
		// WRITE_portUchar(DOport_ByteAddress, DOport_lastPeriodValue | 1<<a_port_bit);
		DOport_thisPeriodNewValue = DOport_thisPeriodNewValue | 1<<a_port_bit;
	}
}
/****************************************************************************
@function	WRITE_DOport_thisPeriodNewValue
@class		C_roboticManipulator
@brief
@param[in]
@param[out]
@return
***************/
void C_roboticManipulator::WRITE_DOport_thisPeriodNewValue()
{
	if(DOport_thisPeriodNewValue != DOport_lastPeriodValue)
	{
		WRITE_portUchar(DOport_ByteAddress, DOport_thisPeriodNewValue);
	}
}


/****************************************************************************
@function	SET_dutyCycleIntervals
@class		C_roboticManipulator
@brief
@param[in]
@param[out]
@return
***************/
/*
int C_roboticManipulator::SET_dutyCycleIntervals(
	int a_servo_i, 
	LARGE_INTEGER a_interval_one, 
	LARGE_INTEGER a_intervalZero)
{
	if(!IS_in_bounds(a_servo_i)) return(ERR_SERVO_INDEX_OUT_OF_BOUNDS);
	serv[a_servo_i].SET_dutyCycleIntervals(a_interval_one,a_intervalZero);
	return(FLAWLESS_EXECUTION);
}*/