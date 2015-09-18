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


xTaskHandle xTask2Handle;

static const char * Task1 = "task1 is running \r\n";
static const char * Task2 = "task2 is running \r\n";

void task1(void * pvParameters)
{
    unsigned portBASE_TYPE priority;
    char *TaskName = (char *)pvParameters;
    
    priority = uxTaskPriorityGet(xTask2Handle);
    
    while(1)
    {
        printf(TaskName);
        printf("about to raise the task2 priority \r\n");
        
        vTaskPrioritySet(xTask2Handle, (priority + 1));
    } 
}


void task2(void *pvParameters)
{
    unsigned portBASE_TYPE priority;
    
    char *taskName = (char *)pvParameters;
    
    priority = uxTaskPriorityGet(NULL);  // 获取自身的优先级
    
    while(1)
    {
        printf(taskName);
        
        printf("about to lower the task2 priority \r\n");
        vTaskPrioritySet(NULL, (priority - 2));
    }
}


int main(void)
{
    // board initialize. 
    LED_Init();		  	
    uart_init(115200);
    
    //create two tasks, with the same priority.
    xTaskCreate( task1, "Task1", configMINIMAL_STACK_SIZE, (void *)Task1, 2, NULL);      
    xTaskCreate( task2, "Task2", configMINIMAL_STACK_SIZE, (void *)Task2, 1, &xTask2Handle);
    
    // start scheduler now
    vTaskStartScheduler();            

    return 0;
}



