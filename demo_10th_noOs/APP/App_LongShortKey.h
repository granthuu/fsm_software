#ifndef _APP_LONGSHORT_KEY_H_
#define _APP_LONGSHORT_KEY_H_

#include "key.h"


#define KEY_NO_PRESS        0
#define KEY_SHORT_PRESS     1
#define KEY_LONG_PRESS      2

#define KEY0_CODE           (char)0x01


#define KEY_SHORT_CODE      (char)0x10
#define KEY_FIRST_LONG_CODE (char)0x80
#define KEY_AFTER_LONG_CODE (char)0xc0

#define KEYBUFFSIZE     4

void key_paraInit(void);
//void keyScan(void);
char key_readBuff(void);
void keyScanTask_init(void);


#endif

