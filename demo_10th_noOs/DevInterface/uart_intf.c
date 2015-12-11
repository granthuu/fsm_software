#include "eventQueue.h"
#include "uart_intf.h"
#include <stdio.h>
#include "usart.h"

#include "Timer_intf.h"
#include "ring_buffer.h"


uint8_t receivedChar;
static void OnCharRecEvent(void* data);
static event_t charRecEvent = EVENT_INIT(OnCharRecEvent);

// ---------------------------------------------------------------------
static str_rbuf rec_buf;
static uint8_t receive_buffer[100];


void uart_ringBuffer_Init(void)
{
    rbuf_init(&rec_buf, receive_buffer, 1, 100);  
}



//------------------------------------------------------------------------
void USART1_IRQHandler(void)  
{ 
    uint8_t tmp_remove = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
    {
        receivedChar = USART_ReceiveData(USART1);          

        if ( rbuf_nr_of_items ( &rec_buf ) == rec_buf.max_entries)
        {
            /*如果buffer已满，并且此时受到数据，则从当前读指针读出一个数据，抛弃掉。*/
            rbuf_pull ( &rec_buf, &tmp_remove );
        }

        /* 将新的数据加入到buffer中 */
        rbuf_push ( &rec_buf, (uint8_t *)&receivedChar );     
        
        
        // 添加事件，通知主程序，发生中断接收事件。
        // 进行异步处理。
        EventQueue_Enqueue_ISR(&charRecEvent);  /* 中断事件发生 */
    }    
}


/* 接收中断事件处理函数 */
static void OnCharRecEvent(void* data)
{  
    static uint8_t  a_tmp = 0;
    
    while (rbuf_nr_of_items ( &rec_buf ) != 0)
    {
        rbuf_pull(&rec_buf, &a_tmp);
        uart_sendChar(a_tmp);
        
        DI_CallbackISR_ByteReceived(a_tmp);
    }
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



