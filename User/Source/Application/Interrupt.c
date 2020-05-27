/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : Interrupt.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 2017-12-26
* Description        : This file contains all the interrupt function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
// #include <Interrupt.h>
#include <FU68xx_2.h>
#include <Myproject.h>
uint16 xdata spidebug[4] = { 0 };
uint8  ExternFlag = 0;
uint8  ReadtimeFlag = 0;
uint16 Temperature;

uint32 Temp_value;
uint32 Temp_value_1;
uint32 Temp_value_2;
uint32	Mother_line_Power;
uint8		Mother_line_Power_num;
uint16	Mother_line_Power_averge;


uint16 code ACOS[181] = { 0,149 ,211 ,259 ,299 ,335 ,367 ,397 ,425 ,451 ,476 ,499 ,522 ,544 ,565 ,585 ,605 ,624 ,643 ,661 ,679 ,697 , 714 , 731 , 747 ,//0-26
													763 ,779 , 795 , 810 , 826 , 841 , 855 , 870 , 884 , 899 , 913 ,927 , 941 , 954 , 968 , 981 , 995 , 1008 , 1021 , 1034 , 1047,//27-47
													1059 , 1072 , 1085 , 1097 , 1110 , 1122 , 1134 , 1146 , 1159 ,1171 , 1183 , 1195 , 1207 , 1218 , 1230 , 1242 , 1254 , 1265 , //48-65
													1277 , 1289 , 1300 , 1312 , 1323 , 1335 , 1346 , 1357 , 1369 ,1380 , 1391 , 1403 , 1414 , 1425 , 1436 , 1447 , 1459 , 1470 ,//66-83
													1481 , 1492 , 1503 , 1514 , 1525 , 1536 , 1548 , 1559 , 1570 ,1581 , 1592 , 1603 , 1614 , 1625 , 1636 , 1648 , 1659 , 1670 , 
													1681 , 1692 , 1703 , 1715 , 1726 , 1737 , 1748 , 1760 , 1771 ,1782 , 1794 , 1805 , 1816 , 1828 , 1839 , 1851 , 1863 , 1874 ,
													1886 , 1897 , 1909 , 1921 , 1933 , 1945 , 1957 , 1969 , 1981 ,1993 , 2005 , 2017 , 2030 , 2042 , 2055 , 2067 , 2080 , 2093 , 
													2106 , 2118 , 2132 , 2145 , 2158 , 2171 , 2185 , 2199 , 2212 ,2226 , 2240 , 2255 , 2269 , 2284 , 2298 , 2313 , 2328 , 2344 ,
													2359 , 2375 , 2391 , 2408 , 2425 , 2442 , 2459 , 2477 , 2495 ,2514 , 2533 , 2553 , 2573 , 2594 , 2616 , 2638 , 2662 , 2686 ,
													2712 , 2740 , 2769 , 2801 , 2836 , 2875 , 2921 , 2980 ,3078 };



/*-------------------------------------------------------------------------------------------------
    Function Name : void FO_INT(void)
    Description   : FO_INT interrupt，硬件FO过流保护，关断输出，中断优先级最高
    Input         : 无
		Output        : 无
-------------------------------------------------------------------------------------------------*/
void FO_INT(void) interrupt 1                                                   // 硬件FO过流中断，关闭输出
{
    FaultProcess();                                                             // 关闭输出
    mcFaultSource = FaultHardOVCurrent;                                         // 硬件过流保护
    mcState       = mcFault;                                                    // 状态为mcFault
    IF0           = 0;                                                          // clear P00 interrupt flag
}
/*-------------------------------------------------------------------------------------------------
    Function Name : void CMP_ISR(void)
    Description   : CMP3：硬件比较器过流保护，关断输出，中断优先级最高
										CMP0/1/2：顺逆风判断
    Input         : 无
		Output        : 无
-------------------------------------------------------------------------------------------------*/
void CMP_ISR(void) interrupt 7
{
    if(ReadBit(CMP_SR, CMP3IF))
    {
        if(mcState!=mcPosiCheck)
        {
            FaultProcess();                                                     // 关闭输出
            mcFaultSource=FaultHardOVCurrent;                                   // 硬件过流保护
            mcState = mcFault;                                                  // 状态为mcFault
        }
        else
        {
            MOE     = 0;                                                                        // 关闭MOE
            RPDPara.InsetCount[RPDPara.injecttimes]  = TIM2__CNTR;                              // 将定时器2的计数值赋值给数组
            RPDPara.DetectCount[RPDPara.injecttimes] = RPDPara.InsetCount[RPDPara.injecttimes]; // 两组数据，一组用于观察原始数据，一组用于处理数据
            TIM2__CNTR                               = 0;                                       // TIM2计数器值清零
            RPDPara.injecttimes++;                                                              // RPD注入拍数累加

        }
        ClrBit(CMP_SR, CMP3IF);
    }

    #if (FRDetectMethod == BEMFMethod)
        //通过BEMF做顺风启动功能
        BEMFDetectFunc();
    #endif
}

