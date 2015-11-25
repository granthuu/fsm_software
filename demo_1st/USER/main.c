#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "App_KeyNonblock.h"



char keyValue = 0;
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
    char i =0;
	LED_Init();		  	// ��ʼ����LED���ӵ�Ӳ���ӿ�
    KEY_Init();
    SysTick_Init();
    uart_init(9600);
    NVIC_Configuration();
     
    key_paraInit(); 
     
	while(1)
	{
        if(flag)  // 10ms�ж��ڣ���ѯ����״̬��������������������
        {
            flag =0;
            keyScanIO();
        }
        

        keyValue= key_readBuff();   /* ע��������Ҫ���ö�ȡbuffer�ĺ��������ö�ȡ�����������������ϲ���뿪���� */
        if(keyValue != 0x00)
        {
            switch(keyValue)
            { 
                case KEY0_CODE:                   
                    printf("key0 pressed now \r\n");
                break;
                
                case KEY1_CODE:
                    printf("key1 pressed now \r\n");
                break;   
                
                case KEY2_CODE:
                    printf("key2 pressed now \r\n");
                break;
                
                default:
                    break;  
            }
            keyValue = 0x00;
        }
	}
}

