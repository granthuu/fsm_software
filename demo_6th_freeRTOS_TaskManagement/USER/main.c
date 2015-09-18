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

extern unsigned long ulIdleCycleCount;


void vTaskFunction(void *pvParameters)
{
    char *taskName;
    taskName = (char *)pvParameters;

    while(1)
    {
        printf(taskName);
        printf("%d \r\n", ulIdleCycleCount);

        vTaskDelay( 500 / portTICK_RATE_MS );
    }
}


static const char * Task1 = "task1 is running \r\n";
static const char * Task2 = "task2 is running \r\n";


int main(void)
{
    // board initialize. 
    LED_Init();		  	
    uart_init(115200);
    
    //create two tasks, with the same priority.
    xTaskCreate( vTaskFunction, "Task1", configMINIMAL_STACK_SIZE, (void *)Task1, 1, NULL);      
    xTaskCreate( vTaskFunction, "Task2", configMINIMAL_STACK_SIZE, (void *)Task2, 1, NULL);
    
    // start scheduler now
    vTaskStartScheduler();            

    return 0;
}



