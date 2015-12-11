#include "App_LongShortKey.h"
#include <stdio.h>
#include "Timer_intf.h"


#define KEY_EVENT_QUEUE_DEBUG

#ifdef KEY_EVENT_QUEUE_DEBUG
#include "eventQueue.h"

static void keyScanEvent(void *data);
static event_t keyPressEvent = EVENT_INIT(keyScanEvent);
#endif


#define KEY0_INPUT  KEY1

static char key0_Status = 0;
static unsigned int key0_TimeS, key0_TimeL;


/**
 * \brief: ��FIFO��صı�������
 */
//#define KEYBUFFSIZE     4
char keyBuff[KEYBUFFSIZE];
char key_indexW = 0;
char key_indexR = 0;
char key_count = 0;

#define ENA_INT() __enable_irq()
#define DIS_INT() __disable_irq()
//#define ENA_INT() 
//#define DIS_INT() 

/**
 * \brief: ��FIFO��ز����ĺ����Ķ���
 */
 
void key_paraInit(void)
{
    char i = 0;
    
    for(i =0; i< KEYBUFFSIZE; i++)
        keyBuff[i] = 0;
    
    key_indexW = 0;
    key_indexR = 0;
    key_count = 0;
}
 
void key_inBuff(char keyValue)
{
    if(key_count >= KEYBUFFSIZE)
        return;
    
    DIS_INT();
    
    key_count ++;
    
    keyBuff[key_indexW] = keyValue;
    
    if(++ key_indexW == KEYBUFFSIZE)
        key_indexW = 0;
    
    ENA_INT();
}

char key_readBuff(void)
{
    char key = 0;

    if(key_count == 0)
        return 0;
    
    DIS_INT(); 
    
    key_count --;
    
    key = keyBuff[key_indexR];
    if( ++key_indexR == KEYBUFFSIZE)
        key_indexR = 0;
    
    ENA_INT(); 
    return key;
}

/**
 * \brief: 
 */
uint8_t keyCodeBuff[1];

void keyScan(void *data)
{

    
    if(key0_Status == KEY_SHORT_PRESS)
        key0_TimeS ++;
    else
        key0_TimeS = 0;
    
    if(key0_Status == KEY_LONG_PRESS)
        key0_TimeL ++;
    else
        key0_TimeL = 0;   
    
    switch(key0_Status)
    {
        case KEY_NO_PRESS: 
            if(KEY0_INPUT == 0)
                key0_Status = KEY_SHORT_PRESS;
        break;
            
        case KEY_SHORT_PRESS:
            
            if(KEY0_INPUT != 0)  // ��ʾ�����ɿ���
            {
                // ���水��
                //key_inBuff(KEY_SHORT_CODE + KEY0_CODE);
                
#ifdef KEY_EVENT_QUEUE_DEBUG

                keyCodeBuff[0] = KEY_SHORT_CODE + KEY0_CODE;
                EventQueue_EnqueueWithData(&keyPressEvent, keyCodeBuff);
#endif             
                
                // ���ص�����δ����ģʽ
                key0_Status = KEY_NO_PRESS;
            }
            else
            {
                if(key0_TimeS >= 2000/10) // 2s detect
                {
                    //key_inBuff(KEY_FIRST_LONG_CODE + KEY0_CODE); 
#ifdef KEY_EVENT_QUEUE_DEBUG
                    
                   keyCodeBuff[0] = KEY_FIRST_LONG_CODE + KEY0_CODE;
                   EventQueue_EnqueueWithData(&keyPressEvent, keyCodeBuff);
#endif        

                    
                    key0_Status = KEY_LONG_PRESS;
                }    
            }
        break;
            
        case KEY_LONG_PRESS:
            
            if(KEY0_INPUT != 0)  // ��ʾ�����ɿ���
            {            
                // ���ص�����δ����ģʽ
                key0_Status = KEY_NO_PRESS;
            }
            else
            {
                if(key0_TimeL >= 100/10)  // 250ms
                {
                    //key_inBuff(KEY_AFTER_LONG_CODE + KEY0_CODE); 
#ifdef KEY_EVENT_QUEUE_DEBUG
                                        
                    keyCodeBuff[0] = KEY_AFTER_LONG_CODE + KEY0_CODE;
                    EventQueue_EnqueueWithData(&keyPressEvent, keyCodeBuff);
#endif        

                    key0_TimeL = 0;
                }    
            }      
        break;
    }
}


void keyScanEvent(void *data)
{
    uint8_t *keyCode = (uint8_t *)data;
    uint8_t key = *keyCode;
    
    switch(key){
        
        case (KEY0_CODE + KEY_SHORT_CODE):                              
            printf("key1 short press \r\n");
        break;
        
        case (KEY0_CODE + KEY_FIRST_LONG_CODE):
            printf("key1 long press \r\n");
        break;   
        
        case (KEY0_CODE + KEY_AFTER_LONG_CODE):
            printf("key1 after long press \r\n");
        break;
        
        default:
            break;
        
    }
}



#define KEY_SCAN_PERIOD_MS     10
static softtimer_t  keyScanTask_timer = TIMER_INIT;


void keyScanTask_init(void)
{
    /* ���ѭ����ӡ���� Software_timer ������ */
    // timerʱ�䵽��ʱ��֪ͨ�¼�������
    Timer_StartPeriodic(&keyScanTask_timer,  KEY_SCAN_PERIOD_MS,  keyScan);
    printf("software time initialize \n");
}




