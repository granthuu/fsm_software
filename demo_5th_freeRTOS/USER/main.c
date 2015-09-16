#include "led.h"
#include "delay.h"
#include "sys.h"

// FreeRTOS head file, add here.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"


void redLed_Task(void *pvParameters)
{
    static u8 state = 0;
    pvParameters = pvParameters;
    
    while(1)
    {
        (state = !state) == 1 ? (LED0 = 0) : (LED0 = 1);
        vTaskDelay(500/portTICK_RATE_MS);        // ��ʱ���ٸ���������  ��ʱ500������
    }
}


void greenLed_Task(void *pvParameters)
{
    static u8 state = 0;
    pvParameters = pvParameters;
    
    while(1)
    {
        (state = !state) == 1 ? (LED1 = 0) : (LED1 = 1);
        vTaskDelay(1000/portTICK_RATE_MS);        // ��ʱ���ٸ���������  ��ʱ1000������
    }
}


int main(void)
{
    //delay_init();	    // ��ʱ������ʼ��	  
    LED_Init();		  	// ��ʼ����LED���ӵ�Ӳ���ӿ�

    //����Ϊ��������
    xTaskCreate( redLed_Task,   "LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);      
    xTaskCreate( greenLed_Task, "LED2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );

    vTaskStartScheduler();            // ����������������ʼִ�� 

    return 0;

}



