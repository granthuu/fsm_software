#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "App_LongShortKey.h"

#include "timer.h"



static u8 hour = 12, min = 8, sec = 34; 
uint8_t flag = 0;

void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
    {
        flag = 1;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
    }
}

void app_dateHandle(void)
{
    static u8 h = 12, m = 8;
    static u8 cnt =0;
    
    if(++cnt >= 100)
    {
        cnt = 0;
        
        sec ++;
        if(sec >= 10)
        {
            sec = 0;
            min ++;
            if(min >= 20)
            {
                min = 0;
                hour ++;
                if(hour >= 24)
                    hour = 0;
            }
        }
        
        if(h != hour)
        {
            h = hour;
            LCD_ShowNum(84,162,hour,2,24);
        }
        
        if(m != min)
        {
            m = min;
            LCD_ShowNum(124,162,min,2,24);  
        }          
    } 
}

void app_LCDinit(void)
{  
 	LCD_Init();				 // ��ʼ��LCD	 	
    
	POINT_COLOR=RED;//��������Ϊ��ɫ 
    LCD_ShowString(60, 162, 200, 24, 24, "    :   ");	
     
    LCD_ShowNum(84,162,hour,2,24);									  
    LCD_ShowNum(124,162,min,2,24);  
    LCD_SetCursor(84, 162);
}

int main(void)
{
    u8 state1 = 0, state2 = 0;
    char key =0;
    
	LED_Init();		  	      // ��ʼ����LED���ӵ�Ӳ���ӿ�
    KEY_Init();               // ������ʼ��
    delay_init();	    	  // ��ʱ������ʼ��	 
    uart_init(9600);          // ���ô��ڲ�����
    TIM3_Int_Init(100,7199);  // 10Khz�ļ���Ƶ�ʣ�������100Ϊ10ms  
    NVIC_Configuration();     // �����жϷ�����Ϣ
    
    
    key_paraInit();
    app_LCDinit();
 
	while(1)
	{
        if(flag)  // 10ms�жϺ�������������timer�������ж�
        {
            flag =0;
            keyScan();
            
            // ʱ�䴦����
            app_dateHandle();
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







