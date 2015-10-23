#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "App_LongShortKey.h"

uint8_t flag = 0;
extern char receive_ok;
extern char rx_flag;


int main(void)
 {	
    char key = 0, i = 0;
	LED_Init();		  	// ��ʼ����LED���ӵ�Ӳ���ӿ�
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

