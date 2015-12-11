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
    
    //printTask_init();
    key_paraInit();
    
    
    printf("board initialize \n");
}


//void keyScan_Task()
//{
//    uint8_t key =0;
//    
//    if(flag)  // 10ms中断内，查询按键状态，用来消除按键抖动。
//    {
//        flag =0;
//        keyScan();
//    }
//    
//    key = key_readBuff();   /* 注意这里是要调用读取buffer的函数。利用读取函数将按键操作和上层隔离开来。 */
//    if(key != 0x00){
//        
//        switch(key){
//            
//            case (KEY0_CODE + KEY_SHORT_CODE):                              
//                printf("key1 short press \r\n");
//            break;
//            
//            case (KEY0_CODE + KEY_FIRST_LONG_CODE):
//                printf("key1 long press \r\n");
//            break;   
//            
//            case (KEY0_CODE + KEY_AFTER_LONG_CODE):
//                printf("key1 after long press \r\n");
//            break;
//            
//            default:
//                break;
//            
//        }
//    }

//}

int main(void)
{	
    volatile static event_t* event;
    board_init();
    keyScanTask_init();  
    uart_ringBuffer_Init();
    
	while(1)
	{
        event = EventQueue_GetPendingEvent();
        if (event)
        {
            event->execute(event->data);
        }
	}
 }

