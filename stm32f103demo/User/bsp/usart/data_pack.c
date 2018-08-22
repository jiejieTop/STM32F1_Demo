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
	* |     4字节     |     2字节    |    buff     |   2字节  |     4字节      |
	* -------------------------------------------------------------------------- 
	* 
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
  * @param   buff:数据地址
  * @return  新的数据包地址
  ******************************************************************
  */ 
int32_t Create_Pack(void *buff,uint16_t data_len)
{
	int32_t ret;
#if USE_USART_DMA_TX	
	uint8_t *pTxBuf = Usart_Tx_Buf;
#else
	uint8_t Usart_Tx_Buf[data_len+14];
	uint8_t *pTxBuf = Usart_Tx_Buf;
#endif
	uint32_t CRCValue;
	
	if(NULL != buff)
	{
		DEBUG_LOG(">> [ERR] data Is Null \n");
		ASSERT(ASSERT_ERR);
		return -1;
	}
	
	*pTxBuf ++= (uint8_t)NAME_HEAD1; 
	*pTxBuf ++= (uint8_t)NAME_HEAD2; 
	*pTxBuf ++= (uint8_t)NAME_HEAD3; 
	*pTxBuf ++= (uint8_t)NAME_HEAD4; 
	
	*pTxBuf ++= (uint8_t)(data_len>>8);  
	*pTxBuf ++= (uint8_t)(data_len);
	
	if(0 != data_len)
	{
		memcpy(Usart_Tx_Buf + 6 , buff , data_len);
	}
	
	pTxBuf = (Usart_Tx_Buf + 6 + data_len);
	
#if USE_DATA_CRC 
	CRCValue = CRC_CalcBlockCRC((uint32_t *)buff, data_len);
	
	*pTxBuf ++= CRCValue >> 24; 
	*pTxBuf ++= CRCValue >> 16; 
	*pTxBuf ++= CRCValue >> 8;  
	*pTxBuf ++= CRCValue ; 
#endif
	*pTxBuf ++= (uint8_t)NAME_TAIL1; 
	*pTxBuf ++= (uint8_t)NAME_TAIL2; 
	*pTxBuf ++= (uint8_t)NAME_TAIL3; 
	*pTxBuf ++= (uint8_t)NAME_TAIL4; 
	
#if USE_USART_DMA_TX	
	
	DMA_Send_Data(data_len + 14);
	
#else
	
	ret = Usart_Write(Usart_Tx_Buf,data_len + 14);
	if(ret < 0)
	{
		DEBUG_LOG("uart write error %d \n", ret);
		ASSERT(ASSERT_ERR);
		return -2;
	}
	
#endif
	
	return 0;
}

int32_t Usart_Write(uint8_t *buf, uint32_t len)
{
	uint32_t i = 0;

	if(NULL == buf)
	{
		return -1;
	}

	for(i=0; i<len; i++)
	{
		Usart_SendByte(DEBUG_USARTx,buf[i]);
	}
	return len;
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









