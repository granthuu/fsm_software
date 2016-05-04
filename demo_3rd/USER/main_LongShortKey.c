#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "App_LongShortKey.h"

uint8_t flag = 0;
extern char receive_ok;
extern char rx_flag;

typedef struct { 
		char b0:1; 
		char b1:1; 
		char b2:1; 
		char b3:1;
		char b4:1; 
		char b5:1; 
		char b6:1; 
		char b7:1;
	}Bit;

int main(void)
 {	
    char key = 0, i = 0;
	LED_Init();		  	// 初始化与LED连接的硬件接口
    KEY_Init();
    uart_init(9600);
    NVIC_Configuration();
     
    bufPara_initial();
    key_paraInit();
     
    printf("board initialize \r\n");
     
	while(1)
	{
        if(rx_flag)
        {
            rx_flag =0;
            rx_process_check();   
        }
        if(receive_ok)
        {
            receive_ok = 0;
            
            for(i=0; i<4; i++)
            {
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
                        
                        case 0x08:
                            printf("CMD N \r\n");
                        break;
                    }
                    key = 0;
                } 
            }
        }
	}
 }

