#ifndef BUTTON_H
#define BUTTON_H

#include "include.h"

#define BTN_NAME_MAX  8

/* 按键消抖时间40ms, 建议调用周期为20ms
 只有连续检测到30ms状态不变才认为有效，包括弹起和按下两种事件
*/

#define CONTINUOS_TRIGGER     1  //是否支持连续触发 	

#define BUTTON_DEBOUNCE_TIME 	2   //消抖时间      2*调用周期
#define BUTTON_CYCLE          2	//连按触发时间  2*调用周期  
#define BUTTON_DOUBLE_TIME    20 	//双击间隔时间  20*调用周期  建议在300-700ms
#define BUTTON_LONG_TIME 	    50		/* 持续1秒(50*调用周期)，认为长按事件 */

#define TRIGGER_CB(event)   \
        if(btn->CallBack_Function[event]) \
          btn->CallBack_Function[event]((Button_t*)btn)

typedef void (*Button_CallBack)(void*);   /* 按键触发回调函数，需要用户实现 */



typedef enum {
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_DOUBLE,
  BUTTON_LONG,
  BUTTON_ALL_RIGGER,
  number_of_event, /* 触发回调的事件 */
  NONE_TRIGGER
}Button_Event;

/*
	每个按键对应1个全局的结构体变量。
	其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct button
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*Read_Button_Level)(void); /* 读取按键电平函数，需要用户实现 */
  
  char Name[BTN_NAME_MAX];
  
  uint8_t Button_Trigger_Event      :   3;	  /* 按键触发事件，单击，双击，长按等 */
	uint8_t Button_State              :   3;	  /* 按键当前状态（按下还是弹起） */
  uint8_t Button_Trigger_Level      :   1;    /* 按键触发电平 */
  uint8_t Button_Last_Level         :   1;    /* 按键当前电平 */

  Button_CallBack CallBack_Function[number_of_event];
  
	uint8_t Button_Cycle;	    /* 连续按键周期 */
//	uint8_t Button_Cycle_Count;	/* 连续按键计数器 */
  
  
  
  uint8_t Timer_Count;			/* 按键时间 */
	uint8_t Debounce_Time;		/* 消抖时间(最大255,表示2550ms) */
//	uint16_t Long_Count;		/* 长按计数器 */
	uint8_t Long_Time;		  /* 按键按下持续时间, 0表示不检测长按 */
  
}Button_t;




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
                  Button_t *btn, 
                  uint8_t(*read_btn_level)(void),
                  uint8_t btn_trigger_level);
                  
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback);   
                  
void Button_Cycle_Process(Button_t *btn);              
#endif


