#ifndef _DEBUG_LOG_H
#define _DEBUG_LOG_H

/**
* @name Log print 
* @{
*/
#define DEBUG_LOG_ENABLE	1
#if DEBUG_LOG_ENABLE
#define DEBUG_LOG 	printf   /* 打印调试日志 */     
#else

#endif


#ifndef DEBUG_LOG_NOFORMAT
#define DEBUG_LOG_NOFORMAT 	printf
#endif

/**@} */
	
//针对不同的编译器调用不同的stdint.h文件
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
#endif

/* 断言 Assert */
#define AssertCalled(char,int) 	printf("Error:%s,%d\r\n",char,int)
#define ASSERT(x)   if((x)==0)  AssertCalled(__FILE__,__LINE__)
  
typedef enum 
{
	ASSERT_ERR = 0,								/* 错误 */
	ASSERT_SUCCESS = !ASSERT_ERR	/* 正确 */
} Assert_ErrorStatus;

typedef enum 
{
	FALSE = 0,		/* 假 */
	TRUE = !FALSE	/* 真 */
}ResultStatus;



#endif /* __DEBUG_LOG_H */

