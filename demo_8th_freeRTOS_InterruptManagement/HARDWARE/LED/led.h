#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


#define RED_ON()        (LED0 = 0)
#define RED_OFF()       (LED0 = 1)
#define GREEN_ON()      (LED1 = 0)
#define GREEN_OFF()     (LED1 = 1)


#define LED0 PAout(8)	// PA8  RED
#define LED1 PDout(2)	// PD2	GREEN

void LED_Init(void);//��ʼ��

		 				    
#endif