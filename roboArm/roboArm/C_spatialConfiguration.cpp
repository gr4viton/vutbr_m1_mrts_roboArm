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
		servIntervalOneChanged[i] = false;
		servIntervalOne[i].QuadPart = 0;
	}
	i_phase = i_phase_max + 1;
	i_phase_max++;
}

/****************************************************************************
@function   C_spatialConfiguration
@class		C_spatialConfiguration
@brief      constructor - input parameters values => member variables
@param[in]  
@param[out] 
@return     
************/
C_spatialConfiguration::C_spatialConfiguration(LARGE_INTEGER* a_phaseInterval, LARGE_INTEGER* a_servIntervalOne){
	phaseInterval.QuadPart = a_phaseInterval->QuadPart;
	for(int i=0; i<SUM_SERVOMOTORS; i++)
	{
		servIntervalOneChanged[i] = true;
		servIntervalOne[i].QuadPart = a_servIntervalOne[i].QuadPart;
	}
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
	//nope delete[] servIntervalOne;
}

/****************************************************************************
@function   SET_servIntervalOne
@class		C_spatialConfiguration
@brief      
@param[in]  
@param[out] 
@return     
************/
void C_spatialConfiguration::SET_servIntervalOne(int a_i_serv, LARGE_INTEGER *a_servIntervalOne)
{
	servIntervalOne[a_i_serv].QuadPart = a_servIntervalOne->QuadPart;
	servIntervalOneChanged[a_i_serv] = true;
}


