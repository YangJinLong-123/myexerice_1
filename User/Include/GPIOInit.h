/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : PIOInit.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __PIOINIT_H_
#define __PIOINIT_H_


#define SPIncPin        GP00
#define SPDecPin        GP02
#define FRPin						GP03

#define PWMPin 					GP11

#define LEDPin			 		GP05
#define SetLEDPin				{GP05 = 1;}
#define ResetLEDPin			{GP05 = 0;}
#define LEDPinONOFF			{GP05 = ~GP05;}

#define YAOTOUPin			 		GP10
#define SetYAOTOUPin			{GP10 = 0;}
#define ResetYAOTOUPin		{GP10 = 1;}

#define FPin 				  GP01
#define SetFGPin			  {GP01 = 1;}
#define ResetFGPin		  {GP01 = 0;}
#define FPinONOFF      {GP01 = ~GP01;}

#define RDPin 				  GP01
#define SetRDPin			  {GP01 = 1;}
#define ResetRDPin		  {GP01 = 0;}

/* Exported functions ---------------------------------------------------------------------------*/
extern void GPIO_Init(void);

#endif