/* ͷ�ļ� */

#include "include.h"
/**
  ******************************************************************
  * @brief   ����֡�����ʽ
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * 
	* --------------------------------------------------------------------------
	* |     ��ʼ֡    |   ���ݳ���   |  ��Ч����   |   У��   |    ����֡      |
	* --------------------------------------------------------------------------
	* | cai(0x636169) |    length    |    buff     |   У��   |     0x55       |
	* --------------------------------------------------------------------------
	* |     uint8     |     uint16   |    buff     |  uint32  |     uint8      |
	* -------------------------------------------------------------------------- 
	* |     1�ֽ�     |     2�ֽ�    |    buff     |   4�ֽ�  |     1�ֽ�      |
	* -------------------------------------------------------------------------- 
	* 
	******************************************************************
  */ 
  
/**                    Usart_Rx_Sta 
	* ----------------------------------------------------------
	* | 0  0  0  0 | 0  0  0  0 | 0  0  0  0 | 0  0  0  0|
	* -----------------------------------------------------------
	* | 1  1  0  0 | 0  0  0  0 | 0  0  0  0 | 0  0  0  0|
	* -----------------------------------------------------------
�����λ���ڱ��������Ƿ�������   0��δ��� 1�����
�θ�λ���ڱ����Ƿ��յ�����֡ͷ     0��δ��� 1�����
����λ���ڱ������ݳ���
*/
/* ����״̬��� */
uint16_t Usart_Rx_Sta = 0;  

  
/**
  ******************************************************************
														�ڲ����ú�������
  ******************************************************************
  */ 


/**
  ******************************************************************
  * @brief   �������ݣ������ݰ���ʽ
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  * @param   buff:������ʼ��ַ
  * @return  �ɹ� or ʧ��
  ******************************************************************
  */ 
int32_t Send_DataPack(void *buff,uint16_t data_len)
{
#if USE_USART_DMA_TX	
	uint8_t *pTxBuf = Usart_Tx_Buf;
#else
	/* ʹ����ͨ���ڷ��� */
	int32_t res;
	uint8_t Usart_Tx_Buf[data_len+14];
	uint8_t *pTxBuf = Usart_Tx_Buf;
#endif
	
#if USE_DATA_CRC 
	__IO uint32_t CRCValue;
#endif
	/* buff ��Ч	*/
	if(NULL == buff)
	{
		DEBUG_LOG(">> [ERR] data Is Null \n");
		ASSERT(ASSERT_ERR);
		return -1;
	}
	/* ����֡ͷ */
	*pTxBuf ++= (uint8_t)DATA_HEAD; 

	/* ���ݳ��� */
	*pTxBuf ++= (uint8_t)(data_len>>8);  
	*pTxBuf ++= (uint8_t)(data_len);
	
	/* ��Ч���� */
	if(0 != data_len)
	{
		memcpy(Usart_Tx_Buf + 3 , buff , data_len);
	}
	/* ����ƫ�� */
	pTxBuf = (Usart_Tx_Buf + 3 + data_len);
	
#if USE_DATA_CRC 
	/* ʹ��CRCУ�� */
	CRCValue = CRC_CalcBlockCRC((uint32_t *)buff, data_len);
	/* У��ֵ */
	*pTxBuf ++= CRCValue >> 24; 
	*pTxBuf ++= CRCValue >> 16; 
	*pTxBuf ++= CRCValue >> 8;  
	*pTxBuf ++= CRCValue ; 
#endif
	/* ����֡β */
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
		DEBUG_LOG("uart write error %d \n", res);
		ASSERT(ASSERT_ERR);
		return -2;
	}
	
#endif
	
	return 0;
}
#if USE_USART_DMA_TX
void DMA_Send_Data(uint32_t len)
{
 
	DMA_Cmd(USART_TX_DMA_CHANNEL, DISABLE);                     //�ر�DMA���� 
//	
////	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
//		
	DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL,len);          //�������ݴ�����  
// 
	DMA_Cmd(USART_TX_DMA_CHANNEL, ENABLE); 
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, ENABLE);       //����DMA���� 
}	  
#else

