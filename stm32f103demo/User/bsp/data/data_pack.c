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
	* |     起始帧    |   数据长度   |  有效数据   |   校验   |  结束帧        |
	* --------------------------------------------------------------------------
	* | cai(0x636169) |    length    |    buff     |   校验   |  jie(0x6a6965) |
	* --------------------------------------------------------------------------
	* |     uint32    |     uint16   |    buff     |  uint16  |     uint32     |
	* -------------------------------------------------------------------------- 
	* |     4字节     |     2字节    |    buff     |   4字节  |     4字节      |
	* -------------------------------------------------------------------------- 
	* 
	******************************************************************
  */ 
/**
  ******************************************************************
														内部调用函数声明
  ******************************************************************
  */ 
//static uint32_t Get_Data_Head(void);
//static uint32_t Get_Data_Tail(void);

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
		DEBUG_LOG(">> [ERR] data Is Null \n");
		ASSERT(ASSERT_ERR);
		return -1;
	}
	/* 数据帧头 */
	*pTxBuf ++= (uint8_t)NAME_HEAD1; 
	*pTxBuf ++= (uint8_t)NAME_HEAD2; 
	*pTxBuf ++= (uint8_t)NAME_HEAD3; 
	*pTxBuf ++= (uint8_t)NAME_HEAD4; 
	
	/* 数据长度 */
	*pTxBuf ++= (uint8_t)(data_len>>8);  
	*pTxBuf ++= (uint8_t)(data_len);
	
	/* 有效数据 */
	if(0 != data_len)
	{
		memcpy(Usart_Tx_Buf + 6 , buff , data_len);
	}
	/* 数据偏移 */
	pTxBuf = (Usart_Tx_Buf + 6 + data_len);
	
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
	*pTxBuf ++= (uint8_t)NAME_TAIL1; 
	*pTxBuf ++= (uint8_t)NAME_TAIL2; 
	*pTxBuf ++= (uint8_t)NAME_TAIL3; 
	*pTxBuf ++= (uint8_t)NAME_TAIL4; 
	
#if USE_USART_DMA_TX	
	
#if USE_DATA_CRC 
	DMA_Send_Data(data_len + 14);
#else
	DMA_Send_Data(data_len + 10);
#endif

#else

#if USE_DATA_CRC 
	res = Usart_Write(Usart_Tx_Buf,data_len + 14);
#else
	res = Usart_Write(Usart_Tx_Buf,data_len + 10);
#endif
	if(res < 0)
	{
		DEBUG_LOG("uart write error %d \n", res);
		ASSERT(ASSERT_ERR);
		return -2;
	}
	
#endif
	
	return 0;
}

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



#if (USE_USART_DMA_RX == 0)
/**                    Usart_Rx_Sta 
	* ----------------------------------------------------------
	* | 0  0  0  0 | 0  0  0  0 | 0  0  0  0 | 0  0  0  0|
	* -----------------------------------------------------------
	* | 1  1  0  0 | 0  0  0  0 | 0  0  0  0 | 0  0  0  0|
	* -----------------------------------------------------------
最高两位用于保存数据是否接收完成   0：未完成 1：完成
次高位用于保存是否收到数据帧头     0：未完成 1：完成
其他位用于保存数据长度
*/

/* 接收状态标记 */
uint16_t Usart_Rx_Sta = 0;  
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
int32_t Receive_DataPack(void)
{
  uint8_t res;
  /* 读取数据 */
  res = USART_ReceiveData(DEBUG_USARTx);
  /* 接收未完成 */
  if((Usart_Rx_Sta&0x8000)==0)
    {
      /* 收到数据帧头 */
      if(Usart_Rx_Sta&0x4000)
        {
          if(res!=NAME_TAIL4)
          {
            Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
            Usart_Rx_Sta++;
            if(Usart_Rx_Sta>(USART_RX_BUFF_SIZE-1))
            {
              Usart_Rx_Sta=0;/* 接收数据错误,重新开始接收 */ 
              return -1;
            }
          }
          else 
          {
            /* 判断数据长度与接受的长度是否一致 */
            if((((Usart_Rx_Buf[4]-'0')*10)+(Usart_Rx_Buf[5]-'0')+10) == (Usart_Rx_Sta&0X3FFF))
            {
              Usart_Rx_Sta|=0x8000;	/* 接收完成 */
              Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
              return 0;
            }
            else
            {
              Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
              Usart_Rx_Sta++;
              if(Usart_Rx_Sta>(USART_RX_BUFF_SIZE-1))
              {
                Usart_Rx_Sta=0;/* 接收数据错误,重新开始接收 */ 
                return -1;
              }
            }
          }
        }
      else 
        {	
          if(res==NAME_HEAD1)
          {
            /* 收到数据帧头 */
            Usart_Rx_Sta|=0x4000;
            Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
            Usart_Rx_Sta++;
          }
          else
            return -1;	
        }
     }  
}



///**
//  ******************************************************************
//  * @brief   获取数据包起始帧(最大是4字节)，目前配置为3字节，按需修改即可
//  * @author  jiejie
//  * @version V1.0
//  * @date    2018-xx-xx
//  ******************************************************************
//  */ 
//static uint32_t Get_Data_Head(void)
//{
//	return  (uint32_t)(NAME_TAIL1 << 24) | (NAME_TAIL2 << 16) | \
//										(NAME_TAIL3 << 8) | (NAME_TAIL4);
//}

///**
//  ******************************************************************
//  * @brief   获取数据包结束帧(最大是4字节)，目前配置为3字节，
//  * @author  jiejie
//  * @version V1.0
//  * @date    2018-xx-xx
//  ******************************************************************
//  */ 
//static uint32_t Get_Data_Tail(void)
//{
//	return  (uint32_t)(NAME_HEAD1 << 24) | (NAME_HEAD2 << 16) | \
//										(NAME_HEAD3 << 8) | (NAME_HEAD4);
//}









