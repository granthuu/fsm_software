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


extern char receive_ok;
extern u8 uart_package[];
extern char rx_flag;
extern char rx_data;


extern char keyBuff[KEYBUFFSIZE];

int main(void)
 {	
    char key =0;
	LED_Init();		  	// 初始化与LED连接的硬件接口
    KEY_Init();
    SysTick_Init();
    uart_init(9600);
    NVIC_Configuration();
     
    bufPara_initial();
    key_paraInit();
     
	while(1)
	{
        if(rx_flag)
        {
            rx_flag =0;
            rx_process_check(); 	 
        }
        
        key = key_readBuff();
        if(key !=0)
        {
            switch(key){
                
                case 0x01:
                    printf("CMD 1 \r\n");
                break;
                
                case 0x02:
                    printf("CMD 2 \r\n");
                break;
                
                case 0x03:
                    printf("CMD 3 \r\n");
                break;
                
                case 0xaa:
                    printf("CMD N \r\n");
                break;
            }
        }
	}
 }

