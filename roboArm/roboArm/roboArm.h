/***************
@project  	roboArm
@filename	INIT.cpp
@authors		Bc. Jiøí Sliž <xslizj00@stud.feec.vutbr.cz> && Bc. Daniel Davídek <danieldavidek@gmail.com>
@date		2013_12_02
@brief		file containing INIT function definitions
@descrptn	Program for communication with PIO821 card and read values of servo angles.
***************/

#ifndef ROBOARM_H
#define ROBOARM_H

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// setup defines and macros
#define RUNNING_ON_1CPU

//#define ROBOARM_SECOND_ROW 
#define ROBOARM_THIRD_ROW

//#define RUNNING_ON_RTX64

#ifdef RUNNING_ON_1CPU
	#define DEFAULT_PREEMPTIVE_INTERVAL		100
// units [100ns]
#endif
//____________________________________________________
// debug
// if DEBUGGING_WITHOUT_HW is defined -> no register writing is performed -> only pushmsg the byte on
#define DEBUGGING_WITHOUT_HW
#define DEBUG
#define DEBUG_PRINT_FDBACK_ADC_DATA_AFTER_PHASES_END

#define SHOW_LOG_ON_SCREEN
//____________________________________________________
// WHICH SEVERITIES TO PRINT
#define SEVERITY_LEVEL						LOG_SEVERITY_PWM_PHASE
//#define SEVERITY_LEVEL						LOG_SEVERITY_PWM_PERIOD
//#define SEVERITY_LEVEL						LOG_SEVERITY_PWM_TIC

//____________________________________________________
// PWM
// 1[s]/x = x Hz
//#define DEFAULT_PWM_PERIOD				NS100_1S / 1
#define DEFAULT_PWM_PERIOD				NS100_1S / 100
#define DEFAULT_INITIAL_PHASE_INTERVAL	(1*NS100_1S)

//____________________________________________________
// file paths
#define LOG_FILE_PATH			"D:\\LogMessage.txt"
//#define LOG_FILE_PATH			"C:\\LogMessage.txt"
#define LOGMSG_LINE_END			"\n"


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// includes

// win & RTX

#include <SDKDDKVer.h>
#include <windows.h>
#include <rtapi.h> // must be after windows.h

#ifdef UNDER_RTSS
	//#include <rtssapi.h>
#endif // UNDER_RTSS

// std
#include <stdio.h>
#include <exception>		// for [new] allocation
#include <list>			// for ROB->phase-list

#ifdef RUNNING_ON_RTX64
// functional only in rtx64(2013)
#include <string>
#endif

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class declarations
class C_servoMotor;
class C_spatialConfiguration;
class C_roboticManipulator;
class C_LogMessageA;

// classes headerfiles (for proper linking)
//____________________________________________________
// this project headerfiles - (not the class headerfiles)
#include "LogMessageA.h"
#include "logSeverities.h"
#include "returnCodeDefines.h"
#include "constantDefines.h"

#include "C_servoMotor.h"
#include "C_roboticManipulator.h"
#include "C_spatialConfiguration.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// external variables & classes

//roboArm.cpp
extern unsigned int		baseAddress;			// base address of register 
extern HMODULE			hLibModule;			// handler to module library
extern char*				G_controlString;		// char array for reading file
extern C_LogMessageA		logMsg;				// Pointer of C_LogMessageA, used for all logging

#ifdef RUNNING_ON_1CPU
	extern LARGE_INTEGER		preemptive_interval; 
#endif

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations of roboArm.cpp
void _cdecl	main(int  argc, char **argv);
DWORD		GET_ADC(UCHAR channel, UCHAR gain);
void			EXIT_process(DWORD error_sum);
int			LogMessage(int iSeverity, char *cMessage, int bBlocking);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations of non-headered .cpp files
//____________________________________________________
// INIT.cpp
int		INIT_HW();
int		INIT_Library();
void		INIT_ADC();
	
//____________________________________________________
// readFile.cpp
DWORD	GET_stringLength(char *a_string, DWORD a_max_lenght, DWORD* error_sum);
DWORD	READ_spatialConfigurationFromFile(C_roboticManipulator* a_ROB, char* a_filePath);

// read file
DWORD	READ_file(char* a_filePath);
DWORD	MOVE_pointer(HANDLE a_hFile, LONG a_distance2move, DWORD* a_file_current_byte, DWORD a_moveMethod = FILE_CURRENT);
DWORD	CLOSE_handleAndReturn(HANDLE a_handle, DWORD error_sum, bool a_logError = true);

// parse text
DWORD	PARSE_controlString(C_roboticManipulator* a_manip);
DWORD	PARSE_controlString(void);
int		char2num(char ch);
		
//____________________________________________________
// threadFunctions.cpp
DWORD CREATE_threads(C_roboticManipulator* ROB, HANDLE *hTh, DWORD* thread_id);
DWORD CREATE_thread(int iTh, HANDLE& a_threadHandle, DWORD* a_threadID, 
					int wanted_priority, LPTHREAD_START_ROUTINE a_threadRoutine, 
					void* a_threadParam);

// exiting functions
void		CLOSE_handleAndExitThread(HANDLE handle, DWORD error_sum);
void		TERMINATE_allThreadsAndExitProcess(HANDLE *hTh, int iTh_max, DWORD error_sum);
DWORD	EXIT_threadPWM(DWORD error_sum, C_roboticManipulator** a_ROB);

// logmsg
void		RTFCNDCL LogMessageThread(void *a_struct);
// TIM_PWM
void		RTFCNDCL PWMthread(void *a_manip);

#endif










//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
channel |	servo		|	ADC_feedBack[b]	|	AI[b]   
--------+---------------+-----------+--------
	0	|	3(maly)		|	3			|	0
	1	|	1(velky)		|	1			|	1
	2	|	2(stred)		|	2			|	2
*/
//____________________________________________________
// set gain and mux
/*
	 x|x|MUX3|MUX2|MUX1|MUX0|GAIN1|GAIN0
	 MUX[3-0] = binary number selecting from 0to15 shifted left 2 
		0b0000 0000<<2 = 0x00<<2 = 0x00 = 0<<2 = AI0
		0b0000 0001<<2 = 0x01<<2 = 0x04 = 1<<2 = AI1
		0b0000 0010<<2 = 0x02<<2 = 0x08 = 2<<2 = AI2
		0b0000 0100<<2 = 0x03<<2 = 0x0C = 3<<2 = AI3
	 etc
*/