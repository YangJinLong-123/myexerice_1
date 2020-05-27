/**************************** (C) COPYRIGHT 2015 Fortiortech shenzhen *****************************
* File Name          : KeyScan.c
* Author             : Billy Long Fortiortech  Market Dept
* Version            : V1.0
* Date               : 01/07/2015
* Description        : This file contains key scan function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/ 


/* Includes -------------------------------------------------------------------------------------*/
#include <FU68xx_2.h>
#include <Myproject.h>

char SW1_Flag=0;
char SW2_Flag=0;
char SW3_Flag=0;

char Flag_Key1Value=0;        //档位变换顺序
char Flag_Key2Value=0;

/* Private variables ----------------------------------------------------------------------------*/
KeyScanParam_TypeDef  KS;
KeyControl_TypeDef  KeyControl;


/*-------------------------------------------------------------------------------------------------
	Function Name :	void KeyInit(void)
	Description   :	按键参数初始化
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void KeyInit(void)
{
  KeyControl.FlagSpeed = 1;
	
	KS.Key1Value = 0x02;
	
	KS.Key2Value = 0x20;
	
	KS.Key3Value = 0x00;
	
	KS.KeyValuetotal     = KS.Key1Value ^ KS.Key2Value ^ KS.Key3Value;
	
	Rom.ReadValue = Flash_GetAddress(); 
	
	if((Rom.ReadValue == 0x11)||(Rom.ReadValue == 0x21) ||(Rom.ReadValue == 0x41) ||(Rom.ReadValue == 0x12) ||(Rom.ReadValue == 0x22)
	||(Rom.ReadValue == 0x42)||(Rom.ReadValue == 0x14)||(Rom.ReadValue== 0x24)||(Rom.ReadValue == 0x44)||(Rom.ReadValue == 0x18)
	||(Rom.ReadValue == 0x28)||(Rom.ReadValue == 0x48))
	{
		KS.KeyValuetotal   = Rom.ReadValue;
		
		KS.Key1Value = Rom.ReadValue & 0x0F;
	
	  KS.Key2Value = Rom.ReadValue & 0x70;;
	
	  KS.Key3Value = 0x00;
	}
	else
	{
		Flash_ErasePageRom();
		Flash_KeyWriteValue((0x41));//0x21
		EA = 1;
	}
	KS.OldKeyValuetotal  = 0;
	KS.ChangeKeyFlg      = 0;
  KS.FlashWriteOnetimes = 0; 
	
	KS.FlagKey0Press = 0;
	KS.Key0PressCnt = 0;	
	
	KS.FlagKey1Press = 0;
	KS.Key1PressCnt = 0;	
	
//	while((KS.Key0PressCnt != KeyFilterTime)&&(KS.Key0PressCnt != -KeyFilterTime))
//	{
//		KeyValue();
//	}	
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	uint8 KeyValue(void)
	Description   :	功能函数，获取按键值，选择返回键位值还是管脚电平KeyValue
	Input         :	无
  Output				:	键位值或者管脚电平KeyValue
-------------------------------------------------------------------------------------------------*/
int KeyValue(void)
{
	/* SW1 Scan */
  if(SW1==0)
	{
		if(SW1_Flag==0)
		{
			KS.Key1PressCnt ++;
			if(KS.Key1PressCnt >= KeyFilterTime)
			{
				KS.Key1PressCnt = KeyFilterTime;
				SW1_Flag=1;
				if((KS.Key1Value < 0x08)&&(Flag_Key1Value==0))
				{
				   KS.Key1Value = KS.Key1Value <<1;
				}
				else if((KS.Key1Value > 0x01)&&(Flag_Key1Value==1))
				{
					 KS.Key1Value = KS.Key1Value >>1;
				}
        else if(KS.Key1Value == 0x08)
        {
					Flag_Key1Value= 1;
					KS.Key1Value = KS.Key1Value >>1;
				}
        else if(KS.Key1Value == 0x01)
				{
          Flag_Key1Value= 0;
					KS.Key1Value = KS.Key1Value <<1;
				}
        else
        {
					Flag_Key1Value= 0;
					KS.Key1Value = 0x01;
				}		
				   
			}
	  }
		else if(SW1_Flag==1)
		{
			 KS.Key1Value = KS.Key1Value ;
		}
		else
			 SW1_Flag=0;
		
	}
  else
	{		
		 KS.Key1PressCnt --;
		 if(KS.Key1PressCnt <= 0)
		 {
			 KS.Key1PressCnt = 0;
			 SW1_Flag=0;
//			 KS.FlagKey0Press = 1;
		 }
	}			
	
	/* SW2 Scan */
  if(SW2==0)
	{
		if(SW2_Flag==0)
		{
			KS.Key2PressCnt ++;
			if(KS.Key2PressCnt >= KeyFilterTime)
			{
				KS.Key2PressCnt = KeyFilterTime;
				SW2_Flag=1;
				
				if((KS.Key2Value < 0x40)&&(Flag_Key2Value==0))
				{
				   KS.Key2Value = KS.Key2Value <<1;
				}
				else if((KS.Key2Value > 0x10)&&(Flag_Key2Value==1))
				{
					 KS.Key2Value = KS.Key2Value >>1;
				}
        else if(KS.Key2Value == 0x40)
        {
					Flag_Key2Value= 1;
					KS.Key2Value = KS.Key2Value >>1;
				}
        else if(KS.Key2Value == 0x10)
				{
          Flag_Key2Value= 0;
					KS.Key2Value = KS.Key2Value <<1;
				}
        else
        {
					Flag_Key2Value= 0;
					KS.Key2Value = 0x20;
				}					
			}
	  }
		else if(SW2_Flag==1)
		{
			 KS.Key2Value = KS.Key2Value ;
		}
		else
			 SW2_Flag=0;
		
	}
  else
	{		
		 KS.Key2PressCnt --;
		 if(KS.Key2PressCnt <= 0)
		 {
			 KS.Key2PressCnt = 0;
			 SW2_Flag=0;
//			 KS.FlagKey0Press = 1;
		 }
	}

	#if 0
	if(SW3==0)
	{
		 KS.Key3releaseCnt=0;
		
		KS.Key3PressCnt=1;
	}
	else
	{
		KS.Key3PressCnt=0;
		
		KS.Key3releaseCnt=1;
	}
	#endif
	
	#if 1
/* SW3 Scan */
  if(SW3==0)
	{
	    KS.Key3releaseCnt=0;
		if(KS.Key3PressCnt<KeyFilterTime+10)
		KS.Key3PressCnt++;
		{
			if(KS.Key3PressCnt >= KeyFilterTime)
			{
				KS.Key3PressCnt = KeyFilterTime;
				SW3_Flag=1;
                
				 KS.Key3Value = 0x08;
				   
			}
	  }
		#if 0
		else if(SW2_Flag==1)
		{
			 KS.Key3Value = KS.Key3Value ;
		}
		#endif
	}	
  else
	{		
		 KS.Key3PressCnt=0;
		if(KS.Key3releaseCnt<100)
		 KS.Key3releaseCnt++;
		 if(KS.Key3releaseCnt >= 80)
		 {
			 SW3_Flag=0;
			 KS.Key3Value = 0x00;			 
		 }
	}	
	#endif
	
	return (KS.Key1Value ^ KS.Key2Value ^ KS.Key3Value);	
}

/*-------------------------------------------------------------------------------------------------
	Function Name :	void KeyScan(void)
	Description   :	功能函数，按键扫描，按键触发传递出按键命令
	Input         :	无
  Output				:	无
-------------------------------------------------------------------------------------------------*/
void KeyScan(void)
{
	KS.OldKeyValuetotal = KS.KeyValuetotal;
	
	KS.KeyValuetotal = KeyValue();

	
	if(KS.OldKeyValuetotal != KS.KeyValuetotal)
	{
		if((mcState == mcRun))
		{
		  KS.ChangeKeyFlg =1;
			KS.FlashWriteOnetimes = 0;			
		}
	}
	
}
