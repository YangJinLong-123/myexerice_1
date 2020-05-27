
#ifndef __FU68XX_2_SYS_H_
#define __FU68XX_2_SYS_H_

/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : FU68xx_2_System.c
* Author             : Fortiortech  Appliction Team
* Version            : V1.0
* Date               : 10-Apr-2017
* Description        : This file contains the system function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/
/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>


void VREFConfig(uint8 Value, bool Status);
void VHALFConfig(bool Status);
void WatchDogConfig(uint16 Value, bool Status);
void WatchDogRefresh(void);
uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress);
uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData);

#endif

