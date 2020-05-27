
/************************ (C) COPYRIGHT 2015 FT *******************************
* File Name          : 
* Author             : Application Team  Tom.wang 
* Version            : V2.0.0
* Date               : 06/15/2015
* Description        : 
********************************************************************************
/*******************************************************************************
* All Rights Reserved
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H
#define __FLASH_H
#define     __I     volatile const    /*!< defines 'read only' permissions      */
#define     __O     volatile          /*!< defines 'write only' permissions     */
#define     __IO    volatile          /*!< defines 'read / write' permissions   */

typedef struct
  {
    uint8  *PageAddress;     //ҳ��ַ
    uint8  OffsetAddressCur; //ƫ�Ƶ�ǰ��ַ 
    uint8  OffsetAddressTar; //ƫ��Ŀ��д���ַ
    uint8  WriteValue;       //ROM д��ֵ
    uint8  ReadValue;        //ROM ����ֵ
  }ROM_TypeDef;

extern ROM_TypeDef xdata  Rom;  
extern uint8 Flash_GetAddress(void);
extern void Flash_ErasePageRom(void);
extern void Flash_KeyWriteValue( uint8 value);
#endif