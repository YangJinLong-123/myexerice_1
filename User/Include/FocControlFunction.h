/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
  ******************************************************************************
  * @file    FocControl.h
  * @author  Fortior Application Team
  * @version V1.0.0
  * @date    10-Apr-2017
  * @brief   define motor contorl parameter
  ******************************************************************************
  */

#ifndef __FocControlFunction_H_
#define __FocControlFunction_H_
/* Define to prevent recursive inclusion -------------------------------------*/
typedef struct
{
  int16   IuOffset;       //Iu的偏置电压
  int32   IuOffsetSum;    //Iu的偏置电压总和
  int16   IvOffset;       //Iv的偏置电压
  int32   IvOffsetSum;    //Iv的偏置电压总和
  int16   Iw_busOffset;   //Iw或Ibus的偏置电压
  int32   Iw_busOffsetSum;//Iw或Ibus的偏置电压总和
  int16   OffsetCount;    //偏置电压采集计数
  int8    OffsetFlag;      //偏置电压结束标志位
}CurrentOffset;

extern CurrentOffset xdata mcCurOffset;
extern FaultVarible  idata 		mcFaultDect;
extern void GetCurrentOffset(void);
extern void Motor_Ready (void);
extern void Motor_Init (void);

extern void FOC_Init(void);
extern void Motor_Charge(void);
extern void MC_Stop(void);
extern void Motor_TailWind(void);
extern void TailWindDealwith(void);
extern void Motor_Open(void);
extern void Motor_Align(void);
extern void McTailWindDealwith(void);
extern void MotorcontrolInit(void);
extern void BEMFTailWindDealwith(void);
extern void FOCCloseLoopStart(void);
extern void Motor_PllStart(void);
#endif