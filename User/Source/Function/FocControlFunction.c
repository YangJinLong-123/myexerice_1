/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : FocControl.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the foc control function used for motor control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>
#include <FU68xx_2_DMA.h>
#endif
CurrentOffset xdata mcCurOffset;

/*---------------------------------------------------------------------------*/
/* Name		:	void FOC_Init(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	mcInit状态下，对FOC的相关寄存器进行配置,先清理寄存器，后配置，最后使能
/*---------------------------------------------------------------------------*/
void FOC_Init(void)
{
	/*使能FOC*/
	ClrBit(DRV_CR, FOCEN);
	SetBit(DRV_CR, FOCEN);
	/*配置FOC寄存器*/
	FOC_CR1 			= 0;																			// 清零 FOC_CR1
	FOC_CR2 			= 0;																			// 清零 FOC_CR2
	FOC_IDREF 		= 0;																			// 清零 Id
	FOC_IQREF 		= 0;																			// 清零 Iq

	FOC__THETA 		= 0;																			// 清零 角度
	FOC_RTHEACC 	= 0;																			// 清零 爬坡函数的初始加速度
	FOC__RTHESTEP 	= 0;																		//
	FOC__RTHECNT 	= 0;																			//
	FOC__THECOMP 	= _Q15(2.0/180.0);												// SMO 估算补偿角
	FOC__THECOR 		= 0x02;																	// 误差角度补偿

	/*电流环参数配置*/
	FOC_DMAX 			= DOUTMAX;
	FOC_DMIN 			= DOUTMIN;

	FOC_QMAX 			= QOUTMAX;
	FOC_QMIN 			= QOUTMIN;

	/*位置估算参数配置*/
	FOC_EK1 			= OBS_K1T;
	FOC_EK2 			= OBS_K2T;
	FOC_EK3 			= OBS_K3T;
	FOC_EK4 			= OBS_K4T;
	FOC_FBASE 		= OBS_FBASE;
	FOC_OMEKLPF 	= SPEED_KLPF;
	FOC_EBMFK 		= OBS_KLPF;

  FOC_TGLI      = PWM_TGLI_LOAD;

	/*********PLL或SMO**********/
	#if (EstimateAlgorithm == SMO)
	{
			ClrBit(FOC_CR2, ESEL);
			FOC_KSLIDE    = OBS_KSLIDE;
			FOC_EKLPFMIN	= OBS_EA_KS;
	}
	#elif (EstimateAlgorithm == PLL)
	{
			SetBit(FOC_CR2, ESEL);
			FOC_KSLIDE    = OBSE_PLLKP_GAIN;
			FOC_EKLPFMIN	= OBSE_PLLKI_GAIN;
	}
	#endif //end SVPMW_Mode

	SetBit(FOC_CR1, SVPWMEN);															// SVPWM模式

//	/*正反转*/
//  if(mcFRState.FR==1)                                 // 设置F/R
//  {
//    SetBit(DRV_CR,DRV_DDIR);
//  }
//  else
//  {
//    ClrBit(DRV_CR,DRV_DDIR);
//  }
	#if ( IRMODE==1)
	{
		SetBit(DRV_CR,DDIR);                               // 反转标志位
	}
	#endif //end IRMODE

	/**过调制**/
	#if (OverModulation == 1)
	{
			SetBit(FOC_CR1,OVMDL);													// 过调制
	}
	#endif //end OverModulation

	/*单电阻采样；需要最小采样窗,FOC_TRGDLY为0，七段式SVPWM方式*/
	#if (Shunt_Resistor_Mode == Single_Resistor)
	{
		SetReg(FOC_CR1, CSM0 | CSM1, 0x00);
		FOC_TSMIN = PWM_TS_LOAD;															// 最小采样窗口
		FOC_TRGDLY = 0x08;          													// 采样时刻在中点，一般考虑开关噪声影响，会设置延迟；0x08
		                                                      // 0x0c表示延迟12个clock，提前用反码形式，如0x84表示提前12个clock。
		ClrBit(FOC_CR2,F5SEG);															  // 7段式
		SetReg(CMP_CR1, CMP3MOD0 | CMP3MOD1, 0x00);
	}
	/*双电阻采样，可设置死区补偿值，在下降沿结束前开始采样Ia，配置81*/
	#elif (Shunt_Resistor_Mode == Double_Resistor)					// double resistor sample
	{
		SetReg(FOC_CR1, CSM0 | CSM1, CSM0);

		FOC_TSMIN = PWM_DT_LOAD;															// 死区补偿值
		FOC_TRGDLY = 0x83;																	  // ADC采样的时刻，采样时刻在计数器零点附近，83为下降沿结束前3个clock采样Ia，与单电阻不同
		                                                      // 01为上升沿开始后第一个clock开始采样。根据实际情况调整。
		FOC_TBLO=PWM_DLOWL_TIME;                              //下桥臂最小脉冲，保证采样
		SetReg(CMP_CR1, CMP3MOD0 | CMP3MOD1, 0x00);

		/*五段式或七段式选择*/
		#if (SVPMW_Mode == SVPWM_7_Segment)
		{
			ClrBit(FOC_CR2,F5SEG);															// 7段式
		}
		#elif (SVPMW_Mode == SVPWM_5_Segment)
		{
			SetBit(FOC_CR2,F5SEG);															// 7段式
		}
		#endif

		#if (DouRes_Sample_Mode == DouRes_1_Cycle)
		{
			ClrBit(FOC_CR2,DSS);															  // 7段式
		}
		#elif (DouRes_Sample_Mode == DouRes_2_Cycle)
		{
			SetBit(FOC_CR2,FOC_DSS);														// 7段式
		}
		#endif //end DouRes_Sample_Mode
	}
	/*三电阻采样*/
	#elif (Shunt_Resistor_Mode == Three_Resistor)					  // signel resistor sample
	{

		SetReg(FOC_CR1, CSM0 | CSM1, CSM0 | CSM1);// 三电阻

		FOC_TSMIN  = PWM_DT_LOAD;															// 死区补偿值
		FOC_TRGDLY =06;																			  // ADC采样的时刻，采样时刻在计数器零点附近，83为下降沿结束前3个clock采样Ia，与单电阻不同。
		                                                      // 01为上升沿开始后第一个clock开始采样。根据实际情况调整。

		SetReg(CMP_CR1, CMP3MOD0 | CMP3MOD1, CMP3MOD0 | CMP3MOD1);
		FOC_TBLO= PWM_OVERMODULE_TIME;                        // 过调制电流采样处理的TB脉宽

		/*五段式或七段式选择*/
		#if (SVPMW_Mode == SVPWM_7_Segment)
		{
			ClrBit(FOC_CR2,F5SEG);															// 7段式
		}
		#elif (SVPMW_Mode == SVPWM_5_Segment)
		{
			SetBit(FOC_CR2,F5SEG);															// 7段式
		}
		#endif //end SVPMW_Mode

		#if (DouRes_Sample_Mode == DouRes_1_Cycle)
		{
			ClrBit(FOC_CR2,DSS);															// 7段式
		}
		#elif (DouRes_Sample_Mode == DouRes_2_Cycle)
		{
			SetBit(FOC_CR2,DSS);															// 7段式
		}
		#endif //end DouRes_Sample_Mode
	}
	#endif	//end Shunt_Resistor_Mode


	/* 使能电流基准校正	*/
	#if (CalibENDIS == Enable)
	{
		if(mcCurOffset.OffsetFlag==1)
		{
			#if (Shunt_Resistor_Mode == Single_Resistor)	      // 单电阻校正
			{
				/*set ibus current sample offset*/
				SetReg(FOC_CR2, CSOC0 | CSOC1, 0x00);
				FOC_CSO = mcCurOffset.Iw_busOffset;	              // 写入Ibus的偏置

			}
			#elif (Shunt_Resistor_Mode == Double_Resistor)			// 双电阻校正
			{
				/*set ia, ib current sample offset*/
				SetReg(FOC_CR2, CSOC0 | CSOC1, CSOC0);
				FOC_CSO  = mcCurOffset.IuOffset;                  // 写入IA的偏置

				SetReg(FOC_CR2, CSOC0 | CSOC1, CSOC1);
				FOC_CSO  = mcCurOffset.IvOffset;	                // 写入IB的偏置

			}
			#elif (Shunt_Resistor_Mode == Three_Resistor)	      // 三电阻校正
			{
				/*set ibus current sample offset*/
				SetReg(FOC_CR2, CSOC0 | CSOC1, CSOC0);
				FOC_CSO = mcCurOffset.IuOffset; 									// 写入IA的偏置

				SetReg(FOC_CR2, CSOC0 | CSOC1, CSOC1);
				FOC_CSO = mcCurOffset.IvOffset;	                  // 写入IB的偏置

				SetReg(FOC_CR2, CSOC0 | CSOC1, 0x00);
				FOC_CSO = mcCurOffset.Iw_busOffset;	              // 写入IC的偏置
			}
			#endif	//end Shunt_Resistor_Mode
		}
	}
	#endif	//end CalibENDIS
	/*-------------------------------------------------------------------------------------------------
	DRV_CTL：PWM来源选择
	OCS = 0, DRV_COMR
	OCS = 1, FOC/SVPWM/SPWM
	-------------------------------------------------------------------------------------------------*/
	/*计数器比较值来源FOC*/
	SetBit(DRV_CR, OCS);
}
/*---------------------------------------------------------------------------*/
/* Name		:	void Motor_Charge(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	预充电，当一直处于预充电状态下，不接电机，可用于验证IPM或者Mos。
预充电分三步，第一步是对U相进行预充电，第二步是对U,V两相进行预充电;第三步是对U、V、W三相进行预充电。
/*---------------------------------------------------------------------------*/
void Motor_Charge(void)
{
     if(McStaSet.SetFlag.ChargeSetFlag==0)
		 {
				McStaSet.SetFlag.ChargeSetFlag = 1;
       	#if (PWM_Level_Mode == Low_Level)	 //用于測試6536
				{
         SetBit(P1_OE, P10);
         GP10=1;
        }
        #endif
			 	#if (IPMState == IPMtest)
				 {
				   DRV_DR = 0.7*DRV_ARR;						    // IPM 70% duty
				 }
				 #elif (IPMState == NormalRun)		      // 正常按电机状态机运行
				 {
           DRV_DR = 0.1*DRV_ARR;				        //下桥臂10% duty
				 }
				#endif
			 	/*-------------------------------------------------------------------------------------------------
				DRV_CTL：PWM来源选择
				OCS = 0, DRV_COMR
				OCS = 1, FOC/SVPWM/SPWM
				-------------------------------------------------------------------------------------------------*/
				ClrBit(DRV_CR, OCS);
				mcFocCtrl.ChargeStep = 0;
		 }
		 if((mcFocCtrl.State_Count < Charge_Time)&&(mcFocCtrl.ChargeStep == 0))
		 {
			 mcFocCtrl.ChargeStep = 1;
			 #if (IPMState == IPMtest)
			 {
			   DRV_CMR |= 0x03;                         // U相输出
			 }
			 #elif (IPMState == NormalRun)		          // 正常按电机状态机运行
			 {
			   DRV_CMR |= 0x01;                         // U相下桥臂通
			 }
			 #endif
			 MOE = 1;
		 }
		 if(( mcFocCtrl.State_Count <= (Charge_Time<<1)/3)&&(mcFocCtrl.ChargeStep== 1))
		 {
			 mcFocCtrl.ChargeStep = 2;
			 #if (IPMState == IPMtest)
			 {
         DRV_CMR |= 0x0F;                         // U、V相输出
			 }
			 #elif (IPMState == NormalRun)		          // 正常按电机状态机运行
			 {
         DRV_CMR |= 0x04;                         // V相下桥臂导通
			 }
       #endif
		 }
		 if((mcFocCtrl.State_Count <= Charge_Time/3)&&(mcFocCtrl.ChargeStep== 2))
		 {
			  mcFocCtrl.ChargeStep = 3;
			 #if (IPMState == IPMtest)
			 {
         DRV_CMR |= 0x3F;                         // U、V、W相输出
	     }
			 #elif (IPMState == NormalRun)		          // 正常按电机状态机运行
			 {
			   DRV_CMR |= 0x10;                         // W相下桥臂导通
			 }
       #endif
		 }
}
/*---------------------------------------------------------------------------*/
/* Name		:	void Motor_Align(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	预定位函数，当无逆风判断时，采用预定位固定初始位置;当有逆风判断时，采用预定位刹车
/*---------------------------------------------------------------------------*/
void Motor_Align(void)
{
	if(McStaSet.SetFlag.AlignSetFlag==0)
	{
		  McStaSet.SetFlag.AlignSetFlag=1;

		  /*FOC初始化*/
			FOC_Init();

		  /*配置预定位的电流、KP、KI*/
			FOC_IDREF = ID_Align_CURRENT;
			FOC_IQREF = IQ_Align_CURRENT;

			FOC_DQKP = DQKP_Alignment;
			FOC_DQKI = DQKI_Alignment;

			FOC_EKP 	= OBSW_KP_GAIN;
			FOC_EKI 	= OBSW_KI_GAIN;

      /*配置预定位角度*/
			#if (AlignTestMode==1)
			{
				FOC__THETA		= Align_Theta;
			}
			#else
			{
				#if (PosCheckEnable==1)
				{
				  FOC__THETA 	= mcFocCtrl.mcPosCheckAngle + _Q15((float)60.0/180.0);
//					if(FOC__THETA >= _Q15((float)180.0/180.0))
//					{
//						FOC__THETA 	= FOC__THETA - _Q15((float)240.0/180.0);
//					}
					
				}
				#else
				{
					FOC__THETA		=Align_Theta;
				}
			  #endif	//end PosCheckEnable
			}
			#endif	//end AlignTestMode
						/*********PLL或SMO**********/
			#if (EstimateAlgorithm == SMO)
			{
			  FOC__ETHETA 	= FOC__THETA-4096;
			}
			#elif (EstimateAlgorithm == PLL)
			{
			  FOC__ETHETA 	= FOC__THETA;
			}
			#endif //end 	EstimateAlgorithm
			TimeCnt=FOC__THETA;

		  /*使能输出*/
			DRV_CMR |= 0x3F;                         // U、V、W相输出
			MOE = 1;

  }
}

