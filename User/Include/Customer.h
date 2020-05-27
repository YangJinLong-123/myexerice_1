/**************************** (C) COPYRIGHT 2017 Fortiortech shenzhen *****************************
* File Name          : Customer.h
* Author             : Fortiortech Appliction Team
* Version            : V1.0
* Date               : 2017-12-21
* Description        : This file contains all the common data types used for
*                      Motor Control.
***************************************************************************************************
* All Rights Reserved
**************************************************************************************************/

/* Define to prevent recursive inclusion --------------------------------------------------------*/
#ifndef __CUSTOMER_H_
#define __CUSTOMER_H_

#define I_ValueX(Curr_Value)            (Curr_Value * HW_RSHUNT * HW_AMPGAIN / (HW_ADC_REF/2))
//#define I_Value(Curr_Value)             _Q15(I_ValueX(Curr_Value))
#define I_Value(Curr_Value)             _Q15((Curr_Value * HW_RSHUNT * HW_AMPGAIN) / (HW_ADC_REF/2))


/*芯片参数值-------------------------------------------------------------------*/
 /*CPU and PWM Parameter*/
 #define MCU_CLOCK                      (24.0)                                  // (MHz) 主频
 #define PWM_FREQUENCY                  (28.0)                                  // (kHz) 载波频率

 /*deadtime Parameter*/
 #define PWM_DEADTIME                   (1.0)                                   // (us) 死区时间

 /*single resistor sample Parameter*/
 #define MIN_WIND_TIME                  (2.2)                                   // (us) 单电阻最小采样窗口，建议值死区时间+0.9us


//#define		Pole_Pairs 			    				 (1.0)					     											// 极对数
//#define LD                             (0.00244 * 1.0)									// (H) D轴电感
//#define LQ                             (0.00244 * 1.0)									// (H) Q轴电感
//#define RS                             (29.15 * 1.0)									// (Ω) 相电阻

#define		Pole_Pairs 			    				 (1.0)					     											// 极对数
#define LD                             (0.00228 * 0.7)									// (H) D轴电感
#define LQ                             (0.00228 * 0.7)									// (H) Q轴电感
#define RS                             (9.99 * 1.0)									// (Ω) 相电阻

#define KeVpp                          (33*1.0)                                  // (V)      反电动势测量的峰峰值
#define KeF                            (158*1.0)                                  // (Hz)     反电动势测量的频率
#define KeT                            (1000 / KeF)                            // (ms)     反电动势测量的周期
#define Ke                             (Pole_Pairs * KeVpp * KeT / 207.84)     // (V/KRPM) 反电动势常数
//#define Ke                             (1.2317)     // (V/KRPM) 反电动势常数

 #define MOTOR_SPEED_BASE               (220000.0)                                // (RPM) 速度基准
 #define S_Value(SpeedValue)          	_Q15((SpeedValue+35)/MOTOR_SPEED_BASE)
/*硬件板子参数设置值------------------------------------------------------------*/
 /*PWM high or low level Mode*/
 /*根据驱动芯片的类型选择，大部分芯片为High_Level*/
 #define High_Level                     (0)                                     // 驱动高电平有效
 #define Low_Level                      (1)                                     // 驱动低电平有效
 #define UP_H_DOWN_L                    (2)                                     // 上桥臂高电平有效，下桥臂低电平有效
 #define UP_L_DOWN_H                    (3)                                     // 上桥臂低电平有效，下桥臂高电平有效
 #define PWM_Level_Mode                 (High_Level)

 /*hardware current sample Parameter*/
 /*电流基准的电路参数*/
 /*weizhen*/
 #define HW_RSHUNT                      (0.1)                                 	// (Ω)  采样电阻
 #define HW_ADC_REF                     (5.0)                                  	// (V)  ADC参考电压
 #define HW_AMPGAIN                     (3.4)                                  	// 运放放大倍数
 /* current set value */

 /*hardware voltage sample Parameter*/
 /*母线电压采样分压电路参数*/
 #define RV1                            (500.0)                                 // (kΩ) 母线电压分压电阻1
 #define RV2                            (500.0)                                 // (kΩ) 母线电压分压电阻2
 #define RV3                            (10.0)                                   // (kΩ) 母线电压分压电阻3
 #define VC1                            (1.2)                                   // 电压补偿系数
 #define RV                             ((RV1 + RV2 + RV3) / RV3)               // 分压比
 #define HW_BOARD_VOLT_MAX              (HW_ADC_REF * RV)                       // (V)  ADC可测得的最大母线电压
