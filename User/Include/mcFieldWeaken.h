/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : mcFieldWeaken.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/ 


/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __mcFieldWeaken_H_
#define __mcFieldWeaken_H_

/* Exported types -------------------------------------------------------------------------------*/
typedef struct
{
		uint16 CompCnt;
		uint8  CtrlFlag;
		int16  IDrefCompRef;
		int16	 InFWTime;
		int16	 OpenFWUQ;
		int16	 ClosedFWUQ;
		int16  SpeedErr;
		int16	 SpeedErrTarget;
		int16	 SpeedOutFlt;
}FIELDWEAKENING;

/* Exported variables ---------------------------------------------------------------------------*/
extern FIELDWEAKENING 	xdata			mcFieldWeaken;	

/* Exported functions ---------------------------------------------------------------------------*/
extern void mcFieldWeakenComp(void);

#endif