/*---------------------------------------------------------------------------*/
/* Name		:	void Motor_Open(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	开环启动的参数配置
/*---------------------------------------------------------------------------*/
void Motor_Open(void)
{
		static uint8 OpenRampCycles;
    if(McStaSet.SetFlag.StartSetFlag==0)
		{
			McStaSet.SetFlag.StartSetFlag=1;


	    /****启动初始角度赋值**/
			#if (PosCheckEnable)
			{
				FOC__THETA 	= mcFocCtrl.mcPosCheckAngle;// 有初始位置检测，则用初始位置角
			}
			#else
			{
				FOC__THETA		= Align_Theta;            // 无初始位置检测，则用预定位角
			}
			#endif
			/*********PLL或SMO**********/
			#if (EstimateAlgorithm == SMO)
			{
			  FOC__ETHETA 	= FOC__THETA-4915;
			}
			#elif (EstimateAlgorithm == PLL)
			{
			  FOC__ETHETA 	= FOC__THETA;
			}
			#endif //end 	EstimateAlgorithm

			TimeCnt=FOC__ETHETA;
			/*启动电流、KP、KI、FOC_EKP、FOC_EKI*/
			FOC_IDREF = ID_Start_CURRENT;                         // D轴启动电流
			mcFocCtrl.mcIqref= IQ_Start_CURRENT;		              // Q轴启动电流

		 #if (FRDetectMethod==BEMFMethod)
			{
				if(BEMFDetect.BEMFCCWFlag==1)
				{
					mcFocCtrl.mcIqref+=I_Value(0.95);                    //逆风时，加大启动电流
				}
				else if(BEMFDetect.BEMFCCWFlag==2)
				{
					mcFocCtrl.mcIqref+=I_Value(0.8);                    //逆风时，加大启动电流
				}
			}
			#endif
			#if (FRDetectMethod==RSDMethod)
			{
				if((RSDDetect.RSDCCWFlag==1)||(RSDDetect.RSDCCWTimes>=4))
				{
					mcFocCtrl.mcIqref+=I_Value(0.95);                    //逆风时，加大启动电流
				}
				else if(RSDDetect.RSDCCWFlag==2)
				{
					mcFocCtrl.mcIqref+=I_Value(0.9);                    //逆风时，加大启动电流
				}
			}
			#endif

//			FOC_DQKP = DQKP;
//			FOC_DQKI = DQKI;
			FOC_DQKP	= DQKPStart;
			FOC_DQKI	= DQKIStart;				

			//启动时设置Q轴PI下限起始值
      FOC_QMAX 			=  QOUTMAX;
		  FOC_QMIN 			= QOUTMIN;
			
			FOC_EKP 	= OBSW_KP_GAIN;
			FOC_EKI 	= OBSW_KI_GAIN;

			
			/*启动方式选择*/
			#if (Open_Start_Mode == Omega_Start)                  // Omega 启动
			{
				FOC_EFREQACC 	= Motor_Omega_Ramp_ACC;
				FOC_EFREQMIN 	= Motor_Omega_Ramp_Min;
				FOC_EFREQHOLD = Motor_Omega_Ramp_End;

				SetBit(FOC_CR1,EFAE);															// 估算器强制输出
				ClrBit(FOC_CR1,RFAE);															// 禁止强拉
				SetBit(FOC_CR1,ANGM);															// 估算模式
			}
			#elif (Open_Start_Mode == Open_Start)
			{
				FOC_RTHEACC 	= Motor_Open_Ramp_ACC;								      // 爬坡函数的初始加速度
				FOC__RTHESTEP 	= Motor_Open_Ramp_Min;								// 0.62 degree acce speed
				RPDPara.ThetaGet=Motor_Open_Ramp_Min;
				FOC__RTHECNT 	= MOTOR_OPEN_ACC_CNT;									// acce time

				ClrBit(FOC_CR1,EFAE);															// 估算器强制输出
				SetBit(FOC_CR1,RFAE);															// 禁止强拉
				ClrBit(FOC_CR1,ANGM);															// 估算模式
			}
			#elif (Open_Start_Mode == Open_Omega_Start)
			{
				FOC_RTHEACC 	= Motor_Open_Ramp_ACC;								      // 爬坡函数的初始加速度
				FOC__RTHESTEP 	= Motor_Open_Ramp_Min;								// 0.62 degree acce speed
				FOC__RTHECNT 	= MOTOR_OPEN_ACC_CNT;									// acce time

				FOC_EFREQACC 	= Motor_Omega_Ramp_ACC;
				FOC_EFREQMIN 	= Motor_Omega_Ramp_Min;
				FOC_EFREQHOLD = Motor_Omega_Ramp_End;

				SetBit(FOC_CR1,EFAE);															// 估算器强制输出
				SetBit(FOC_CR1,RFAE);															// 禁止强拉
				SetBit(FOC_CR1,ANGM);															// 估算模式
			}
			#endif //end Open_Start_Mode
			
	  }
      /*不同启动方式下，切换到MCRUN状态*/
			#if (Open_Start_Mode == Open_Start)			//OPEN状态启动时拖动多次
			{
				if(OpenRampCycles<(MOTOR_OPEN_ACC_CYCLE-1))
				{
					if(!ReadBit(FOC_CR1,RFAE))
					{
						SetBit(FOC_CR1,RFAE);
						OpenRampCycles++;
					}
				}
				else
				{
					mcFocCtrl.State_Count = 2;

					mcState = mcRun;
				}
					FOC_EKP = OBSW_KP_GAIN_RUN4;	                        // 估算器里的PI的KP
				  FOC_EKI	= OBSW_KI_GAIN_RUN4;				                  // 估算器里的PI的KI
			}
			#elif (Open_Start_Mode == Open_Omega_Start)
			{
					mcFocCtrl.State_Count = 200;
					mcState = mcRun;
			}
			#elif (Open_Start_Mode == Omega_Start)
			{
				/*********PLL或SMO**********/
				#if (EstimateAlgorithm == SMO)
				{
				   mcFocCtrl.State_Count = 120;
				}
				#elif (EstimateAlgorithm == PLL)
				{
			    FOC_EKP = OBSW_KP_GAIN_RUN4;	                        // 估算器里的PI的KP
				  FOC_EKI	= OBSW_KI_GAIN_RUN4;				                  // 估算器里的PI的KI
				}
				#endif //end 	EstimateAlgorithm
				mcState = mcRun;
			}
			#endif //end Open_Start_Mode
			
			FOC_IQREF = mcFocCtrl.mcIqref;		                    // Q轴启动电流
}

