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
            /*���buffer���������Ҵ�ʱ�ܵ����ݣ���ӵ�ǰ��ָ�����һ�����ݣ���������*/
            rbuf_pull ( &rec_buf, &tmp_remove );
        }

        /* ���µ����ݼ��뵽buffer�� */
        rbuf_push ( &rec_buf, (uint8_t *)&receivedChar );     
        
        
        // ����¼���֪ͨ�����򣬷����жϽ����¼���
        // �����첽����
        EventQueue_Enqueue_ISR(&charRecEvent);  /* �ж��¼����� */
    }    
}


/* �����ж��¼������� */
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
    /* ���ѭ����ӡ���� Software_timer ������ */
    // timerʱ�䵽��ʱ��֪ͨ�¼�������
    Timer_StartPeriodic(&printTask_timer,  HAL_READ_TRH_PERIOD,  printTask_handler);
    printf("software time initialize \n");
}



