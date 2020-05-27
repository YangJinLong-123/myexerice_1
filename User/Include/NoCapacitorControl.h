/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : NoCapacitorControl.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __NoCapacitorControl_H_
#define __NoCapacitorControl_H_

typedef struct
{
		int16	ExtRef;																					//
		int16	ExtFed;
	  int16	ExtKP;          																//
		int16	ExtKI;         																	//
	  int16	ExtErr;      																		//
//	  int16	ExtErrL;																				//
	  int16	ExtOut;    																			// 本次运算输出值
		int16	ExtOutMax;    																			// 本次运算输出值
		int16	ExtOutMin;    																			// 本次运算输出值
//		int16	ExtOutL;    																		// 上一次输出值
} OUTLOOP;


typedef struct
{
	  uint16	QPowerDC;          															//
		uint16	QPowerRef;          														//
	  uint16	QPowerFed;      																//
	  uint16	QPowerErr;																			//
	  uint16	QPowerOut;    																	//
} MCQPOWER;

extern OUTLOOP	xdata	SpeedPICtrl;
extern OUTLOOP	xdata	QAxisPOWPI;
extern MCQPOWER	xdata	PowerControl;


extern void OutLoopParameterClear(void);
extern void Qaxis_PowerContorl(void);
#endif