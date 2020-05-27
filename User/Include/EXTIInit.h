/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : EXTIInit.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __EXTIINIT_H_
#define __EXTIINIT_H_


#if (HallMode == HallIC)
	#define	HA	P14
	#define	HB	P16
	#define	HC	P21

#elif (HallMode == HallSensor)
	#define	HA	ReadBit(CMP_SR, CMP0_OUT)
	#define	HB	ReadBit(CMP_SR, CMP1_OUT)
	#define	HC	ReadBit(CMP_SR, CMP2_OUT)

#endif

typedef struct
{  
		uint16 ScrOnAngleTimeDatum;  //期望给定 导通延迟时间  单位为  1/载波频率
	  uint16 ScrOnAngleTime;       //实际给定 导通延迟时间  单位为  1/载波频率
	
	  uint16 Temperature;                //温度值
	  uint16 TemperatureDatum;           //温度期望值
	  uint16 TemperatureDatumRamp0;      //温度期望值爬坡输入
	  uint16 TemperatureDatumRamp1;      //温度期望值爬坡输出
	
	  uint16 TemperatureLast;            //上一次的温度值
	  int16  TemperatureEkL;        //前一次  温度误差
	  int16  TPCtrlPIOutputL;       //前一次  PI计算输出值
	  int16  TPCtrlPIOutput;        //当  前  PI计算输出值
	  int16  EdgeLevelTime;         //外部中断的沿电平时间
	
}USER_TYPEDEF;

/* Exported variables ---------------------------------------------------------------------------*/
extern USER_TYPEDEF  User;

/* Exported functions ---------------------------------------------------------------------------*/
extern void EXTI_Init(void);
extern void Sleepmode_Init(void);
extern void ZeroCrossing_Init(void);

#endif


