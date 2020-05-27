/**************************** (C) COPYRIGHT 2015 Fortiortech shenzhen *****************************
* File Name          : KeyScan.h
* Author             : Billy Long Fortiortech  Market Dept
* Version            : V1.0
* Date               : 01/07/2015
* Description        : This file contains all the common data types used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/ 

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __KEYSCAN_H_
#define __KEYSCAN_H_

//#define Key0 	          SpeedPin	
//#define Key1 	          FRPin

#define SW3				      GP13   //冷风按键 冷风

#define SW1             GP15   //按键SW1  控制温度

#define SW2             GP17   //按键SW2  控制转速


/* Exported types -------------------------------------------------------------------------------*/
typedef struct
{
	uint8 Key1Value;
	uint8 Key2Value;
	uint8 Key3Value;
	
	uint8 FlagKey0Press;
	
	
	int16 Key1PressCnt;
	int16 Key2PressCnt;
	int16 Key3PressCnt;
	int16 Key3releaseCnt;
	int16 Key0PressCnt;
	
	int16 KeyValuetotal;
	int16 OldKeyValuetotal;
	uint8 ChangeKeyFlg;
	uint8 FlashWriteOnetimes;
	
	uint8 FlagKey1Press;
	
} KeyScanParam_TypeDef;

typedef struct
{
	uint8 FlagSpeed;
  uint32 TargetSpeed;
	uint16 TargetDuty;
	uint32 TargetPower;
	
} KeyControl_TypeDef;

/* Exported variables ---------------------------------------------------------------------------*/
extern KeyScanParam_TypeDef xdata KS;
extern KeyControl_TypeDef xdata KeyControl;


/* Exported functions ---------------------------------------------------------------------------*/
extern void KeyInit(void);
extern int KeyValue(void);
extern void KeyScan(void);

#endif

