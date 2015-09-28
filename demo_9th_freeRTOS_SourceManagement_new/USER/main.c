#include "led.h"
#include "key.h"
#include "exti.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "printf-stdarg.h"

// FreeRTOS head file, add here.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"



xQueueHandle xPrintQueue;

static const char *pcStringToPrint[] = {

    "this is print string task 1.-------------------------------------------------\r\n",
    "this is print string task 2.*************************************************\r\n",
    "this is print string task 3.*************************************************\r\n",
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
                        //xQueueSendToBack(xIntegerQueue, &sendInt, 0);     // send integer: 1, 2, 3, 4, 5;  
                        
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



void Keeper_Task(void *pvParameters)
{
    char *pcStr;
    
    while(1)
    {
        /* Wait to receive message from queue, the task will blocking until data coming into queue. */
        xQueueReceive(xPrintQueue, &pcStr, portMAX_DELAY);
        
        printf("%s", pcStr);   
    }
}


void PrintSring_Task(void *pvParameters)
{
    int iIndexToString;
    
    iIndexToString = (int)pvParameters;
    
    while(1)
    {
        /* send pointer data to queue, if full, then return */
        xQueueSendToBack(xPrintQueue, &(pcStringToPrint[iIndexToString]), 0);
        //print_test();
        
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

/* task for test printf function, see printf-stdarg.c file. */
extern void print_test(void);
void printf_test_task(void *pvParameters)
{
    while(1)
    {
        print_test();
        
        vTaskDelay(1000/portTICK_RATE_MS);
    }  
}


int main(void)
{
    // board initialize. 
    board_Init();
    printf("board initialize finish. \r\n");

    // create queues
    xPrintQueue = xQueueCreate(5, sizeof(char *));
    
    // 判断队列是否创建成功
    if(xPrintQueue != NULL)
    {
        xTaskCreate(PrintSring_Task, "PrintSring_Task1", configMINIMAL_STACK_SIZE, (void *)0, 1, NULL);
        xTaskCreate(PrintSring_Task, "PrintSring_Task2", configMINIMAL_STACK_SIZE, (void *)1, 2, NULL);
        
        /* create keeper task here.*/
        xTaskCreate(Keeper_Task, "PrintSring_Task2", configMINIMAL_STACK_SIZE, NULL, 0, NULL);   
        
        
        /* task for test printf function. */
        xTaskCreate(printf_test_task, "printf_test_task", configMINIMAL_STACK_SIZE, NULL, 3, NULL);  
        
        // start scheduler now
        vTaskStartScheduler();            
    }
    else
    {
        // semaphore create unsuccessful here. add your code.
        printf("semaphore create failed \r\n");
    }
    
    return 0;
}


void vApplicationTickHook(void)
{
    static int iCount = 0;
    BaseType_t xHigherPrioritTaskWoken;

	// We have not woken a task at the start of the ISR.
	xHigherPrioritTaskWoken = pdFALSE; 
    
    iCount ++;
    
    /* systick interrupt = 10ms. hook interrupt time = 10ms * 100 = 1s */
    if(iCount >= 100)
    {
        xQueueSendToFrontFromISR(xPrintQueue, &(pcStringToPrint[2]), &xHigherPrioritTaskWoken);
        
        iCount = 0;
    }
}



void TIM3_IRQHandler(void)   //TIM3中断, 1ms
{
    static char state = 0;
    static int cnt = 0;
//    unsigned long value;
    
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
        
        if(++cnt >= 100) // per 100ms
        {
            cnt = 0;
      
            ((state = !state) == 1) ? RED_ON() : RED_OFF(); 
        }   
    }
}

