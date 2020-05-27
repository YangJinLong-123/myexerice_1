
/**************************** (C) COPYRIGHT 2015 Fortiortech shenzhen *****************************
* File Name          : LPF.c
* Author             : Billy Long Fortiortech  Market Dept
* Version            : V1.0
* Date               : 01/07/2015
* Description        : This file contains LPF control function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx.h>
#include <Myproject.h>


/*-------------------------------------------------------------------------------------------------
	Function Name :	uint16 LPF(uint16 Xn1, uint16 Xn0, uint16 K)
	Description   :	LFP����
	Input         :	Xn1--��ǰ����ֵ
	                Xn0--��һ���˲����ֵ
									K--LPF�˲�ϵ��
  Output				:	Temp16--��ǰ�˲����ֵ
-------------------------------------------------------------------------------------------------*/
// uint16 LPF(uint16 Xn1, uint16 Xn0, uint16 K)
// {
// 	uint16 Temp16 = 0;
// 	uint32_t Temp32 = 0;
//
// 	Temp32 = (((uint32_t)Xn1 - (uint32_t)Xn0) * (uint32_t)K) >> 15;
// 	Temp16 = Xn0 + (uint16)Temp32;
// 	return Temp16;
// }


/*-------------------------------------------------------------------------------------------------
	Function Name :	uint16 LPF(uint16 Xn1, uint16 Xn0, uint16 K)
	Description   :	LFP����
	Input         :	Xn1--��ǰ����ֵ
	                Xn0--��һ���˲����ֵ
									K--LPF�˲�ϵ��
  Output				:	LPF_Y--��ǰ�˲����ֵ
-------------------------------------------------------------------------------------------------*/
uint16 LPF(uint16 Xn1, uint16 Xn0, uint16 K)
{
	LPF_K = K;
	LPF_X = Xn1;
  LPF_Y = Xn0;
	SetBit(SV_CR, LPFSTA, 1);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	return LPF_Y;
}
