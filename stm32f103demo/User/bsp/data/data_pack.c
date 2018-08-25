/* 头文件 */

#include "include.h"
/**
  ******************************************************************
  * @brief   数据帧打包格式
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * 
	* --------------------------------------------------------------------------
	* |     起始帧    |   数据长度   |  有效数据   |   校验   |    结束帧      |
	* --------------------------------------------------------------------------
	* | cai(0x636169) |    length    |    buff     |   校验   |     0x55       |
	* --------------------------------------------------------------------------
	* |     uint8     |     uint16   |    buff     |  uint32  |     uint8      |
	* -------------------------------------------------------------------------- 
	* |     1字节     |     2字节    |    buff     |   4字节  |     1字节      |
	* -------------------------------------------------------------------------- 
	* 
	******************************************************************
  */ 
  
/**                    Usart_Rx_Sta 
	* ----------------------------------------------------------
	* | 1  1  0  0 | 0  0  0  0 | 0  0  0  0 | 0  0  0  0|
	* -----------------------------------------------------------
	* | 0  0  0  0 | 0  0  0  0 | 0  0  0  0 | 0  0  0  0|
	* -----------------------------------------------------------
最高两位用于保存数据是否接收完成   0：未完成 1：完成
次高位用于保存是否收到数据帧头     0：未完成 1：完成
其他位用于保存数据长度
*/
/* 接收状态标记 */
uint32_t Usart_Rx_Sta = 0;  

  
/**
  ******************************************************************
														内部调用函数声明
  ******************************************************************
  */ 


/**
  ******************************************************************
  * @brief   发送数据，以数据包形式
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * @param   buff:数据起始地址
  * @return  成功 or 失败
  ******************************************************************
  */ 
int32_t Send_DataPack(void *buff,uint16_t data_len)
{
#if USE_USART_DMA_TX	
	uint8_t *pTxBuf = Usart_Tx_Buf;
#else
	/* 使用普通串口发送 */
	int32_t res;
	uint8_t Usart_Tx_Buf[data_len+14];
	uint8_t *pTxBuf = Usart_Tx_Buf;
#endif
	
#if USE_DATA_CRC 
	__IO uint32_t CRCValue;
#endif
	/* buff 无效	*/
	if(NULL == buff)
	{
		PRINT_ERR("data Is Null \n");
		ASSERT(ASSERT_ERR);
		return -1;
	}
	/* 数据帧头 */
	*pTxBuf ++= (uint8_t)DATA_HEAD; 

	/* 数据长度 */
	*pTxBuf ++= (uint8_t)(data_len>>8);  
	*pTxBuf ++= (uint8_t)(data_len);
	
	/* 有效数据 */
	if(0 != data_len)
	{
		memcpy(Usart_Tx_Buf + 3 , buff , data_len);
	}
	/* 数据偏移 */
	pTxBuf = (Usart_Tx_Buf + 3 + data_len);
	
#if USE_DATA_CRC 
	/* 使用CRC校验 */
	CRCValue = CRC_CalcBlockCRC((uint32_t *)buff, data_len);
	/* 校验值 */
	*pTxBuf ++= CRCValue >> 24; 
	*pTxBuf ++= CRCValue >> 16; 
	*pTxBuf ++= CRCValue >> 8;  
	*pTxBuf ++= CRCValue ; 
#endif
	/* 数据帧尾 */
	*pTxBuf ++= (uint8_t)DATA_TAIL; 

	
#if USE_USART_DMA_TX	
	
#if USE_DATA_CRC 
	DMA_Send_Data(data_len + 8);
#else
	DMA_Send_Data(data_len + 4);
#endif

#else

#if USE_DATA_CRC 
	res = Usart_Write(Usart_Tx_Buf,data_len + 8);
#else
	res = Usart_Write(Usart_Tx_Buf,data_len + 4);
#endif
	if(res < 0)
	{
		PRINT_ERR("uart write error %d \n", res);
		ASSERT(ASSERT_ERR);
		return -2;
	}
	
#endif
	
	return 0;
}
#if USE_USART_DMA_TX
void DMA_Send_Data(uint32_t len)
{
 
	DMA_Cmd(USART_TX_DMA_CHANNEL, DISABLE);                     //关闭DMA传输 
//	
////	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//确保DMA可以被设置  
//		
	DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL,len);          //设置数据传输量  
// 
	DMA_Cmd(USART_TX_DMA_CHANNEL, ENABLE); 
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, ENABLE);       //开启DMA传输 
}	  
#else

int32_t Usart_Write(uint8_t *buf, uint32_t len)
{
	uint32_t i = 0;
	/* 判断 buff 非空 */
	if(NULL == buf)
	{
		return -1;
	}
	/* 循环发送，一个字节 */
	for(i=0; i<len; i++)
	{
		Usart_SendByte(DEBUG_USARTx,buf[i]);
	}
	return len;
}  

