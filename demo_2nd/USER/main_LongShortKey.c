#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "App_LongShortKey.h"

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
}



 int main(void)
 {	
    char key = 0;
	LED_Init();		  	// ��ʼ����LED���ӵ�Ӳ���ӿ�
    KEY_Init();
    SysTick_Init();
    uart_init(9600);
    NVIC_Configuration();

    __enable_irq();
     
    key_paraInit(); 
     
	while(1)
	{
        if(flag)  // 10ms�ж��ڣ���ѯ����״̬��������������������
        {
            flag =0;
            keyScan();
        }

        key = key_readBuff();
        if(key != 0x00)
        {
            switch(key)
            {
                case (KEY0_CODE+KEY_SHORT_CODE):
                    printf("short key pressed \r\n");
                break;
                
                case (KEY0_CODE+KEY_FIRST_LONG_CODE):
                    printf("long first pressed \r\n");
                break;
                
                case (KEY0_CODE+KEY_AFTER_LONG_CODE):
                    printf("long after pressed \r\n");
                break;
            }
        }

	}
 }

