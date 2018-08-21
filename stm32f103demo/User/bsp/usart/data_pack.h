#ifndef __DATA_PACK_H
#define __DATA_PACK_H

#include "stm32f10x.h"
#include "include.h"
/* c 标准库 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>


/**
  ******************************************************************
													   宏定义
  ******************************************************************
  */ 
#define NAME_HEAD1   'x'
#define NAME_HEAD2   'c'
#define NAME_HEAD3   'a'
#define NAME_HEAD4   'i'

#define NAME_TAIL1   'x'
#define NAME_TAIL2   'j'
#define NAME_TAIL3   'i'
#define NAME_TAIL4   'e'

#define DATA_HEAD   Get_Data_Head()
#define DATA_TAIL   Get_Data_Tail()



/**
  ******************************************************************
													   变量声明
  ******************************************************************
  */ 
typedef struct datapack
{
	uint32_t data_head; //数据头
	uint32_t data_length; // 数据长度
//	char data_type; 			//数据类型
	void* data; 		//数据
	uint32_t data_tail; //数据尾
}DataPack;




/**
  ******************************************************************
														函数声明
  ******************************************************************
  */ 

void Create_Pack(void *buff,uint32_t len);

uint32_t Get_Data_Head(void);
uint32_t Get_Data_Tail(void);





#endif /* __DATA_PACK_H */
