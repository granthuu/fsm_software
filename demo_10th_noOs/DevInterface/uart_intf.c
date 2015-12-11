#include "eventQueue.h"
#include "uart_intf.h"
#include <stdio.h>
#include "usart.h"

#include "Timer_intf.h"

uint8_t receivedChar;
static void OnCharRecEvent(void* data);
static event_t charRecEvent = EVENT_INIT(OnCharRecEvent);



void USART1_IRQHandler(void)  
{ 
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
    {
        receivedChar = USART_ReceiveData(USART1);          
        
        // 添加事件，通知主程序，发生中断接收事件。
        // 进行异步处理。
        EventQueue_Enqueue_ISR(&charRecEvent);  /* 中断事件发生 */
    }    
}


/* 接收中断事件处理函数 */
static void OnCharRecEvent(void* data)
{  
    static uint8_t a_tmp = 0;

    a_tmp = receivedChar;
    uart_sendChar(a_tmp);

    // 真正的处理函数: callback function. 
    DI_CallbackISR_ByteReceived(a_tmp);
}


void DI_CallbackISR_ByteReceived(uint8_t rxByte)
{
    // state machine here.
}


//-----------------------------------------------------------------------------------
// add print_task

static softtimer_t  printTask_timer = TIMER_INIT;
#define HAL_READ_TRH_PERIOD     1000

void printTask_handler(void* data)
{
    printf("printTask_handler \n");
}


void printTask_init(void)
{
    /* 添加循环打印任务到 Software_timer 链表中 */
    // timer时间到的时候，通知事件发生。
    Timer_StartPeriodic(&printTask_timer,  HAL_READ_TRH_PERIOD,  printTask_handler);
    printf("software time initialize \n");
}



