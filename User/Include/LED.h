/**************************** (C) COPYRIGHT 2015 Fortiortech shenzhen *****************************
* File Name          : LED.h
* Author             : Billy Long Fortiortech  Market Dept
* Version            : V1.0
* Date               : 01/07/2015
* Description        : This file contains all the common data types used for Motor Control.
***************************************************************************************************
/**************************** (C) COPYRIGHT 2015 Fortiortech shenzhen *****************************
* File Name          : LED.h
* Author             : Billy Long Fortiortech  Market Dept
* Version            : V1.0
* Date               : 01/07/2015
* Description        : This file contains all the common data types used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/ 

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __LED_H_
#define __LED_H_
//#define PID_CONTROL_TEM

/* Exported types -------------------------------------------------------------------------------*/
typedef struct
{
	uint8 LEDCnt;
	uint8 FlagLED;
	uint8 FlagStepTime;
	uint16 LEDTimeCnt;
	
} LEDCtl_TypeDef;


#define D1              GP36
#define D2              GP37
#define D3              GP00
#define D4              GP01
#define D5              GP02
#define D6              GP03

/* Exported variables ---------------------------------------------------------------------------*/
extern LEDCtl_TypeDef LEDCtl;

/* Exported functions ---------------------------------------------------------------------------*/
extern void LEDControlInit(void);
extern void LEDControl(void);


#endif