/*---------------------------------------------------------------------------*/
/* Name     :   void TIM2_INT(void) interrupt 4
/* Input    :   NO
/* Output   :   NO
/* Description:	正反转检测(RSD)
/*---------------------------------------------------------------------------*/
void TIM2_INT(void) interrupt 4
{
    if(ReadBit(TIM2_CR1, T2IR))
    {
        ClrBit(TIM2_CR1, T2IR);
    }
    if(ReadBit(TIM2_CR1, T2IP))
    {
        #if (FRDetectMethod == RSDMethod)
            RSDFRDetect();																											//RSD正反转检测
        #endif

        ClrBit(TIM2_CR1, T2IP);
    }
    if(ReadBit(TIM2_CR1, T2IF))																									//溢出中断,用于判断静止,时间为349ms。
    {
        #if (FRDetectMethod == RSDMethod)
            RSDDetect.RSDState=Static;
            RSDDetect.RSDFlag=1;
        #endif

        ClrBit(TIM2_CR1, T2IF);
    }
		
}
/*---------------------------------------------------------------------------*/
/* Name     :   void TIM23_INT(void) interrupt 9
/* Input    :   NO
/* Output   :   NO
/* Description: Capture PWM ，中断优先级第二，高于FOC中断，用于PWM调速
/*---------------------------------------------------------------------------*/
void TIM3_INT(void) interrupt 9
{
    if(ReadBit(TIM3_CR1, T3IR))
    {
			 GP11 = 1;
       ClrBit(TIM3_CR1, T3IR);
    }
    if(ReadBit(TIM3_CR1, T3IP))																									//周期中断
    {
			//GP11 = 0;
       ClrBit(TIM3_CR1, T3IP);
    }
    if(ReadBit(TIM3_CR1, T3IF))
    {
			 GP11 = 0;
       ClrBit(TIM3_CR1, T3EN);				    //关闭计数器			
       ClrBit(TIM3_CR1, T3IF);
    }
}
/*---------------------------------------------------------------------------*/
/* Name     :   void FOC_INT(void) interrupt 3
/* Input    :   NO
/* Output   :   NO
/* Description: FOC中断(Drv中断),每个载波周期执行一次，用于处理响应较高的程序，中断优先级第二。DCEN开了就会产生中断。
/*---------------------------------------------------------------------------*/
void FOC_INT(void) interrupt 3
{
    if(ReadBit(DRV_SR, DCIF))                               										// 比较中断
    {
        APP_DIV(); 																															//启动除法器，避免与过调值中的除法冲突

        Fault_Overcurrent(&mcCurVarible); 																			// 软件过流保护.约30us

        #if ((FRDetectMethod == FOCMethod) && (TailWind_Mode == TailWind))
            TailWindSpeedDetect();																							//顺逆风检测
        #endif

        #if defined (SPI_DBG_SW)																								//软件调试模式
            spidebug[0] = mcFocCtrl.mcDcbusFlt<<1;															//SPIDATA0;
            spidebug[1] = FOC__IA;																							//SPIDATA1;
            spidebug[2] = FOC__IB;																							//SPIDATA2;
            spidebug[3] = FOC__THETA;																						//SPIDATA3;
        #endif

        ClrBit(DRV_SR, DCIF);
    }
}