/*时间设置值-------------------------------------------------------------------*/
 #define Calib_Time                     (1000)                                  // 校正次数，固定1000次，单位:次
 #define Charge_Time                    (100)                                   // (ms) 预充电时间，单位：ms
 #define Align_Time                     (30)                                  // (ms) 预定位时间，单位：ms
/*正常运行时估算算法的参数设置值-------------------------------------------------*/
 #define OBS_KSLIDE                     _Q15(0.85)                              // SMO算法里的滑膜增益值
 #define E_BW                           (1.5*BASE_FREQ)                          // PLL算法里的反电动势滤波值
/*逆风判断时的估算算法设置值-----------------------------------------------------*/
 #define TailWind_Time                  (250)                                   // (ms) 顺逆风检测时间
 #define ATO_BW_Wind                    (150.0)//120.0-PLL 120.0-smo            // 逆风判断观测器带宽的滤波值，经典值为8.0-100.0
 #define SPD_BW_Wind                    (10.0)//10.0-PLL  ,10.0-smo             // 逆风判断速度带宽的滤波值，经典值为5.0-40.0

 /**逆风顺风状态下的KP、KI****/
 #define DQKP_TailWind                  _Q12(1.5)                               //_Q12(1.0)-PLL ,   _Q12(1.5)   -smo
 #define DQKI_TailWind                  _Q15(0.18)                              //_Q15(0.08)-PLL  ,_Q15(0.2)-smo
/*启动参数参数值----------------------------------------------------------------*/
 /*********** RPD parameter ***********/
 /******* 初始位置检查参数 **********/
 #define PosCheckEnable                 (1)                                     // 初始位置使能
 #define AlignTestMode                  (0)                                     // 预定位测试模式
 /*脉冲注入时间长于2ms 或 低于2ms*/
 #define Long_Inject                    (0)                                     // 脉冲注入时间长于2ms,若时间长于4ms，则要修改定时器分频
 #define Short_Inject                   (1)                                     // 脉冲注入时间低于2ms
 #define InjectTime                     (Short_Inject)

 #define RPD_Time                       (3)                                     // (ms) 每次RPD的时间
 #define RPD_CurValue                   (1.0)                                   // (A)  RPD过流值
 #define DAC_RPDCurValue                _Q7(I_ValueX(RPD_CurValue * 2))

 /***预定位的Kp、Ki****/
 #define DQKP_Alignment                 _Q12(0.6)                               // 预定位的KP
 #define DQKI_Alignment                 _Q12(0.01)                              // 预定位的KI
 #define ID_Align_CURRENT               I_Value(0.0)                            // (A) D轴定位电流
 #define IQ_Align_CURRENT               I_Value(0.0)                            // (A) Q轴定位电流
 #define Align_Angle                    (0.0)																		// (°) 预定位角度

 /***启动电流****/                                   
 #define ID_Start_CURRENT               I_Value(0.0)                            // (A) D轴启动电流
 #define IQ_Start_CURRENT               I_Value(0.3)                            // (A) Q轴启动电流

 /***运行电流****/
 #define ID_RUN_CURRENT                 I_Value(0.0)                            // (A) D轴运行电流
 #define IQ_RUN_CURRENT                 I_Value(0.2)                             // (A) Q轴运行电流

 /***限制电流****/
 #define LIMIT_MIN_CURRENT              I_Value(0.2)                           	// (A) Q轴限制电流

 ///********Omega启动的参数**********/
 #define ATO_BW                         (300)                                   // 观测器带宽的滤波值，经典值为1.0-200.0
 
 #define ATO_BW_RUN                     (20.0)
 #define ATO_BW_RUN1                    (80.0)
 #define ATO_BW_RUN2                    (200.0)
 #define ATO_BW_RUN3                    (300.0)
 #define ATO_BW_RUN4                    (400.0)


 #define SPD_BW                         (15.0)                                  // 速度带宽的滤波值，经典值为5.0-40.0
 #define ATT_COEF                       (0.85)                                  // 无需改动
