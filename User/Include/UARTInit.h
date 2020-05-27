/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : UARTInit.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/
#ifndef __UARTINIT_H_
#define __UARTINIT_H_

typedef struct
{
	uint16     R_DATA[10];//={0,0,0,0,0,0,0,0,0,0};
  uint16     T_DATA[10];//={0,0,0,0,0,0,0,0,0,0};
	uint16   Uredata;
  uint16   UARxCnt;
  uint16   RxFSM;
  uint16   UsaRxLen;
  uint16   flagUsaRxComm;
  uint32   CheckDate;
	uint16   ResponceCount;
	uint8    ResponceFlag;
}MCUART;

extern MCUART Uart;
extern void UART_Init(void);
extern void UART_SendData(uint8 T_Data);
extern void UartDealResponse(void);
extern void UartDealComm(void);

#endif