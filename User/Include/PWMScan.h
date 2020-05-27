/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : PWMScan.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 2017-12-27
* Description        : This file contains main function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __PWMSCAN_H_
#define __PWMSCAN_H_


/* Exported types -------------------------------------------------------------------------------*/
typedef struct
{
	uint16 PWMINCnt;
	uint16 PWMINHigh;
	uint16 PWMINPeriod;
	uint16 PWMINHighDuty;
	uint16 PWMINFre;	
  uint16 PWMINTime;
	uint16 PWMSpeedRef;

} PWMFREQINPUTTRPE;

/* Exported variables ---------------------------------------------------------------------------*/
extern PWMFREQINPUTTRPE xdata mcPWMFreqInputCtrl;

/* Exported functions ---------------------------------------------------------------------------*/
//extern void PWMIN_Init(void);
extern void PWMScan(void);
//extern void FGScan(void);
#endif

