#include "App_KeyNonblock.h"


#define KEY_RELEASE 1
#define KEY_PRESS   0




/**
 *
 * \brief: 按键状态值，用来保存上一次和当前的按键状态值，利用这两个按键状态值，来判定按键是否按下。 
 *         按键按下，为低电平
 *
 */
char p_key0_status = 0xff;
char n_key0_status = 0xff;
char p_key1_status = 0xff;
char n_key1_status = 0xff;
char p_key2_status = 0xff;
char n_key2_status = 0xff;


void key_inBuff(char keyValue);

/**
 * \brief: 在10ms中断里面运行，可以除掉按键抖动。
 *
 */
void keyScanIO(void)
{
    p_key0_status = n_key0_status;
    n_key0_status = KEY0_IN;
    p_key1_status = n_key1_status;
    n_key1_status = KEY1_IN;   
    p_key2_status = n_key2_status;
    n_key2_status = KEY2_IN;    
    
    if((p_key0_status != KEY_PRESS) && (n_key0_status == KEY_PRESS)){
        key_inBuff(KEY0_CODE);
    }
    
    if((p_key1_status != KEY_PRESS) && (n_key1_status == KEY_PRESS)){
        key_inBuff(KEY1_CODE);
    }  
    
    if((p_key2_status != KEY_PRESS) && (n_key2_status == KEY_PRESS)){
        key_inBuff(KEY2_CODE);
    }  
}




//#define KEYBUFFSIZE     4
static char keyBuff[KEYBUFFSIZE] = {0};
char key_indexW = 0;
char key_indexR = 0;
char key_count = 0;


#define DISABLE_INT() Common_AtomicSection_Begin() 
#define ENABLE_INT()  __enable_irq() 


void key_paraInit(void)
{
    char i = 0;
    
    for(i =0; i< KEYBUFFSIZE; i++)
        keyBuff[i] = 0;
    
    key_indexW = 0;
    key_indexR = 0;
    key_count = 0;
}

/**
 * \brief: 保存按键值到FIFO中。
 *
 */
void key_inBuff(char keyValue)
{
    if(key_count >= KEYBUFFSIZE)
        return;

    DISABLE_INT();
    
    key_count ++;
    keyBuff[key_indexW] = keyValue;
    
    if(++key_indexW >= KEYBUFFSIZE)
        key_indexW = 0;
    
    ENABLE_INT();
}

char key_readBuff(void)
{
    char key =0;
    
    if(key_count == 0)  /* 没有按键按下 */
        return 0;  
    
    DISABLE_INT(); 
    
    key_count --;
    
    key = keyBuff[key_indexR];
    if(++key_indexR >= KEYBUFFSIZE)
        key_indexR = 0;
    
    ENABLE_INT();
    
    return key;
}



