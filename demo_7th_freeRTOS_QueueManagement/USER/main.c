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

#define SENDER_1 (unsigned char )1
#define SENDER_2 (unsigned char )2

typedef struct data_type{

    unsigned char ucValue;
    unsigned char ucSource;
    
}data_type;


static const data_type structDateToSend[2] = {
    {100, SENDER_1},
    {200, SENDER_2},    
};


static char dataToSend_arr1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
static char dataToSend_arr2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};




/* declare a queueHandle variable, using to save queue handler. */
xQueueHandle xQueue;




void vSendTask(void *pvParameters)
{
    data_type * valueToSend;
    portBASE_TYPE status;
    
    valueToSend = (data_type *) pvParameters;
    
    while(1)
    {
        status = xQueueSendToBack(xQueue, valueToSend, 100 / portTICK_RATE_MS);
        if(status != pdPASS)
        {
            printf("could not send to the queue. \r\n");
        }
        
        taskYIELD();
    }
}


void vReceiveTask(void *pvParameters)
{
    data_type lReceivedValue;
    portBASE_TYPE status;

    while(1)
    {        
        if( uxQueueMessagesWaiting( xQueue ) != 3 )
        {
            printf( "Queue should have been full! \r\n");
        }

        status = xQueueReceive( xQueue, &lReceivedValue, 0 );
        if( status == pdPASS )
        {
            if(lReceivedValue.ucSource == SENDER_1)
                printf("from sender1 = %d \r\n", lReceivedValue.ucValue);
            else if(lReceivedValue.ucSource == SENDER_2)
                printf("from sender2 = %d \r\n", lReceivedValue.ucValue);
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
    
    while(1)
    {
        // add your key scan code here.
        keyScan();
        if((key = keyScan_readBuff()) != 0)
        {
            switch(key)
            {
                case (KEY_CODE + SHORT_KEY):
                    printf("short key pressed \r\n");
                break;
                
                case (KEY_CODE+FIRSTLONG_KEY_CODE):
                    printf("long first pressed \r\n");
                break;
                
                case (KEY_CODE+AFTERLONG_KEY_CODE):
                    printf("long after pressed \r\n");
                break;
            }
        }
        
        vTaskDelay(10/portTICK_RATE_MS);
    }
}




int main(void)
{
    // board initialize. 
    board_Init();
    
    // create queue, can store 3 value which data type is data_type
    xQueue = xQueueCreate(3, sizeof(data_type));
    
    if(xQueue != NULL)  // adjust the return value, to confirm whether create queue successful.
    {
        // Create two write queue task, priority = 2;
        //xTaskCreate(vSendTask, "SendTask1", configMINIMAL_STACK_SIZE, (void *)&structDateToSend[0], 2, NULL);
        //xTaskCreate(vSendTask, "SendTask2", configMINIMAL_STACK_SIZE, (void *)&structDateToSend[1], 2, NULL);
        
        // create one read queue task, priority = 1;
        //xTaskCreate(vReceiveTask, "RecTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        
        
        xTaskCreate(keyScan_Task, "KeyScanTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        
        // start scheduler now
        vTaskStartScheduler();            
    }
    else
    {
        // queue create unsuccessful here. add your code.
    }
    
    return 0;
}



