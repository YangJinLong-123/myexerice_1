/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : EXTIInit.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __EXTIINIT_H_
#define __EXTIINIT_H_


#if (HallMode == HallIC)
	#define	HA	P14
	#define	HB	P16
	#define	HC	P21

#elif (HallMode == HallSensor)
	#define	HA	ReadBit(CMP_SR, CMP0_OUT)
	#define	HB	ReadBit(CMP_SR, CMP1_OUT)
	#define	HC	ReadBit(CMP_SR, CMP2_OUT)

#endif

typedef struct
{  
		uint16 ScrOnAngleTimeDatum;  //�������� ��ͨ�ӳ�ʱ��  ��λΪ  1/�ز�Ƶ��
	  uint16 ScrOnAngleTime;       //ʵ�ʸ��� ��ͨ�ӳ�ʱ��  ��λΪ  1/�ز�Ƶ��
	
	  uint16 Temperature;                //�¶�ֵ
	  uint16 TemperatureDatum;           //�¶�����ֵ
	  uint16 TemperatureDatumRamp0;      //�¶�����ֵ��������
	  uint16 TemperatureDatumRamp1;      //�¶�����ֵ�������
	
	  uint16 TemperatureLast;            //��һ�ε��¶�ֵ
	  int16  TemperatureEkL;        //ǰһ��  �¶����
	  int16  TPCtrlPIOutputL;       //ǰһ��  PI�������ֵ
	  int16  TPCtrlPIOutput;        //��  ǰ  PI�������ֵ
	  int16  EdgeLevelTime;         //�ⲿ�жϵ��ص�ƽʱ��
	
}USER_TYPEDEF;

/* Exported variables ---------------------------------------------------------------------------*/
extern USER_TYPEDEF  User;

/* Exported functions ---------------------------------------------------------------------------*/
extern void EXTI_Init(void);
extern void Sleepmode_Init(void);
extern void ZeroCrossing_Init(void);

#endif


