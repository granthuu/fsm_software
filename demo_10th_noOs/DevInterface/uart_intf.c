#include "eventQueue.h"
#include "uart_intf.h"
#include <stdio.h>
#include "usart.h"

uint8_t receivedChar;
static void OnCharRecEvent(void* data);
static event_t charRecEvent = EVENT_INIT(OnCharRecEvent);


/* 接收中断事件处理函数 */
static void OnCharRecEvent(void* data)
{
    static uint8_t a_tmp = 0;

    a_tmp = receivedChar;
    uart_sendChar(a_tmp);
    
    // 真正的处理函数: ccallback function. 
    DI_CallbackISR_ByteReceived(a_tmp);
}


void DI_CallbackISR_ByteReceived(uint8_t rxByte)
{
    // state machine here.
}


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
