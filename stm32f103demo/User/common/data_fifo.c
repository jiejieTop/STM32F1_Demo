#include "./common/data_fifo.h"

#define kmalloc(size, mask) 		malloc(size)
#define kfree(ptr) 							free(ptr)
#define EXPORT_SYMBOL(sym)
#define BUG_ON(cond) 						ASSERT(!(cond))
#define ERR_PTR(val) NULL
#define IS_ERR(val) (!(val))

#define min(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);		\
        _x < _y ? _x : _y; })

				
#if defined(__GNUC__) || defined(__x86_64__)
#define TPOOL_COMPILER_BARRIER() __asm__ __volatile("" : : : "memory")

static inline void FullMemoryBarrier()
{
    __asm__ __volatile__("mfence": : : "memory");
}
#define smp_mb() FullMemoryBarrier()
#define smp_rmb() TPOOL_COMPILER_BARRIER()
#define smp_wmb() TPOOL_COMPILER_BARRIER()
				
#endif

static int fls(int x)
{
        int r = 32;

        if (!x)
                return 0;
        if (!(x & 0xffff0000u)) {
                x <<= 16;
                r -= 16;
        }
        if (!(x & 0xff000000u)) {
                x <<= 8;
                r -= 8;
        }
        if (!(x & 0xf0000000u)) {
                x <<= 4;
                r -= 4;
        }
        if (!(x & 0xc0000000u)) {
                x <<= 2;
                r -= 2;
        }
        if (!(x & 0x80000000u)) {
                x <<= 1;
                r -= 1;
        }
        return r;
}

#ifdef __GNUC__
#define __attribute_const__     __attribute__((__const__))
#endif

static unsigned long roundup_pow_of_two(unsigned long x)
{
        return (1UL << fls(x - 1));
}

#if USE_FIFO_ALLOC
struct kfifo *kfifo_init(unsigned char *buffer, 
												unsigned int size
#if USE_MUTEX
												,spinlock_t *lock
#endif
												)
{
        struct kfifo *fifo;

        /* size must be a power of 2 */
        BUG_ON(size & (size - 1));
	
        fifo = kmalloc(sizeof(struct kfifo));
	
        if (!fifo)
                return ERR_PTR(-ENOMEM);

        fifo->buffer = buffer;
        fifo->size = size;
        fifo->in = fifo->out = 0;
#if USE_MUTEX	
        fifo->lock = lock;
#endif
        return fifo;
}
#else



#endif
EXPORT_SYMBOL(kfifo_init);





