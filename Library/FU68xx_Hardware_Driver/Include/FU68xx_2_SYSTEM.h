/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : FU6812_19_SYSTEM.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __FU6812_19_SYSTEM_H_
#define __FU6812_19_SYSTEM_H_

/* Includes -------------------------------------------------------------------------------------*/
// #include <absacc.h>

/* System Clock Config---------------------------------------------------------------------------*/

#define InternalFastClock    0
#define ExternalFastClock    1

/*-------------------------------------------------------------------------------------------------
	Function Name :	InternalFastClockConfig(Status)
	Description   :	�ڲ���ʱ�����ã�ǿ��ʹ���ڲ���ʱ�Ӿ����·���л�ϵͳʱ��Ϊ�ڲ���ʱ��
	Input         :	Statue--ʹ�ܿ��ƣ�Disable��Enable
  Output				:	None
-------------------------------------------------------------------------------------------------*/
extern void InternalFastClockConfig(bit Status);


/*-------------------------------------------------------------------------------------------------
	Function Name :	SystemFastClockConfig(Mode)
	Description   :	ϵͳ��ʱ�������л����ⲿ��ʱ��Ĭ��Ϊ��Դ����ģʽ��
	Input         :	Mode--ϵͳ��ʱ��ģʽ��InternalFastClock��ExternalFastClock
  Output				:	None
-------------------------------------------------------------------------------------------------*/
extern void SystemFastClockConfig(bit Mode);


/*-------------------------------------------------------------------------------------------------
	Function Name :	ExternalSlowClockConfig(Status)
	Description   :	�ⲿ��ʱ�����ã�ʹ���ⲿ��ʱ�Ӿ����·
	Input         :	Statue--ʹ�ܿ��ƣ�Disable��Enable
  Output				:	None
-------------------------------------------------------------------------------------------------*/
extern void ExternalSlowClockConfig(bit Status);


/* System Power Config---------------------------------------------------------------------------*/
#define VREF4_5			0
#define VREF5_0			1
#define VREF3_0     2
#define VREF4_0			3


/*-------------------------------------------------------------------------------------------------
	Function Name :	LVDPowerDownDetectConfig(Status)
	Description   :	������ʹ�ܣ���⵽VCC��ѹ�����趨ֵ�󴥷�Ԥ������VDD5��ѹ�����趨
	                ֵ�󴥷���λ��
	Input         :	Statue--ʹ�ܿ��ƣ�1--Disable��0--Enable
  Output				:	None
-------------------------------------------------------------------------------------------------*/
extern void LVDPowerDownDetectConfig(bit Status);


/*-------------------------------------------------------------------------------------------------
	Function Name :	LVDPowerDownWarnIEConfig(Status)
	Description   :	�����ѹԤ���ж�ʹ�ܣ���⵽VCC��ѹ�����趨ֵ�󴥷�Ԥ���������ѹԤ
                  ���ж�����Ӧ�жϡ�
	Input         :	Statue--ʹ�ܿ��ƣ�Disable��Enable
  Output				:	None
-------------------------------------------------------------------------------------------------*/
extern void LVDPowerDownWarnIEConfig(bit Status);


/*-------------------------------------------------------------------------------------------------
	Function Name :	VREFConfig(Value, Status)
	Description   :	�ο���ѹVREF���ã����ʹ��VREFģ�飬���ڲ�����VREF���뵽ADC��ΪADC
									�ο���ѹ�������ֹVREFģ�飬�����ⲿ�ṩVREF���뵽ADC��ΪADC�ο���ѹ��
	Input         :	Value--VCC�͵�ѹԤ��ֵ
                  0: VREF3_0, �ο���ѹVREF���Ϊ3.0V
									1: VREF4_0, �ο���ѹVREF���Ϊ4.0V
									2: VREF4_5, �ο���ѹVREF���Ϊ4.5V
									3: VREF5_0, �ο���ѹVREF���Ϊ5.0V
                  Statue--ʹ�ܿ��ƣ�Disable��Enable��
  Output				:	None
-------------------------------------------------------------------------------------------------*/
extern void VREFConfig(uint8 Value, bit Status);


/*-------------------------------------------------------------------------------------------------
	Function Name :	VHALFConfig(Mode, Status)
	Description   :	��׼��ѹVHALF���ã�ʹ��VHALF����ģ�飬ѡ��VREF/2��Ϊ��׼
									��ѹVHALF�����VHALF�������˷Ż�׼��ѹ��
	Input         : Statue--ʹ�ܿ��ƣ�Disable��Enable��
  Output				:	None
-------------------------------------------------------------------------------------------------*/
extern void VHALFConfig( bit Status);


/* WatchDog Config-------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------
	Function Name ��void WatchDogRefresh(void
	Description   �����Ź���ʱ������ʼ�������Ź�ʹ�ܺ͸�λʹ�ܣ���ʱ����
	Input         ��Value--��ʱʱ�䣬��λms����С��ʱʱ��8ms�����ʱ1800ms
                  Statue--ʹ�ܿ��ƣ�Disable��Enable
  Output				��None
-------------------------------------------------------------------------------------------------*/
extern void WatchDogConfig(uint16 Value, bit Status);

/*-------------------------------------------------------------------------------------------------
	Function Name ��WatchDogRefresh(Value ,Status)
	Description   ��ˢ�¿��Ź�������
	Input         ��None
  Output				��None
-------------------------------------------------------------------------------------------------*/
extern void WatchDogRefresh(void);



/* Flash Program Config--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------
	Function Name :	uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress)
	Description   :	�����Բ���: ָ����Ҫ������Flash������ÿ������128Byte����128��������
									����0~127��ӦFlash��ַ0x0000~0x3fff��ͨ��ָ��Flash��ַ��ָ��Ҫ����
									��Flash��ַ����������һ��ֻ�ܲ���һ���������Բ�������Ϊ����ֵ��һ��
									Ҫ�ڽ�����Ÿ�DPTR��ֵ��
	Input         :	FlashAddress--Flash�Բ��������������ַ
  Output				:	0--Flash�Բ����ɹ���1--Flash�Բ���ʧ��
-------------------------------------------------------------------------------------------------*/
extern uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress);

/*-------------------------------------------------------------------------------------------------
	Function Name :	uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData)
	Description   :	Flash����д: ������Ԥ��̺��Բ����󣬿��Զ������ڵĵ�ַ����Flash��д��
                  һ����дһ��byte,һ��Ҫ�ڽ�����Ÿ�DPTR��ֵ
	Input         :	FlashAddress--Flash��д��ַ
									FlashData--Flash��д����
  Output				:	0--Flash����д�ɹ���1--Flash����дʧ��
-------------------------------------------------------------------------------------------------*/
extern uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData);


#endif
