#ifndef __KEY_H
#define __KEY_H	 


#include "sys.h"

#define KEY0    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//读取按键0
#define KEY1    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)//读取按键1
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键2 


#define KEY0_PRES	1		//KEY0  
#define KEY1_PRES	2		//KEY1 
#define WKUP_PRES	3		//WK_UP  

#define NO_KEY          0
#define SHORT_KEY       1 
#define LONG_KEY        2


#define KEY_CODE             (unsigned char)0x00
#define SHORT_KEY_CODE       (unsigned char)0x01 
#define FIRSTLONG_KEY_CODE   (unsigned char)0x02
#define AFTERLONG_KEY_CODE   (unsigned char)0x03


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数	

void keyScan(void);
char keyScan_readBuff(void);


		 				    
#endif


