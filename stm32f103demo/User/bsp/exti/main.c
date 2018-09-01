/**
  ******************************************************************************
  * @file    main.c
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   main
  ******************************************************************************
  * @attention 此demo是无RTOS版本，需要RTOS的demo请关注杰杰的GitHub：
  *	GitHub：https://github.com/jiejieTop
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "include.h"
/**
  ******************************************************************
														函数声明
  ******************************************************************
  */ 
static void BSP_Init(void);

/**
  ******************************************************************
													   变量声明
  ******************************************************************
  */ 
	
	
/**
  ******************************************************************
  * @brief   主函数
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
int main(void)
{
  uint8_t ABC[] = "aaaaaa";
	BSP_Init();
	while(1)                            
	{
    Send_DataPack(ABC,sizeof(ABC));
		LED1_TOGGLE;
		Delay_ms(500);
	}
}

/**
  ******************************************************************
  * @brief   BSP_Init，用于所有板级初始化
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
static void BSP_Init(void)
{
	/* LED 初始化 */
	LED_GPIO_Config();
	
#if USE_DWT_DELAY
	/* 内核精确定时器初始化 */
	CPU_TS_TmrInit();
#else
	/* 滴答定时器初始化 */
	SysTick_Init();
#endif
	
	/* 串口初始化 */
	USART_Config();
	
	/* 外部中断初始化 */
	EXTI_Key_Config(); 
	
	CRC_Config();
	
	/* 打印信息 */
	PRINT_INFO("welcome to learn jiejie stm32 library!\n");
	
}




/********************************END OF FILE***************************************/
