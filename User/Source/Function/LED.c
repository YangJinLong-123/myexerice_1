/**************************** (C) COPYRIGHT 2015 Fortiortech shenzhen *****************************
* File Name          : LED.c
* Author             : Billy Long Fortiortech  Market Dept
* Version            : V1.0
* Date               : 01/07/2015
* Description        : This file contains LPF control function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/ 


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>

/* Private variables ----------------------------------------------------------------------------*/
LEDCtl_TypeDef LEDCtl;
uint8 temp_step;

extern char SW3_Flag;

extern uint32 Temp_value;

uint8 current_temp_control_level;
//extern USER_TYPEDEF     User;
/*-------------------------------------------------------------------------------------------------
	Function Name :	void LEDControlInit(void)
	Description   :	    
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void LEDControlInit(void)
{
	LEDCtl.LEDCnt = 0;
	LEDCtl.FlagLED = 0;
	LEDCtl.FlagStepTime = 0;
	LEDCtl.LEDTimeCnt = 0;
	
	current_temp_control_level = 0;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	void LEDControl(void)
	Description   :	功能函数，LED控制，显示系统状态
                  Stop状态--LED长灭
                  Normal状态--LED长亮
                  Fault状态--LED闪烁        
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void LEDControl(void)
{
	if((mcState != mcFault))
	{
	 //if((KS.KeyValuetotal & 0x80) == 0)                     //SW3 没有按下
		if(KS.Key3Value == 0x00)
	 { 
		 switch(KS.KeyValuetotal & 0x70)
		 {
			 case 0x10:                          //判定转速
						switch(KS.KeyValuetotal & 0x0F)           //判定温度
						{
						 case 0x01:
									D1 = 1;
									D2 = 1;
									D3 = 1;
									D4 = 0;                      //D4亮
									D5 = 1;
									D6 = 1;
									User.ScrOnAngleTime  = Temperature_Off;		//1
									break;
						 case 0x02:
									D1 = 1;
									D2 = 1;
									D3 = 0;
									D4 = 0;
									D5 = 1;
									D6 = 1;

									User.ScrOnAngleTime  = Temperature_LOW_3;		//2
									User.TemperatureDatum= Temperature_ADCLOW;
									break;
						 case 0x04:
									D1 = 1;
									D2 = 0;
									D3 = 0;
									D4 = 0;
									D5 = 1;
									D6 = 1;
									User.ScrOnAngleTime  = Temperature_Mid_3;		//3
									User.TemperatureDatum= Temperature_ADCMid;
									break;
						 case 0x08:
									D1 = 0;
									D2 = 0;
									D3 = 0;
									D4 = 0;
									D5 = 1;
									D6 = 1;
									User.ScrOnAngleTime = Temperature_High_3;		//4
									User.TemperatureDatum= Temperature_ADCHigh;
									break;
						 default:
							 break;
						}
						mcSpeedRamp.TargetValue = Motor_Speed_Low;		    // 转速赋值			 
						break;
				case 0x20:
						switch(KS.KeyValuetotal & 0x0F)
						{
							case 0x01:
									D1 = 1;
									D2 = 1;
									D3 = 1;
									D4 = 0;
									D5 = 0;
									D6 = 1;

									User.ScrOnAngleTime  = Temperature_Off;

									break;
							case 0x02:
									D1 = 1;
									D2 = 1;
									D3 = 0;
									D4 = 0;
									D5 = 0;
									D6 = 1;

									User.ScrOnAngleTime  = Temperature_LOW_2;
									User.TemperatureDatum= Temperature_ADCLOW;						 
									break;
							case 0x04:
									D1 = 1;
									D2 = 0;
									D3 = 0;
									D4 = 0;
									D5 = 0;
									D6 = 1;

									User.ScrOnAngleTime  = Temperature_Mid_2;
									User.TemperatureDatum= Temperature_ADCMid;

									break;
							case 0x08:
									D1 = 0;
									D2 = 0;
									D3 = 0;
									D4 = 0;
									D5 = 0;
									D6 = 1;

									User.ScrOnAngleTime  = Temperature_High_2;
									User.TemperatureDatum= Temperature_ADCHigh;
									 
									break;
							default:
									break;
						}				 
						mcSpeedRamp.TargetValue   = Motor_Speed_Mid;           // 转速赋值					 
						break;
			
			case 0x40:
				switch(KS.KeyValuetotal & 0x0F)
			   {
			     case 0x01:
								D1 = 1;
								D2 = 1;
								D3 = 1;
								D4 = 0;
								D5 = 0;
								D6 = 0;

								User.ScrOnAngleTime  = Temperature_Off;

								break;
					 case 0x02:
								D1 = 1;
								D2 = 1;
								D3 = 0;
								D4 = 0;
								D5 = 0;
								D6 = 0;

								User.ScrOnAngleTime  = Temperature_LOW_1;
								User.TemperatureDatum= Temperature_ADCLOW;

								break;
					 case 0x04:
								D1 = 1;
								D2 = 0;
								D3 = 0;
								D4 = 0;
								D5 = 0;
								D6 = 0;
								User.ScrOnAngleTime  = Temperature_Mid_1;
								User.TemperatureDatum= Temperature_ADCMid;
								break;
					 case 0x08:
								D1 = 0;
								D2 = 0;
								D3 = 0;
								D4 = 0;
								D5 = 0;
								D6 = 0;
								User.ScrOnAngleTime  = Temperature_High_1;
								User.TemperatureDatum= Temperature_ADCHigh;
								break;
					 default:
						 break;
				 }
				 mcSpeedRamp.TargetValue = Motor_Speed_HIgh;           // 转速赋值	
				break;		

			default:
				 break;
		 }
	 }
	 else
	 {
			User.ScrOnAngleTime  = Temperature_Off;
	 
		 switch(KS.KeyValuetotal & 0x70)
		 {
			 case 0x10:
				 D1 = 1;
				 D2 = 1;
				 D3 = 1;
				 D4 = 0;
				 D5 = 1;
				 D6 = 1;

         mcSpeedRamp.TargetValue = Motor_Speed_Low;		         // 转速赋值		 
				 break;
			 case 0x20:
				 D1 = 1;
				 D2 = 1;
				 D3 = 1;
				 D4 = 0;
				 D5 = 0;
				 D6 = 1;	

         mcSpeedRamp.TargetValue = Motor_Speed_Mid;		        // 转速赋值		 
				 break;
			 case 0x40:
				 D1 = 1;
				 D2 = 1;
				 D3 = 1;
				 D4 = 0;
				 D5 = 0;
				 D6 = 0;

         mcSpeedRamp.TargetValue = Motor_Speed_HIgh;	        // 转速赋值			 
				 break;
			 default:
				 break;
	  }
	 }
 } 
 
 
  //User.ScrOnAngleTimeDatum  = User.ScrOnAngleTime	;
 Temp_value = User.ScrOnAngleTime;
  #if (Correct_optical_Error ==1)
     {
			 User.ScrOnAngleTimeDatum =User.ScrOnAngleTimeDatum +(ActualEdgeLevelTime - User.EdgeLevelTime);
     }
	#endif
		 
		 
//	if(MC.MotorStatus == MOTOR_FAULT)
//	{
//      if(MC.StauteCtl.LedFaultCnt < 200)
//			{
//				 D1 = 0;
//				 D2 = 0;
//				 D3 = 0;
//				 D4 = 0;
//				 D5 = 0;
//				 D6 = 0;
//			}
//			else if(MC.StauteCtl.LedFaultCnt < 400)
//			{
//				 D1 = 1;
//				 D2 = 1;
//				 D3 = 1;
//				 D4 = 1;
//				 D5 = 1;
//				 D6 = 1;
//			}
//			else
//			{
//				MC.StauteCtl.LedFaultCnt =0;
//			}
//	}
}


