/***********
@project		roboArm
@filename	C_spatialConfiguration.cpp
@author		xdavid10, xslizj00 @ FEEC-VUTBR 
@date		2013_12_02
@brief		file containing member function definitions 
			of classes C_spatialConfiguration
***********/

#include "roboArm.h"

/****************************************************************************
@function   C_spatialConfiguration
@class		C_spatialConfiguration
@brief      constructor - implicite
@param[in]  
@param[out] 
@return     
************/
C_spatialConfiguration::C_spatialConfiguration(void){
	phaseInterval.QuadPart = 0;
	for(int i=0; i<SUM_SERVOMOTORS; i++)
	{
		servIntervalZeroChanged[i] = false;
		servIntervalZero[i].QuadPart = 0;
	}
}

/****************************************************************************
@function   C_spatialConfiguration
@class		C_spatialConfiguration
@brief      constructor - input parameters values => member variables
@param[in]  
@param[out] 
@return     
************/
C_spatialConfiguration::C_spatialConfiguration(LARGE_INTEGER* a_phaseInterval, LARGE_INTEGER* a_servIntervalZero){
	phaseInterval.QuadPart = a_phaseInterval->QuadPart;
	for(int i=0; i<SUM_SERVOMOTORS; i++)
	{
		servIntervalZeroChanged[i] = true;
		servIntervalZero[i].QuadPart = a_servIntervalZero[i].QuadPart;
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
	//nope delete[] servIntervalZero;
}

/****************************************************************************
@function   SET_servIntervalZero
@class		C_spatialConfiguration
@brief      
@param[in]  
@param[out] 
@return     
************/
void C_spatialConfiguration::SET_servIntervalZero(int a_i_serv, LARGE_INTEGER *a_servIntervalZero)
{
	servIntervalZero[a_i_serv].QuadPart = a_servIntervalZero->QuadPart;
	servIntervalZeroChanged[a_i_serv] = true;
}


