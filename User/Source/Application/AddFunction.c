/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : AddFunction.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 2017-12-26
* Description        : This file contains all the add function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
// #include <AddFunction.h>
#include <FU68xx_2.h>
#include <Myproject.h>

/* Private variables ---------------------------------------------------------*/
FaultStateType			       mcFaultSource;
PWMINPUTCAL        idata   mcPwmInput;
FaultVarible       idata   mcFaultDect;
MotorRSDTypeDef    idata   RSDDetect;
CurrentVarible     idata   mcCurVarible;
ProtectVarible     idata   mcProtectTime;

OUTLOOP						 xdata   SpeedPICtrl;
FOCCTRL            xdata   mcFocCtrl;
ADCSample          xdata   AdcSampleValue;
ONVarible          xdata   ONOFFTest;
MCLedDisplay       xdata   mcLedDisplay;
MCRAMP             xdata   mcSpeedRamp;
SLEEPMODE          xdata   SleepSet;
MotorFRTypeDef		 xdata   mcFRState;
int16 						 xdata   VSP;

const int16  PowerGiven[10] ={0,500,1000,2200,2900,4300,5900,7300,7900,8550};

extern char Flag_Key1Value;
extern char Flag_Key2Value;

uint8 low_flag = 0;

/*---------------------------------------------------------------------------*/
/* Name     :   void OutLoopParameterSet(void)
/* Input    :   NO
/* Output   :   NO
/* Description: 
/*---------------------------------------------------------------------------*/
void OutLoopParameterSet(void)
{
	memset(&SpeedPICtrl,0, sizeof(OUTLOOP));																// SpeedControl clear

	mcFocCtrl.SpeedLoopTime = SPEED_LOOP_TIME;
	mcFocCtrl.SpeedRampTime = SPEEDRAMPTIME;
	
	mcSpeedRamp.IncValue		= SPEEDRAMPSTARTINC;
	mcSpeedRamp.DecValue		= SPEEDRAMPSTARTDEC;

	SpeedPICtrl.ExtKP 			= SKP;
	SpeedPICtrl.ExtKI 			= SKI;
	SpeedPICtrl.ExtOutMax 	= SOUTMAX;
	SpeedPICtrl.ExtOutMin 	= SOUTMIN;
	
	PI_KP 		= SpeedPICtrl.ExtKP;
	PI_KI 		= SpeedPICtrl.ExtKI;
	PI_UKMAX 	= SpeedPICtrl.ExtOutMax;
	PI_UKMIN 	= SpeedPICtrl.ExtOutMin;
	
//	PI_UK			= I_Value(0.4) ;
	FOC_IQREF = I_Value(0.07);
	
	PI_UK			= FOC_IQREF ;	
}
/*---------------------------------------------------------------------------*/
/* Name		:	int16 KLPF_VALUE(int16 INVlaue, int16 OutLastValue)
/* Input	:	INVlaue，OutLastValue
/* Output	:	int16的变量
/* Description:	滤波函数,用乘法器做的
/*---------------------------------------------------------------------------*/
int16 KLPF_VALUE(int16 INVlaue, int16 OutLastValue)
{
	int16 Result = 0;
	MDU_MA = (INVlaue-OutLastValue);
	MDU_MB = (int16)480;		           			/*写被乘数和乘数*/

	Result = MDU_MB;
	Result += OutLastValue;
	return(Result);
}

/*---------------------------------------------------------------------------*/
/* Name		:	void FaultProcess(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	保护处理函数，关闭FOC输出，同时将状态变为mcFault
/*---------------------------------------------------------------------------*/
void FaultProcess(void)
{
	MOE     = 0;
	ClrBit(DRV_CR, FOCEN);	//关闭FOC
	mcState = mcFault;

}

