/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : UARTInit.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains UART initial function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/
#include <FU68xx_2.h>
#include <Myproject.h>
/* Private variables ----------------------------------------------------------------------------*/
MCUART Uart;

void UART_Init(void)
{
    SetBit(PH_SEL, UARTEN);     //p0[6]as UART_RXD; p0[5]as UART_TXD
    UT_MOD1 = 0;
    UT_MOD0 = 1;                //8bit波特率可变UART模式
    SM2 = 0;                    //禁止Mode2和Mode3多机通讯
    REN = 1;                    //使能接收
    ES0 = 0;                    //先关中断

    PUART1=0;                   //中断优先级时最低
    PUART0=0;

    UT_BAUD =0x9b;              //default baudrate:9600-0x9b,1200-0x4E1
    ES0 = 1;                    //发送/接受中断使能
}

void UartTxdate(uint16* sndBuf, int32 len)
{
    uint16 i=0;
  for(i=0;i<len;i++)
	{
		UART_SendData(*sndBuf++);
	}
}

void UART_SendData(unsigned char T_Data)
{
    UT_DR = T_Data;
    while(!(TI==1));        //等待发送完成
    TI = 0;                 //发送完成中断标志位清零
}

/***************处理串口接收到的数据************/
void UartDealResponse(void)
{
	
}

void UartDealComm(void)
{
	 uint16 j=0;
	 uint16 checkdate=0x00;
	 if(Uart.ResponceFlag==1)//10ms
	 {
			 UartDealResponse();
			 Uart.T_DATA[0]     = 0xaa;
			 Uart.T_DATA[1]     = mcState;
			 Uart.T_DATA[2]     = (uint16)(((int32)mcFocCtrl.SpeedFlt*2400)>>19);// actual speed/16 ,if actual speed=1000rpm/min,then TxBuf[2]=63
			 Uart.T_DATA[3]     = (uint16)(((int32)mcFocCtrl.mcDcbusFlt*626)>>16);// DC bus feedback, 1 stand for 2V
			 Uart.T_DATA[4]     = mcFaultSource;//Fault state
			 Uart.T_DATA[5]     = 0x00;
			 Uart.T_DATA[6]     = 0x00;
			 Uart.T_DATA[7]     = 0x00;
			 for( j = 0; j < 7; j++ )
			 {
				checkdate += Uart.T_DATA[j];
			 }
			 Uart.T_DATA[8]     = checkdate;
			 Uart.T_DATA[9]     = 0x55;
			 UartTxdate(Uart.T_DATA,10);
			 Uart.ResponceFlag=0;
	 }
}