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

// linker error
// vs2008 - LNK2019 -> it means vs does not know the RT functions
// - probably bad including header of declaration of RT functions
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

// return codes
#include "returnCodeDefines.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// setup defines and macros
#define DEBUG
#define DEBUGGING_WITHOUT_HW
#define DEBUG_PRINT_READ_FUNCTIONS

// if angle in file is out of bounds do not exit but convert it to limit instead and continue
#define CUT_OFF_OUT_OF_BOUNDS_IN_FILE 

//____________________________________________________
// file path
//#define CONTROL_FILE_PATH L"D:\\EDUC\\m1\\R_MRTS\\proj_robo_ruka\\GIT\\roboArm\\control.txt"
#define CONTROL_FILE_PATH		L"D:\\EDUC\\m1\\R_MRTS\\proj_robo_ruka\\GIT\\roboArm\\control.txt"
#define FILE_MAX_CHARS			100000

//____________________________________________________
// reading control
#define NUM_OF_THREADS			2

#define CHARS_PER_LINE			9
#define FIRST_DIGIT_CHAR			2
#define NUM_OF_DIGITS			5
// MAX_DWORD = 4 294 967 295
// MAX_DWORD / CHARS_PER_LINE > MAX(chunk_lines)
// DWORD is sufficient for chunk_lines < 477 218 588
//#define CHUNK_LINES				1024
#define CHUNK_LINES				9000

//____________________________________________________
// reading constants
// end character lenght (\0)
#define CZERO						1 
// end of file (\0)
#define CEOF							1 
// end line characters (\r\n)
#define CEND_LINE					2
//#define DEFAULT_STACK_SIZE			8192


//____________________________________________________
// class servoMotor macros
#define SUM_SERVOMOTORS					6


/* ____________________________________________________
indexes of servmotors HW from the bottom one [1-6]
channel |	servo		|	FDBACK	|	AI   
--------+---------------+-----------+--------
	0	|	3(maly)		|	3		|	0
	1	|	1(velky)		|	1		|	1
	2	|	2(stred)		|	2		|	2


- servo S[1-6] is on address bit DO_High_Byte 1 << x

		+-----------+-------+------------+
		|			| 1<<x	|  interval	 |
		+-----------+-------+------------+
		|  servo		|	x	| min-max[us]|
		+-----------+-------+------------+
		|	S1		|	2	|			 |
		+-----------+-------+------------+
		|	S2		|	3	|			 |
		+-----------+-------+------------+
		|	S3		|	4	|			 |
		+-----------+-------+------------+
		|	S4		|	6	| 500-2500	 |
		+-----------+-------+------------+
		|	S5		|	5	| 500-2500	 |
		+-----------+-------+------------+
		|	S6		|	7	| 500-2500	 |
		+-----------+-------+------------+
*/
typedef enum {S1=2, S2=3, S3=4, S5=6, S4=5, S6=7}E_servos;

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

#define CLOCK_X				CLOCK_2



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// external variables & classes
extern DWORD baseAddress;
extern HMODULE hLibModule;
extern char str[]; 

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class declarations
class C_servoMotor;
class C_roboticManipulator;

// this project classes headerfiles
#include "C_servoMotor.h"
#include "C_roboticManipulator.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations of roboArm.cpp
DWORD	GET_ADC(UCHAR channel, UCHAR gain);
void RTFCNDCL TIM_PWMfunction(void *a_manip);

int CLOSE_handleAndReturn(HANDLE handle, int error_sum);
void CLOSE_handleAndExitThread(HANDLE handle, int error_sum);

void TERMINATE_allThreadsAndExitProcess(HANDLE *hTh, int iTh_max, int error_sum);
//____________________________________________________
// function declarations of non-headered .cpp files
// INIT.cpp
int		INIT_All();
int		INIT_Library();
void		INIT_ADC();

#endif