/*---------------------------------------------------------------------------*/
/* Name		:	int16 Abs_F16(int16 value)
/* Input	:	value
/* Output	:	int16
/* Description:	对变量取16位的绝对值
/*---------------------------------------------------------------------------*/
uint16 Abs_F16(int16 value)
{
	if(value < 0)
	{
		return (- value);
	}
	else
	{
		return (value);
	}
}
/*---------------------------------------------------------------------------*/
/* Name		:	int32 Abs_F32(int32 value)
/* Input	:	value
/* Output	:	int16
/* Description:	对变量取16位的绝对值
/*---------------------------------------------------------------------------*/
uint32 Abs_F32(int32 value)
{
	if(value < 0)
	{
		return (- value);
	}
	else
	{
		return (value);
	}
}
/*---------------------------------------------------------------------------*/
/* Name		:	void APP_DIV(void)
/* Input	:	void
/* Output	:	void
/* Description:	将所有用到除法的地方，放在同一个中断，以避免中断串扰
/*---------------------------------------------------------------------------*/
void APP_DIV(void)
{
    if( mcPwmInput.PWMDivFlag==1)  //启动除法器，避免与过调值中的除法冲突
    {
       mcPwmInput.PWMDuty = MDU_DIV_IDATA_U32(&mcPwmInput.pwm.PWMCompareAMP, &mcPwmInput.PWMARRUpdate);
       mcPwmInput.PWMDivFlag=0;
    }
    if( mcFocCtrl.ESDIVFlag==1)  //启动除法器，避免与过调值中的除法冲突
    {
       mcFocCtrl.SQUSpeedDIVEs = MDU_DIV_XDATA_U32(&mcFocCtrl.SQUSysSpeed,&mcFocCtrl.EsValue);
       mcFocCtrl.ESDIVFlag=0;
    }
}
/*---------------------------------------------------------------------------*/
/* Name		:	void PWMInputCapture(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	输入PWM处理
/*---------------------------------------------------------------------------*/
void PWMInputCapture(void)
{
  uint16 MotorControlVSP;

	 if(mcPwmInput.PWMUpdateFlag==1)  // 有新的duty更新
	 {
      if((Abs_F32(mcPwmInput.PWMCompare-mcPwmInput.PWMCompareOld)<50)// 两次比较值相近，减少读错率
        &&(Abs_F32(mcPwmInput.PWMARROld-mcPwmInput.PWMARR)<50)// 两次周期值相近，减少读错率
//        &&((100<mcPwmInput.PWMARR)&&(mcPwmInput.PWMARR<6000))// 周期值在一定范围内才认为有效，即一定频率范围
        &&(mcPwmInput.PWMDivFlag==0))
        {
          mcPwmInput.PWMFlag=1;                               // PWMFlag置1期间，不读取TIM3__DR和TIM3__ARR;，防止出错
          mcPwmInput.pwm.PWMCompareUpdate[0]=(mcPwmInput.PWMCompare>>1);// 对其乘以32768
          mcPwmInput.PWMARRUpdate=mcPwmInput.PWMARR;
          mcPwmInput.PWMDivFlag=1;                            // 启动除法
          mcPwmInput.PWMFlag=0;
        }
       if(mcPwmInput.PWMDivFlag==0)
        {
          if(mcPwmInput.PWMcnt<3)//2次求平均值
          {
            mcPwmInput.PWMcnt++;
            mcPwmInput.PWMVSum +=mcPwmInput.PWMDuty;
          }
          else
          {
            MotorControlVSP= (mcPwmInput.PWMVSum >>1);//注意其有一个右移与前面的比较值标幺化有关
            mcPwmInput.PWMVSum=0;
            mcPwmInput.PWMcnt =0;
          }
           MotorControlVSP=mcPwmInput.PWMDuty;
         if((MotorControlVSP > ONPWMDuty)&&(MotorControlVSP <=(OFFPWMDutyHigh+1)))
          {
            mcSpeedRamp.FlagONOFF = 1;
          }
         else if(MotorControlVSP < OFFPWMDuty)//||(MotorControlVSP >= OFFPWMDutyHigh))
          {
            mcSpeedRamp.FlagONOFF = 0;
          }

          //转速曲线计算
          if(mcSpeedRamp.FlagONOFF==1)
          {
            if(MotorControlVSP <= MINPWMDuty)
            {
              mcSpeedRamp.TargetValue = Motor_Min_Speed;
            }
            else if(MotorControlVSP < MAXPWMDuty)
            {
              mcSpeedRamp.TargetValue = Motor_Min_Speed + SPEED_K*(MotorControlVSP-MINPWMDuty);
            }
            else
            {
              mcSpeedRamp.TargetValue	=	Motor_Max_Speed;
            }
          }
          else
          {
            mcSpeedRamp.TargetValue =0;
          }
        }

      mcPwmInput.PWMUpdateFlag =0;
      mcPwmInput.PWMCompareOld=mcPwmInput.PWMCompare;//将此次比较值赋值给上次比较值
      mcPwmInput.PWMARROld=mcPwmInput.PWMARR;//将此次周期值赋值给上次周期值
	  }
}
/*****************************************************************************
 * Function:		 void	Fault_OverVoltage(mcFaultVarible *h_Fault)
 * Description:	 过压欠压保护函数：程序每5ms判断一次，母线电压大于过压保护值时，计数器加一，计数器值超过20次，判断为过压保护，关闭输出;反之，计数器慢慢减
                 同理，欠压保护。
	               电机过欠压保护状态下，母线电压恢复到欠压恢复值以上，过压恢复值以下时，计数器加一，超过200次后，恢复。根据档位信息来决定恢复到哪个状态。
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
void Fault_OverUnderVoltage(FaultVarible *h_Fault)
{
	//过压保护
		if(mcFaultSource == FaultNoSource)//程序无其他保护下
		{
				if(mcFocCtrl.mcDcbusFlt > OVER_PROTECT_VALUE)	 //母线电压大于过压保护值时，计数，超过20次，判断为过压保护，关闭输出;反之，计数器慢慢减
				{
					h_Fault->OverVoltDetecCnt++;
					if(h_Fault->OverVoltDetecCnt > 20)//检测100ms
					{
						h_Fault->OverVoltDetecCnt = 0;
						mcFaultSource=FaultOverVoltage;
						FaultProcess();
					}
				}
				else
				{
					if(h_Fault->OverVoltDetecCnt>0)
					{
						h_Fault->OverVoltDetecCnt--;
					}
				}

				//低电关闭LED
				
				if(mcFocCtrl.mcDcbusFlt < CLOSE_LED_VALUE)
				{
						h_Fault->CloseLedCnt++;
						if(h_Fault->CloseLedCnt > 30)
						{
								h_Fault->CloseLedCnt = 0;
								D1 = 1; D2 = 1; D3 = 1; D4 = 1;  D5 = 1; D6 = 1;
							  low_flag = 1;
						}
				}
				else
				{
						if(h_Fault->CloseLedCnt > 0)
						{
								h_Fault->CloseLedCnt--;
						}
						else
						{
								if(low_flag)
								{
									low_flag = 0;
									KS.ChangeKeyFlg = 1;
								}
						}
				}	
				
				
			//欠压保护
				if(mcFocCtrl.mcDcbusFlt< UNDER_PROTECT_VALUE)
				{
					h_Fault->UnderVoltDetecCnt++;

					if(h_Fault->UnderVoltDetecCnt > 30)//检测100ms
					{
						h_Fault->UnderVoltDetecCnt = 0;
						h_Fault->VoltDetecBraketCount = 120;
						mcFaultSource=FaultUnderVoltage;
						FaultProcess();

						Flag_Key1Value = 0;
						Flag_Key2Value = 0;
						
						if(KS.FlashWriteOnetimes == 0)
						{
												
							Rom.WriteValue = KS.KeyValuetotal;   //档位存储
							Rom.ReadValue = Flash_GetAddress();     //读
							if((Rom.ReadValue == 0x11)||(Rom.ReadValue == 0x21) ||(Rom.ReadValue == 0x41) ||(Rom.ReadValue == 0x12) ||(Rom.ReadValue == 0x22)
							||(Rom.ReadValue == 0x42)||(Rom.ReadValue == 0x14)||(Rom.ReadValue== 0x24)||(Rom.ReadValue == 0x44)||(Rom.ReadValue == 0x18)
							||(Rom.ReadValue == 0x28)||(Rom.ReadValue == 0x48))
							{
								EA = 0;
								Flash_ErasePageRom();                  //擦
								Flash_KeyWriteValue((Rom.WriteValue));  //写 
								EA = 1;
								KS.FlashWriteOnetimes = 1;
							}
//							else
//							{
//								Flash_ErasePageRom();                  //擦
//								Flash_KeyWriteValue((0x21));  //写 
//								EA = 1;
//							}
						}
					}
				}
				else
				{
					if(h_Fault->UnderVoltDetecCnt>0)
					{
						h_Fault->UnderVoltDetecCnt--;
					}
				}
		}
		if(((mcFaultSource==FaultOverVoltage)||(mcFaultSource==FaultUnderVoltage))&&(h_Fault->VoltDetecBraketCount >0))
		{  
			h_Fault->VoltDetecBraketCount--;
			if((h_Fault->VoltDetecBraketCount >= 60))
			{
          DRV_DR = 0.1*DRV_ARR;				            //下桥臂10% duty
				  ClrBit(DRV_CR, OCS);                    //OCS = 0, PWM来源DRV_COMR
				  DRV_CMR |= 0x03;                         // UVW相下桥输出
      		SetBit(DRV_CR, DRVEN);                    /*DRV计数器使能，0-禁止，1-使能*/
				  MOE = 1;	

			}
			else if((h_Fault->VoltDetecBraketCount <= 40))
			{
	      MOE = 0;
				DRV_OUT = 0x00;
        //D1 = 1; D2 = 1; D3 = 1; D4 = 1;  D5 = 1; D6 = 1;				
			}
			else
			{
				h_Fault->VoltDetecBraketCount = 60;
			}
			
