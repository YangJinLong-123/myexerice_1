/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : CMPInit.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains CMP initial function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>

/*-------------------------------------------------------------------------------------------------
	Function Name :	void CMP3_Iint(void)
	Description   :	比较器3初始化,用于硬件过流保护
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void CMP3_Init(void)
{
	#if (Shunt_Resistor_Mode == Single_Resistor)
	{
		SetBit(P2_AN, P27);	                               // CMP3

  	SetReg(CMP_CR1, CMP3MOD1|CMP3MOD0,0x0);
	}
	#elif (Shunt_Resistor_Mode == Double_Resistor)
	{
		SetBit(P2_AN, P27);	                               // CMP3
    SetReg(CMP_CR1, CMP3MOD1|CMP3MOD0,0x0);
	}
	#elif (Shunt_Resistor_Mode == Three_Resistor)
	{
		SetBit(P2_AN, P27 | P23 | P20);	                               // CMP3
		SetReg(CMP_CR1, CMP3MOD1|CMP3MOD0,CMP3MOD1|CMP3MOD0);

	}
	#endif	//end Shunt_Resistor_Mode
  #if (Compare_Mode == Compare_Hardware)
	{
    /**P2.6使能其模拟功能，使能数字输出**/
    SetBit(P2_AN, P26);
    SetBit(P2_OE, P26); 
    ClrBit(DAC_CR, DAC_EN );
  }
  #else
  {   
		SetBit(P2_AN, P26);
    ClrBit(P2_OE, P26); 
    
    /******************************
    0: 正常模式，DAC输出电压范围为0到VREF，适用于硬件过流保护
    1: 半电压转换模式，DAC输出电压范围为VHALF到VREF
    ****************************/
    ClrBit(DAC_CR, DACMOD);
    /**********设置DAC过流值*****************/
    DAC_DR = DAC_OvercurrentValue;

		/**********使能DAC******************/
    SetBit(DAC_CR, DACEN );
  }
  #endif	//end Compare_Mode

	SetBit(CMP_CR1, CMP3HYS, 1);                            // 比较器3有迟滞电压

/*-------------------------------------------------------------------------------------------------
	选择母线电流保护触发信号源，外部中断0或者比较器3中断。
	 1-INT0, 0-CMP3
-------------------------------------------------------------------------------------------------*/
	ClrBit(EVT_FILT, EFSRC);
/*-------------------------------------------------------------------------------------------------
	触发硬件保护后硬件关闭驱动输出MOE配置
	00--MOE不自动清零
	01--MOE自动清零
-------------------------------------------------------------------------------------------------*/
	SetReg(EVT_FILT, MOEMD0 | MOEMD1, MOEMD0);
//  SetReg(EVT_FILT, MOEMD0 | MOEMD1, 0x00);
/*-------------------------------------------------------------------------------------------------
	  母线电流保护时间滤波宽度
	  00-不滤波    01-4cpu clock    10-8cpu clock    11-16cpu clock
-------------------------------------------------------------------------------------------------*/
	SetReg(EVT_FILT, EFDIV0 | EFDIV1, EFDIV0|EFDIV1);

/*比较器使能-------------------------------------------------------------------------------------*/
	SetBit(CMP_CR1, CMP3EN);

}


void CMP3_Inter_Init(void)
{
  /*-------------------------------------------------------------------------------------------------
	比较器中断模式配置
	00: 不产生中断  01: 上升沿产生中断  10: 下降沿产生中断  11: 上升/下降沿产生中断
-------------------------------------------------------------------------------------------------*/
	ClrBit(CMP_SR, CMP3IF);
	SetReg(CMP_CR0, CMP3IM1 | CMP3IM0, CMP3IM0);

	PCMP1 = 1;
	PCMP0 = 1;									                             // 中断优先级别3

  EA = 1;
}