/*转速参数值-------------------------------------------------------------------*/
 /* motor start speed value */
 //open 算法启动参数
 #define MOTOR_OPEN_ACC                 (3.0)                                 // 强拖启动的增量(每载波周期加一次)
 #define MOTOR_OPEN_ACC_MIN             (5.0)                                  // 强拖启动的初始速度
 #define MOTOR_OPEN_ACC_CNT             (15.0)                                 // 强拖启动的执行次数(MOTOR_OPEN_ACC_CNT*256)
 #define MOTOR_OPEN_ACC_CYCLE           (1)                                     // 强拖启动循环拖动的次数

 //OMEGA启动参数
 #define Motor_Omega_Ramp_ACC_Antiwind  (20.0)                                   // omega启动的增量   12
 #define Motor_Omega_Ramp_ACC           (40.0)                                   // omega启动的增量   12		50
 #define MOTOR_OMEGA_ACC_MIN            (4800.0)                                  // (RPM) omega启动的最小切换转速	4600
 #define MOTOR_OMEGA_ACC_END            (18000.0)                                  // (RPM) omega启动的限制转速	18000

 /* motor loop control speed value */
 #define MOTOR_LOOP_RPM                 (32000.0)                                 // (RPM) 由mode 0到mode1切换转速，即闭环切换转速	36000


// //OMEGA启动参数
// #define Motor_Omega_Ramp_ACC_Antiwind  (20.0)                                   // omega启动的增量   12
// #define Motor_Omega_Ramp_ACC           (50.0)                                   // omega启动的增量   12		50
// #define MOTOR_OMEGA_ACC_MIN            (6000.0)                                  // (RPM) omega启动的最小切换转速	4600
// #define MOTOR_OMEGA_ACC_END            (8000.0)                                  // (RPM) omega启动的限制转速	18000

// /* motor loop control speed value */
// #define MOTOR_LOOP_RPM                 (10000.0)                                 // (RPM) 由mode 0到mode1切换转速，即闭环切换转速	36000



 /* motor run speed value */
 //电机运行时最大最小转速、堵转保护转速
 #define MOTOR_SPEED_SMOMIN_RPM         (6000.0)                                 // (RPM) SMO运行最小转速
 #define MOTOR_SPEED_MIN_RPM            (20000.0)                                // (RPM) 运行最小转速
 #define MOTOR_SPEED_MAX_RPM            (100000.0)                                // (RPM) 运行最大转速
 #define MOTOR_SPEED_LIMIT_RPM          (2350.0)
 #define MOTOR_SPEED_STAL_MAX_RPM       (140000.0)                                // (RPM) 堵转保护转速
 #define MOTOR_SPEED_STAL_MIN_RPM       (50000)

 #define MOTOR_SPEED_STOP_RPM           (850.0)                                 // (RPM) 运行最小转速

 #define Motor_Max_Power                (4000)
 #define Motor_Min_Power                (400)
/*电机开机、关机的设置----------------------------------------------------------*/
 /* motor ON/0FF value */
 #define OFFPWMDuty                     _Q15(0.02)                              // 关机PWM占空比，小于该占空比关机                                                                                //关机PWM占空比，小于该占空比时关机
 #define OFFPWMDutyHigh                 _Q15(1.0)                               // 关机PWM占空比，大于该占空比关机
 #define ONPWMDuty                      _Q15(0.05)                              // 开机PWM占空比，大于该占空比时开机
 #define MINPWMDuty                     _Q15(0.05)                              // 速度曲线上最小PWM占空比
 #define MAXPWMDuty                     _Q15(1.0)                               // 速度曲线上最大PWM占空比

 /*******运行时的参数*****************/
