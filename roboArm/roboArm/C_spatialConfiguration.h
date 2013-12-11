/***************
@project  roboArm
@filename C_spatialConfiguration.h
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    file containing C_spatialConfiguration class definition 
			= member functions and member variables declarations
***************/

#ifndef C_SPATIALCONFIGURATION_H
#define C_SPATIALCONFIGURATION_H

#include "roboArm.h"



/****************************************************************************
@class		C_spatialConfiguration
@brief      
***********/
class C_spatialConfiguration
{
//____________________________________________________
// member variables
public:
	DWORD i_phase;										// phase index
	static DWORD i_phase_max;							// num of currently added phases
	LARGE_INTEGER phaseInterval;							// how long to be in this phase
	LARGE_INTEGER servIntervalOne[SUM_SERVOMOTORS];		// intervalZeros for each servo in this phase
	bool servIntervalOneChanged[SUM_SERVOMOTORS];		// if it was changed after constructor = true
//____________________________________________________
// declaration of external defined member functions 
public:	C_spatialConfiguration(void);
public:	C_spatialConfiguration(LARGE_INTEGER* a_phaseInterval, LARGE_INTEGER* a_servIntervalOne);
public: ~C_spatialConfiguration(void);
public: void SET_servIntervalOne(int a_i_serv, LARGE_INTEGER *a_servIntervalOne);
};



#endif