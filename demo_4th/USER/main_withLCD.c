#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "App_LongShortKey.h"

#include "timer.h"



static u8 hour = 12, min = 8, sec = 34; 
uint8_t flag = 0;

void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        flag = 1;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
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
 	LCD_Init();				 // 初始化LCD	 	
    
	POINT_COLOR=RED;//设置字体为红色 
    LCD_ShowString(60, 162, 200, 24, 24, "    :   ");	
     
    LCD_ShowNum(84,162,hour,2,24);									  
    LCD_ShowNum(124,162,min,2,24);  
    LCD_SetCursor(84, 162);
}

int main(void)
{
    u8 state1 = 0, state2 = 0;
    char key =0;
    
	LED_Init();		  	      // 初始化与LED连接的硬件接口
    KEY_Init();               // 按键初始化
    delay_init();	    	  // 延时函数初始化	 
    uart_init(9600);          // 设置串口波特率
    TIM3_Int_Init(100,7199);  // 10Khz的计数频率，计数到100为10ms  
    NVIC_Configuration();     // 设置中断分组信息
    
    
    key_paraInit();
    app_LCDinit();
 
	while(1)
	{
        if(flag)  // 10ms中断函数，这是利用timer来产生中断
        {
            flag =0;
            keyScan();
            
            // 时间处理函数
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







