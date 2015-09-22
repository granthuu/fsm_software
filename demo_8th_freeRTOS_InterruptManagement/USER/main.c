#include "led.h"
#include "key.h"
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


void vReceiveTask(void *pvParameters)
{
    char key ;
    
    portBASE_TYPE status;

    while(1)
    {        
        if( uxQueueMessagesWaiting( xQueue ) != 0 )
        {
            /* 由于该优先级高，则导致queue队列一直为空，使得这句话不会执行。*/
            printf( "Queue should have been empty! \r\n");
        }
        
        /* 1. 设置成： 100/portTICK_RATE_MS，表示超时等待时间，当超过这个时间还没有数据到来，同样返回。*/
        //status = xQueueReceive( xQueue, &key, 100 / portTICK_RATE_MS );
        
        /**
         * \brief: 2. 设置成portMAX_DELAY，将永久等待，直到queue有数据可以读取
         */
        status = xQueueReceive( xQueue, &key, portMAX_DELAY );
        if( status == pdPASS )
        {
            printf("Queue received Key pressed msg, key value: %d \r\n", key);
        }
        else
        {
            printf( "Could not receive from the queue.\r\n" );
        }
    }
}

void board_Init(void)
{
    LED_Init();	
    KEY_Init();    
    uart_init(115200);    
}


void keyScan_Task(void *pvParameters)
{
    char key = 0x00;
    portBASE_TYPE status;

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
                break;
                
                case ( KEY_CODE+FIRSTLONG_KEY_CODE):
                    printf("long first pressed \r\n");
                break;
                
                case ( KEY_CODE+AFTERLONG_KEY_CODE):
                    printf("long after pressed \r\n");
                break;
            }
            
            status = xQueueSendToBack(xQueue, &key, 0);
            if(status != pdPASS)
            {
                printf("could not send to the queue. \r\n");
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
        ((state = !state) == 1) ? RED_ON() : RED_OFF();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}



int main(void)
{
    // board initialize. 
    board_Init();
    
    // create queue, can store 3 value which data type is data_type
    xQueue = xQueueCreate(3, sizeof(char));
    
    if(xQueue != NULL)  // adjust the return value, to confirm whether create queue successful.
    {
        // create one read queue task, priority = 1;
        xTaskCreate(vReceiveTask, "RecTask",     configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(keyScan_Task, "KeyScanTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(LED_task,     "LEDTask",     configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        
        // start scheduler now
        vTaskStartScheduler();            
    }
    else
    {
        // queue create unsuccessful here. add your code.
    }
    
    return 0;
}