/*---------------------------------------------------------------------------*/
/* Name		:	void Motor_Open(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	开环启动的参数配置
/*---------------------------------------------------------------------------*/
void Motor_PllStart(void)
{
		static uint8 OpenRampCycles;
		if(TailWindDetect.PLLFlag==1)
		{
			TailWindDetect.PLLSpeed=FOC__EOME;
			if(Abs_F16(TailWindDetect.PLLSpeed)<100)//逆风状态下，速度低于100时，计数，达到一定时间后，PLLFlag清零。
			{
				TailWindDetect.PLLDetectCnt++;
				if(TailWindDetect.PLLDetectCnt>3000)//防止估算错误
				{
				  TailWindDetect.PLLFlag=0;
					TailWindDetect.PLLTheta=FOC__ETHETA;
					FOC_IQREF =0;
				}
			}
			else
			{
				if(TailWindDetect.PLLDetectCnt>0)
				{
					TailWindDetect.PLLDetectCnt--;
        }
      }
		}
    if((McStaSet.SetFlag.StartSetFlag==0)&&(TailWindDetect.PLLFlag==0))
		{
			McStaSet.SetFlag.StartSetFlag=1;
			MOE = 0;
		  FOC_Init();
			FOC__ETHETA		= TailWindDetect.PLLTheta+19000;//16668;//

			/*启动电流、KP、KI、FOC_EKP、FOC_EKI*/
			FOC_IDREF = ID_Start_CURRENT;                         // D轴启动电流
			FOC_IQREF = mcFocCtrl.mcIqref;		                    // Q轴启动电流

			FOC_DQKP = DQKP;
			FOC_DQKI = DQKI;

			/*启动方式选择*/
			#if (Open_Start_Mode == Omega_Start)                  // Omega 启动
			{
				FOC_EFREQACC 	= Motor_Omega_Ramp_ACC;
				FOC_EFREQMIN 	= Motor_Omega_Ramp_Min;
				FOC_EFREQHOLD = Motor_Omega_Ramp_End;

				SetBit(FOC_CR1,EFAE);															// 禁止估算器强制输出
				ClrBit(FOC_CR1,RFAE);															// 使能强拉
				SetBit(FOC_CR1,ANGM);															// 估算模式
			}
			#elif (Open_Start_Mode == Open_Start)
			{
				FOC_RTHEACC 	= Motor_Open_Ramp_ACC;								      // 爬坡函数的初始加速度
				FOC__RTHESTEP 	= Motor_Open_Ramp_Min;								// 0.62 degree acce speed
				FOC__RTHECNT 	= MOTOR_OPEN_ACC_CNT;									// acce time

				ClrBit(FOC_CR1,EFAE);															// 禁止估算器强制输出
				SetBit(FOC_CR1,RFAE);															// 使能强拉
				ClrBit(FOC_CR1,ANGM);															// 估算模式
			}
			#elif (Open_Start_Mode == Open_Omega_Start)
			{
				FOC_RTHEACC 	= Motor_Open_Ramp_ACC;								      // 爬坡函数的初始加速度
				FOC__RTHESTEP 	= Motor_Open_Ramp_Min;								// 0.62 degree acce speed
				FOC__RTHECNT 	= MOTOR_OPEN_ACC_CNT;									// acce time

				FOC_EFREQACC 	= Motor_Omega_Ramp_ACC;
				FOC_EFREQMIN 	= Motor_Omega_Ramp_Min;
				FOC_EFREQHOLD = Motor_Omega_Ramp_End;

				SetBit(FOC_CR1,EFAE);															// 禁止估算器强制输出
				SetBit(FOC_CR1,RFAE);															// 使能强拉
				SetBit(FOC_CR1,ANGM);															// 估算模式
			}
			#endif //end Open_Start_Mode
			DRV_CMR |= 0x3F;                         // U、V、W相输出
			MOE = 1;
	  }

    if((McStaSet.SetFlag.StartSetFlag==1)&&(TailWindDetect.PLLFlag==0))
		{
      /*不同启动方式下，切换到MCRUN状态*/
			#if (Open_Start_Mode == Open_Start)			//OPEN状态启动时拖动多次
			{
				if(OpenRampCycles<(MOTOR_OPEN_ACC_CYCLE-1))
				{
					if(!ReadBit(FOC_CR1,RFAE))
					{
						SetBit(FOC_CR1,RFAE);
						OpenRampCycles++;
					}
				}
				else
				{
					mcFocCtrl.State_Count = 2;
					mcState = mcRun;
				}
					FOC_EKP = OBSW_KP_GAIN_RUN4;	                        // 估算器里的PI的KP
				  FOC_EKI	= OBSW_KI_GAIN_RUN4;				                  // 估算器里的PI的KI
			}
			#elif (Open_Start_Mode == Open_Omega_Start)
			{
					mcFocCtrl.State_Count = 2600;
					mcState = mcRun;
			}
			#elif (Open_Start_Mode == Omega_Start)
			{

			    FOC_EKP = OBSW_KP_GAIN_RUN4;	                        // 估算器里的PI的KP
				  FOC_EKI	= OBSW_KI_GAIN_RUN4;				                  // 估算器里的PI的KI
				  mcState = mcRun;
			}
			#endif //end Open_Start_Mode
		}

}
/*---------------------------------------------------------------------------*/
/* Name		:	void Motor_Align(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	顺风逆风参数配置函数
/*---------------------------------------------------------------------------*/
void Motor_TailWind(void)
{
	if(mcFocCtrl.State_Count == 0)
	{
	  if(McStaSet.SetFlag.TailWindSetFlag  ==0)//初始化
	  {
				McStaSet.SetFlag.TailWindSetFlag =1;

				#if (FRDetectMethod==RSDMethod)
				 {
					 RSDDetectInit();
				 }
				#elif (FRDetectMethod==BEMFMethod)
				 {
					 BEMFDetectInit();
         }
				#elif (FRDetectMethod==FOCMethod)
				 {
		       TailWindDetectInit();
         }
				#endif

	  }
		#if (FRDetectMethod==RSDMethod)
	  {
      RSDDealwith();
	  }
		#elif (FRDetectMethod==BEMFMethod)
	  {
		  BEMFDealwith();
	  }
	  #elif (FRDetectMethod==FOCMethod)
	  {
      FOCTailWindDealwith();
	  }
	 #endif

	}
}