//			if(h_Fault->mcVoltDetecBraketCount == 1)
//			{
//        FaultProcess();
//			}
    }

		/*******过压欠压保护恢复*********/
		if((mcState == mcFault) &&((mcFaultSource==FaultUnderVoltage)||(mcFaultSource==FaultOverVoltage)))
		{
//			if((mcFocCtrl.mcDcbusFlt< OVER_RECOVER_VALUE)&&(mcFocCtrl.mcDcbusFlt> UNDER_RECOVER_VALUE))
//			{
//				h_Fault->VoltRecoverCnt++;
//				if(h_Fault->VoltRecoverCnt>200)//连续检测1s，若正常则恢复
//				{
//          mcState = mcReady;
//					mcFaultSource=FaultNoSource;
//					h_Fault->VoltRecoverCnt = 0;
//				}
//			}
//			else
//			{
//				h_Fault->VoltRecoverCnt = 0;
//			}
	 }
}


/*****************************************************************************
 * Function:		 void	Fault_Power(mcFaultVarible *h_Fault)
 * Description:	 功率保护函数
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
void Fault_Power(FaultVarible *h_Fault)
{

		if(mcFaultSource == FaultNoSource)//程序无其他保护下
		{
				if(mcFocCtrl.Powerlpf > PowerLimit)	 //功率大于保护值时计数，超过20次，判断为过载保护，关闭输出;反之，计数器慢慢减
				{
					h_Fault->OverPowerDetecCnt++;
					if(h_Fault->OverPowerDetecCnt > 20)
					{
						h_Fault->OverPowerDetecCnt = 0;
						mcFaultSource=FaultOverPower;
						FaultProcess();
					}
				}
				else
				{
					if(h_Fault->OverPowerDetecCnt>0)
					{
						h_Fault->OverPowerDetecCnt--;
					}
				}
		}

	}


/*****************************************************************************
 * Function:		 void Fault_Overcurrent(CurrentVarible *h_Cur)
 * Description:	 电机运行或者启动时，当三相中某一相最大值大于OverCurrentValue，则OverCurCnt加1。
								 连续累加3次，判断为软件过流保护。执行时间约30.4us。
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
void Fault_Overcurrent(CurrentVarible *h_Cur)
{
	if((mcState == mcRun)||(mcState == mcStart))						// check over current in rum and open mode
	{

		h_Cur->Abs_ia = Abs_F16(FOC__IA);
		h_Cur->Abs_ib = Abs_F16(FOC__IB);
		h_Cur->Abs_ic = Abs_F16(FOC__IC);
		if(h_Cur->Abs_ia> h_Cur->Max_ia)                      // 此部分既用于软件过流保护，又用于缺相保护
		{
			 h_Cur->Max_ia = h_Cur->Abs_ia;
		}
		if(h_Cur->Abs_ib > h_Cur->Max_ib)
		{
			 h_Cur->Max_ib = h_Cur->Abs_ib;
		}
		if(h_Cur->Abs_ic > h_Cur->Max_ic)
		{
			 h_Cur->Max_ic = h_Cur->Abs_ic;
		}

		if((h_Cur->Max_ia>=OverSoftCurrentValue)||(h_Cur->Max_ib>=OverSoftCurrentValue)||(h_Cur->Max_ic>=OverSoftCurrentValue))
		{
			h_Cur->OverCurCnt++;
			if(h_Cur->OverCurCnt>=3)
			{
        GP36=~GP36;
				h_Cur->Max_ia=0;
				h_Cur->Max_ib=0;
				h_Cur->Max_ic=0;
				h_Cur->OverCurCnt=0;
			  mcFaultSource=FaultSoftOVCurrent;
			  FaultProcess();
			}
		}
    else
		{
			if(h_Cur->OverCurCnt>0)
			{
				h_Cur->OverCurCnt--;
			}
		}
	}
}


/*****************************************************************************
 * Function:		 void	Fault_OverCurrentRecover(mcFaultVarible *h_Fault)
 * Description:	 软硬件过流保护恢复
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
void Fault_OverCurrentRecover(FaultVarible *h_Fault)
{
	if((mcState == mcFault)&&((mcFaultSource==FaultSoftOVCurrent)||(mcFaultSource==FaultHardOVCurrent))&&(mcProtectTime.CurrentPretectTimes<5))
	{
		h_Fault->CurrentRecoverCnt++;
		if(h_Fault->CurrentRecoverCnt>=OverCurrentRecoverTime)//1000*5=5s
		{
			h_Fault->CurrentRecoverCnt=0;
			mcProtectTime.CurrentPretectTimes++;
			mcState = mcReady;
			mcFaultSource=FaultNoSource;
    }
  }
}

/*****************************************************************************
 * Function:		 void	Fault_OverPowerRecover(mcFaultVarible *h_Fault)
 * Description:	 功率保护恢复函数
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
void Fault_OverPowerRecover(FaultVarible *h_Fault)
{
	if((mcState == mcFault)&&(mcFaultSource==FaultOverPower)&&(mcProtectTime.PowerPretectTimes<5))
	{
		h_Fault->OverPowerDetecCnt++;
		if(h_Fault->OverPowerDetecCnt>=OverPowerRecoverTime)
		{
			h_Fault->OverPowerDetecCnt=0;
			mcProtectTime.PowerPretectTimes++;
			mcState = mcReady;
			mcFaultSource=FaultNoSource;
    }
  }
}



/*****************************************************************************
 * Function:		 void	Fault_Start(mcFaultVarible *h_Fault)
 * Description:	 启动保护函数，电机运行状态下，电机在前5s估算转速达到堵转保护值或者5s后反电动势值太低(此方法未验证)
                  或4s内还在CtrlMode状态，即速度低于MOTOR_LOOP_RPM，程序判断为启动失败，电机停机。
                  当程序判断为启动失败后，若重启次数少于或等于5次，程序立即进入校准状态，等待重启。
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
  void Fault_Start(FaultVarible *h_Fault)
  {
		/*******启动保护恢复*********/
