#ifndef _KEYNONBLOCK_H_
#define _KEYNONBLOCK_H_


#include "key.h"



#define     KEY0_IN     KEY0
#define     KEY1_IN     KEY1
#define     KEY2_IN     WK_UP

#define     KEY0_CODE   (char)0x80
#define     KEY1_CODE   (char)0x81
#define     KEY2_CODE   (char)0x82


#define KEYBUFFSIZE     4



void keyScanIO(void);
char key_readBuff(void);
void key_paraInit(void);


#endif

