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
    
    "String 0 \r\n",
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
    TIM3_Int_Init(10,7200);// 10kHZ, 1ms interrupt.
    
    // interrupt initialize
	NVIC_Configuration();//    
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


int main(void)
{
    // board initialize. 
    board_Init();
    printf("board initialize finish. \r\n");
    
    // create two queues
    xIntegerQueue = xQueueCreate(10, sizeof(unsigned long));
    xStringQueue  = xQueueCreate(10, sizeof(char *));

    // �ж϶����Ƿ񴴽��ɹ�
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

    return 0;
}


void TIM3_IRQHandler(void)   //TIM3�ж�, 1ms
{
    static char state = 0;
    static int cnt = 0;
    unsigned long value;
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
        
        if(++cnt >= 100) // per 100ms
        {
            cnt = 0;
            
            // ��ȡqueue��ֱ����ȡ����Ϊ�ա�
            while(xQueueReceiveFromISR(xIntegerQueue, &value, &xHigherPriorityTaskWoken) != errQUEUE_EMPTY)
            {
                printf("read value: %ld \r\n", value);
                value &= 0x03;
                
                // �����ַ������С�
                xQueueSendToBackFromISR(xStringQueue, &pcString[value], &xHigherPriorityTaskWoken);
            }
                  
            ((state = !state) == 1) ? RED_ON() : RED_OFF(); 
        }   
    }
}

