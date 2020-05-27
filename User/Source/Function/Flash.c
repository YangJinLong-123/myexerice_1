
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

/* Includes ------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>
#include <Customer_Debug.h>

#define STARTPAGEROMADDRESS 0X3F00 

ROM_TypeDef xdata  Rom;

uint8 Flash_GetAddress(void);
void Flash_ErasePageRom(void);
void Flash_KeyWriteValue( uint8 value);

/*-------------------------------------------------------------------------------------------------
	Function Name :	void Flash_GetAddress(void)
	Description   :	读取Flash当前地址中的值
	Input         :	null
  Output				: null
-------------------------------------------------------------------------------------------------*/
uint8 Flash_GetAddress(void)
{
  uint8 i;
  __IO uint8 tevalue = 0;        //临时变量
  __IO uint8 revalue = 0;        //返回值

  revalue = 0;
  Rom.PageAddress = STARTPAGEROMADDRESS;
  
  tevalue = *(uint8 code *)Rom.PageAddress;
  
  while(tevalue != 0x7F)
  {
    Rom.PageAddress = Rom.PageAddress + 0x80; 
    tevalue = *(uint8 code *)Rom.PageAddress;
    
    if(Rom.PageAddress > 0x3F00)
    {
      Rom.PageAddress = STARTPAGEROMADDRESS;        
      revalue = 1;
      Rom.OffsetAddressCur = 0;
      Rom.OffsetAddressTar = Rom.OffsetAddressCur + 1;
      Flash_Sector_Write(Rom.PageAddress,0X7F); 
      tevalue = 0x7F;
    }      
  }

  for(i = 0; i < 128;i++)
  {
    tevalue = *(uint8 code *)(Rom.PageAddress + i);
    if(tevalue > 0)
    {
      Rom.OffsetAddressCur = i;
      Rom.OffsetAddressTar = Rom.OffsetAddressCur + 1;
      revalue = tevalue;
    }
  }

  return revalue;
}


void Flash_ErasePageRom(void)
{
  if(Rom.OffsetAddressCur >= 127)
  { 
    Rom.OffsetAddressCur = 0;
    Rom.OffsetAddressTar = Rom.OffsetAddressCur + 1;

    Flash_Sector_Erase(Rom.PageAddress);                   //擦除本页

    if(Rom.PageAddress < 0x3F00)    
    {
      Rom.PageAddress = Rom.PageAddress + 0x80;
    }
    else 
    {
      Rom.PageAddress = STARTPAGEROMADDRESS;
    }
    
    Flash_Sector_Write(Rom.PageAddress + Rom.OffsetAddressCur, 0x7F);    //首地址写入
  }
}

void Flash_KeyWriteValue( uint8 value)
{
  uint8 key;
  uint8 i;
  uint8 TempReadRomValue;

  key = value; 

  TempReadRomValue = Rom.ReadValue;                       
  
  for(i = 0;((Rom.WriteValue != TempReadRomValue)&&(i < 5));i++)
  {
    Flash_Sector_Write(Rom.PageAddress + Rom.OffsetAddressTar, key);
    TempReadRomValue = *(uint8 code *)(Rom.PageAddress + Rom.OffsetAddressTar);
  }
  
  if(Rom.WriteValue == TempReadRomValue)
  {
    Rom.ReadValue = TempReadRomValue;
  }                                                                    //读出有效数据
}