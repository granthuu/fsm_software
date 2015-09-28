#ifndef _PRINTF_STDARG_H
#define _PRINTF_STDARG_H


#include "usart.h"


int printf(const char *format, ...);
int sprintf(char *out, const char *format, ...);
int snprintf( char *buf, unsigned int count, const char *format, ... );





#endif

