/***************
@project  	roboArm
@filename	INIT.cpp
@author		xdavid10, xslizj00 @ FEEC-VUTBR 
@date		2013_12_02
@brief		file containing INIT function definitions
***************/
/**
 @description Program for communication with PIO821 card and read values of servo angles.
 @authors Bc. Jiøí Sliž <xslizj00@stud.feec.vutbr.cz> and Bc. 
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
#include <rtapi.h> // must be after windows.h

#ifdef UNDER_RTSS
	#include <rtssapi.h>
#endif // UNDER_RTSS

// std
#include <stdio.h>
#include <exception>		// for [new] allocation
#include <list>			// for ROB->phase-list

// functional only in rtx64(2013)
//#include <string>

//#include <iostream>
//#include <ctype.h>
//#include <conio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <errno.h>

//____________________________________________________
// this project headerfiles - (not the class headerfiles)
#include "LogMessageA.h"
#include "returnCodeDefines.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// setup defines and macros
#define CLOCK_X							CLOCK_1
#define RUNNING_ON_1CPU
//____________________________________________________
// debug
#define DEBUG
#define DEBUGGING_WITHOUT_HW
#define DEBUG_PRINT_READ_FUNCTIONS


//#define SHOW_LOG_ON_SCREEN

//____________________________________________________
// optional walk-arounds for little file inconsistency
#define IGNORE_NOT_NUMBER_ANGLE_IN_CONTROL_FILE
#define IGNORE_INCONSISTENT_FILE_LINE
// if angle in file is out of bounds do not exit but convert it to limit instead and continue
#define CUT_OFF_OUT_OF_BOUNDS_ANGLE_IN_CONTROL_FILE 

//____________________________________________________
// severities
#define 	PUSHMSG_SEVERITY_LOWEST			SEVERITY_MAX - 9
#define 	PUSHMSG_SEVERITY_LOWER			SEVERITY_MAX - 8
#define 	PUSHMSG_SEVERITY_LOW				SEVERITY_MAX - 7 
#define PUSHMSG_SEVERITY_MEDIUM			SEVERITY_MAX - 5	
#define 	PUSHMSG_SEVERITY_HIGH			SEVERITY_MAX - 3
#define 	PUSHMSG_SEVERITY_HIGHER			SEVERITY_MAX - 2
#define 	PUSHMSG_SEVERITY_HIGHEST			SEVERITY_MAX - 1

#define PUSHMSG_SEVERITY_NORMAL			PUSHMSG_SEVERITY_MEDIUM		

//____________________________________________________
// file path
//#define CONTROL_FILE_PATH		L"D:\\EDUC\\m1\\R_MRTS\\proj_robo_ruka\\GIT\\roboArm\\control2.txt"
//#define CONTROL_FILE_PATH		L"D:\\EDUC\\m1\\R_MRTS\\proj_robo_ruka\\GIT\\roboArm\\control.txt"
#define CONTROL_FILE_PATH		L"D:\\EDUC\\m1\\R_MRTS\\float.txt"
#define FILE_MAX_CHARS			100000

//____________________________________________________
// thread counts
#define NUM_OF_THREADS				(PWM_CONTROLLING_THREAD + LOGMSG_THREAD)
#define LOGMSG_THREAD				1
#define PWM_CONTROLLING_THREAD		1
// thread indexes
#define TH_LOG_I						0
#define TH_PWM_I						1
// thread priorities
#define TH_LOG_PRIORITY				RT_PRIORITY_MAX - 6
#define TH_PWM_PRIORITY				RT_PRIORITY_MAX - 7

//____________________________________________________
// class servoMotor macros
#define SUM_SERVOMOTORS					6
// 1[s]/x = x Hz
#define DEFAULT_PWM_PERIOD				NS100_1S / 1
//#define DEFAULT_PWM_PERIOD				NS100_1S / 100
#define DEFAULT_INITIAL_PHASE_INTERVAL	(5*NS100_1S)
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
typedef enum {S1=2, S2=3, S3=4, S5=6, S4=5, S6=7}E_servos;



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class declarations
class C_servoMotor;
class C_spatialConfiguration;
class C_roboticManipulator;
class C_LogMessageA;

// classes headerfiles (for proper linking)
#include "C_servoMotor.h"
#include "C_roboticManipulator.h"
#include "C_spatialConfiguration.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// external variables & classes

//roboArm.cpp
extern unsigned int		baseAddress;		
extern HMODULE			hLibModule;	
extern char*				G_controlString;			
extern C_LogMessageA		logMsg;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations of roboArm.cpp
void _cdecl	main(int  argc, char **argv);
DWORD		GET_ADC(UCHAR channel, UCHAR gain);
void			EXIT_process(DWORD error_sum);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations of non-headered .cpp files
//____________________________________________________
// INIT.cpp
int		INIT_HW();
int		INIT_Library();
void		INIT_ADC();
	
//____________________________________________________
// readFile.cpp
DWORD	GET_stringLenght(char *a_string, DWORD a_max_lenght, DWORD* error_sum);
DWORD	READ_spatialConfigurationFromFile(C_roboticManipulator* a_ROB, char* a_filePath);

// read file
DWORD	READ_file(char* a_filePath);
DWORD	MOVE_pointerOrReturn(HANDLE hFile, LONG distance2move, DWORD* file_current_byte, DWORD MoveMethod);
DWORD	CLOSE_handleAndReturn(HANDLE handle, DWORD error_sum);

// parse text
DWORD	PARSE_controlString(C_roboticManipulator* a_manip);
DWORD	PARSE_controlString(void);
int		char2num(char ch);
		
//____________________________________________________
// threadFunctions.cpp
HANDLE* CREATE_threads(void);

// exiting functions
void		CLOSE_handleAndExitThread(HANDLE handle, DWORD error_sum);
void		TERMINATE_allThreadsAndExitProcess(HANDLE *hTh, int iTh_max, DWORD error_sum);

// thread functions
void RTFCNDCL LogMessageThread(void *a_struct);
void		RTFCNDCL TIM_PWMfunction(void *a_manip);

#endif










//____________________________________________________
// tables of indexation
/*
Control pin of servo S[1-6] is on address bit DO_High_Byte 1 << x
			| 1<<x	|  interval	 |
+-----------+-------+------------+
	servo	|	x	| min-max[us]|
+-----------+-------+------------+
	S1		|	2	|			 |
	S2		|	3	|			 |
	S3		|	4	|			 |
	S4		|	6	| 500-2500	 |
	S5		|	5	| 500-2500	 |
	S6		|	7	| 500-2500	 |

	
indexes of servmotors HW from the biggest bottom one [1-6]
channel |	servo		|	FDBACK[b]	|	AI[b]   
--------+---------------+-----------+--------
	0	|	3(maly)		|	3			|	0
	1	|	1(velky)		|	1			|	1
	2	|	2(stred)		|	2			|	2
*/