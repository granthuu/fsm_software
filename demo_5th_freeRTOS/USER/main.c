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
        vTaskDelay(500/portTICK_RATE_MS);        // 延时多少个心跳骤起  延时500个毫秒
    }
}


void greenLed_Task(void *pvParameters)
{
    static u8 state = 0;
    pvParameters = pvParameters;
    
    while(1)
    {
        (state = !state) == 1 ? (LED1 = 0) : (LED1 = 1);
        vTaskDelay(1000/portTICK_RATE_MS);        // 延时多少个心跳骤起  延时1000个毫秒
    }
}


int main(void)
{
    //delay_init();	    // 延时函数初始化	  
    LED_Init();		  	// 初始化与LED连接的硬件接口

    //下面为创建任务
    xTaskCreate( redLed_Task,   "LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);      
    xTaskCreate( greenLed_Task, "LED2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );

    vTaskStartScheduler();            // 启动调度器，任务开始执行 

    return 0;

}