//		h_Fault->mcEsValue = FOC__ESQU;

		if(mcState == mcRun)
		{
			//方法一，5s内速度大于最大速度，同时反电动势值低于一定值
			if(h_Fault->StartSpeedCnt<=1000)
			{
			  h_Fault->StartSpeedCnt++;
				if((mcFocCtrl.SpeedFlt > Motor_Max_Speed)&&(mcFocCtrl.EsValue<50))
				{
					h_Fault->StartSpeedCnt = 0;
					mcFaultSource=FaultStart;
					FaultProcess();
					mcProtectTime.SecondStartTimes++;
					mcProtectTime.StartFlag  =  1;
        }
      }
     //方法二
			if(h_Fault->StartEsCnt<=1200)//前6s，等待1.5s后，开始判断Es，如果超过一定次数，则失败
			{
				h_Fault->StartEsCnt++;
				h_Fault->StartDelay++;
				if(h_Fault->StartDelay>=300)				// 1.5s
				{
					 h_Fault->StartDelay=300;
					 if((mcFocCtrl.EsValue <20))//&&(mcFocCtrl.CtrlMode==0))
						{
							h_Fault->StartESCount++;
							if(h_Fault->StartESCount>=50)     //50次Es小于设定值，启动失败
							{
								mcFaultSource=FaultStart;
								FaultProcess();
								mcProtectTime.SecondStartTimes++;
								h_Fault->StartDelay=0;
								h_Fault->StartESCount=0;
								mcProtectTime.StartFlag  =  2;
							}
						}
            else
						{
							if(h_Fault->StartESCount>0)
								h_Fault->StartESCount--;
						}
			 }
		 }
     else
     {
       h_Fault->StartESCount=0;
     }
		 
		 
		 	//方法三，长时间在CtrlMode=0状态
			if(mcFocCtrl.CtrlMode==0)         
			{
				h_Fault->StartFocmode++;
				if(h_Fault->StartFocmode>=8000)   //在MODE0模式超过8s起动失败
				{
					h_Fault->StartFocmode=0;
					mcFaultSource=FaultStart;
					FaultProcess();
					mcProtectTime.SecondStartTimes++;
          mcProtectTime.StartFlag  =  3;
        }
			}
			
	  }
    
		//起动保护恢复
 #if (!StartONOFF_Enable)    //起停测试时屏蔽起动保护恢复
		{
		 if((mcFaultSource==FaultStart)&&(mcState == mcFault)&&(mcProtectTime.SecondStartTimes<=StartProtectRestartTimes))
		 {
			 
			 h_Fault->StartRecoverDelayCnt++;
			 if(h_Fault->StartRecoverDelayCnt > StartRecoverDelayTimes)   //启动保护延时恢复时间
	     {
					h_Fault->StartRecoverDelayCnt = 0;
				  mcFaultSource=FaultNoSource;
					mcState = mcInit;
			 }
		 }
    }
	#endif
		
	}

 


 /*****************************************************************************
 * Function:		 void	Fault_Stall(mcFaultVarible *h_Fault)
 * Description:	 堵转保护函数，有三种保护方式，
	               第一种，
	               第二种，电机运行状态下，延迟4s判断，估算速度绝对值超过堵转速度连续5次；
	               第三种，电机运行状态下，当U,V两相电流绝对值大于堵转电流保护值连续6次；
	               当以上三种的任何一种保护触发时，电机停机，程序判断为堵转保护；
	               当堵转保护状态下，U相采集值低于堵转恢复值时，若堵转次数小于或等于堵转重启次数8次，
	               程序延迟mcStallRecover重新启动，进行校准状态。
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
//堵转保护
void Fault_Stall(FaultVarible *h_Fault,CurrentVarible *h_Cur)
{
//	h_Fault->mcEsValue = FOC__ESQU;
	if(mcState == mcRun)
	{
		if(h_Fault->StallDelayCnt <=1000)// 启动后延时5s判断是否堵转保护
		{
			h_Fault->StallDelayCnt ++;
		}
		else
		{
	    //method 1，当反电动势太小 或当 转速太大但反电动势却很小时进入保护
      if((&mcFocCtrl.EsValue< 20)||((FOC__EOME > _Q15(1200.0/MOTOR_SPEED_BASE))&&(mcFocCtrl.EsValue< 50)))
			{
				h_Fault->StallDectEs++;
				if(h_Fault->StallDectEs >= 50)   //判断满足条件达到设置次数，保护动作
				{
					h_Fault->StallDectEs=0;
					mcFaultSource=FaultStall;
					mcProtectTime.StallTimes++;        //堵转次数+1
					FaultProcess();
					h_Fault->StallDelayCnt = 0;        //保护后重启开始重新计数，保持5S延迟
					mcProtectTime.StallFlag  =  1;
				}
			}
			else
			{
        if(	h_Fault->StallDectEs>0)
				  h_Fault->StallDectEs--;
			}
			
			//method 2，判断速度低于堵转最小值或者超过堵转最大值
			if((mcFocCtrl.SpeedFlt<Motor_Stall_Min_Speed)||(mcFocCtrl.SpeedFlt > Motor_Stall_Max_Speed))
			{
				h_Fault->StallDectSpeed++;
				if(h_Fault->StallDectSpeed >= 20)   //判断满足条件达到设置次数，保护动作
				{
					h_Fault->StallDectSpeed=0;
					mcFaultSource=FaultStall;
					mcProtectTime.StallTimes++;				//堵转次数+1
					FaultProcess();
					h_Fault->StallDelayCnt = 0; 			//保护后重启开始重新计数，保持5S延迟
 					mcProtectTime.StallFlag =2;
				}
			}
			else
			{
        if(h_Fault->StallDectSpeed>0)
				h_Fault->StallDectSpeed--;
			}
			
			//method 3		  A/B/C三相任意一相相电流大于堵转电流，堵转保护
		if((h_Cur->Max_ia >= StallCurrentValue)||
			 (h_Cur->Max_ib >= StallCurrentValue)||
		   (h_Cur->Max_ic >= StallCurrentValue))
		{			 
			h_Fault->mcStallDeCurrent++;              
			if(h_Fault->mcStallDeCurrent >= 20)					//判断满足条件达到设置次数，保护动作
			{
				h_Fault->mcStallDeCurrent=0;
				mcFaultSource=FaultStall;	
				mcProtectTime.StallTimes++;
				FaultProcess();		
				h_Fault->StallDelayCnt = 0; 			//保护后重启开始重新计数，保持5S延迟
				mcProtectTime.StallFlag  =  3;
			}						 
		}	
		else
			{
        if(h_Fault->mcStallDeCurrent>0)
				h_Fault->mcStallDeCurrent--;
			}
  }
}
   
 #if (!StartONOFF_Enable)
{
		 /*******堵转保护恢复*********/
        if((mcFaultSource==FaultStall)&&(mcState == mcFault)&&(mcProtectTime.StallTimes<=4))  //堵转重启次数
        {
          h_Fault->StallReCount++;
          if(h_Fault->StallReCount>=StallRecoverTime)
          {
            h_Fault->StallReCount=0;        //16000
            mcFaultSource=FaultNoSource;
            mcState =   mcInit;
          }
        }
        else
        {
          h_Fault->StallReCount=0;
        }
}
#endif
    
}
 /*****************************************************************************
 * Function:		 void	Fault_phaseloss(mcFaultVarible *h_Fault)
 * Description:	 缺相保护函数，当电机运行状态下，10ms取三相电流的最大值，
	               1.5s判断各相电流最大值，若存在两相电流值大于一定值，而第三相电流值却非常小，则判断为缺相保护，电机停机；
 * Parameter:		 mcFaultVarible *h_Fault
 * Return:			 no
 *****************************************************************************/
  void Fault_phaseloss(FaultVarible *h_Fault)
  {		
			if(mcState == mcRun)
		 {
				h_Fault->Lphasecnt++;
				if(h_Fault->Lphasecnt>50)//100*5=500ms  500ms判断一次
				{
					 h_Fault->Lphasecnt=0;

					 if(((mcCurVarible.Max_ia>(mcCurVarible.Max_ib*3))||(mcCurVarible.Max_ia>(mcCurVarible.Max_ic*3))) ||(mcCurVarible.Max_ia>PhaseLossCurrentValue))
					 {
							h_Fault->AOpencnt++;
					 }
					 else
					 {
						if(h_Fault->AOpencnt>0)
							h_Fault->AOpencnt --;
					 }
					 if(((mcCurVarible.Max_ib >(mcCurVarible.Max_ia*3))||(mcCurVarible.Max_ib >(mcCurVarible.Max_ic*3))) ||(mcCurVarible.Max_ib >PhaseLossCurrentValue))
					 {
						 h_Fault->BOpencnt++;
					 }
					else
					 {
             if(h_Fault->BOpencnt>0)
							h_Fault->BOpencnt --;
					 }
					 if(((mcCurVarible.Max_ic >(mcCurVarible.Max_ia*3))||(mcCurVarible.Max_ic >(mcCurVarible.Max_ib*3))) ||(mcCurVarible.Max_ic >PhaseLossCurrentValue))
					 {
						 h_Fault->COpencnt++;
					 }
					else
					 {
             if(h_Fault->COpencnt>0)
							h_Fault->COpencnt --;
					 }
						mcCurVarible.Max_ia = 0;
						mcCurVarible.Max_ib = 0;
						mcCurVarible.Max_ic = 0;
					if(h_Fault->AOpencnt > 20|| h_Fault->BOpencnt > 20 || h_Fault->COpencnt > 20)
					 {
							mcProtectTime.LossPHTimes++;
							mcFaultSource=FaultLossPhase;
							FaultProcess();
					 }
				}
			}

		 /*******缺相保护恢复*********/
#if (!StartONOFF_Enable)           //起停测试时屏蔽缺相保护恢复
{
//      if((mcFaultSource==FaultLossPhase)&&(mcState == mcFault)&&(mcProtectTime.LossPHTimes<5))//可重启5次
//			 {
//					 h_Fault->mcLossPHRecCount++;
//					 if(h_Fault->mcLossPHRecCount>=PhaseLossRecoverTime)
//					 {
//						 h_Fault->AOpencnt=0;
//						 h_Fault->BOpencnt=0;
//						 h_Fault->COpencnt=0;
//             mcState = mcReady;
//						 mcFaultSource=FaultNoSource;
//           }
//       }
//    else
//			 {
//			     h_Fault->mcLossPHRecCount=0;
//			 }
}
#endif
			 		 
    }


