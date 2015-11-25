#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "App_LongShortKey.h"

#include "eventQueue.h"


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


extern char receive_ok;
extern u8 uart_package[];
extern char rx_flag;
extern char rx_data;


extern char keyBuff[KEYBUFFSIZE];

int main(void)
 {	
    u8 state1 = 0, state2 = 0;
    char key =0;
    
	LED_Init();		  	      // ��ʼ����LED���ӵ�Ӳ���ӿ�
    KEY_Init();               // ������ʼ��
    delay_init();	    	  // ��ʱ������ʼ��	 
    uart_init(9600);          // ���ô��ڲ�����
    SysTick_Init();
    //TIM3_Int_Init(100,7199);  // 10Khz�ļ���Ƶ�ʣ�������100Ϊ10ms  
    NVIC_Configuration();     // �����жϷ�����Ϣ
     
	while(1)
	{
        if(flag)  // 10ms�жϺ�������������timer�������ж�
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
                    (state1 = !state1) == 1 ? RED_ON() : RED_OFF();
                break;
                
                case (KEY0_CODE+KEY_FIRST_LONG_CODE):
                    printf("long first pressed \r\n");
                    (state2 = !state2) == 1 ? GREEN_ON() : GREEN_OFF();
                break;
                
                case (KEY0_CODE+KEY_AFTER_LONG_CODE):
                    printf("long after pressed \r\n");
                    (state2 = !state2) == 1 ? GREEN_ON() : GREEN_OFF();
                break;
            }
        }
	}
 }

