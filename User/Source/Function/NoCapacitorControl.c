/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : FocControl.c
* Author             : Vina Peng,Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 2017-12-26
* Description        : This file contains all the foc control framework used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>

/* Private variables ----------------------------------------------------------------------------*/
MCQPOWER			PowerControl;
OUTLOOP				SpeedPICtrl;
OUTLOOP				QAxisPOWPI;

/*---------------------------------------------------------------------------*/
/* Name     :   void OutLoopParameterClear(void)
/* Input    :   NO
/* Output   :   NO
/* Description: 
/*---------------------------------------------------------------------------*/
void OutLoopParameterClear(void)
{
	memset(&SpeedPICtrl,0, sizeof(OUTLOOP));																// SpeedControl«Â¡„
	memset(&QAxisPOWPI,0, sizeof(OUTLOOP));																	// QAxisPower«Â¡„
	
	SpeedPICtrl.ExtKP = SKP;
	SpeedPICtrl.ExtKI = SKI;

	QAxisPOWPI.ExtKP = SKP;
	QAxisPOWPI.ExtKI = SKI;
	
	PI_KP = SpeedPICtrl.ExtKP;
	PI_KI = SpeedPICtrl.ExtKI;
}

/*---------------------------------------------------------------------------*/
/* Name     :   void Qaxis_PowerContorl(void)
/* Input    :   NO
/* Output   :   NO
/* Description: 
/*---------------------------------------------------------------------------*/
void Qaxis_PowerContorl(void)
{
	SpeedPICtrl.ExtRef = mcSpeedRamp.ActualValue;
	SpeedPICtrl.ExtFed = mcFocCtrl.SpeedFlt;
	HW_TWO_PI(&SpeedPICtrl);
//	FOC_IQREF = SpeedPICtrl.ExtOut;
	
	MDU_MUL_XDATA_U16(SpeedPICtrl.ExtOut, mcFocCtrl.mcDcbusFlt, &QAxisPOWPI.ExtRef);
	QAxisPOWPI.ExtFed = mcFocCtrl.Powerlpf;
	HW_TWO_PI(&QAxisPOWPI);
	FOC_IQREF = QAxisPOWPI.ExtOut;
}

