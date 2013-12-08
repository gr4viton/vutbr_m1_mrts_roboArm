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
class C_spatialConfiguration{
//____________________________________________________
// member variables
public:
	LARGE_INTEGER phaseInterval;
	LARGE_INTEGER servIntervalZero[SUM_SERVOMOTORS]; // intervalZeros for each servo in this phase
	bool servIntervalZeroChanged[SUM_SERVOMOTORS]; // if it was changed after constructor = true
//____________________________________________________
// declaration of external defined member functions 
public:	C_spatialConfiguration(void);
public:	C_spatialConfiguration(LARGE_INTEGER* a_phaseInterval, LARGE_INTEGER* a_servIntervalZero);
public: ~C_spatialConfiguration(void);
public: void SET_servIntervalZero(int a_i_serv, LARGE_INTEGER *a_servIntervalZero);
};



#endif