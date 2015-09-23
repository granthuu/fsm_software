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


/* declare a SemaphoreHandle_t variable, using to save queue handler. */
SemaphoreHandle_t xBinarySemaphore = NULL;

SemaphoreHandle_t xCountingSemaphore;


void board_Init(void)
{
    LED_Init();	
    KEY_Init();  
    //EXTIX_Init();
    uart_init(115200);    
    TIM3_Int_Init(10,7200);// 10kHZ, 1ms interrupt.
    
    // interrupt initialize
	NVIC_Configuration();//    
}


void keyScan_Task(void *pvParameters)
{
    char key = 0x00;
    int keyCnt =0;
    
    while(1)
    {
        // add your key scan code here.
        keyScan();
        if((key = keyScan_readBuff()) != 0)
        { 
            switch(key)
            {
                case ( KEY_CODE + SHORT_KEY):
                    printf("short key pressed, cnt: %d\r\n", ++keyCnt);
                
                    //xSemaphoreGive(xBinarySemaphore);
                
                    xSemaphoreGive(xCountingSemaphore);
                    printf("key pressed, give semaphore \r\n");  
                
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


void Key_HanderFun(void *pvParameters)
{
    //char state = 0;
    
    while(1)
    {
        //xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
        xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
        printf("take semaphore here \r\n");
        
        //(state =!state) == 1 ? RED_ON() : RED_OFF();
    }
}



int main(void)
{
    // board initialize. 
    board_Init();
    printf("board initialize finish. \r\n");
    
    // create binary semaphore
    //vSemaphoreCreateBinary(xBinarySemaphore);
    
    /* 在使用信号量之前先创建。本例中创建了一个计数信号量，最大计数值为10，初始计数值为0 */
    xCountingSemaphore = xSemaphoreCreateCounting(10, 0);
    
    if(xCountingSemaphore != NULL)  
    {
        xTaskCreate(Key_HanderFun,    "keyHandlerTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
        xTaskCreate(keyScan_Task,     "keyScanTask",     configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xTaskCreate(LED_task,         "LED_task",        configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        //xSemaphoreTake(xBinarySemaphore, 0);
        
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


void TIM3_IRQHandler(void)   //TIM3中断, 1ms
{
    static char state = 0;
    static int cnt = 0;
    static int sempCnt = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
        
        if(++cnt >= 1000)
        {
            cnt = 0;
            
             xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
             printf("give semaphore, sempCnt: %d \r\n", ++sempCnt);
            ((state = !state) == 1) ? RED_ON() : RED_OFF(); 
        }   
    }
}


void EXTI0_IRQHandler(void)
{   
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    
	if(WK_UP == 1)
	{	  
        // Unblock the task by releasing the semaphore.
        xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
        printf("key pressed, give semaphore \r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  
}

