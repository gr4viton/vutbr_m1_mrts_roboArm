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
	DWORD i_phase;						// phase index
	static DWORD i_phase_max;			// num of currently added phases
	LARGE_INTEGER phaseInterval;			// how long to be in this phase

	bool	 serv_fixedPositioning[SUM_SERVOMOTORS];			// if fixed positioning is true -> no linear change of position in PWM
	LARGE_INTEGER serv_intervalOne[SUM_SERVOMOTORS];		// intervalZeros for each servo in this phase
	LARGE_INTEGER serv_intervalOne_difference[SUM_SERVOMOTORS];		// difference of interval one from previous phase - counted on loading
	
	bool serv_intervalOne_growing[SUM_SERVOMOTORS];		// =true if the intervalOne from previous phase is smaller than in this phase
														// - it is created because LARGE_INTEGER is unsigned
	bool serv_intervalOne_changed[SUM_SERVOMOTORS];		// =true if intervaOne has changed after constructor 
														// - previous phase serv_intervalOne[x] is loaded to this phase on loading
//____________________________________________________
// declaration of external defined member functions 
public:	C_spatialConfiguration(void);
public:	C_spatialConfiguration(LARGE_INTEGER* a_phaseInterval, LARGE_INTEGER* a_serv_intervalOne, bool a_serv_fixedPositioning=false);
public: ~C_spatialConfiguration(void);
public: void SET_serv_intervalOne(int a_i_serv, LARGE_INTEGER *a_serv_intervalOne);
};



#endif