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
    
    //printTask_init();
    key_paraInit();
    
    
    printf("board initialize \n");
}


//void keyScan_Task()
//{
//    uint8_t key =0;
//    
//    if(flag)  // 10ms�ж��ڣ���ѯ����״̬��������������������
//    {
//        flag =0;
//        keyScan();
//    }
//    
//    key = key_readBuff();   /* ע��������Ҫ���ö�ȡbuffer�ĺ��������ö�ȡ�����������������ϲ���뿪���� */
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