#endif


/************************************************************
  * @brief   Usart_Receive_Data
  * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    不使用串口 DMA 接收时调用的函数
  ***********************************************************/
#if USE_USART_DMA_RX
void Uart_DMA_Rx_Data(void)
{
	/* 接收的数据长度 */
	uint32_t buff_length;
	
	/* 关闭DMA ，防止干扰 */
	DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);  
	
	/* 获取接收到的数据长度 单位为字节*/
	buff_length = USART_RX_BUFF_SIZE - DMA_GetCurrDataCounter(USART_RX_DMA_CHANNEL);
  
  /* 获取数据长度 */
  Usart_Rx_Sta = buff_length;

	PRINT_DEBUG("buff_length = %d\n ",buff_length);
  
	/* 清DMA标志位 */
	DMA_ClearFlag( DMA1_FLAG_TC5 );          
	
	/* 重新赋值计数值，必须大于等于最大可能接收到的数据帧数目 */
	USART_RX_DMA_CHANNEL->CNDTR = USART_RX_BUFF_SIZE;    
  
//	DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);    /* 暂时关闭dma，数据尚未处理 */   
	
	/* 给出信号 ，发送接收到新数据标志，供前台程序查询 */
	
    /* 标记接收完成，在 DataPack_Handle 处理*/
  Usart_Rx_Sta |= 0x8000;
  
	/* 
	DMA 开启，等待数据。注意，如果中断发送数据帧的速率很快，MCU来不及处理此次接收到的数据，
	中断又发来数据的话，这里不能开启，否则数据会被覆盖。有2种方式解决：

	1. 在重新开启接收DMA通道之前，将LumMod_Rx_Buf缓冲区里面的数据复制到另外一个数组中，
	然后再开启DMA，然后马上处理复制出来的数据。

	2. 建立双缓冲，在LumMod_Uart_DMA_Rx_Data函数中，重新配置DMA_MemoryBaseAddr 的缓冲区地址，
	那么下次接收到的数据就会保存到新的缓冲区中，不至于被覆盖。
	*/
}

#else
void Receive_DataPack(void)
{
  uint8_t res;
  /* 读取数据会清除中断标志位 */
  res = USART_ReceiveData(DEBUG_USARTx);

	if((Usart_Rx_Sta&0x80000000)==0)//接收未完成
	{
		if(Usart_Rx_Sta&0x40000000)//接收到了DATA_HEAD
		{
			if(res!=DATA_TAIL)
			{
			Usart_Rx_Buf[Usart_Rx_Sta&0XFFFF]=res ;
			Usart_Rx_Sta++;
			}
			else
			{
			Usart_Rx_Buf[Usart_Rx_Sta&0XFFFF]=res ;
			Usart_Rx_Sta++;
			Usart_Rx_Sta|=0x80000000;		//接收完成了 
			PRINT_ERR("receive ok!");
			}
		}
		else //还没收到DATA_HEAD
		{	
			if(res==DATA_HEAD)
			{
				Usart_Rx_Buf[Usart_Rx_Sta&0XFFFF]=res ;
				Usart_Rx_Sta++;
				Usart_Rx_Sta|=0x40000000;
			}
			else
			{
				PRINT_ERR("receive fail!");
			}		 
		}
	} 
}
	
#endif

/************************************************************
  * @brief   DataPack_Handle
  * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    数据包处理，解析数据
  ***********************************************************/
int32_t DataPack_Handle(uint8_t* buff,DataPack* datapack)
{
  uint16_t data_len;
  if((NULL == buff)||(NULL == datapack))
  {
    PRINT_ERR("buff or len is NULL\n");
    return -1;
  }
  /* 接收完成 */
  if( Usart_Rx_Sta & 0x80000000 )
  {
    /* 获取数据长度 */
    data_len = Usart_Rx_Sta & 0xffff;
#if USE_DATA_CRC
    *len = data_len - 8;
#else
    datapack->data_length = data_len - 4;
#endif
    
    /* 清除接收完成标志位 */
    Usart_Rx_Sta = 0;
    /* 校验数据包是否一致 */
    if((DATA_HEAD == Usart_Rx_Buf[0])&&(DATA_TAIL == Usart_Rx_Buf[data_len-1]))
    {
      memcpy(buff,&Usart_Rx_Buf[3],datapack->data_length);
    }
    else
    {
			PRINT_DEBUG("data length is not equal!\n");
			memset(Usart_Rx_Buf,0,data_len+1);
      buff = NULL;
      datapack->data_length = 0;
    }
#if USE_USART_DMA_RX
    /* 打开DMA，可以进行下一次接收 */
    DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);
#endif
  return 0;
  }
  return -1;
}










