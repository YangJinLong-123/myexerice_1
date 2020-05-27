/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : AMPInit.c
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 01/04/2017
* Description        : This file contains AMP initial function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>

/*-------------------------------------------------------------------------------------------------
	Function Name :	void AMP_Init(void)
	Description   :	AMP初始化配置,使能运放电压VHALF，配置运放的电流采样正向输入，反向输入和输出，包括I_BUS,I_U,I_V
	                并使能对应的运放
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void AMP_Init(void)
{
	/**使能VHALF电压**/
	SetBit(VREF_VHALF_CR, VHALFEN);												// 使能VHALF

  /*AMP管脚配置*/
 	SetBit(P2_AN, P27);            
	SetBit(P3_AN, P30|P31);                           // AMP0--I_BUS   

//  SetBit(P1_AN, P16|P17);
//  SetBit(P2_AN, P20);                               // AMP1--I_U

  SetBit(P2_AN, P21|P22|P23);                       // AMP2--I_V

  /*运放使能*/
  	#if (Shunt_Resistor_Mode == Single_Resistor)	//单电阻电流采样			      
	  {
	    SetBit(AMP_CR, AMP0EN);
		}
		#else	      																	//双/三电阻电流采样
    {
     	SetBit(AMP_CR, AMP0EN|AMP1EN|AMP2EN);       // 使能运放0、1、2 
		}
		#endif

}