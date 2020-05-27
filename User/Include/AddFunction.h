/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : AddFunction.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __AddFuntion_H_
#define __AddFuntion_H_

/* Exported types -------------------------------------------------------------------------------*/
typedef struct
{
    //Current protect
		uint8		OverCurCnt;          	  // 软件过流计数

		uint16      Abs_ia;				      // IA的绝对值
		uint16      Abs_ib;				      // IB的绝对值
		uint16      Abs_ic;				      // IC的绝对值

		uint16      Max_ia;				      // IA的最大值
		uint16      Max_ib;				      // IB的最大值
		uint16      Max_ic;				      // IC的最大值
}CurrentVarible;
typedef struct
{
    uint16      SecondStartTimes;   // 二次启动保护的次数
  	uint8       StartFlag;          // 启动保护的标志位，用于判断哪个方法起作用
		uint16      StallTimes;	        // 堵转保护次数
		uint8       StallFlag;          // 堵转保护的标志位，用于判断哪个方法起作用
		uint16      LossPHTimes;        // 缺相保护次数
		uint16      CurrentPretectTimes;// 过流保护次数
	  uint16      PowerPretectTimes;  // 功率保护次数
}ProtectVarible;
typedef struct
{
	  uint16 			segment;            // 分段执行
  
//voltage protect
    uint16 			OverVoltDetecCnt; // 过压检测计数
    uint16 		  UnderVoltDetecCnt;// 欠压检测计数
		uint16      VoltRecoverCnt;	  // 过压恢复计数
	  uint16      VoltDetecBraketCount; //电压保护刹车计数

		uint16			CloseLedCnt;
//Power Protect
	  uint16 			OverPowerDetecCnt;   //过功率检测次数
//OVER Current protect recover
	  uint16      CurrentRecoverCnt;  // 过流保护恢复计数


//stall protect
		uint16     	StallDelayCnt;    // 堵转延迟判断计时
		uint16      StallDectEs;      // method 1，与ES相关
		uint16  		StallDectSpeed;   // method 2，与速度相关
		uint16    	StallDectESSpeed; // method 3，与ES和速度相关
//		uint16 			StallCurrentVale;// = StallCurrentValue1;
	  uint16      mcStallDeCurrent;     //堵转电流检测次数
		uint16      StallReCount;	    // 堵转保护恢复计数
	
	
//		uint16      StallRecover;     // 堵转保护恢复时间

//Loss Phase protect

		uint16      Lphasecnt;          // 缺相保护计时
		uint16      AOpencnt ;          // A缺相计数
		uint16      BOpencnt ;          // B缺相计数
		uint16      COpencnt ;          // C缺相计数
		uint16      mcLossPHRecCount;   // 缺相恢复计数
//start protect

		int16       StartESCount;     // 启动保护判断ES的计数
		int16       StartEsCnt;       // 启动保护判断ES的计时
		int16       StartDelay;       // 启动保护判断ES的延迟
		int16       StartFocmode;     // 启动保护判断FOCMODE状态的计时
		int16       StartSpeedCnt;    // 启动保护判断速度和ES的计时
		int16  			StartRecoverDelayCnt;   //启动保护恢复延时计数

}FaultVarible;

typedef enum
{
	  FaultNoSource				= 0,        // 无故障
		FaultHardOVCurrent  = 1,	      // 硬件过流
    FaultSoftOVCurrent	= 2,        // 软件过流
    FaultUnderVoltage	  = 3,	      // 欠压保护
    FaultOverVoltage    = 4,        // 过压保护
		FaultLossPhase   		= 5,        // 缺相保护
	  FaultStall 			    = 6,        // 堵转保护
	  FaultStart       	  = 7,        // 启动保护
		FaultOverwind 	    = 8,	      // 顺逆风失败保护
	  FaultOverPower   		= 9,        //功率保护
} FaultStateType;

