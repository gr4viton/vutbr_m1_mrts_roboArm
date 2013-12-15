/***************
@project  roboArm
@filename C_roboticManipulator.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing C_roboticManipulator class definition 
			= member functions and member variables declarations
***************/

#ifndef C_ROBOTICMANIPULATOR_H
#define C_ROBOTICMANIPULATOR_H

#include "roboArm.h"
//#include <rtapi.h> // LNK2019 problem? nope

/****************************************************************************
@class		C_roboticManipulator
@brief      
***************/
class C_roboticManipulator
{
//____________________________________________________
// member variables
private:
	C_servoMotor serv[SUM_SERVOMOTORS];	
	PUCHAR	DOport_ByteAddress; // address of DO port in register to whom are servoMotors connected to
	UCHAR	DOport_lastPeriodValue; // value of DOport from last period
	UCHAR	DOport_thisPeriodNewValue; // new DOport value before writing it to register -> more servos at the same angle
	
	// as defined in text file
	DWORD	angle_min;
	DWORD	angle_max; 
public:
	LONGLONG			phaseTic_sum;		// counting phase time

	LARGE_INTEGER	PWMtic_interval;		// how long does one PWMtic take
	LONGLONG			PWMtic_sum;			// incrementing variable by tic_interval

	LARGE_INTEGER	PWMperiod_interval;	// how long does one period of PWM take - how often to rewrite DO port
	LONGLONG			PWMperiod_sum;		// counter of periods for act phase 
	LONGLONG			PWMperiod_sum_max;	// how many PWM periods are in act phase interval
#ifdef DEBUG
	LONGLONG			PWMperiod_sum_last;	// only for DEBUG breakpointing after every new period
#endif

	LARGE_INTEGER	tim_startPWMperiod; 
	LARGE_INTEGER	tim_endPWMperiod; 


	std::list<C_spatialConfiguration> phases;

	std::list<C_spatialConfiguration>::iterator phase_act; 
	std::list<C_spatialConfiguration>::iterator phase_prev;

//____________________________________________________
// declaration of external defined member functions 
public:		DWORD IS_in_bounds(int servo_i);	
public:		C_roboticManipulator(DWORD &error_sum);
//public:		int SET_dutyCycleIntervals(int servo_i, LARGE_INTEGER a_interval_one, LARGE_INTEGER a_intervalZero);
public:		DWORD PUSHFRONT_InitialPhases(void);
//public:		std::list<C_spatialConfiguration> GET_InitialPhases(void);

public:		DWORD GET_servoMotor(int a_servo_i, C_servoMotor** servoMotor);
//public:		DWORD GET_servoMotor(int a_servo_i, C_servoMotor* servoMotor);
			


// phasing
public:		DWORD LOAD_actualPhase(void);
public:		DWORD SET_NextPhase();
public:		void PUSH_frontNewPhase(C_spatialConfiguration* a_phase); 
			
public:		void FINISH_period();
public:		bool	 IS_endOfPhase();
public:		bool IS_endOfPeriod();
public:		bool IS_timeToWriteOne(int a_i_serv);
			
public:		void CALC_DOport_thisPeriodNewValue();
public:		void SET_DOport_thisPeriodNewValue(UCHAR a_port_bit);
// DO port writing
public:		void RESET_DOport();
public:		void WRITE_DOport_thisPeriodNewValue();
public:		void WRITE_portUchar(PUCHAR a_port_address, UCHAR a_port_data);


public:		int CONVERT_angle2intervalOne(int a_angle, int a_i_serv, LARGE_INTEGER* a_intervalZero);

public:		void DEBUG_fillPhases(void);
};

#endif