extern uint8 test_flag;
/*---------------------------------------------------------------------------*/
/* Name		:	void Fault_Detection(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	保护函数，因保护的时间响应不会很高，采用分段处理，每5个定时器中断执行一次对应的保护
	              常见保护有过欠压、过温、堵转、启动、缺相等保护，调试时，可根据需求，一个个的调试加入。
/*---------------------------------------------------------------------------*/
void Fault_Detection(void)
{
		mcFaultDect.segment++;
		if(mcFaultDect.segment>=5)
		{
			mcFaultDect.segment=0;
		}
		if(mcFaultDect.segment==0)
		{
			if(CurrentRecoverEnable)//过流保护恢复使能
			{
				Fault_OverCurrentRecover(&mcFaultDect);
			}
			if(PowerRecoverEnable) //功率保护恢复使能
			{
				Fault_OverPowerRecover(&mcFaultDect);
			}
			
		}
		else if(mcFaultDect.segment==1)
		{
			if((VoltageProtectEnable==1)&&(test_flag == 0))//过压保护使能
			{
				Fault_OverUnderVoltage(&mcFaultDect);
			}
			
			if(OverPowerProtectEnable==1); //功率保护使能
			{
			  Fault_Power(&mcFaultDect);
			}				
		}
		else if(mcFaultDect.segment==2)
		{
			if(StartProtectEnable==1)//启动保护使能
			{
				Fault_Start(&mcFaultDect);
			}
		}
	  else if(mcFaultDect.segment==3)
		{
			if(StallProtectEnable==1)//堵转保护使能
			{
				Fault_Stall(&mcFaultDect,&mcCurVarible);
			}
		}
		else if(mcFaultDect.segment==4)
		{
			if(PhaseLossProtectEnable==1)//缺相保护使能
			{
				Fault_phaseloss(&mcFaultDect);
			}
			
		}
		else
		{

		}
}

