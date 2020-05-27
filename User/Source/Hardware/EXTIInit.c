/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : EXTIInit.c
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains EXTI init used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>

USER_TYPEDEF        User;

/*-------------------------------------------------------------------------------------------------
	Function Name :	void EXTI_Init(void)
	Description   :	外部中断配置
	Input         :	无
	Output		  :	无
-------------------------------------------------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EXTERN_Init
* Description    : P10
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI_Init(void)
{
  /*EXTI Input Pin 配置 ---------------------------------------- */
    ClrBit(P0_OE, P00);  // config P00 as input，配置P0.0为硬件过流FO端口
    ClrBit(P0_PU, P00);  // 0:disable pull up, 1:enable P10 pull up

  /*-------------------------------------------------------------------------------------------------
	外部中断模式配置
  00: 上升沿产生中断  01: 下降沿产生中断  1x: 上升/下降沿产生中断
-------------------------------------------------------------------------------------------------*/
    IF0	 = 0;								// clear P00 interrupt flag
    IT01 = 0;
    IT00 = 1;								// 00: rising edge interrupt，01: failing edge interrupt，1x: edge-change mode interrupt

  /*-------------------------------------------------------------------------------------------------
	外部中断优先级配置及芯片中断总使能
	PX01-PX00，中断优先级控制值从0-3依次表示优先级从最低到最高，共4级优化级控制
	EX0，外部中断0使能
	EA,芯片中断总使能
	外部中断是否有滤波的配置，决定于选择硬件保护触发信号源，外部中断0或者比较器3中断。
	若选择了比较器3中断，则外部中断无法进行滤波配置
	-------------------------------------------------------------------------------------------------*/
		PX01 = 1;
		PX00 = 1;								// 中断优先级别3，中断优先级最高,
		EX0 = 1;								// 使能外部中断0, POO位外部中断输入硬件
		EA = 1;									// 使能全局中断

}
void Sleepmode_Init(void)
{
	ClrBit(P1_OE, P11);  // config P11 as input
	ClrBit(P1_PU, P11);  // diasable P11 Pull Up
	// ClrBit(P1_AN, P11);  // diasable P11 analog function
	ClrBit(P1_IF, P11);         // clear P11 interrupt flag
	ClrBit(P1_IE, P11);   // config P11 as the source of EXTI1

	IT11 = 1;
	IT10 = 0;								// 00: posedge mode interrupt，01: negedge mode interrupt，1x: edge-change mode interrupt

	PX11 = 0;
	PX10 = 0;								// 中断优先级别0，中断优先级最高

	EX1 = 1;								// 使能外部中断1, P11位外部中断输入硬件
	EA = 1;									// 使能全局中断
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	ZeroCrossing_Init(void)
	Description   :	外部中断配置
	Input         :	供电频率
	Output		    :	无
-------------------------------------------------------------------------------------------------*/
void ZeroCrossing_Init(void)
{
	ClrBit(P2_OE, P20);  // config P20 as input
	ClrBit(P2_PU, P20);  // diasable P20 Pull Up
  ClrBit(P2_AN, P20);  // diasable P20 analog function
	
	ClrBit(P2_IF, P20);         // clear P11 interrupt flag
	SetBit(P2_IE, P20);   // config P11 as the source of EXTI1

	IT11 = 0;
	IT10 = 1;								// 00: posedge mode interrupt，01: negedge mode interrupt，1x: edge-change mode interrupt

	PX11 = 0;
	PX10 = 1;								// 中断优先级别0，中断优先级最高

	EX1 = 1;								// 使能外部中断1, P20位外部中断输入硬件
	EA = 1;									// 使能全局中断
	
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	TemperaturePID(void)
	Description   :	温度PI调节
	Input         :	供电频率
	Output		    :	无
-------------------------------------------------------------------------------------------------*/
void TemperaturePID(void)
{
	
#if 1
//  if((abs(User.TemperatureEkL) > 500))
//	{
		PI_KP 		= _Q12(0.6);
		PI_KI 		= _Q12(0.01);
		PI_EK     = User.TemperatureEkL;
		
		SetBit(PI_LPF_CR,PISTA);																										// Start PI
		_nop_();  _nop_();  _nop_();  _nop_();  _nop_();
		
		PI_UKMAX 	= User.ScrOnAngleTime +1000;
		PI_UKMIN 	= User.ScrOnAngleTime -800;
		PI_UK     = User.ScrOnAngleTimeDatum;	
		User.TemperatureEkL   = User.TemperatureDatum - User.Temperature;
		PI_EK     = User.TemperatureEkL;
		
		SetBit(PI_LPF_CR,PISTA);																										// Start PI
		_nop_();  _nop_();  _nop_();  _nop_();  _nop_();
		
		User.ScrOnAngleTimeDatum  = PI_UK;			
//	}
#endif
}