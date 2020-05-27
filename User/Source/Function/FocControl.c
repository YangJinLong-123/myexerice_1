/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : FocControl.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 2017-12-26
* Description        : This file contains all the foc control framework used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
// #include <FocControl.h>
#include <FU68xx_2.h>
#include <Myproject.h>

/* Private variables ----------------------------------------------------------------------------*/
MotStaType mcState;
//MotStaTim  MotorStateTime;
MotStaM    McStaSet;
//TailWindSet xdata  mcTailwind;

uint16 TimeCnt;

//uint16 EFREQACC;
//uint16 EFREQMIN;
//uint16 EFREQHOLD;

StartTimeCnt Time;

/*---------------------------------------------------------------------------*/
/* Name     :   void MC_Control(void)
/* Input    :   NO
/* Output   :   NO
/* Description: 电机状态机函数，包括初始化、预充电、顺风逆风判断、预定位、启动、运行、故障等
/*---------------------------------------------------------------------------*/
void MC_Control(void)
{
	switch(mcState)
	{
		case mcReady:    // 关闭输出,上电会对电流进行采集校准,当采样校准结束标志置1且启动指令置1后，才跳转到mcInit
			Motor_Ready();
		
			if((mcCurOffset.OffsetFlag == 1) && (mcSpeedRamp.FlagONOFF == 1) && (mcFocCtrl.State_Count == 0)) 
			{
					mcState = mcInit;
					FOC_EFREQACC 	= 0;               
					FOC_EFREQMIN 	= 0;
//				  FOC_EFREQMIN 	= Motor_Omega_Ramp_Min;
					FOC_EFREQHOLD = 0;
			}
		break;

		case mcInit:                          // 初始化状态，进入mcCharge状态
			Motor_Init();
			mcState               =  mcCharge;               // 跳入mcCharge状态
			mcFocCtrl.State_Count = Charge_Time;
		  mcFocCtrl.TPCtrlDealy = 1000;                     //电机正常运行500ms后再开发热丝
		break;

		case mcCharge:                        // 预充电状态，MCU输出固定频率占空比，预充电结束后，跳入mcTailWind
			Motor_Charge();
			#if (IPMState == NormalRun)           // 正常按电机状态机运行
				if( mcFocCtrl.State_Count == 0)
				{
						MOE = 0;                      // 关闭输出

						mcState               = mcTailWind;
						mcFocCtrl.State_Count = 0;
				}

			#endif
		break;

		case mcTailWind:
			#if (TailWind_Mode == NoTailWind)     // 无顺逆风处理的，直接跳入下一个状态
				mcState                           = mcPosiCheck;
				McStaSet.SetFlag.PosiCheckSetFlag = 0;
				mcFocCtrl.mcPosCheckAngle         = 0xffff;         // 角度赋初值

			#elif (TailWind_Mode == TailWind)
				Motor_TailWind();

			#endif
		break;

		case mcPosiCheck:
			#if (PosCheckEnable==0)             //初始位置检测不使能时初始角度为预定位角度
				mcFocCtrl.mcPosCheckAngle = Align_Angle;
				mcState = mcAlign;
				mcFocCtrl.State_Count = Align_Time;

			#else
				RPD();

			#endif
		break;

		case mcAlign:       // 预定位时间结束后，直接启动; AlignTestMode=1用于初始位置检测调试用
			Motor_Align();

			
			#if (AlignTestMode==1)
					while(1);

			#else
					if(mcFocCtrl.State_Count == 0)
					{
  					mcState = mcStart;
					}

			#endif
		break;

		case mcStart:                           // 配置电机启动参数，进入mcRun状态。
			Motor_Open();
		break;

		case mcPllTect:                           // 配置电机启动参数，进入mcRun状态。
			#if (EstimateAlgorithm == PLL)
					Motor_PllStart();

			#endif
		break;

		case mcRun:                             // 运行状态，若运行状态的给定变为0，进入mcStop状态。
			if(mcSpeedRamp.TargetValue == 0)//&&(mcSpeedRamp.ActualValue < Motor_Min_Speed))
			{
					mcState               = mcStop;
//					mcFocCtrl.State_Count = 2000;
					FOC_IQREF             = 0;
			}
		break;

		case mcStop:
			if((mcFocCtrl.SpeedFlt < Motor_Min_Speed)||(mcFocCtrl.State_Count==0))		// 实际转速低于Motor_Min_Speed或延时到了，关闭PWM输出或进入刹车
			{
				#if (StopBrakeFlag == 0)
				{
						FOC_CR1 = 0x00;
						/*关闭FOC*/
						ClrBit(DRV_CR, FOCEN);
	
						mcState = mcReady;
				}
				#else
				{
						if(mcFocCtrl.SpeedFlt < Motor_Stop_Speed)														// 低于StopSpeed,刹车
						{
								MOE = 0;
								FOC_CR1 = 0x00;
								ClrBit(DRV_CR, FOCEN);
								DRV_DR  = DRV_ARR+1;
								DRV_CMR &= 0xFFC0;
								DRV_CMR |= 0x015;            																		// 三相下桥臂通，刹车
								ClrBit(DRV_CR, OCS);         																		// OCS = 0, DRV_COMR;OCS = 1, FOC/SVPWM/SPWM
								MOE = 1;
								mcState               = mcBrake;
								mcFocCtrl.State_Count = StopWaitTime;
						}
				}
				#endif
			}
			else if(mcSpeedRamp.TargetValue > 0)																			// Stop状态时，电机在减速状态，又开机进入正常运行模式
			{
					mcState            = mcRun;
					mcFocCtrl.CtrlMode = 0;
					FOC_IQREF          = IQ_RUN_CURRENT;
			}
		break;

		case mcBrake:
			if(mcFocCtrl.State_Count == 0)
			{
					MOE=0;
					ClrBit(DRV_CR, FOCEN);
					mcState = mcReady;
			}
		break;

		case mcFault:
		break;
	}
}


