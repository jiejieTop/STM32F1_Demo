#ifndef __DATA_PACK_H
#define __DATA_PACK_H

#include "stm32f10x.h"
/* c 标准库 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "./usart/bsp_usart.h"

/**
  ******************************************************************
													   宏定义
  ******************************************************************
  */ 

#define USE_DATA_CRC  0
/* 数据帧头 */
#define DATA_HEAD   0x02

/* 数据帧尾 */
#define DATA_TAIL   0x03




/**
  ******************************************************************
													   变量声明
  ******************************************************************
  */ 
typedef struct datapack
{
	uint8_t data_head; //数据头
	uint8_t data_tail; //数据尾
//	uint8_t data_type; 			//数据类型
	uint16_t data_length; // 数据长度
	uint32_t data_crc; // 数据校验
}DataPack;




/**
  ******************************************************************
														函数声明
  ******************************************************************
  */ 
int32_t Send_DataPack(void *buff,uint16_t len);
int32_t DataPack_Handle(uint8_t* buff,DataPack* datapack);

#if USE_USART_DMA_RX
void Uart_DMA_Rx_Data(void);
#else
void Receive_DataPack(void);
#endif

#if USE_USART_DMA_TX
void DMA_Send_Data(uint32_t len);
#else
int32_t Usart_Write(uint8_t *buf, uint32_t len);
#endif




#endif /* __DATA_PACK_H */
