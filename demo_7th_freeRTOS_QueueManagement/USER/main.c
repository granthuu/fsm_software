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

void task2(void *pvParameters);

xTaskHandle xTask2Handle;

static const char * Task1 = "task1 is running \r\n";
static const char * Task2 = "task2 is running \r\n";

void task1(void * pvParameters)
{
    char *TaskName = (char *)pvParameters;

    while(1)
    {
        printf(TaskName);
        
        xTaskCreate( task2, "Task2", configMINIMAL_STACK_SIZE, (void *)Task2, 2, &xTask2Handle);      
        vTaskDelay(1000/portTICK_RATE_MS);
    } 
}


void task2(void *pvParameters)
{
    char *taskName = (char *)pvParameters;
    printf(taskName);
     
    printf("task 2 is running and about to delete itself \r\n");
    vTaskDelete(xTask2Handle);
    // == vTaskDelete(NULL);
}


int main(void)
{
    // board initialize. 
    LED_Init();		  	
    uart_init(115200);
    
    //create task here.
    xTaskCreate( task1, "Task1", configMINIMAL_STACK_SIZE, (void *)Task1, 1, NULL);      
    
    // start scheduler now
    vTaskStartScheduler();            

    return 0;
}



