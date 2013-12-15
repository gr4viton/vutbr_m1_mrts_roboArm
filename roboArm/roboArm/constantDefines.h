/***************
@project  roboArm
@filename constantDefines.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing definitions of things that arent constantly changed
***************/

#ifndef CONSTANTDEFINES_H
#define CONSTANTDEFINES_H

//____________________________________________________
// clocks
#define CLOCK_TIC_INTERVAL	CLOCK_2
#define CLOCK_LOG_ACTUAL		CLOCK_2
#define CLOCK_MEASUREMENT	CLOCK_1

//____________________________________________________
// thread counts
#define LOGMSG_THREAD				1
#define PWM_CONTROLLING_THREAD		1
#define NUM_OF_THREADS				(PWM_CONTROLLING_THREAD + LOGMSG_THREAD)

// thread indexes
#define TH_LOG_I						0
#define TH_PWM_I						1
// thread priorities
#define TH_LOG_PRIORITY				RT_PRIORITY_MAX - 6
#define TH_PWM_PRIORITY				RT_PRIORITY_MAX - 7

#define USE_DEFAULT_STACK_SIZE		0
#define NORMAL_THREAD_STACK_SIZE		USE_DEFAULT_STACK_SIZE

//____________________________________________________
// class servoMotor macros
#define SUM_SERVOMOTORS					6

//____________________________________________________
// optional walk-arounds for little file inconsistency
#define IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE
#define IGNORE_INCONSISTENT_FILE_LINE
// if angle in file is out of bounds do not exit but convert it to limit instead and continue
#define CUT_OFF_OUT_OF_BOUNDS_ANGLE_IN_CONTROL_FILE 

//____________________________________________________
// reading constants
// end character lenght (\0)
#define CZERO						1 
// end of file (\0)
#define CEOF							1 
// end line characters (\r\n)
#define CEND_LINE					2
// maximal Wait time command number of digits
#define MAX_WAIT_TIME_CMD_NUM_OF_DIGITS
#define DEFAULT_ADC_GAIN			0

//____________________________________________________
// predefined time intervals
// multiplicatives of [100ns]
#define NS100_1US		10
#define NS100_1MS		10000
#define NS100_1S	 		10000000

// 50Hz = 0.02s = 200000[100ns]
#define NS100_50HZ			NS100_1S/50
#define NS100_100HZ			NS100_1S/100
// do not use -> dividing by zero unhandled possibility -> write an inline fct?
// #define NS100_X_HZ(x_hz)		(NS100_1S/(x_hz)) 

//____________________________________________________
// addresses of control bytes in register
#define AD_GAIN					0xe0
#define AD_MODE_CONTROLL			0xe4
#define AD_SOFT_TRIGGER_START	0xe8
#define AD_STATUS				0xec
#define AD_LOWBYTE				0xd0
#define AD_HIGHBYTE				0xd4
#define DO_Low_Byte				0xd8
#define DO_High_Byte				0xdc

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// typedefs
//____________________________________________________
// defines connection of real physical position indexing (SX) to port address DO bit of each servoMotor
#define GAIN_OFFSET				2
// COMP 2nd row in the middle
#ifdef ROBOARM_SECOND_ROW 
typedef enum {
	S1=GAIN_OFFSET + 0, 
	S2=GAIN_OFFSET + 1, 
	S3=GAIN_OFFSET + 2, 
	S5=GAIN_OFFSET + 4, 
	S4=GAIN_OFFSET + 3, 
	S6=GAIN_OFFSET + 5
}E_servos;
#endif

#ifdef ROBOARM_THIRD_ROW
// COMP 3nd row in the middle
typedef enum {
	S1=GAIN_OFFSET + 0, 
	S2=GAIN_OFFSET + 1, 
	S3=GAIN_OFFSET + 2, 
	S5=GAIN_OFFSET + 3, 
	S4=GAIN_OFFSET + 4, 
	S6=GAIN_OFFSET + 5
}E_servos;
#endif


#endif // CONSTANTDEFINES_H