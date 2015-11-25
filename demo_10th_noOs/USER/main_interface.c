#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "App_LongShortKey.h"

#include "eventQueue.h"
#include "uart_intf.h"
#include "Timer_intf.h"


uint8_t flag = 0;

void SysTick_Init(void)
{
    if(SysTick_Config( SystemCoreClock / 100))  // 10ms 
    {
        while(1);
    }
    
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}



void SysTick_Handler(void)
{
    flag = 1;  
    Callback_Timer_TickISR(); 
}



void board_init(void)
{
	LED_Init();		  	      // 初始化与LED连接的硬件接口
    KEY_Init();               // 按键初始化
    delay_init();	    	  // 延时函数初始化	 
    uart_init(115200);          // 设置串口波特率
    SysTick_Init();
    
    //TIM3_Int_Init(100,7199);  // 10Khz的计数频率，计数到100为10ms  
    NVIC_Configuration();     // 设置中断分组信息
    
    printTask_init();
}


int main(void)
{	
    //static int cnt = 0;
    volatile static event_t* event;
    board_init();
     
	while(1)
	{
        event = EventQueue_GetPendingEvent();
        if (event)
        {
            event->execute(event->data);
        }
        
//        if(flag)
//        {
//            flag = 0;
//            if(cnt ++ >= 100)
//            {
//                cnt = 0;
//                printf("1s interrupt \n");
//            }
//        }
	}
 }