typedef struct
{
		uint16      ADCDcbus;           // 母线电压
		uint16      ADCSpeed;           // 模拟速度
		uint16			ADCVref;            // ADC参考
} ADCSample;
typedef struct
{
    uint16      ON_Count;           // 运行时间计数
    uint16      OFF_Count;          // 停止时间计数
    uint32      ONOFF_Times;        // 启停次数
    uint8       ONOFF_Flag;         // 启停测试中启动标志位
} ONVarible;
typedef struct
{
		uint16 LedCount;                // 闪灯周期计数
		uint8  LedTimCot;               // 闪灯次数计数
		uint16 Counttime;               // 闪灯周期
		uint16 Count;                   // 电平翻转计数
}MCLedDisplay;
typedef struct
{
	  uint16 mcDcbusFlt;              // 母线电压
		uint16 CtrlMode;                // 控制模式
	  uint16 CurrentPower;            // 当前功率
	  int16  Powerlpf;                // 功率滤波后的值
		int16  mcIqref;                 // Q轴给定电流
	  int16  McuRef;
		uint16 mcPosCheckAngle;         // 位置检测的角度
	  int16  SpeedFlt;                // 当前速度滤波后的值
    int32  SQUSysSpeed;              // 速度的平方和
    int16  SpeedFlag;
  	uint16 EsValue;                 // 当前FOC_ESQU滤波后的值
    uint8  ESDIVFlag;               // 除法执行标志位
    uint16 SQUSpeedDIVEs;           // 速度平方和除以ESQU的值
	  uint16 Smo_EK2;                 // EK2的调试值
	  uint8  Uqdelaycnt;
		int16	 UqFlt;										// UQ滤波后的值
		int16	 UdFlt;										// UD滤波后的值
    uint16 RunStateCnt;             // 运行状态计数
		uint16 SpeedRampTime;						// 速度环增量时间
  	uint16 SpeedLoopTime;           // 速度环时间
		uint16 PowerLoopTime;           // 功率环时间
	  uint16 TorqueLoopTime;          // 转矩环时间
		uint16 RunCurrent;              // 运行电流
    uint16 ChargeStep;              // 预充电的步骤
    uint16 State_Count;	            // 电机各个状态的时间计数
		uint16 TPCtrlDealy;              // 发热丝导通计数

}FOCCTRL;


typedef struct
{
	  uint32 SleepDelayCout;
	  int32  SleepFlag;
    uint8  SleepEn;
}SLEEPMODE;

typedef struct
{
		uint16 Read;
		uint16 Sum;
		uint16 RealValue;
}VspInput;

typedef struct
{
		int16  TargetValue;
		int16  ActualValue;
		int16  IncValue;
		int16  DecValue;
	  int16  DelayCount;
	  int16  DelayPeriod;
		int8   FlagONOFF;
		int16	 Output;
}MCRAMP;
typedef union 
 {
   uint32 PWMCompareAMP;// 比较值标幺化的值
   uint16 PWMCompareUpdate[2];
 }PWMD;
typedef struct
{
		uint16 PWMCompare;   // 比较值
		uint16 PWMARR;       // 周期值
		uint16 PWMCompareOld;// 上一次的比较值  
    PWMD   pwm;          // 使用共同体，将比较值乘以32768
	  uint16 PWMARROld;    // 上一次的周期值
	  uint16 PWMFlag;      // PWM标志位，置1期间不更新duty
    uint16 PWMARRUpdate; // 计算中的周期值
	  uint16 PWMDuty;      // PWM占空比，Q15格式
	  int16 PWMUpdateFlag; // PWM新的duty更新
  	uint32 PWMVSum;      // PWM的总和
	  uint16 PWMcnt;       // PWM求平均技术
    uint8  PWMDivFlag;   // PWM的除法标志位

}PWMINPUTCAL;