/*---------------------------------------------------------------------------*/
/* Name		:	void ONOFF_Starttest(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	启动测试
/*---------------------------------------------------------------------------*/
void ONOFF_Starttest(ONVarible  *h_test)
{
	if(h_test->ONOFF_Flag==1)
	{
		 h_test->ON_Count++;
		if(h_test->ON_Count>StartON_Time)
		{
			h_test->ON_Count=0;
			h_test->ONOFF_Times++;
			h_test->ONOFF_Flag=0;
		  mcSpeedRamp.FlagONOFF = 0;
			mcSpeedRamp.TargetValue = 0;

		}
  }
	else
	{
		if(mcState!=mcFault)
		{
		  h_test->OFF_Count++;
			if(h_test->OFF_Count>StartOFF_Time)
			{
				h_test->OFF_Count=0;
				h_test->ONOFF_Flag=1;
				mcSpeedRamp.FlagONOFF = 1;
				mcSpeedRamp.TargetValue = Motor_Test_Speed;

			}
	  }

  }
}

/*---------------------------------------------------------------------------*/
/* Name		:	void Speed_response(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	速度响应函数，可根据需求加入控制环，如恒转矩控制、恒转速控制、恒功率控制
/*---------------------------------------------------------------------------*/


void Speed_response(void)
{
		uint16  UqMinTemp = 0;	   //UQ限幅最小值缓存变量
	
		if((mcState ==mcRun)||(mcState ==mcStop))
		{	
				mcSpeedRamp.IncValue	= SPEEDRAMPINC;      				//运行速度环增量
				mcSpeedRamp.DecValue	= SPEEDRAMPDEC;
   	   		
			switch(mcFocCtrl.CtrlMode)
			{
				case 0:
				{
						if(FOC__EOME > Motor_Loop_Speed)//>=MOTOR_LOOP_RPM
						{
							FOC__THECOMP 	= _Q15(0.0/180.0);												// SMO 估算补偿角
	            FOC__THECOR 		= 0x04;		
							mcFocCtrl.CtrlMode = 1;
							FOC_DQKP = DQKP;
							FOC_DQKI = DQKI;
							FOC_QMAX 			= QOUTMAX;
							# if (Motor_Speed_Control_Mode == SPEED_LOOP_CONTROL)
							{
								   mcSpeedRamp.ActualValue = FOC__EOME ;  //防止mcSpeedRamp.ActualValue从0开始爬坡，造成估算异常，电流异常
							}
							#elif (Motor_Speed_Control_Mode == POWER_LOOP_CONTROL)
							{
								mcSpeedRamp.ActualValue = mcFocCtrl.Powerlpf;
							}
							#endif
							
							OutLoopParameterSet();       //环路基本参数设置
						}
						else
						{
							
//							if(FOC_IQREF < I_Value(0.6))
//							{
//								FOC_IQREF += 1;
//							}
							
//							UqMinTemp = FOC_QMIN;
//							if(UqMinTemp < QOUTMINSTEND)
//							{
//								UqMinTemp += 10 ;
//								FOC_QMIN = UqMinTemp;
//							}
						}
					}
					break;
					case 1:
					{
												
						#if (OUTLoop_Mode== OUTLoop_Disable)
						{
								mcFocCtrl.mcIqref = FOC_IQREF;
								if (FOC_IQREF < QOUTVALUE)
								{
										mcFocCtrl.mcIqref += QOUTINC;
										if (mcFocCtrl.mcIqref > QOUTVALUE) mcFocCtrl.mcIqref = QOUTVALUE;
										FOC_IQREF = mcFocCtrl.mcIqref;
								}
								else if (FOC_IQREF > QOUTVALUE)
								{
										mcFocCtrl.mcIqref -= QOUTINC;
										if (mcFocCtrl.mcIqref < QOUTVALUE) mcFocCtrl.mcIqref = QOUTVALUE;
										FOC_IQREF = mcFocCtrl.mcIqref;
								}
						}

						#elif (OUTLoop_Mode== OUTLoop_Enable)
						{
								mcFocCtrl.SpeedRampTime++;
								if(mcFocCtrl.SpeedRampTime > SPEEDRAMPTIME)
								{
										mcFocCtrl.SpeedRampTime = 0;
										mc_ramp(&mcSpeedRamp);       //外环速度爬坡或减速。每SpeedRampTime执行一次
								}
								
								mcFocCtrl.SpeedLoopTime++;
								if(mcFocCtrl.SpeedLoopTime > SPEED_LOOP_TIME)  //外环调节周期
								{
										mcFocCtrl.SpeedLoopTime=0;

										#if (Motor_Speed_Control_Mode == SPEED_LOOP_CONTROL)
										{							
											SpeedPICtrl.ExtRef = mcSpeedRamp.ActualValue;
											SpeedPICtrl.ExtFed = mcFocCtrl.SpeedFlt;
											
											if(mcFocCtrl.SpeedFlt <(_Q15(80000 / MOTOR_SPEED_BASE)))
											{
											  PI_KP 		  = SKP;
	                      PI_KI 		  = SKI;
											}
											else
											{
												mcSpeedRamp.IncValue	= SPEEDRAMPINC /5;      				//运行速度环增量
				                mcSpeedRamp.DecValue	= SPEEDRAMPDEC /5;
												PI_KP 		  = SKPH;
	                      PI_KI 		  = SKIH;
											}
											HW_PI_Control(&SpeedPICtrl);             //
											FOC_IQREF = SpeedPICtrl.ExtOut;
										}
										#elif (Motor_Speed_Control_Mode == POWER_LOOP_CONTROL)
										{
											 FOC_IQREF= HW_One_PI(mcSpeedRamp.ActualValue- mcFocCtrl.Powerlpf);
										}
										#endif
								}
								

				 	// 判断FOC_QMIN值恢复到正常值
//					UqMinTemp = FOC_QMIN;
//					if(UqMinTemp > QOUTMIN)
//					{
//						UqMinTemp -- ;
//						FOC_QMIN = UqMinTemp;
//					}	
								
					 }
					 #endif //END OUTLoop_Mode


					 if(FOC_EK2 < OBS_K2T_Actual-10)
					 {
						 mcFocCtrl.Smo_EK2=FOC_EK2;
						 mcFocCtrl.Smo_EK2+=10;
						 FOC_EK2=mcFocCtrl.Smo_EK2;
					 }
					 else if(FOC_EK2 > OBS_K2T_Actual+10)
					 {
						 mcFocCtrl.Smo_EK2=FOC_EK2;
						 mcFocCtrl.Smo_EK2-=10;
						 FOC_EK2=mcFocCtrl.Smo_EK2;
					 }
					 else
					 {
						 FOC_EK2=OBS_K2T_Actual;
					 }
						
				}
				break;
			}
		}

}


