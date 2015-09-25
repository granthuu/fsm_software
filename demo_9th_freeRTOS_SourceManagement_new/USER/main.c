#include "led.h"
#include "key.h"
#include "exti.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"

// FreeRTOS head file, add here.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"


xQueueHandle xIntegerQueue;
xQueueHandle xStringQueue;

static const char *pcString[] ={
    
    "String 0, demo for test now \r\n",
    "String 1 \r\n",
    "String 2 \r\n",
    "String 3 \r\n",
};


void board_Init(void)
{
    LED_Init();	
    KEY_Init();  
    //EXTIX_Init();
    uart_init(115200);    
    //TIM3_Int_Init(10,7200);// 10kHZ, 1ms interrupt.
    
    // interrupt initialize
	NVIC_Configuration();   // bug fixed. by modify priority in FreeRTOSConfig.h file. 
}


void keyScan_Task(void *pvParameters)
{
    char key = 0x00;
    char i =0, sendInt = 0;
    
    while(1)
    {
        // add your key scan code here.
        keyScan();
        if((key = keyScan_readBuff()) != 0)
        { 
            switch(key)
            {
                case ( KEY_CODE + SHORT_KEY):
                    
                    printf("send Int: ");
                    for(i=0; i<5; i++)
                    {
                        printf("%d ", sendInt);
                        xQueueSendToBack(xIntegerQueue, &sendInt, 0);     // send integer: 1, 2, 3, 4, 5;  
                        
                        sendInt ++;
                    }    
                    printf("\r\n");
                
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


void PrintTask(void *pvParameters)
{
    char *pcStr;
    
    while(1)
    {
        xQueueReceive(xStringQueue, &pcStr, portMAX_DELAY);
        printf("%s", pcStr);
    }
}

SemaphoreHandle_t xMutex;

void PrintSring_Task(void *pvParameters)
{
    char *pcStr = (char *)pvParameters;
    
    while(1)
    {
        // 1. enter critical section.
        xSemaphoreTake(xMutex, portMAX_DELAY);
        
        printf("%s", (char *)pcStr);
        
        // 2. exit critical section.
        xSemaphoreGive(xMutex);
        
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

const char * printStr1 = "this is print string task 1.-------------------------------------------------\r\n";
const char * printStr2 = "this is print string task 2.*************************************************\r\n";

int main(void)
{
    // board initialize. 
    board_Init();
    printf("board initialize finish. \r\n");

    
#if 1

    xMutex = xSemaphoreCreateMutex();
    if( xMutex != NULL )
    {
        // The semaphore was created successfully.
        // The semaphore can now be used.
        xTaskCreate(PrintSring_Task, "printStringTask1", configMINIMAL_STACK_SIZE, (void *)printStr1, 1, NULL);
        xTaskCreate(PrintSring_Task, "printStringTask2", configMINIMAL_STACK_SIZE, (void *)printStr2, 1, NULL);
        
        // start scheduler now
        vTaskStartScheduler();            
    }  
  
#else
    
    // create two queues
    xIntegerQueue = xQueueCreate(10, sizeof(unsigned long));
    xStringQueue  = xQueueCreate(10, sizeof(char *));

    // 判断队列是否创建成功
    if((xIntegerQueue != NULL) && (xStringQueue != NULL))      
    {
        //xTaskCreate(Key_HanderFun,    "keyHandlerTask",  configMINIMAL_STACK_SIZE, NULL,   2, NULL);
        xTaskCreate(keyScan_Task, "keyScanTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(LED_task,     "LED_task",    configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(PrintTask,    "print_task",  configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        
        // start scheduler now
        vTaskStartScheduler();            
    }
    else
    {
        // semaphore create unsuccessful here. add your code.
        printf("semaphore create failed \r\n");
    }
#endif
    
    return 0;
}


void TIM3_IRQHandler(void)   //TIM3中断, 1ms
{
    static char state = 0;
    static int cnt = 0;
    unsigned long value;
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
        
        if(++cnt >= 100) // per 100ms
        {
            cnt = 0;
            
            // 读取queue，直到读取队列为空。
            while(xQueueReceiveFromISR(xIntegerQueue, &value, &xHigherPriorityTaskWoken) != errQUEUE_EMPTY)
            {
                printf("read value: %ld \r\n", value);
                value &= 0x03;
                
                // 发送字符串队列。
                xQueueSendToBackFromISR(xStringQueue, &pcString[value], &xHigherPriorityTaskWoken);
            }
                  
            ((state = !state) == 1) ? RED_ON() : RED_OFF(); 
        }   
    }
}

