#include "led.h"
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


void task1(void *pvParameters)
{
    u8 state = 0;
    
    while(1)
    {
        printf("task 1 is running \r\n");
        
        (state = !state) == 1 ? RED_ON(): RED_OFF(); 
        vTaskDelay(1000/portTICK_RATE_MS);        
    }
}


void task2(void *pvParameters)
{
    u8 state1 = 0; 
    
    while(1)
    {
        printf("task 2 is running \r\n");
        
        (state1 = !state1) == 1 ? GREEN_ON() : GREEN_OFF();
        vTaskDelay(1000/portTICK_RATE_MS);         
    }
}



int main(void)
{
    // board initialize. 
    LED_Init();		  	
    uart_init(115200);
    
    //create two tasks, with the same priority.
    xTaskCreate( task2, "task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);      
    xTaskCreate( task1, "task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    
    // start scheduler now
    vTaskStartScheduler();            

    return 0;
}



