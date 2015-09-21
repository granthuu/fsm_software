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



//按键初始化函数 
//PA15和PC5 设置成输入
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTC时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//PA15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC5
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
} 