int32_t Usart_Write(uint8_t *buf, uint32_t len)
{
	uint32_t i = 0;
	/* �ж� buff �ǿ� */
	if(NULL == buf)
	{
		return -1;
	}
	/* ѭ�����ͣ�һ���ֽ� */
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
  * @note    ��ʹ�ô��� DMA ����ʱ���õĺ���
  ***********************************************************/
#if USE_USART_DMA_RX
void Uart_DMA_Rx_Data(void)
{
	/* ���յ����ݳ��� */
	uint32_t buff_length;
	
	/* �ر�DMA ����ֹ���� */
	DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);  
	
	/* ��ȡ���յ������ݳ��� ��λΪ�ֽ�*/
	buff_length = USART_RX_BUFF_SIZE - DMA_GetCurrDataCounter(USART_RX_DMA_CHANNEL);
  
  /* ��ȡ���ݳ��� */
  Usart_Rx_Sta = buff_length;
  
#if DEBUG_LOG_ENABLE	
	DEBUG_LOG("buff_length = %d\n ",buff_length);
#endif
  
	/* ��DMA��־λ */
	DMA_ClearFlag( DMA1_FLAG_TC5 );          
	
	/* ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ */
	USART_RX_DMA_CHANNEL->CNDTR = USART_RX_BUFF_SIZE;    
  
//	DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);    /* ��ʱ�ر�dma��������δ���� */   
	
	/* �����ź� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ */
	
    /* ��ǽ�����ɣ��� DataPack_Handle ����*/
  Usart_Rx_Sta |= 0x8000;
  
	/* 
	DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU�����������˴ν��յ������ݣ�
	�ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����

	1. �����¿�������DMAͨ��֮ǰ����LumMod_Rx_Buf��������������ݸ��Ƶ�����һ�������У�
	Ȼ���ٿ���DMA��Ȼ�����ϴ������Ƴ��������ݡ�

	2. ����˫���壬��LumMod_Uart_DMA_Rx_Data�����У���������DMA_MemoryBaseAddr �Ļ�������ַ��
	��ô�´ν��յ������ݾͻᱣ�浽�µĻ������У������ڱ����ǡ�
	*/
}

#else
void Receive_DataPack(void)
{
  uint8_t res;
  /* ��ȡ���� */
  res = USART_ReceiveData(DEBUG_USARTx);
  /* ����δ��� */
  if((Usart_Rx_Sta&0x8000)==0)
  {
    /* �յ�����֡ͷ */
    if(Usart_Rx_Sta&0x4000)
    {
      if(res!=NAME_TAIL)
      {
        Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
        Usart_Rx_Sta++;
        if(Usart_Rx_Sta>(USART_RX_BUFF_SIZE-1))
        {
          Usart_Rx_Sta=0;/* �������ݴ���,���¿�ʼ���� */ 
          #if DEBUG_LOG_ENABLE
          DEBUG_LOG(">> DataPack Receive Fail!\n");
          #endif
        }
      }
      else 
      {
        /* �ж����ݳ�������ܵĳ����Ƿ�һ�� */
        #if USE_DATA_CRC
        if((((Usart_Rx_Buf[1]-'0')*10)+(Usart_Rx_Buf[2]-'0')+8) == (Usart_Rx_Sta&0X3FFF))
        #else
        if((((Usart_Rx_Buf[1]-'0')*10)+(Usart_Rx_Buf[2]-'0')+4) == (Usart_Rx_Sta&0X3FFF))
        #endif
        {
          Usart_Rx_Sta|=0x8000;	/* ������� */
          Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
          #if DEBUG_LOG_ENABLE
          DEBUG_LOG(">> DataPack Receive Succes!\n");
          #endif
        }
        else
        {
          Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
          Usart_Rx_Sta++;
          if(Usart_Rx_Sta>(USART_RX_BUFF_SIZE-1))
          {
            /* �������ݴ���,���¿�ʼ���� */
            Usart_Rx_Sta=0; 
            #if DEBUG_LOG_ENABLE
            DEBUG_LOG(">> DataPack Receive Fail!\n");
            #endif
          }
        }
      }
    }
    else 
    {	
      if(res==NAME_HEAD)
      {
        /* �յ�����֡ͷ */
        Usart_Rx_Sta|=0x4000;
        Usart_Rx_Buf[Usart_Rx_Sta&0X3FFF]=res;
        Usart_Rx_Sta++;
      }
      else
      {
        /* �������ݴ��� */
        Usart_Rx_Sta = 0;
        #if DEBUG_LOG_ENABLE
        DEBUG_LOG("DataPack Receive Fail!");
        #endif
      }
    }
  }
  else
  {
    #if DEBUG_LOG_ENABLE
    DEBUG_LOG(">> Data Receive Too Fast\n");
    #endif 
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
  * @note    ���ݰ���������������
  ***********************************************************/
int32_t DataPack_Handle(uint8_t *buff,uint8_t *len)
{
  uint16_t data_len;
  if((NULL == buff)||(NULL == len))
  {
    #if DEBUG_LOG_ENABLE
    DEBUG_LOG(">> buff or len is NULL\n");
    #endif 
    return -1;
  }
  /* ������� */
  if( Usart_Rx_Sta & 0x8000 )
  {
    /* ��ȡ���ݳ��� */
    data_len = Usart_Rx_Sta & 0x3fff;
    /* ���������ɱ�־λ */
    Usart_Rx_Sta = 0;
    /* У�����ݰ��Ƿ�һ�� */
    if((DATA_HEAD == Usart_Rx_Buf[0])&&(DATA_TAIL == Usart_Rx_Buf[data_len-1]))
    {
      memcpy(buff,Usart_Rx_Buf,data_len);
    }
#if USE_USART_DMA_RX
    /* ��DMA�����Խ�����һ�ν��� */
    DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);
#endif
  return 0;
  }
  return -1;
}








