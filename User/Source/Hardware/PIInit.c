
/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : PIInit.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains PI initial function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>

void PI_Init(void)
{
	PI_KP = SKP;
	PI_KI = SKI;
	PI_EK = 0;
	ClrBit(PI_LPF_CR, RANGE);																// 0:Q12; 1:Q15
	SetBit(PI_LPF_CR, PISTA);																// Start PI
	PI_UKMAX = SOUTMAX;
	PI_UKMIN = SOUTMIN;
	_nop_(); _nop_(); _nop_(); _nop_();
	PI_UK =   0;
	PI_LPF_CR &= 0xFD;																			// stop PI
}