typedef struct
{
	  uint8 FR;          //当前给定的转子方向
	  uint8 FlagFR;      //反向的标志位
	  uint8 FRStatus;
	  uint8	TargetFR;    //设置的目标方向
	  uint16 OldTargetSpeed;
} MotorFRTypeDef;

typedef struct
{
		uint32 TEST1;
		uint16 TEST2;
		uint16 TEST3;
		int16 TEST4;
		int16 TEST5;
		int16 TEST6;
		int32 TEST7;
		uint16 TEST8;
		uint16 TEST9;
}testInput;

typedef struct
{
		int16	ExtRef;																					// refer value
		int16	ExtFed;																					// feedback value
	  int16	ExtKP;          																// KP
		int16	ExtKI;         																	// KI
	  int16	ExtErr;      																		// PI Err value
	  int16	ExtOut;    																			// output value
	  int16	ExtOutL;    																		// Last output value
		int16	ExtOutMax;    																	// output max value
		int16	ExtOutMin;    																	// output min value
} OUTLOOP;

/* Exported variables ---------------------------------------------------------------------------*/
extern PWMINPUTCAL      idata     mcPwmInput;
extern FaultVarible     idata 		mcFaultDect;
extern CurrentVarible   idata     mcCurVarible;
extern ProtectVarible   idata     mcProtectTime;
extern ONVarible        xdata     ONOFFTest;
extern FaultStateType		xdata	  	mcFaultSource;
extern ADCSample        xdata    	AdcSampleValue;
extern FOCCTRL          xdata 		mcFocCtrl;
extern MCLedDisplay     xdata     mcLedDisplay;
extern MCRAMP           xdata     mcSpeedRamp;
extern int16						xdata     mcSpeedFdb;
extern int16 						xdata     VSP;
extern MotorFRTypeDef		xdata     mcFRState;
extern SLEEPMODE        xdata     SleepSet;
extern OUTLOOP					xdata   	SpeedPICtrl;

/* Exported functions ---------------------------------------------------------------------------*/
extern void Fault_OverUnderVoltage(FaultVarible *h_Fault);
extern void Fault_Power(FaultVarible *h_Fault);
extern  void Fault_Overcurrent(CurrentVarible *h_Cur);
extern void Fault_OverCurrentRecover(FaultVarible *h_Fault);
extern void Fault_OverPowerRecover(FaultVarible *h_Fault);
extern void Fault_Stall(FaultVarible *h_Fault,CurrentVarible *h_Cur);
extern void Fault_phaseloss(FaultVarible *h_Fault);
extern void VariablesPreInit(void);
extern void Fault_Detection(void);
extern int16 KLPF_VALUE(int16 INVlaue, int16 OutLastValue);
extern void Led_OnOff(MCLedDisplay *hLedDisplay,uint8 htime);
extern void Speed_response(void);
extern void mc_ramp(MCRAMP *hSpeedramp);
extern void FGOutput(void);
extern void VSPSample(void);
//extern void HW_Speed_PI(void);
//extern int16 HW_Genal_PI(int16 Xn1, int16 Yn0, int16 Xn2);

extern  int16 SoftLPF(int16 Xn1, int16 Xn0, int16 K);
extern void Sleepmode(void);
extern void LED_Display(void);
extern void FaultProcess(void);
extern void SpeedRegulation(void);
extern int16 LPFFunction(int16 Xn1, int16 Xn0, int8 K);
extern uint32 Abs_F32(int32 value);
extern uint16 Abs_F16(int16 value);
extern void StarRampDealwith(void);
extern void ONOFF_Starttest(ONVarible  *h_test);
extern void MULDIV_test(void);
extern void APP_DIV(void);
extern int16 HW_One_PI(int16 Xn1);
extern void HW_PI_Control(OUTLOOP *PIPara);
extern void HW_TWO_PI(OUTLOOP *PIPara);



extern void TailWindSpeedDetect(void);

extern void PWMInputCapture(void);
extern void RDOutput(void);
extern void OutLoopParameterSet(void);

#endif