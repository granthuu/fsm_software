#include "key.h"


#define KEYBUFFSIZE 4
char keybuff[KEYBUFFSIZE] = {0};
char key_indexW = 0;
char key_indexR = 0;
char length = 0;
static uint32_t keyCounterL = 0, keyCounterS = 0;
char keyState = 0;

#define KEY_IN() KEY0




extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );



void keyScan_writeBuff(char keyValue)
{
    if(length >= KEYBUFFSIZE )
        return;
    
    vPortEnterCritical();
    
    length ++;
    keybuff[key_indexW] = keyValue;
    if(++key_indexW >= KEYBUFFSIZE)
        key_indexW = 0;
    
    vPortExitCritical();
}


char keyScan_readBuff(void)
{
    char key;
    if(length == 0) return 0;
    
    vPortEnterCritical();
    
    length --;
    key = keybuff[key_indexR];
    if(++key_indexR >= KEYBUFFSIZE)
        key_indexR = 0;
    
    vPortExitCritical();
    
    return key;
}


void keyScan(void)
{
    // scan key and write key code to buffer.
    if(keyState == LONG_KEY) 
        keyCounterL++;
    else
        keyCounterL =0;
    
    if(keyState == SHORT_KEY) 
        keyCounterS++;
    else
        keyCounterS =0;
    
    
    switch(keyState)
    {
        case NO_KEY:
            if(KEY_IN() == 0)   keyState = SHORT_KEY;
            else                keyState = NO_KEY;
        break;
        
        case SHORT_KEY:
            if(KEY_IN() != 0)  // release key, short key
            {
                keyScan_writeBuff(KEY_CODE + SHORT_KEY);
                keyState = NO_KEY;
            }
            else
            {
                if(keyCounterS >=  2000/10)
                {
                    keyCounterS = 0;
                    keyScan_writeBuff(KEY_CODE + FIRSTLONG_KEY_CODE);
                    keyState = LONG_KEY;
                } 
            }
        break;
            
        case LONG_KEY:
            if(KEY_IN() != 0)  // release key, short key
            {
                keyState = NO_KEY;
            }
            else
            {
                if(keyCounterL >=  250/10)
                {
                    keyCounterL = 0;
                    keyScan_writeBuff(KEY_CODE + AFTERLONG_KEY_CODE);
                } 
            }         
        break;
    }
}



//������ʼ������ 
//PA15��PC5 ���ó�����
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTCʱ��

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//PA15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC5
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
} 





