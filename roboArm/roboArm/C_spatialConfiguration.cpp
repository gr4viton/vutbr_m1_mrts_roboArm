/***********
@project		roboArm
@filename	C_spatialConfiguration.cpp
@author		xdavid10, xslizj00 @ FEEC-VUTBR 
@date		2013_12_02
@brief		file containing member function definitions 
			of classes C_spatialConfiguration
***********/

#include "roboArm.h"
//____________________________________________________
// static variables definition
DWORD C_spatialConfiguration::i_phase_max = 0;

/****************************************************************************
@function   C_spatialConfiguration
@class		C_spatialConfiguration
@brief      constructor - implicite
@param[in]  
@param[out] 
@return     
************/
C_spatialConfiguration::C_spatialConfiguration(void)
{
	phaseInterval.QuadPart = 1000;
	for(int i=0; i<SUM_SERVOMOTORS; i++)
	{
		serv_fixedPositioning[i] = false;
		serv_intervalOne_changed[i] = false;
		serv_intervalOne[i].QuadPart = 0;
	}
}

/****************************************************************************
@function   C_spatialConfiguration
@class		C_spatialConfiguration
@brief      constructor - input parameters values => member variables
@param[in]  LARGE_INTEGER* a_phaseInterval | time of this new phase
			LARGE_INTEGER* a_serv_intervalOne | intervalOne for each thread of this new phase
			bool a_serv_fixedPositioning = false | defaultly linear positioning - ramp
@param[out] 
@return     
************/
C_spatialConfiguration::C_spatialConfiguration(
	LARGE_INTEGER* a_phaseInterval, LARGE_INTEGER* a_serv_intervalOne, bool a_serv_fixedPositioning){
	phaseInterval.QuadPart = a_phaseInterval->QuadPart;
	for(int i=0; i<SUM_SERVOMOTORS; i++)
	{
		serv_intervalOne_changed[i] = true;
		serv_intervalOne[i].QuadPart = a_serv_intervalOne[i].QuadPart;
	}
	a_serv_fixedPositioning = false;
}

/****************************************************************************
@function   ~C_spatialConfiguration
@class		C_spatialConfiguration
@brief      destructor
@param[in]  
@param[out] 
@return     
************/
C_spatialConfiguration::~C_spatialConfiguration(void){
	//nope delete[] serv_intervalOne;
}

/****************************************************************************
@function   SET_serv_intervalOne
@class		C_spatialConfiguration
@brief      
@param[in]  
@param[out] 
@return     
************/
void C_spatialConfiguration::SET_serv_intervalOne(int a_i_serv, LARGE_INTEGER *a_serv_intervalOne)
{
	serv_intervalOne[a_i_serv].QuadPart = a_serv_intervalOne->QuadPart;
	serv_intervalOne_changed[a_i_serv] = true;
}


