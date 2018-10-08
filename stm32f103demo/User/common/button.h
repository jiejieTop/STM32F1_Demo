#ifndef BUTTON_H
#define BUTTON_H

#include "include.h"


#define BTN_NAME_MAX  8

/* 按键消抖时间50ms, 单位10ms
 只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
*/
#define BUTTON_DEBOUNCE_TIME 	5
#define BUTTON_LONG_TIME 	100		/* 持续1秒，认为长按事件 */

typedef void (*Button_CallBack)(void);   /* 按键触发回调函数，需要用户实现 */

typedef enum {
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_DOUBLE,
  BUTTON_THREE,
  BUTTON_LONG,
  number_of_event, /* 触发回调的事件 */
  NONE_TRIGGER
}Button_Event;

/*
	每个按键对应1个全局的结构体变量。
	其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct Button_t
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*Read_Button_Level)(void); /* 读取按键电平函数，需要用户实现 */
  
  char Name[BTN_NAME_MAX];
  
  uint8_t Button_Trigger_Event        :   3;	  /* 按键触发事件，单击，双击，长按等 */
	uint8_t Button_State                :   3;	  /* 按键当前状态（按下还是弹起） */
  uint8_t Button_Trigger_Level        :   1;    /* 按键触发电平 */
  uint8_t Button_Current_Level        :   1;    /* 按键当前电平 */

  Button_CallBack CallBack_Function[number_of_event];
  
//	uint8_t Button_Cycle;	    /* 连续按键周期 */
//	uint8_t Button_Cycle_Count;	/* 连续按键计数器 */
//  
//  uint8_t Count;			    /* 滤波器计数器 */
	uint8_t Debounce_Time;		/* 消抖时间(最大255,表示2550ms) */
//	uint16_t Long_Count;		/* 长按计数器 */
	uint16_t Long_Time;		  /* 按键按下持续时间, 0表示不检测长按 */
  
}Button_t;

/* 定义键值代码
	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2)	编译器可帮我们避免键值重复。
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_DOWN_USER,			/* User键按下 */
	KEY_UP_USER,			/* User键弹起 */
	KEY_LONG_USER,			/* User键长按 */

	KEY_DOWN_WAKEUP,		/* WakeUp键按下 */
	KEY_UP_WAKEUP,			/* WakeUp键弹起 */

	KEY_DOWN_TAMPER,		/* Tamper键按下 */
	KEY_UP_TAMPER,			/* Tamper键弹起 */

	KEY_DOWN_JOY_UP,		/* 摇杆UP键按下 */
	KEY_DOWN_JOY_DOWN,		/* 摇杆DOWN键按下 */
	KEY_DOWN_JOY_LEFT,		/* 摇杆LEFT键按下 */
	KEY_DOWN_JOY_RIGHT,		/* 摇杆RIGHT键按下 */
	KEY_DOWN_JOY_OK,		/* 摇杆OK键按下 */

	KEY_UP_JOY_OK,			/* 摇杆OK键释放 */
	
	KEY_DOWN_USER_TAMPER	/* 组合键，USER键和WAKEUP键同时按下 */
}KEY_ENUM;

/* 按键ID */
enum
{
	KID_TAMPER = 0,
	KID_WAKEUP,
	KID_USER,
	KID_JOY_UP,
	KID_JOY_DOWN,
	KID_JOY_LEFT,
	KID_JOY_RIGHT,
	KID_JOY_OK
};

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	20
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	uint8_t Read;					/* 缓冲区读指针 */
	uint8_t Write;					/* 缓冲区写指针 */
}KEY_FIFO_T;

/* 供外部调用的函数声明 */
/************************************************************
  * @note    example
uint8_t Read_TestBtn_Level(void)
{
  return GPIO_ReadInputDataBit(KEY1_GPIO_PORT,KEY1_GPIO_PIN);
}

void TestBtn_CallBack(void)
{
  printf("按键按下!\n");
}

     Button_Create("test_button",
                  test_button, 
                  Read_TestBtn_Level,
                  1, 
                  BUTTON_DOWM,
                  TestBtn_CallBack);
  ***********************************************************/
void Button_Create(const char *name,
                  struct Button_t *btn, 
                  uint8_t(*read_btn_level)(void),
                  uint8_t btn_trigger_level, 
                  Button_Event btn_event,
                  Button_CallBack btn_callback);
                  
                  
#endif


