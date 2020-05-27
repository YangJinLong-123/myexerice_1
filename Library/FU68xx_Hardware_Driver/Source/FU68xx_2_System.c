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


/*-------------------------------------------------------------------------------------------------
    Function Name : VREFConfig(Value, Status)
    Description   : 参考电压VREF配置，如果使能VREF模块，则内部产生VREF输入到ADC作为ADC
                                    参考电压，如果禁止VREF模块，则由外部提供VREF输入到ADC作为ADC参考电压。
    Input         : Value--VCC低电压预警值
                  0: VREF4_5, 参考电压VREF输出为4.5V
                  1: VREF5_0, 参考电压VREF输出为5.0V
                  2: VREF3_0, 参考电压VREF输出为3.0V
                  3: VREF4_0, 参考电压VREF输出为4.0V
                  Statue--使能控制，Disable或Enable。
  Output                :   None
-------------------------------------------------------------------------------------------------*/
void VREFConfig(uint8 Value, bool Status)
{
    switch(Value)
    {
        case 0: ClrBit(VREF_VHALF_CR, VRVSEL0 | VRVSEL1);          break;
        case 1: SetReg(VREF_VHALF_CR, VRVSEL0 | VRVSEL1, VRVSEL0); break;
        case 2: SetReg(VREF_VHALF_CR, VRVSEL0 | VRVSEL1, VRVSEL1); break;
        case 3: SetBit(VREF_VHALF_CR, VRVSEL0 | VRVSEL1);          break;
    }

    SetReg(VREF_VHALF_CR, VREFEN, (Status ? VREFEN : 0x00));
}


/*-------------------------------------------------------------------------------------------------
    Function Name : VHALFConfig(Mode, Status)
    Description   : 基准电压VHALF配置，使能VHALF工作模块，选择VREF/2作为基准
                                    电压VHALF输出，VHALF可用作运放基准电压。
    Input         : Statue--使能控制，0-Disable或1-Enable。
  Output                :   None
-------------------------------------------------------------------------------------------------*/
void VHALFConfig(bool Status)
{
    SetReg(VREF_VHALF_CR, VHALFEN, (Status ? VHALFEN : 0x00));
}

/* WatchDog Config-------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------
    Function Name ：WatchDogConfig(Value ,Status)
    Description   ：看门狗定时函数初始化，看门狗使能和复位使能，定时配置
    Input         ：Value--定时时间，单位ms，最小定时时间8ms，最大定时1800ms
                  Statue--使能控制，Disable或Enable
  Output                ：None
-------------------------------------------------------------------------------------------------*/
void WatchDogConfig(uint16 Value, bool Status)
{
    SetReg(CCFG1, WDTEN, (Status ? WDTEN : 0x00));
    WDT_REL = ((uint16)(65532-(uint32)Value*32768/1000) >> 8);
    ClrBit(WDT_CR, WDTF);
    SetBit(WDT_CR, WDTRF);
}


/*-------------------------------------------------------------------------------------------------
    Function Name ：WatchDogRefresh(Value ,Status)
    Description   ：刷新看门狗计数器
    Input         ：None
  Output                ：None
-------------------------------------------------------------------------------------------------*/
void WatchDogRefresh(void)
{
    SetBit(WDT_CR, WDTRF);
}

/* Flash Program Config--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------
    Function Name : uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress)
    Description   : 扇区自擦除: 指定将要擦除的Flash扇区，每个扇区128Byte，共128个扇区，
                                    扇区0~127对应Flash地址0x0000~0x3fff，通过指定Flash地址来指定要擦除
                                    的Flash地址所在扇区。一次只能擦除一个扇区，自擦除数据为任意值，一定
                                    要在解锁后才给DPTR赋值。
    Input         : FlashAddress--Flash自擦除扇区内任意地址
  Output                :   0--Flash自擦除成功，1--Flash自擦除失败
-------------------------------------------------------------------------------------------------*/
uint8 Flash_Sector_Erase(uint8 xdata *FlashAddress)
{
    bool TempEA;

    TempEA = EA;
    EA = 0;

    if(FlashAddress < 0x3f80)       // 不擦除最后一个扇区
    {
        FLA_CR = 0x03;                                   //使能自擦除
        FLA_KEY = 0x5a;
        FLA_KEY = 0x1f;                                   //flash预编程解锁
        _nop_();
        *FlashAddress = 0xff;                   //写任意数据
        FLA_CR = 0x08;                                   //开始预编程，完成后Flash再次上锁
    }

    EA = TempEA;

    if(ReadBit(FLA_CR, FLAERR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*-------------------------------------------------------------------------------------------------
    Function Name : uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData)
    Description   : Flash自烧写: 对扇区预编程和自擦除后，可以对扇区内的地址进行Flash烧写，
                  一次烧写一个byte,一定要在解锁后才给DPTR赋值
    Input         : FlashAddress--Flash烧写地址
                                    FlashData--Flash烧写数据
  Output                :   0--Flash自烧写成功，1--Flash自烧写失败
-------------------------------------------------------------------------------------------------*/
uint8 Flash_Sector_Write(uint8 xdata *FlashAddress, uint8 FlashData)
{
    bool TempEA;

    TempEA = EA;
    EA = 0;

  if(FlashAddress < 0x3f80)         // 不编程最后一个扇区
    {
        FLA_CR = 0x01;              // 使能Flash编程
        FLA_KEY = 0x5a;
        FLA_KEY = 0x1f;             // flash预编程解锁
        _nop_();
        *FlashAddress = FlashData;  // 写编程数据
        FLA_CR = 0x08;              // 开始预编程，完成后Flash再次上锁
    }

    EA = TempEA;

    if(ReadBit(FLA_CR, FLAERR))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
