/**
 * @description Program for communication with PIO821 card and read values of servo angles.
 * @authors Bc. Jiøí Sliž <xslizj00@stud.feec.vutbr.cz> and Bc. 
 *
 *
 */

#ifndef ROBOARM_H
#define ROBOARM_H


#include <rtapi.h>

#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <conio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <errno.h>

//#include "INIT.h"

// DEFINES
#define AD_GAIN					0xe0
#define AD_MODE_CONTROLL		0xe4
#define AD_SOFT_TRIGGER_START	0xe8
#define AD_STATUS				0xec
#define AD_LOWBYTE				0xd0
#define AD_HIGHBYTE				0xd4
#define DO_Low_Byte				0xd8
#define DO_High_Byte			0xdc

/*
cislovani serv od spoda 1-6
channel |	servo		|	FDBACK	|	AI   
--------+---------------+-----------+--------
	0	|	3(maly)		|	3		|	0
	1	|	1(velky)	|	1		|	1
	2	|	2(stred)	|	2		|	2


- servo [1-6] odpovida DO [10-15]
*/

// external variables
extern DWORD baseAddress;
extern HMODULE hLibModule;


/**
 * @name 	INIT_Library
 * @brief	function open library for communication with PIO821 card
 * @retval	0 - function succeeded
 * @retval	1 - loadLibrary error, communication FAILED
 * @retval	2 - get adrees error, communication FAILED
*/
DWORD GET_ADC(UCHAR channel, UCHAR gain);

int INIT_Library();
void INIT_ADC();

#endif