/*---------------------------------------------------------------------------*/
/* Name		:	void FGOutput(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	FG信号输出
/*---------------------------------------------------------------------------*/
void FGOutput(void)
{//FG可用中断FGIF进行判断
	if(mcState==mcRun)
	{
		if((FOC__THETA>=0)&&(FOC__THETA<32768))//0-180
		{
			ResetFGPin;
		}
		else if((FOC__THETA>=32768)&&(FOC__THETA<65536))//180-360
		{
			SetFGPin;
		}
  }
	else if(mcState == mcFault)
	{
			SetFGPin;
	}
	else
	{
			ResetFGPin;
	}
}
/*---------------------------------------------------------------------------*/
/* Name		:	 uint16 SoftLPF(uint16 Xn1, uint16 Xn0, uint16 K)
/* Input	:	uint16 Xn1, uint16 Xn0, uint16 K
/* Output	:	uint16
/* Description:	软件低通滤波
/*---------------------------------------------------------------------------*/
 int16 SoftLPF(int16 Xn1, int16 Xn0, int16 K)
 {
 	 int16 Temp16 = 0;
 	 int32 Temp32 = 0;

 	 Temp32 = (((int32)Xn1 - (int32)Xn0) * (int32)K) >> 15;
 	 Temp16 = Xn0 + (int16)Temp32;
 	 return Temp16;
 }


/*---------------------------------------------------------------------------*/
/* Name		:	void LED_Display(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	LED灯显示
/*---------------------------------------------------------------------------*/
void LED_Display(void)
{
	switch(mcFaultSource)
	{
		case FaultNoSource:
		SetLEDPin;				  	              //低电平点亮，高电平灭
			break;
		case FaultHardOVCurrent:             //硬件过流保护闪烁10次           
				Led_OnOff(&mcLedDisplay,10);
			break;
		case FaultSoftOVCurrent:            //软件过流保护闪烁4次           
				Led_OnOff(&mcLedDisplay,4);
			break;
		case FaultUnderVoltage:              //欠压保护闪烁2次
				Led_OnOff(&mcLedDisplay,2);
			break;
		case FaultOverVoltage:								//过压保护闪烁3次
				Led_OnOff(&mcLedDisplay,3);
			break;
		case FaultLossPhase:                  //缺相保护闪烁8次
				Led_OnOff(&mcLedDisplay,5);
			break;
		case FaultStall:                     //堵转保护闪烁6次
				Led_OnOff(&mcLedDisplay,6);
			break;
		case  FaultOverPower:               //功率保护闪烁7次
		    Led_OnOff(&mcLedDisplay,7);
		  break;

		default:
			break;
  }

}

  //LED灯的闪烁
void Led_OnOff(MCLedDisplay *hLedDisplay,uint8 htime)
{
	hLedDisplay->LedCount++;
	if(hLedDisplay->LedCount<hLedDisplay->Counttime)
	 {
			if(hLedDisplay->Count<200)
			{
			 hLedDisplay->Count++;

			}
			else if((hLedDisplay->Count>=200)&&(hLedDisplay->Count<201))
			{
				hLedDisplay->Count=0;
				LEDPinONOFF;
				hLedDisplay->LedTimCot++;
			}
			else
				;
			if(hLedDisplay->LedTimCot>=2*htime)
			{
				hLedDisplay->Count=202;
				SetLEDPin;
			}
		}
		else if(hLedDisplay->LedCount>=hLedDisplay->Counttime)
		{
		 hLedDisplay->LedCount=0;
		 hLedDisplay->LedTimCot=0;
		 hLedDisplay->Count=0;
		}
}
/*---------------------------------------------------------------------------*/
/* Name		:	void mc_ramp(void)
/* Input	:	hTarget,MC_RAMP *hSpeedramp
/* Output	:	NO
/* Description:
/*---------------------------------------------------------------------------*/