/*---------------------------------------------------------------------------*/
/* Name		:	void MC_Stop(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	inital motor control parameter
/*---------------------------------------------------------------------------*/
void MC_Stop(void)
{
	MOE     = 0;
	ClrBit(DRV_CR, FOCEN, 0);	//关闭FOC																											// disable FOC output and initial register
	mcState = mcInit;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	void MotorControlInit(void)
	Description   :	控制变量初始化清零,包括保护参数的初始化、电机状态初始化
	Input         :	输入说明（详细）
  Output				:	输出说明（详细）
-------------------------------------------------------------------------------------------------*/
void MotorcontrolInit(void)
{
  /***********保护******************/
	memset(&mcFaultDect,0, sizeof(FaultVarible));																// FaultVarible变量清零
  /******堵转保护*********/
//	mcFaultDect.StallRecover          = 1000;//5s

  /******保护次数*********/
	memset(&mcProtectTime,0, sizeof(ProtectVarible));														// ProtectVarible保护次数清零 
  /*******过流保护*****************/
	memset(&mcCurVarible,0, sizeof(CurrentVarible));														// 电流保护的变量清零

  /*****顺逆风判断，FOCMethod不需要在此赋初值*******/
	#if (FRDetectMethod==RSDMethod)
	{
   memset(&RSDDetect,0, sizeof(MotorRSDTypeDef));	//	RSDDetect所有变量清零
	}
	#elif (FRDetectMethod==BEMFMethod)
	{
   memset(&BEMFDetect,0, sizeof(BEMFDetect_TypeDef));		//	BEMFDetect所有变量清零
	} 
  #endif

	/*******启停测试的参数***************/
	memset(&ONOFFTest,0, sizeof(ONVarible));

  /*****电机状态机时序变量***********/
	McStaSet.SetMode                   = 0;

  /*****电机目标方向**********/
	mcFRState.TargetFR                 = 0;

  /*****外部控制环*******/
	memset(&mcFocCtrl,0, sizeof(FOCCTRL));																// mcFocCtrl变量清零

  /******ADC采样滤波值*********/
	memset(&AdcSampleValue,0, sizeof(ADCSample));													// ADCSample变量清零

  /******电流偏置校准变量*****/
	memset(&mcCurOffset,0, sizeof(CurrentOffset));												// mcCurOffset变量清零
	mcCurOffset.IuOffsetSum            = 16383;
	mcCurOffset.IvOffsetSum            = 16383;
	mcCurOffset.Iw_busOffsetSum        = 16383;

	/*****LED灯响应***/
	memset(&mcLedDisplay,0, sizeof(MCLedDisplay));												// mcLedDisplay变量清零
	mcLedDisplay.Counttime            = 4999;

	/*****速度环的响应***/
	memset(&mcSpeedRamp,0, sizeof(MCRAMP));												        // mcSpeedRamp变量清零
//	mcSpeedRamp.IncValue              = Motor_Speed_Inc;
//	mcSpeedRamp.DecValue              = Motor_Speed_Dec;
	mcSpeedRamp.DelayPeriod           = 1;

	/*PWM调速变量*/
	memset(&mcPwmInput,0, sizeof(PWMINPUTCAL));												    				// 清零PWM duty 调速变量mcPwmInput
	memset(&mcPWMFreqInputCtrl,0, sizeof(PWMFREQINPUTTRPE));																// 清零PWM 调频调速变量PWMINCtl

	/*串口变量*/
	memset(&Uart,0, sizeof(MCUART));												              				// MCUART变量清零

	/*睡眠模式*/  
	memset(&SleepSet,0, sizeof(SLEEPMODE));		                                  	// 睡眠模式清零

//
  Time.Start_CNT                     = 0;
	Time.UnderVoltageCount             = 0;
}

/*---------------------------------------------------------------------------*/
/* Name		:	void VariablesPreInit(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	初始化电机参数
/*---------------------------------------------------------------------------*/
void VariablesPreInit(void)
{
	/***********保护******************/
	mcFaultSource = 0;
	memset(&mcFaultDect,0, sizeof(FaultVarible));																	// FaultVarible变量清零

  /*****外部控制环*******/
	memset(&mcFocCtrl,0, sizeof(FOCCTRL));																				// mcFocCtrl变量清零

  /*****顺逆风判断*******/
	#if (FRDetectMethod==RSDMethod)
	{
	  RSDDetect.RSDCCWTimes    = 0;                             									//  CCW刹车次数清零
	}
	#elif (FRDetectMethod==BEMFMethod)
	{
	}
	#elif (FRDetectMethod==FOCMethod)
	{
	  memset(&TailWindDetect,0, sizeof(MotorTailWindTypeDef));										//	TailWindDetect所有变量清零
	}
  #endif
  /*****电机状态机时序变量***********/
  McStaSet.SetMode                   = 0;

	/*****LED灯响应***/
	memset(&mcLedDisplay,0, sizeof(MCLedDisplay));																// mcLedDisplay变量清零
	mcLedDisplay.Counttime            = 4999;

	//堵转保护电流值设置
 // mcFaultDect.StallCurrentVale       = StallCurrentValue1;
}
/*---------------------------------------------------------------------------*/
/* Name		:	void GetCurrentOffset(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	上电时，先对硬件电路的电流进行采集，写入对应的校准寄存器中。
								调试时，需观察mcCurOffset结构体中对应变量是否在范围内。采集结束后，OffsetFlag置1。
/*---------------------------------------------------------------------------*/
void GetCurrentOffset(void)
{
	if(mcCurOffset.OffsetFlag==0)
  {      
	  SetBit(ADC_CR, ADCBSY);		                           // 使能ADC
		while(ReadBit(ADC_CR, ADCBSY));
		#if (Shunt_Resistor_Mode == Single_Resistor)			       // 29.2ms 单电阻模式，上电验证硬件电路时，需观察mcCurOffset.IbusOffset是否为4096
	  {
			mcCurOffset.Iw_busOffsetSum+=((ADC4_DR& 0x0fff) << 3);
			mcCurOffset.Iw_busOffset = mcCurOffset.Iw_busOffsetSum >> 4;
			mcCurOffset.Iw_busOffsetSum -= mcCurOffset.Iw_busOffset;
		}
		#elif (Shunt_Resistor_Mode == Double_Resistor)				   //44ms 双电阻模式，上电验证硬件电路时，需观察mcCurOffset.IaOffset、mcCurOffset.IbOffset是否为4096
		{
			mcCurOffset.IuOffsetSum+=((ADC0_DR& 0x0fff) << 3);
			mcCurOffset.IuOffset = mcCurOffset.IuOffsetSum >> 4;
			mcCurOffset.IuOffsetSum -= mcCurOffset.IuOffset;

			mcCurOffset.IvOffsetSum+=((ADC1_DR& 0x0fff) << 3);
			mcCurOffset.IvOffset = mcCurOffset.IvOffsetSum >> 4;
			mcCurOffset.IvOffsetSum -= mcCurOffset.IvOffset;

		}
		#elif (Shunt_Resistor_Mode == Three_Resistor)			      //58.2ms 三电阻模式，上电验证硬件电路时，需观察mcCurOffset.IaOffset、mcCurOffset.IbOffset、mcCurOffset.IcOffset是否为4096
		{
			mcCurOffset.IuOffsetSum+=((ADC0_DR& 0x0fff) << 3);
			mcCurOffset.IuOffset = mcCurOffset.IuOffsetSum >> 4;
			mcCurOffset.IuOffsetSum -= mcCurOffset.IuOffset;

			mcCurOffset.IvOffsetSum+=((ADC1_DR& 0x0fff) << 3);
			mcCurOffset.IvOffset = mcCurOffset.IvOffsetSum >> 4;
			mcCurOffset.IvOffsetSum -= mcCurOffset.IvOffset;

			mcCurOffset.Iw_busOffsetSum+=((ADC4_DR& 0x0fff) << 3);
			mcCurOffset.Iw_busOffset = mcCurOffset.Iw_busOffsetSum >> 4;
			mcCurOffset.Iw_busOffsetSum -= mcCurOffset.Iw_busOffset;
		}
		#endif

		mcCurOffset.OffsetCount++;
		if(mcCurOffset.OffsetCount>Calib_Time)
		{
			mcCurOffset.OffsetFlag=1;
    }
	}
}
/*---------------------------------------------------------------------------*/
/* Name		:	void Motor_Ready(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	上电时，关闭输出，先对硬件电路的电流进行采集，在FOC_Init中写入对应的校准寄存器中。
                调试时，需观察mcCurOffset结构体中对应变量是否在范围内。
/*---------------------------------------------------------------------------*/
void Motor_Ready(void)
{
   if(McStaSet.SetFlag.CalibFlag==0)
   {
     McStaSet.SetFlag.CalibFlag=1;
     ClrBit(DRV_CR, FOCEN);      // 关闭FOC
     MOE			= 0;               // 关闭MOE
     SetBit(ADC_MASK_SYSC, CH7EN |CH4EN | CH3EN | CH2EN | CH1EN | CH0EN);// 开启ADC
     mcCurOffset.OffsetFlag=0;   // 开始电流采集
   }
}
/*---------------------------------------------------------------------------*/
/* Name		:	void Motor_Init(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	对电机相关变量、PI进行初始化设置
/*---------------------------------------------------------------------------*/
void Motor_Init(void)
{
   ClrBit(ADC_MASK_SYSC, CH4EN |  CH1EN | CH0EN);// 关闭软件电流采样的ADC
	 VariablesPreInit();                           // 电机相关变量初始化
	 PI_Init();			                               // PI初始化
	 User.Temperature   = 0;
}
#endif