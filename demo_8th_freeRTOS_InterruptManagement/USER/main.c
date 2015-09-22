#include "led.h"
#include "key.h"
#include "exti.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

// FreeRTOS head file, add here.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"


/* declare a SemaphoreHandle_t variable, using to save queue handler. */
SemaphoreHandle_t xBinarySemaphore = NULL;



void board_Init(void)
{
    LED_Init();	
    KEY_Init();  
    //EXTIX_Init();
    uart_init(115200);    
    
    // interrupt initialize
	//NVIC_Configuration();//    
}


void keyScan_Task(void *pvParameters)
{
    char key = 0x00;
    
    while(1)
    {
        // add your key scan code here.
        keyScan();
        if((key = keyScan_readBuff()) != 0)
        { 
            switch(key)
            {
                case ( KEY_CODE + SHORT_KEY):
                    printf("short key pressed \r\n");
                
                    xSemaphoreGive(xBinarySemaphore);
                    printf("key pressed, give semaphore \r\n");  
                
                break;
                
                case ( KEY_CODE+FIRSTLONG_KEY_CODE):
                    printf("long first pressed \r\n");
                break;
                
                case ( KEY_CODE+AFTERLONG_KEY_CODE):
                    printf("long after pressed \r\n");
                break;
            }
        }
        
        vTaskDelay(10/portTICK_RATE_MS);
    }
}

void LED_task(void *pvParameters)
{
    char state = 0;
    
    while(1)
    {
        ((state = !state) == 1) ? GREEN_ON() : GREEN_OFF();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}




void Key_HanderFun(void *pvParameters)
{
    char state = 0;
    
    while(1)
    {
        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
        printf("take semaphore here \r\n");
        
        (state =!state) == 1 ? RED_ON() : RED_OFF();
    }
}



int main(void)
{
    // board initialize. 
    board_Init();
    
    // create binary semaphore
    vSemaphoreCreateBinary(xBinarySemaphore);

    if(xBinarySemaphore != NULL)  
    {
        xTaskCreate(Key_HanderFun,    "keyHandlerTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(keyScan_Task,     "keyScanTask",     configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(LED_task,         "LED_task",        configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xSemaphoreTake(xBinarySemaphore, 0);
        
        // start scheduler now
        vTaskStartScheduler();            
    }
    else
    {
        // semaphore create unsuccessful here. add your code.
        printf("semaphore create failed \r\n");
    }
    
    return 0;
}

void EXTI0_IRQHandler(void)
{   
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    
	if(WK_UP == 1)
	{	  
        // Unblock the task by releasing the semaphore.
        xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
        printf("key pressed, give semaphore \r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  
}


void EXTI9_5_IRQHandler(void)
{			
	delay_ms(10);   //??			 
	if(KEY0==0)	{
		LED0=!LED0;
	}
 	 EXTI_ClearITPendingBit(EXTI_Line5);    //??LINE5???????  
}


void EXTI15_10_IRQHandler(void)
{
  delay_ms(10);    //??			 
  if(KEY1==0)	{
		LED1=!LED1;
	}
	 EXTI_ClearITPendingBit(EXTI_Line15);  //??LINE15?????
}