/*电流环参数设置值--------------------------------------------------------------*/
// #define DQKP                           _Q12(0.6)                               // DQ轴KP
// #define DQKI                           _Q15(0.01)                              // DQ轴KI

 #define DQKPStart                      _Q12(0.6)                               // DQ轴KP
 #define DQKIStart                      _Q15(0.01)                              // DQ轴KI

 #define DQKP                           _Q12(1.5)                               // DQ轴KP	1.5
 #define DQKI                           _Q15(0.6)                              // DQ轴KI	0.4


 /* D轴参数设置 */
 #define DOUTMAX                        _Q15(0.1)                             	// D轴最大限幅值，单位：输出占空比
 #define DOUTMIN                        _Q15(-0.1)                          		// D轴最小限幅值，单位：输出占空比
 /* Q轴参数设置，默认0.99即可 */
 #define QOUTMAX                        _Q15(0.96)                              // Q轴最大限幅值，单位：输出占空比
 #define QOUTMIN                        _Q15(-0.96)                             // Q轴最小限幅值，单位：输出占空比
 
 #define 	QOUTMINST										  _Q15(0.15)            										// 启动时Q轴最小限幅
 #define 	QOUTMINSTEND								  _Q15(0.06)            	
 
 //电流环的参数值
 #define QOUTINC                        (1)                                    	// Q轴电流增大步进值,开环控制时有效
 #define QOUTCURRENT                    (0.8)                                   // (A) Q轴输出电流,开环控制时有效
 #define QOUTVALUE                      I_Value(QOUTCURRENT)
/*外环参数设置值----------------------------------------------------------------*/
 #define SPEED_LOOP_TIME                (5)                                    	// (ms) 速度环调节周期 风扇速度环50，功率环5
 
 #define SKP                            _Q12(0.3)                               // 外环KP
 #define SKI                            _Q12(0.01)                             	// 外环KI
 
//高转速KP，KI
 #define SKPH                            _Q12(0.1)                                // 外环KP
 #define SKIH                            _Q12(0.01)                             	// 外环KI 

 #define SOUTMAX                        I_Value(2.0)                            	// (A) 外环最大限幅值
 #define SOUTMIN                        I_Value(0.06)                           	// (A) 外环最小限幅值

 #define SPEEDRAMPTIME									(2.0)																	// (ms) 速度环增量时间，13ms增/减一次

 #define SpeedRampStartInc           		 (1000.0)					  											// 启动速度环增量 
 #define SpeedRampStartDec           		 (1000.0)																// 启动速度环减量 

 #define SpeedRampInc                		(1000.0)																	// 运行速度环增量 
 #define SpeedRampDec                		(100.0)								 									// 运行速度环减量 

 /*外环使能*/
 #define OUTLoop_Disable                (0)                                     // 关闭外环
 #define OUTLoop_Enable                 (1)                                     // 使能外环
 #define OUTLoop_Mode                   (OUTLoop_Enable)

 /*外环选择功率环或速度环*/
 #define POWER_LOOP_CONTROL             (0)                                     //恒功率
 #define SPEED_LOOP_CONTROL             (1)                                     //恒转速
 #define Motor_Speed_Control_Mode       (SPEED_LOOP_CONTROL)

 //调速模式
 #define PWMMODE                        (0)                                     // PWM调速
 #define SREFMODE                       (1)                                     // 模拟调速
 #define NONEMODE                       (2)                                     // 直接给定值，不调速
 #define SPEED_MODE                     (NONEMODE)
 
 
 /*Key Scan Parameter*/
#define KeyFilterTime										(50)													       // Key滤波时间1->100us


