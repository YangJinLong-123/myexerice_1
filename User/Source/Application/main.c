/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : main.c
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 2017-12-27
* Description        : This file contains main function used for Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Includes -------------------------------------------------------------------------------------*/
// #include <main.h>
#include <FU68xx_2.h>
#include <Myproject.h>

/* Private typedef ------------------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------------------*/
void SoftwareInit(void);
void HardwareInit(void);
void DebugSet(void);
void MotorControlInit(void);

extern uint32 Temp_value;

uint8 test_flag = 0;
/*-------------------------------------------------------------------------------------------------
        Function Name : void main(void)
        Description   : 主函数主要功能是初始化，包括上电等待，软件初始化，硬件初始化，调试模式设置，主循环扫描。
                                    软件初始化--初始化所有定义的变量
                                    硬件初始化--初始化硬件设备配置
                                    调试模式设置--调试模式
        Input         : 无
        Output        : 无
-------------------------------------------------------------------------------------------------*/
void main(void)
{
    uint16 PowerUpCnt = 0;
	
		Temp_value = Temperature_Off;
		test_flag = 0;
    for(PowerUpCnt=0;PowerUpCnt<SystemPowerUpTime;PowerUpCnt++){};

		if((SW1==0)&&(SW2==0))
		{
			test_flag = 1;
		}				
    /*Software Init*/
    SoftwareInit();

    /*Hardware Init*/
    HardwareInit();

    /*调试模式设置--内部变量查询；CMP输出查询；ADC触发信号查询*/
    DebugSet();
			
		LEDControl();                 //上电点亮

    while(1)
    {
			GetCurrentOffset();      																								// 电流偏置的获取
			MC_Control();            																								// 主控函数，状态扫描 2.6k

			#if (!StartONOFF_Enable) 																								// 调速方式
					#if (SPEED_MODE == PWMMODE)																					// PWM调速模式
//                PWMInputCapture();																						// PWM duty调速模式
							PWMScan();																											// PWM调频调速模式

					#elif (SPEED_MODE == NONEMODE)																			// 直接上电运行模式
							mcSpeedRamp.FlagONOFF   = 1;
//                mcSpeedRamp.TargetValue = _Q15(60000 / MOTOR_SPEED_BASE);
					#endif
			#endif
    }
}
/*-------------------------------------------------------------------------------------------------
        Function Name : void DebugSet(void)
        Description   : 调试模式配置
        Input         : 无
        Output        : 无
-------------------------------------------------------------------------------------------------*/
void DebugSet(void)
{
    #if defined (SPI_DBG_HW)      																							// 硬件调试模式
        Set_DBG_DMA(&HARD_SPIDATA);
    #elif defined (SPI_DBG_SW)    																							// 软件调试模式
        Set_DBG_DMA(spidebug);
    #endif

    #if defined (SPI_DBG_HW) && defined (SPI_DBG_SW)
        #error Only one DBG mode can be selected
    #endif

    SetReg(CMP_CR3, DBGSEL0 | DBGSEL1,  GP01_DBG_Conf);
    SetReg(CMP_CR3, CMPSEL0 | CMPSEL1 | CMPSEL2, GP07_DBG_Conf);
}

/*-------------------------------------------------------------------------------------------------
        Function Name : void SoftwareInit(void)
        Description   : 软件初始化，初始化所有定义变量，按键初始化扫描
        Input         : 无
        Output        : 无
-------------------------------------------------------------------------------------------------*/
void SoftwareInit(void)
{
    /****初始化所有定义的参数变量****/
    MotorcontrolInit();
	
	 /******按键初始化******/
	  KeyInit();
	
	/******LED初始化******/
	  LEDControlInit();
	
    /****电机初始状态为mcReady，故障保护为无故障******/
    mcState = mcReady;
    mcFaultSource = 0;
}

/*-------------------------------------------------------------------------------------------------
        Function Name : void HardwareInit(void)
        Description   : 硬件初始化，初始化需要使用的硬件设备配置，FOC必须配置的是运放电压、运放初始化、ADC初始化、Driver初始化
                                    TIM4初始化，其他的可根据实际需求加。
        Input         : 无
        Output        : 无
-------------------------------------------------------------------------------------------------*/
void HardwareInit(void)
{

    // 为提高芯片的抗干扰能力，降低芯片功耗，请在具体项目时，将不需要用的GPIO默认都配置为输入上拉。
    // 具体配置可在GPIO_Default_Init设置。
    //    GPIO_Default_Init();

    /******硬件FO过流，比较器初始化，用于硬件过流比较保护******/
    #if (HardwareCurrent_Protect == Hardware_FO_Protect)  											//外部中断初始化，用于外部中断硬件过流FO的保护
        EXTI_Init();
    #elif (HardwareCurrent_Protect == Hardware_CMP_Protect)											//选择比较过流，比较器初始化
        CMP3_Init();
    #elif (HardwareCurrent_Protect == Hardware_FO_CMP_Protect)    							//两者都选择
        EXTI_Init();
        CMP3_Init();
    #endif

    //  Sleepmode_Init();
	 
    /****功能IO初始化***********/
    GPIO_Init();
	  
	 /****过零检测外部中断初始化***********/
	  ZeroCrossing_Init();

    /*****运算放大器初始化*********/
    AMP_Init();

    /*****ADC初始化*********/
    ADC_Init();
    /****比较器中断配置***********/
    CMP3_Inter_Init();                         																	// 建议和比较器初始化间隔一段时间

    /*****Driver初始化*********/
    Driver_Init();
    /*****UART初始化*********/
    // UART_Init();//未配置

    #if defined (SPI_DBG_HW) | defined (SPI_DBG_SW)   													// 调试模式
//			SPI_Init();   																													//调试模式下，占用SPI端口的NSS(GP04),MOSI(GP05),SCK(GP06)
    #endif
    /*****Timer初始化*******/
//    TIM2_Init();																														//端口GP07 & GP10
    TIM3_Init();																																//端口GP11,采样定时器3中断作为PWM捕获中断
    TIM4_Init();//端口GP01
    TIM1ms_Init();																															//采用1ms定时器中断作为常见中断,处理故障保护等附加功能
 }
