#include "App_LongShortKey.h"


#define KEY0_INPUT  KEY0

static char key0_Status = 0;
static unsigned int key0_TimeS, key0_TimeL;


/**
 * \brief: 与FIFO相关的变量定义
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
 * \brief: 与FIFO相关操作的函数的定义
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
void keyScan(void)
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
            
            if(KEY0_INPUT != 0)  // 表示按键松开了
            {
                // 保存按键
                key_inBuff(KEY_SHORT_CODE + KEY0_CODE);
                
                // 返回到按键未按下模式
                key0_Status = KEY_NO_PRESS;
            }
            else
            {
                if(key0_TimeS >= 2000/10) // 2s detect
                {
                    key_inBuff(KEY_FIRST_LONG_CODE + KEY0_CODE); 
                    key0_Status = KEY_LONG_PRESS;
                }    
            }
        break;
            
        case KEY_LONG_PRESS:
            
            if(KEY0_INPUT != 0)  // 表示按键松开了
            {            
                // 返回到按键未按下模式
                key0_Status = KEY_NO_PRESS;
            }
            else
            {
                if(key0_TimeL >= 100/10)  // 250ms
                {
                    key_inBuff(KEY_AFTER_LONG_CODE + KEY0_CODE); 
                    key0_TimeL = 0;
                }    
            }      
        break;
    }
}








