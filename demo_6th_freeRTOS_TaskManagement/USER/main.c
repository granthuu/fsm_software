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



void vTaskFunction(void *pvParameters)
{
    char *taskName;
    portTickType xLastWakeTime;
    
    taskName = (char *)pvParameters;
    xLastWakeTime = xTaskGetTickCount();  
    
    while(1)
    {
        printf(taskName);
        
        //vTaskDelay(1000/portTICK_RATE_MS); 
        vTaskDelayUntil( &xLastWakeTime, ( 1000 / portTICK_RATE_MS ) );
    }
}


static const char * task1 = "task 1 is running \r\n";
static const char * task2 = "task 2 is running \r\n";

int main(void)
{
    // board initialize. 
    LED_Init();		  	
    uart_init(115200);
    
    //create two tasks, with the same priority.
    xTaskCreate( vTaskFunction, "task1", configMINIMAL_STACK_SIZE, (void *)task1, 1, NULL);      
    xTaskCreate( vTaskFunction, "task2", configMINIMAL_STACK_SIZE, (void *)task2, 2, NULL);
    
    // start scheduler now
    vTaskStartScheduler();            

    return 0;
}