/*---------------------------------------------------------------------------*/
/* Name     :   void TIM_1MS_INT(void) interrupt 10
/* Input    :   NO
/* Output   :   NO
/* Description: 1ms定时器中断（SYS TICK中断），用于处理附加功能，如控制环路响应、各种保护等。中断优先级低于FO中断和FOC中断。
/*---------------------------------------------------------------------------*/
void TIM_1MS_INT(void) interrupt 10
{
    if(ReadBit(DRV_SR, SYSTIF))          																				// SYS TICK中断
    {
        SetBit(ADC_CR, ADCBSY);          																				//使能ADC的DCBUS采样
        /****功率滤波*****/
        if(mcState == mcRun)
        {
            mcFocCtrl.CurrentPower = FOC__POW << 1;
            mcFocCtrl.Powerlpf     = LPFFunction(mcFocCtrl.CurrentPower,mcFocCtrl.Powerlpf,20);			//注意低通滤波器系数范围为0---127
        }

        /****速度滤波、反电动势滤波*****/
        if((mcState != mcInit) && (mcState != mcReady))
        {
            mcFocCtrl.SpeedFlt = LPFFunction(FOC__EOME, mcFocCtrl.SpeedFlt, 30);							//100			//注意低通滤波器系数范围为0---127
            mcFocCtrl.EsValue  = LPFFunction(FOC__ESQU,mcFocCtrl.EsValue,10);
        }
        else
        {
            mcFocCtrl.SpeedFlt = 0;
        }
				
				/****UQ电压值滤波****/
				mcFocCtrl.UqFlt = LPFFunction(FOC__UQ,mcFocCtrl.UqFlt,10);							// UQ值
				mcFocCtrl.UdFlt = LPFFunction(FOC__UD,mcFocCtrl.UdFlt,10);							// UD值

        Speed_response();        																								//环路响应，如速度环、转矩环、功率环等
								
        KeyScan();                                                              //获取按键值

				
				if(KS.ChangeKeyFlg)                                                     //更新LED显示
				{
				  LEDControl();
					KS.ChangeKeyFlg = 0;
				}
				
        #if (StartONOFF_Enable)
            ONOFF_Starttest(&ONOFFTest);
        #endif

        /*****DCbus的采样获取值并滤波******/
//        AdcSampleValue.ADCDcbus = ADC2_DR;
//        mcFocCtrl.mcDcbusFlt    = AdcSampleValue.ADCDcbus << 3;
        if(mcState == mcRun)                               
        {
            mcFocCtrl.mcDcbusFlt=FOC__UDCFLT;
        }
        else
        {
            AdcSampleValue.ADCDcbus = ADC2_DR;
            mcFocCtrl.mcDcbusFlt    = AdcSampleValue.ADCDcbus << 3;
        }					
				
				/***********温度采集***********/
				Temperature        = ADC1_DR<<3 ;
				User.Temperature = LPFFunction(Temperature ,User.Temperature,10);							// 

        /*****故障保护函数功能，如过欠压保护、启动保护、缺相、堵转等********/
        Fault_Detection();

//        LED_Display();//LED灯显示

        /****模拟调速****/
        #if (SPEED_MODE == SREFMODE)
//            VSPSample();
        #endif

        /********睡眠模式*******/
//            Sleepmode();

        /*****电机状态机的时序处理*****/
        if(mcFocCtrl.State_Count > 0)    			mcFocCtrl.State_Count--;
        if(BEMFDetect.BEMFTimeCount > 0) 			BEMFDetect.BEMFTimeCount--;
        if(RSDDetect.RSDCCWSBRCnt > 0)     		RSDDetect.RSDCCWSBRCnt--;
				
				Time.UnderVoltageCount ++;
				if(Time.UnderVoltageCount >200)	
				{
					Time.UnderVoltageCount = 200;
				}
				if( mcState == mcRun )
		    {
				  if(mcFocCtrl.TPCtrlDealy > 0)         mcFocCtrl.TPCtrlDealy--;
				}

        #if (FRDetectMethod==FOCMethod)
            FOCTailWindTimeLimit();
        #endif

        /*****电机启动爬坡函数处理*****/
        StarRampDealwith();

        ClrBit(DRV_SR, SYSTIF);                             										// 清零标志位
    }
		
		
		
		if(ReadBit(TIM4_CR1, T4IR))
    {
      ClrBit(TIM4_CR1, T4IR);
    }
    if(ReadBit(TIM4_CR1, T4IP))																									//周期中断
    {
			ClrBit(TIM4_CR1, T4IP);
    }
    if(ReadBit(TIM4_CR1, T4IF))
    {
      ClrBit(TIM4_CR1, T4IF);
    }
}


