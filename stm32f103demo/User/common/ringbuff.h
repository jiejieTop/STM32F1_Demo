#ifndef __DATA_FIFO_H
#define __DATA_FIFO_H

/************************************************************
  * @brief   __DATA_FIFO_H
  * @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    这个文件是数据的缓冲区处理文件
  ***********************************************************/

#include "include.h"


#if USE_MUTEX
typedef pthread_mutex_t spinlock_t;
#endif

#define spin_lock_irqsave(lock_ptr, flags) pthread_mutex_lock(lock_ptr)
#define spin_unlock_irqrestore(lock_ptr, flags) pthread_mutex_unlock(lock_ptr)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ringbuff 
{
	uint8_t *buffer;  	/* the buffer holding the data */
	uint32_t size;      /* the size of the allocated buffer */
	uint32_t in;        /* data is added at offset (in % size) */
	uint32_t out;       /* data is extracted from off. (out % size) */
#if USE_MUTEX
	spinlock_t *lock;       /* protects concurrent modifications */
#endif
}RingBuff_t;

extern err_t Create_RingBuff(RingBuff_t *rb, 
                             uint8_t *buffer,
                             uint32_t size
#if USE_MUTEX
                             ,spinlock_t *lock
#endif
                            );
																
//extern struct kfifo *kfifo_alloc(unsigned int size
//#if USE_MUTEX
//																,spinlock_t *lock
//#endif
//																);
//																
//extern void kfifo_free(struct kfifo *fifo);

extern uint32_t Write_RingBuff(RingBuff_t *rb,
                               uint8_t *buffer, 
                               uint32_t len);
																
extern uint32_t Read_RingBuff(RingBuff_t *rb,
                              uint8_t *buffer, 
                              uint32_t len);




#endif /* __DATA_FIFO_H */