/*模式选择设置值----------------------------------------------------------------*/
 /*IPM测试模式*/
 #define IPMtest                        (0)                                     // IPM测试或者MOS测试，MCU输出固定占空比
 #define NormalRun                      (1)                                     // 正常按电机状态机运行
 #define IPMState                       (NormalRun)

 /*估算器模式选择*/
 #define SMO                            (0)                                     // SMO ,滑膜估算
 #define PLL                            (1)                                     // PLL ,锁相环
 #define EstimateAlgorithm              (SMO)

 /*顺逆风判断设置*/
 #define NoTailWind                     (0)                                     // 无逆风顺风判断
 #define TailWind                       (1)                                     // 逆风顺风判断
 #define TailWind_Mode                  (NoTailWind)

 /*顺逆风判断方法*/
 #define RSDMethod                      (0)                                     // RSD比较器方法
 #define BEMFMethod                     (1)                                     // BEMF方法
 #define FOCMethod                      (2)                                     // FOC计算方法
 #define FRDetectMethod                 (FOCMethod)

 /*开环启动模式选择*/
 //大力矩,速度响应慢,顺风会造成电流冲击
 #define Open_Start                     (0)                                     // 开环强拖启动
 #define Omega_Start                    (1)                                     // Omega启动
 #define Open_Omega_Start               (2)                                     // 先开环启，后Omega启动
 #define Open_Start_Mode                (Omega_Start)

 /*电流采样模式*/
 #define Single_Resistor                (0)                                     // 单电阻电流采样模式
 #define Double_Resistor                (1)                                     // 双电阻电流采样模式
 #define Three_Resistor                 (2)                                     // 三电阻电流采样模式
 #define Shunt_Resistor_Mode            (Single_Resistor)

 #define OverModulation                 (0)                                     // 0-禁止过调制，1-使能过调制
 #define IRMODE                         (1)                                     // 正反转模式，正转为0，反转为1
/*保护参数值-------------------------------------------------------------------*/
 /*硬件过流保护*/
 #define Hardware_FO_Protect            (1)                                     // 硬件FO过流保护使能，适用于IPM有FO保护的场合
 #define Hardware_CMP_Protect           (2)                                     // 硬件CMP比较过流保护使能，适用于MOS管应用场合
 #define Hardware_FO_CMP_Protect        (3)                                     // 硬件CMP比较和FO过流保护都使能
 #define Hardware_Protect_Disable       (4)                                     // 硬件过流保护禁止，用于测试
 #define HardwareCurrent_Protect        (Hardware_CMP_Protect)                  // 硬件过流保护实现方式

 /*硬件过流保护比较值来源*/
 #define Compare_DAC                    (0)                                     // DAC设置硬件过流值
 #define Compare_Hardware               (1)                                     // 硬件设置硬件过流值
 #define Compare_Mode                   (Compare_DAC)                           // 硬件过流值的来源
 #define OverHardcurrentValue           (5.5)                                   // (A) DAC模式下的硬件过流值

 /*软件过流保护*/
 #define OverSoftCurrentValue           I_Value(1.5)                           	// (A) 软件过流值

 /*过流恢复*/
 #define CurrentRecoverEnable           (0)                                     // 过流保护使能位, 0，不使能；1，使能
 #define OverCurrentRecoverTime         (1000)                                  // (5ms) 过流保护恢复时间

 /*过欠压保护*/
 #define Over_Protect_Voltage           (400)                                  // (V) 直流电压过压保护值
 #define Over_Recover_Vlotage           (380)                                // (V) 直流电压过压保护恢复值
 #define Under_Protect_Voltage          (150)                           	// (V) 直流电压欠压保护值
 #define Under_Recover_Vlotage          (200)                                    	// (V) 直流电压欠压保护恢复值

 /*缺相保护*/
 #define PhaseLossCurrentValue          I_Value(0.75)                            // (A)  缺相电流值
 #define PhaseLossRecoverTime           (1000)                                   // (5ms) 缺相保护时间

 /*堵转保护*/
 #define StallCurrentValue             	I_Value(2)                            // (A)  堵转过流值	2(默认)
 #define StallRecoverTime               (1000)                                  // (5ms) 堵转保护运行时间

 /*启动保护*/
 #define StartProtectRestartTimes       (10)                                   // 启动保护重启次数，单位：次
 #define StartRecoverDelayTimes         (2000)                                 //(5ms)启动保护延时恢复时间

 /*功率保护*/
 #define PowerLimit                     (4500)                                   // 功率上限值 6500:300W
 #define OverPowerRecoverTime           (12000)                                   // (5ms) 功率保护恢复时间
 /*功率保护恢复*/
 #define PowerRecoverEnable             (1)                                     //0，不使能；1，使能 

 /*保护使能*/
 #define VoltageProtectEnable           (1)                                     // 电压保护，0,不使能；1，使能
 #define StartProtectEnable             (1)                                     // 启动保护，0,不使能；1，使能
 #define StallProtectEnable             (1)                                     // 堵转保护，0,不使能；1，使能
 #define PhaseLossProtectEnable         (1)                                     // 缺相保护，0,不使能；1，使能
 #define OverPowerProtectEnable         (1)                                     // 功率保护，0,不使能；1，使能
 
 /******启停测试参数******/
 #define StartONOFF_Enable              (0)
 #define StartON_Time                   (2500)                                  // (ms) 启动运行时间
 #define StartOFF_Time                  (2500)                                  // (ms) 停止时间
 #define Motor_Test_Speed               _Q15(120000 / MOTOR_SPEED_BASE)        // (RPM) 运行最小转速Motor_Test_Speed
 

 #define StopBrakeFlag                  (0)
 #define StopWaitTime                   (2000)                                  // (ms) 刹车等待时间

