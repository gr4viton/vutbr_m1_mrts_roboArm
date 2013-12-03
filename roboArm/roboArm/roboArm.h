/***************
* @filename		INIT.cpp
* @author 		xdavid10, xslizj00 @ FEEC-VUTBR 
* @date			2013_12_02
* @brief			file containing INIT function definitions
***************/
/**
 * @description Program for communication with PIO821 card and read values of servo angles.
 * @authors Bc. Jiøí Sliž <xslizj00@stud.feec.vutbr.cz> and Bc. 
 *
 *
 */

#ifndef ROBOARM_H
#define ROBOARM_H
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// includes

// win & RTX
#include <SDKDDKVer.h>
#include <windows.h>
#include <rtapi.h>

#ifdef UNDER_RTSS
	#include <rtssapi.h>
#endif // UNDER_RTSS

// std
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <conio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <errno.h>

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// setup defines and macros
//____________________________________________________
// addresses
#define AD_GAIN					0xe0
#define AD_MODE_CONTROLL			0xe4
#define AD_SOFT_TRIGGER_START	0xe8
#define AD_STATUS				0xec
#define AD_LOWBYTE				0xd0
#define AD_HIGHBYTE				0xd4
#define DO_Low_Byte				0xd8
#define DO_High_Byte				0xdc

//____________________________________________________
// class servoMotor macros
#define SERVOMOTOR_ADDRESS_HIGHBYTE		true
#define SERVOMOTOR_ADDRESS_LOWBYTE		false

#define SUM_SERVOMOTORS					6

/* ____________________________________________________
cislovani serv od spoda 1-6
channel |	servo		|	FDBACK	|	AI   
--------+---------------+-----------+--------
	0	|	3(maly)		|	3		|	0
	1	|	1(velky)		|	1		|	1
	2	|	2(stred)		|	2		|	2


- servo [1-6] odpovida DO [10-15]
*/
//____________________________________________________
// predefined time intervals
// multiplicatives of [100ns]
//#define NS100_1US			(LARGE_INTEGER)10
#define NS100_1US			10
#define NS100_1MS			10000
#define NS100_1S				10000000

// 50Hz = 0.02s = 200000[100ns]
#define NS100_50HZ			NS100_1S/50
// do not use -> dividing by zero unhandled possibility -> write an inline fct?
// #define NS100_X_HZ(x_hz)		(NS100_1S/(x_hz)) 

#define CLOCK_X				CLOCK_1

// ____________________________________________________
// roboticManipulator_error
#define FLAWLESS_EXECUTION					0
#define DESTRUCT_EVERYTHING					1
#define SERVO_INDEX_OUT_OF_BOUNDS			2
#define C_SERVOMOTOR_SETTIMERREL_ERROR		3
#define C_SERVOMOTOR_TIMER_INVALID_HANDLE	4
#define CONSTRUCOR_ERROR_OFFSET				127
// free positions offset + max_servo_i


//____________________________________________________
// ExitProcess return value constants - ERRORS & etc
#define SUCCESSFUL_END								0
#define SUMFLOATS_ERROR_CMESSAGE_TOO_LONG			1
#define SUMFLOATS_ERROR_CREATEFILE_FAIL				2
#define SUMFLOATS_ERROR_SETFILEPOINTER_FAIL			4
#define SUMFLOATS_ERROR_READFILE_FAIL				8
#define SUMFLOATS_ERROR_CLOSEHANDLE_FAIL				16
#define SUMFLOATS_ERRORSTRING_FAIL					32
#define SUMFLOATS_ERROR_READ_BYTES_MISMATCH			64
#define SUMFLOATS_ERROR_COULD_NOT_TERMINATE_THREAD	128
#define SUMFLOATS_ERROR_COULD_NOT_RESUME_THREAD		256
#define SUMFLOATS_ERROR_COULD_NOT_CHANGE_PRIORITY	512
#define SUMFLOATS_ERROR_COULD_NOT_CREATE_THREAD		1024
#define SUMFLOATS_ERROR_SPRINTF_S					99252
#define SUMFLOATS_ERROR_SEVERITY_TOO_LOW				99253
#define SUMFLOATS_ERROR_SEVERITY_TOO_BIG				99254

//____________________________________________________
#define EXITCODE_SUCCESSFUL_END					0
#define EXITCODE_TERMINATED_BY_MAIN				1

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// external variables & classes
extern DWORD baseAddress;
extern HMODULE hLibModule;

class C_servoMotor;
class C_roboticManipulator;

// this project classes headerfiles
#include "C_servoMotor.h"
#include "C_roboticManipulator.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function headers
DWORD	GET_ADC(UCHAR channel, UCHAR gain);
// INIT - funcions
int		INIT_All();
int		INIT_Library();
void		INIT_ADC();

#endif