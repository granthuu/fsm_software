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

/* declare a queueHandle variable, using to save queue handler. */
xQueueHandle xQueue;



void vSendTask(void *pvParameters)
{
    long valueToSend;
    portBASE_TYPE status;
    
    valueToSend = (long)pvParameters;
    
    while(1)
    {
        status = xQueueSendToBack(xQueue, &valueToSend, 0);
        if(status != pdPASS)
        {
            printf("could not send to the queue. \r\n");
        }
        
        //vTaskDelay(1000 / portTICK_RATE_MS);
        taskYIELD();
    }
}


void vReceiveTask(void *pvParameters)
{
    long lReceivedValue;
    portBASE_TYPE status;

    while(1)
    {        
        if( uxQueueMessagesWaiting( xQueue ) != 0 )
        {
            printf( "Queue should have been empty! \r\n");
        }

        status = xQueueReceive( xQueue, &lReceivedValue, 3000 / portTICK_RATE_MS );
        if( status == pdPASS )
        {
            printf( "Received = %ld\r\n", lReceivedValue );
        }
        else
        {
            printf( "Could not receive from the queue.\r\n" );
        }
    }
}

int main(void)
{
    // board initialize. 
    LED_Init();		  	
    uart_init(115200);
    
    // create queue, can store 5 value which data type is long
    xQueue = xQueueCreate(5, sizeof(long));
    
    if(xQueue != NULL)  // adjust the return value, to confirm whether create queue successful.
    {
        // Create two write queue task, priority = 1;
        xTaskCreate(vSendTask, "SendTask1", configMINIMAL_STACK_SIZE, (void *)100, 1, NULL);
        xTaskCreate(vSendTask, "SendTask2", configMINIMAL_STACK_SIZE, (void *)200, 1, NULL);
        
        // create one read queue task, priority = 2;
        xTaskCreate(vReceiveTask, "RecTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        
        // start scheduler now
        vTaskStartScheduler();            
    }
    else
    {
        // queue create unsuccessful here. add your code.
    }
    
    return 0;
}



