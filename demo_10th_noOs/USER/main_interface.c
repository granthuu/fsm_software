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
	LED_Init();		  	      // ��ʼ����LED���ӵ�Ӳ���ӿ�
    KEY_Init();               // ������ʼ��
    delay_init();	    	  // ��ʱ������ʼ��	 
    uart_init(115200);          // ���ô��ڲ�����
    SysTick_Init();
    
    //TIM3_Int_Init(100,7199);  // 10Khz�ļ���Ƶ�ʣ�������100Ϊ10ms  
    NVIC_Configuration();     // �����жϷ�����Ϣ
    
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

