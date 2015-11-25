#ifndef DIUART_H_
#define DIUART_H_

#include "stm32f10x.h"

#ifndef bool
#define bool bool_type
#endif



void diUart_init(void);
void diUart_writeChar(uint8_t buffer);
bool diUart_writeReady(void);
extern void DI_CallbackISR_ByteReceived(uint8_t byte);

void printTask_init(void);

#endif /* DIUART_H_ */

