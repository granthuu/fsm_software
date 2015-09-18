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


 void delay(uint32_t t)
{
    uint32_t x, y;
    for(x=t; x>0; x--)
        for(y = 150; y>0; y--);
}


void vContinousProcessingTask(void * pvParameters)
{
    char *TaskName;
    TaskName = (char *)pvParameters;
    
    while(1)
    {
        printf(TaskName);
        delay(2000);
    }
}


void vTaskPeriodic(void *pvParameters)
{
    char *taskName;
    portTickType xLastWakeTime;
    
    taskName = (char *)pvParameters;
    xLastWakeTime = xTaskGetTickCount();  
    
    while(1)
    {
        printf(taskName);
        vTaskDelayUntil( &xLastWakeTime, ( 200 / portTICK_RATE_MS ) );
    }
}


static const char * ContinousTask1 = "Continous task1\r\n";
static const char * ContinousTask2 = "Continous task2\r\n";
static const char * PeriodicTask   = "Periodic task \r\n";

int main(void)
{
    // board initialize. 
    LED_Init();		  	
    uart_init(115200);
    
    //create two tasks, with the same priority.
    xTaskCreate( vTaskPeriodic,            "PeriodicTask",  configMINIMAL_STACK_SIZE, (void *)PeriodicTask,   2, NULL);      
    xTaskCreate( vContinousProcessingTask, "ContinousTask1", configMINIMAL_STACK_SIZE, (void *)ContinousTask1, 1, NULL);
    xTaskCreate( vContinousProcessingTask, "ContinousTask2", configMINIMAL_STACK_SIZE, (void *)ContinousTask2, 1, NULL);
    
    // start scheduler now
    vTaskStartScheduler();            

    return 0;
}