/*************速度控制挡位******************/
// #define Motor_Speed_Low                _Q15(99400 / MOTOR_SPEED_BASE);
// #define Motor_Speed_Mid                _Q15(104450 / MOTOR_SPEED_BASE);
// #define Motor_Speed_HIgh               _Q15(109400 / MOTOR_SPEED_BASE);


/*************速度控制挡位******************/
 #define Motor_Speed_Low                _Q15(80000 / MOTOR_SPEED_BASE); //80000						90000
 #define Motor_Speed_Mid                _Q15(92000 / MOTOR_SPEED_BASE);//95000						97000
 #define Motor_Speed_HIgh               _Q15(105000 / MOTOR_SPEED_BASE); //110000		


/*************温度控制挡位******************/
 #define Temperature_Off                (2600)          
 

//最高功率1400W
// #define Temperature_LOW_1                (11100)	//800			//风速高挡
// #define Temperature_Mid_1                (8600)	//1050	
// #define Temperature_High_1               (5600)	//1450		5600

// #define Temperature_LOW_2                (11400)	//750			//风速中挡
// #define Temperature_Mid_2                (9500)	//950	
// #define Temperature_High_2               (6500)	//1300	
// 
// #define Temperature_LOW_3                (11500)	//700			//风速低挡
// #define Temperature_Mid_3                (10000)	//850	
// #define Temperature_High_3               (8200) //1050
 
 
 //功率取值范围为2~14
 //最高功率1350W
 #define Temperature_LOW_1                (3000)				//风速高挡
 #define Temperature_Mid_1                (3800)		
 #define Temperature_High_1               (5000)		

 #define Temperature_LOW_2                (2900)				//风速中挡500w
 #define Temperature_Mid_2                (3650)						//700
 #define Temperature_High_2               (4800)						//1030
 
 #define Temperature_LOW_3                (2700)				//风速低挡
 #define Temperature_Mid_3                (3400)		
 #define Temperature_High_3               (4300)
 
 
 
 #define Correct_optical_Error          (1)                      								//校正光耦硬件误差
 #define ActualEdgeLevelTime            (128)                                 //校正光耦器件误差，填写User.EdgeLevelTime值

 
 #define Temperature_ADCLOW             (13000)                                //NTC60度电阻值24K
 #define Temperature_ADCMid             (8000)                                //NTC80度电阻值12K
 #define Temperature_ADCHigh            (5200)                                //NTC100度电阻值6.6K
 
#endif
