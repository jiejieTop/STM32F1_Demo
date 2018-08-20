/* 头文件 */
#include "./usart/data_pack.h"

/**
  ******************************************************************
  * @brief   数据帧打包格式
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * 
	* --------------------------------------------------------------------------
	* |     起始帧    |   数据长度   |  有效数据   |   校验   |  结束帧        |
	* --------------------------------------------------------------------------
	* | cai(0x636169) |    length    |    buff     |   校验   |  jie(0x6a6965) |
	* --------------------------------------------------------------------------
	*      
	* 
	* 
	* 
	* 
	*  
	* 
	* 
	* 
	******************************************************************
  */ 

void Read_Ack(DataPack *p,u8 len);

/**
  ******************************************************************
  * @brief   创建一个数据包
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * @param   buff:数据地址
  * @return  新的数据包地址
  ******************************************************************
  */ 
void Create_Pack(void *buff)
{
	DataPack data_pack;
	DataPack  *send_data=&data_pack;  
	
	data_pack.data_head = DATA_HEAD; 
	data_pack.data_length = 50;
	data_pack.data = "faklfasd";
	data_pack.data_tail = DATA_TAIL;
	
//	USART_Send((u8*)&T,sizeof(T));
	Read_Ack(send_data,50);
//	printf("%s",(u8 *)&data_pack);
//	
}

void Read_Ack(DataPack *p,u8 len)     //形参1为结构体的首地址 &read_back   形参2 为发送帧的长度
{
         static u8 date,i;	   
	 for(i=0;i<len;i++)
	{
	 date= * (((u8*) &p->data_head)+i);    //每次循环将指针指向结构体中的下一个数据
         USART_SendData(USART1,date);   //通过串口3发送数据
         while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET); //等待发送完成
         }
}


char Name_Head[] = NAME_HEAD;
char Name_Tail[] = NAME_TAIL;
/**
  ******************************************************************
  * @brief   获取数据包起始帧(最大是4字节)，目前配置为3字节，按需修改即可
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
uint32_t Get_Data_Head(void)
{
	return  (uint32_t)(Name_Head[0] << 16) | (Name_Head[1] << 8) | Name_Head[2];
}

/**
  ******************************************************************
  * @brief   获取数据包结束帧(最大是4字节)，目前配置为3字节，
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
uint32_t Get_Data_Tail(void)
{
	return  (uint32_t)(Name_Tail[0] << 16) | (Name_Tail[1] << 8) | Name_Tail[2];
}









