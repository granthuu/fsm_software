#include "App_KeyNonblock.h"


#define KEY_RELEASE 1
#define KEY_PRESS   0




/**
 *
 * \brief: ����״ֵ̬������������һ�κ͵�ǰ�İ���״ֵ̬����������������״ֵ̬�����ж������Ƿ��¡� 
 *         �������£�Ϊ�͵�ƽ
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
 * \brief: ��10ms�ж��������У����Գ�������������
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
 * \brief: ���水��ֵ��FIFO�С�
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
    
    if(key_count == 0)  /* û�а������� */
        return 0;  
    
    DISABLE_INT(); 
    
    key_count --;
    
    key = keyBuff[key_indexR];
    if(++key_indexR >= KEYBUFFSIZE)
        key_indexR = 0;
    
    ENABLE_INT();
    
    return key;
}



