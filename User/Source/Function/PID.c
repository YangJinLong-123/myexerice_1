/**************************** (C) COPYRIGHT 2015 Fortiortech shenzhen *****************************
* File Name          : PID.c
* Author             : Billy Long Fortiortech  Market Dept
* Version            : V1.0
* Date               : 01/07/2015
* Description        : This file contains PI control function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx.h>
#include <Myproject.h>

/* Private variables ----------------------------------------------------------------------------*/
bit FlagStartPI = 0;

Ramp_TypeDef idata SpeedRamp;
PID_TypeDef idata SpeedPID;
PID_TypeDef idata CurrentPID;


/*-------------------------------------------------------------------------------------------------
	Function Name :	void SpeedPIDInit(void)
	Description   :	速度PID参数初始化
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void SpeedPIDInit(void)
{
	SpeedPID.Kp = TempSpeedKP;
	SpeedPID.Ki = TempSpeedKI;
	SpeedPID.Err = 0;
	SpeedPID.Err_Last1 = 0;
	SpeedPID.Err_Err = 0;
	#if (SpeedCloseLoopEnable)
	{
		SpeedPID.Out = TempMotorStartDuty;
		SpeedPID.OutMax = MotorVSMax;
		SpeedPID.OutMin = TempMotorStartDuty;
	}
	#elif ((SpeedCloseLoopEnable)&&(CurrentCloseLoopEnable))
	{
		SpeedPID.Out = 0;
		SpeedPID.OutMax = TempCurrentMax;
		SpeedPID.OutMin = TempCurrentMin;
	}
	#endif
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	void SpeedRampInit(void)
	Description   :	速度爬坡参数初始化
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void SpeedRampInit(void)
{
	SpeedRamp.RampInc = SpeedRampInc;
	SpeedRamp.RampDec = SpeedRampDec;
	SpeedRamp.RampStep = MC.SpeedCtl.PISpeed;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	void CurrentPIDInit(void)
	Description   :	电流PI参数初始化
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void CurrentPIDInit(void)
{
	CurrentPID.Kp = TempCurrentKP;
	CurrentPID.Ki = TempCurrentKI;
	CurrentPID.Err = 0;
	CurrentPID.Err_Last1 = 0;
	CurrentPID.Err_Err = 0;
	CurrentPID.Out = TempMotorStartDuty;
	CurrentPID.OutMax = MotorVSMax;
	CurrentPID.OutMin = TempMotorStartDuty;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	int16 RampControl(Ramp_TypeDef *Ramp, int16 Ref)
	Description   :	爬坡控制函数，根据设定爬坡增量把当前值向目标值靠拢
	Input         :	Ramp--爬坡对象数组
                  Ref--爬坡参考目标
  Output				:	RampStep--爬坡当前输出
-------------------------------------------------------------------------------------------------*/
int16 RampControl(Ramp_TypeDef *Ramp, int16 Ref)
{
	Ramp->RampTarget = Ref;
	if(Ramp->RampTarget > Ramp->RampStep)
	{
		if((Ramp->RampStep + Ramp->RampInc) <= Ramp->RampTarget)
		{
			Ramp->RampStep += Ramp->RampInc;
		}
		else
		{
			Ramp->RampStep = Ramp->RampTarget;
		}
	}
	else if(Ramp->RampTarget < Ramp->RampStep)
	{
		if((Ramp->RampStep - Ramp->RampDec) >= Ramp->RampTarget)
		{
			Ramp->RampStep -= Ramp->RampDec;
		}
		else
		{
			Ramp->RampStep = Ramp->RampTarget;
		}
	}

	return 	Ramp->RampStep;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	int16 PIDControl(PID_TypeDef *PID, int16 Ref, int16 Cur)
	Description   :	PI调节函数
	Input         :	PID--PI调节对象数组
                  Ref--PI调节参考目标
                  Cur--PI当前输入
  Output				:	Out--PI调节输出
-------------------------------------------------------------------------------------------------*/
int16 PIDControl(PID_TypeDef *PID, int16 Ref, int16 Cur)
{
	int32 Kp_Out, Ki_Out, PID_Out;

	/*PI运算时间67us*/
	if(!PID->Err)
	{
		PID->Err_Last1 = Ref - Cur;														 // 初始化PID上次偏差
		PID->Err = Ref - Cur;																	 // 初始化PID当前偏差
		PID->Err_Err=PID->Err - PID->Err_Last1;								 // 初始化PID上次偏差和上上次偏差之差
	}
	else
	{
		PID->Err_Last1 = PID->Err;														 // 保存PID上次偏差
		PID->Err = Ref - Cur;																	 // 计算PID当前偏差
		PID->Err_Err=PID->Err - PID->Err_Last1;								 // 计算PID上次偏差和上上次偏差之差
	}

	Kp_Out = ((int32)PID->Kp * (int32)PID->Err_Err) >> 12;
	Ki_Out = ((int32)PID->Ki * (int32)PID->Err) >> 12;

	PID_Out = PID->Out;
	PID_Out += Kp_Out + Ki_Out;

	if(PID_Out > PID->OutMax)
	{
		PID_Out = PID->OutMax;												         // PID最高输出
	}
	if(PID_Out < PID->OutMin)
	{
		PID_Out = PID->OutMin;												         // PID最低输出
	}
	PID->Out = PID_Out;

	return PID->Out;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	int16 PIDControl(PID_TypeDef *PID, int16 Ref, int16 Cur)
	Description   :	PI调节函数
	Input         :	PID--PI调节对象数组
                  Ref--PI调节参考目标
                  Cur--PI当前输入
  Output				:	Out--PI调节输出
-------------------------------------------------------------------------------------------------*/
int16 PID_Control(PID_TypeDef *PID, int16 Ref, int16 Cur)
{
	int16 Kp_Out;
	int16 Ki_Out;
	int32 PID_Out;

	/*PI运算时间42us*/
	if(!PID->Err)
	{
		PID->Err_Last1 = Ref - Cur;														 // 初始化PID上次偏差
		PID->Err = Ref - Cur;																	 // 初始化PID当前偏差
		PID->Err_Err=PID->Err - PID->Err_Last1;								 // 初始化PID上次偏差和上上次偏差之差
	}
	else
	{
		PID->Err_Last1 = PID->Err;														 // 保存PID上次偏差
		PID->Err = Ref - Cur;																	 // 计算PID当前偏差
		PID->Err_Err=PID->Err - PID->Err_Last1;								 // 计算PID上次偏差和上上次偏差之差
	}

	Kp_Out = MDU_MULA_S16(PID->Kp, PID->Err_Err, 12);
	Ki_Out = MDU_MULA_S16(PID->Ki, PID->Err, 12);
	PID_Out = PID->Out;
	PID_Out += Kp_Out + Ki_Out;

	if(PID_Out > PID->OutMax)
	{
		PID_Out = PID->OutMax;												         // PID最高输出
	}
	if(PID_Out < PID->OutMin)
	{
		PID_Out = PID->OutMin;												         // PID最低输出
	}
	PID->Out = PID_Out;

	return PID->Out;
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	void PI_Init(void)
	Description   :	PI模块初始化
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void PI_Init(void)
{
/*-------------------------------------------------------------------------------------------------
	PI数据格式
	Q = PIRANGE + 8(Q8~Q23数据格式)
  给寄存器赋值时要统一数据格式
-------------------------------------------------------------------------------------------------*/
	SetBit(SV_CR, PIRANGE3, 0);															// 0100: 12bit PI
	SetBit(SV_CR, PIRANGE2, 1);
	SetBit(SV_CR, PIRANGE1, 0);
	SetBit(SV_CR, PIRANGE0, 0);				                       // PI截位Q=PIRANGE+8，即PI数据格式

  PI_KP = 0;												                       // 寄存器数据类型：int16 Q12格式，PI_KP=Kp*2^12，
	PI_KI = 0;												                       // 寄存器数据类型：int16 Q12格式，PI_KI=Kp*2^12
	PI_EK = 0;
  PI_UK = 0;												                       // 寄存器数据类型：int16
	PI_UKMAX = 0;											                       // 寄存器数据类型：int16
	PI_UKMIN = 0;											                       // 寄存器数据类型：int16
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	int16 PIDControl(PID_TypeDef *PID, int16 Ref, int16 Cur)
	Description   :	PI调节函数
	Input         :	PID--PI调节对象数组
                  Ref--PI调节参考目标
                  Cur--PI当前输入
  Output				:	Out--PI调节输出
-------------------------------------------------------------------------------------------------*/
int16 PI_Control(PID_TypeDef *PID, int16 Ref, int16 Cur)
{
	static uint8 *PIDType = 0;

	/*PI运算时间10us*/
	FlagStartPI = 1;
	while(FlagStartPI)                                       // MDU分时复用标志
	{
		FlagStartPI = 0;

		/*指针类型数据判断执行时间2.5us*/
		if(PIDType != PID)                                     // 通过PID数据地址获取PID类型，
		{
			PIDType = PID;

			PI_KP = PID->Kp;									                   // 寄存器数据类型：int16 Q12格式，PI_KP=Kp*2^12，
			PI_KI = PID->Ki;									                   // 寄存器数据类型：int16 Q12格式，PI_KI=Kp*2^12
			PI_EK = PID->Err;
			PI_UK = PID->Out;									                   // 寄存器数据类型：int16
			PI_UKMAX = PID->OutMax;						                   // 寄存器数据类型：int16
			PI_UKMIN = PID->OutMin;						                   // 寄存器数据类型：int16

			SetBit(SV_CR,PISTA,1);						                   // 启动PI
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();													                   // 等待PI运算完成,PI_LastErr初始化成

		}
		/*赋值运算时间3.2us*/
		PID->Err = Ref - Cur;								                   // 计算PID当前偏差

		/*PI硬核运算时间1.8us*/
		PI_EK = PID->Err;										                   // PI输入
		PI_UK = PID->Out;									                     // 寄存器数据类型：int16
		SetBit(SV_CR,PISTA,1);
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		PID->Out = PI_UK;										                    // PI输出
	}
  FlagStartPI = 1;

  return PID->Out;
}