/*---------------------------------------------------------------------------*/
/* Name     :   void USART_INT(void) interrupt 12
/* Input    :   NO
/* Output   :   NO
/* Description: 串口中断，中断优先级最低，用于接收调速信号,无中断插入时8us
/*---------------------------------------------------------------------------*/
void USART_INT(void)  interrupt 12
{
    if(RI == 1)
    {
        RI = 0;
        Uart.Uredata= UT_DR;            																				//读接收数据
    }
}
/*---------------------------------------------------------------------------*/
/* Name     :   void EXTERN_INT(void) interrupt 2
/* Input    :   NO
/* Output   :   NO
/* Description: 过零点检测
/*---------------------------------------------------------------------------*/
void EXTERN_INT(void) interrupt 2
{
	 ClrBit(TIM3_CR1, T3EN);				    //关闭计数器
			
	//用于检测外部中断的高电平时间
	if((mcState != mcRun)||(mcState != mcStart))
	{
		if(ExternFlag == 0)
		{
			IT11 = 0;
			IT10 = 1;								// 00: posedge mode interrupt，01: negedge mode interrupt，1x: edge-change mode interrupt
			ExternFlag =1;
			User.EdgeLevelTime =0;
			SetBit(TIM4_CR1, T4EN);				    //使能计数器，启动计数
		}
		else
		{
			if(ReadtimeFlag ==0)
			{
				User.EdgeLevelTime = TIM4__CNTR;
				ClrBit(TIM4_CR1, T4EN);				    //关闭计数器
				TIM4__CNTR = 0;
				ReadtimeFlag = 1;
			}
		}
  }
	
	//GP06 = 0;

/*
 #define Temperature_Off                (260000)          
 #define Temperature_LOW                (18000)
 #define Temperature_Mid                (15000)
 #define Temperature_High               (12400)	
*/
	
	GP11 = 0;
	if((mcFocCtrl.TPCtrlDealy ==0)&&(mcState == mcRun)&&(Temp_value != 2600)&&(mcFaultSource == 0)&&(mcFocCtrl.SpeedFlt > 8000)&&(mcFocCtrl.SpeedFlt < 18000) )
	{  
	  //TemperaturePID();
	  //TIM3__ARR = User.ScrOnAngleTimeDatum;
		
		
		
		Mother_line_Power+=mcFocCtrl.mcDcbusFlt;
		Mother_line_Power_num++;
		if(Mother_line_Power_num>=50)
		{
				Mother_line_Power_num=0;
				Mother_line_Power_averge=Mother_line_Power/50;
				Mother_line_Power=0;
				Temp_value_2=Temp_value*1200;
				Temp_value_1 = (ACOS[180-((Temp_value_2)/((Mother_line_Power_averge/90)*(Mother_line_Power_averge/90)))])*7+1000;
				if(Temp_value_1<=1600)
				{
						Temp_value_1=1600;
				}
				if(Temp_value_1>=18000)
				{
						Temp_value_1=18000;
				}
							
		}
		
			TIM3__ARR = Temp_value_1;
		//TIM3__ARR = Temp_value;
		TIM3__DR  = TIM3__ARR - 800;	
		TIM3__CNTR = 0;
	
	  SetBit(TIM3_CR1, T3EN);				    //开启计数器
		
	}
	if((mcState == mcFault)&&(mcFaultSource == FaultUnderVoltage)&& (Time.UnderVoltageCount > 80))
	{
		mcState = mcReady;
		mcFaultSource=FaultNoSource;
		mcFaultDect.VoltRecoverCnt = 0;
	}

   Time.UnderVoltageCount =0; 
   ClrBit(P2_IF, P20);                    																			// 清零P20标志位
}


/* Private variables ----------------------------------------------------------------------------*/
void INT0(void) interrupt 0
{
}
void INT5(void) interrupt 5
{
}
void INT6(void) interrupt 6
{
}
void INT8(void) interrupt 8
{
}
void INT11(void) interrupt 11
{
}
void INT13(void) interrupt 13
{
}
void INT14(void) interrupt 14
{
}
void INT15(void) interrupt 15
{
}