void mc_ramp(MCRAMP *hSpeedramp)
{
//	if( --hSpeedramp->DelayCount < 0)
//	{
//		hSpeedramp->DelayCount = hSpeedramp->DelayPeriod;

		if (hSpeedramp->ActualValue < hSpeedramp->TargetValue)
		{
				if(hSpeedramp->ActualValue + hSpeedramp->IncValue < hSpeedramp->TargetValue)
				{
						hSpeedramp->ActualValue += hSpeedramp->IncValue;
				}
				else
				{
						hSpeedramp->ActualValue = hSpeedramp->TargetValue;
				}
		}
		else
		{
				if(hSpeedramp->ActualValue - hSpeedramp->DecValue > hSpeedramp->TargetValue)
				{

						hSpeedramp->ActualValue -= hSpeedramp->DecValue;
				}
				else
				{
						hSpeedramp->ActualValue = hSpeedramp->TargetValue;
				}
		}
//	}
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	int16 HW_One_PI(int16 Xn1, int16 Yn0, int16 Xn2)
	Description   :	PI控制
	Input         :	Xn1--E(K)
	Output		  :	PI_UK--当前PI输出值,执行时间us
-------------------------------------------------------------------------------------------------*/
int16 HW_One_PI(int16 Xn1)
{
	  PI_EK =  Xn1;	      												//填入EK
		SetBit(PI_LPF_CR,PISTA);										// Start PI
//	  PI_LPF_CR |= 0x02;	  											// Start PI
	  _nop_();  _nop_();  _nop_();  _nop_();  _nop_();
	  PI_UK += ((SKP>>12) +1);
	  return PI_UK;
}

void HW_PI_Control(OUTLOOP *PIPara)
{	  
		PI_EK =  PIPara->ExtErr;	      																						//填入本次EK 误差值
		SetBit(PI_LPF_CR,PISTA);																										// Start PI
	  _nop_();  _nop_();  _nop_();  _nop_();  _nop_();
	
		PI_UK			= PIPara->ExtOutL;
	  PI_UKMAX 	= SOUTMAX;
	  PI_UKMIN 	= SOUTMIN;
	  PIPara->ExtErr = PIPara->ExtRef - PIPara->ExtFed;
	  PI_EK =  PIPara->ExtErr;
	
	  SetBit(PI_LPF_CR,PISTA);																										// Start PI
	  _nop_();  _nop_();  _nop_();  _nop_();  _nop_();
		PIPara->ExtOut = PI_UK + 1;																									// 补偿PI输出误差
	  PIPara->ExtOutL= PIPara->ExtOut;
		
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	int16 LPF(int16 Xn1, int16 Xn0, int8 K)
	Description   :	LFP控制
	Input         :	Xn1--当前输入值
	                Xn0--上一次滤波输出值
									K--LPF滤波系数
  Output				:	LPF_Y--当前滤波输出值，执行时间为4us。
-------------------------------------------------------------------------------------------------*/
int16 LPFFunction(int16 Xn1, int16 Xn0, int8 K)
{
	LPF_K = K;
	LPF_X = Xn1;
	LPF_Y = Xn0;
	SetBit(PI_LPF_CR, LPFSTA);
	_nop_();_nop_();_nop_();_nop_();_nop_();
	return LPF_Y;
}

/*---------------------------------------------------------------------------*/
/* Name		:	void VSPSample(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	VSP采样
/*---------------------------------------------------------------------------*/
void VSPSample(void)
{
	/*****VREF的采样获取值并滤波******/
	AdcSampleValue.ADCVref = (ADC7_DR<<3);//采集对应端口
	VSP=LPFFunction(AdcSampleValue.ADCVref,VSP,30);//低通滤波

	  if((VSP > ONPWMDuty)&&(VSP <= OFFPWMDutyHigh))//在ONPWMDuty-OFFPWMDutyHigh之间，电机有转速运行
	  {
		  mcSpeedRamp.FlagONOFF = 1;
	  }
	  else if((VSP < OFFPWMDuty)||(VSP > OFFPWMDutyHigh))//电机停机
	  {
			mcSpeedRamp.FlagONOFF = 0;
	  }
		//转速曲线计算
		if(mcSpeedRamp.FlagONOFF==1)//
		{
      #if (Motor_Speed_Control_Mode == SPEED_LOOP_CONTROL)
      {
        if(VSP <= MINPWMDuty)    //最小转速运行
        {
          mcSpeedRamp.TargetValue = Motor_Min_Speed;
        }
        else if(VSP < MAXPWMDuty)//调速
        {
          mcSpeedRamp.TargetValue = Motor_Min_Speed + SPEED_K*(VSP-MINPWMDuty);
        }
        else		     //最大转速运行
        {
          mcSpeedRamp.TargetValue	=	Motor_Max_Speed;
        }
      }
      #elif (Motor_Speed_Control_Mode == POWER_LOOP_CONTROL)
      {
        if(VSP <= MINPWMDuty)    //最小转速运行
        {
          mcSpeedRamp.TargetValue = Motor_Min_Power;
        }
        else if(VSP < MAXPWMDuty)//调速
        {
          mcSpeedRamp.TargetValue = Motor_Min_Power + POWER_K*(VSP-MINPWMDuty);
        }
        else		     //最大转速运行
        {
          mcSpeedRamp.TargetValue	=	Motor_Max_Power;
        }
      }
      #endif

		}
		else
		{
			mcSpeedRamp.TargetValue =0;
		}
}
/*---------------------------------------------------------------------------*/
/* Name		:	void Sleepmode(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	睡眠模式测试
/*---------------------------------------------------------------------------*/
 void Sleepmode(void)
 {
	 	SleepSet.SleepDelayCout++;
		if(SleepSet.SleepDelayCout>=20000)//最大65530，若要再大，需改数据类型
		{
//				FOC_EFREQMIN 	= -Motor_Omega_Ramp_Min;
//				FOC_EFREQHOLD = -Motor_Omega_Ramp_End;
			mcSpeedRamp.TargetValue=0;
			MOE     = 0;
			ClrBit(DRV_CR, FOCEN);	//关闭FOC
			SleepSet.SleepDelayCout=0;
			SleepSet.SleepFlag=1;
			SetBit(P1_IE, P11);   // config P11 as the source of EXTI1
			SetBit(PCON, STOP);
		}

 }
/*---------------------------------------------------------------------------*/
/* Name		:	void StarRampDealwith(void)
/* Input	:	NO
/* Output	:	NO
/* Description:
/*---------------------------------------------------------------------------*/
void StarRampDealwith(void)
{
		if((mcState == mcRun))
		{
			if(mcFocCtrl.State_Count == 100)//2300
			{
				FOC_EKP = OBSW_KP_GAIN_RUN;	                          // 估算器里的PI的KP
				FOC_EKI	= OBSW_KI_GAIN_RUN;				                    // 估算器里的PI的KI
			}
			else if(mcFocCtrl.State_Count == 60)//2000
			{
				FOC_EKP = OBSW_KP_GAIN_RUN1;	                        // 估算器里的PI的KP
				FOC_EKI	= OBSW_KI_GAIN_RUN1;				                  // 估算器里的PI的KI
			}
			else if(mcFocCtrl.State_Count == 40)//1600
			{
				FOC_EKP = OBSW_KP_GAIN_RUN2;	                        // 估算器里的PI的KP
				FOC_EKI	= OBSW_KI_GAIN_RUN2;				                  // 估算器里的PI的KI
			}
			else if(mcFocCtrl.State_Count ==20)//1200
			{
				FOC_EKP = OBSW_KP_GAIN_RUN3;	                        // 估算器里的PI的KP
				FOC_EKI	= OBSW_KI_GAIN_RUN3;				                  // 估算器里的PI的KI
			}
			else if(mcFocCtrl.State_Count == 10)
			{
				FOC_EKP = OBSW_KP_GAIN_RUN4;	                        // 估算器里的PI的KP
				FOC_EKI	= OBSW_KI_GAIN_RUN4;				                  // 估算器里的PI的KI
			}
			else;
		}
}
