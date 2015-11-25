
#ifndef TIMERS_INTF_H
#define	TIMERS_INTF_H

#include "stm32f10x.h"

#ifndef bool
#define bool bool_type
#endif

#define UNUSED_PARAMETER(x)    ((void)(x))


#define TIMER_MILLISECONDS_PER_TICK 10

#define TIMER_INIT { /*isRunning*/false, /*currentTicksRemaining*/0, /*timeOutCallback*/NULL, /*periodInTicks*/0, /*next*/NULL }
#define TIMER_DYNINIT(x) { (x).isRunning = false; (x).next=NULL; }


typedef void (*TimeOutCallbackPtr)(void* data);

/* A timer MUST be initialized with TIMER_INIT or TIMER_DYNINIT */
/* A timer MUST not not be a stack (auto) variable! */
typedef struct softtimer_t
{
    bool isRunning;
    uint32_t currentTicksRemaining;

    TimeOutCallbackPtr timeOutCallback;
    uint32_t periodInTicks;	// non-zero for periodic timer

    struct softtimer_t* next;
} softtimer_t;

void Timer_Initialize(void);

void Timer_StartSingleShot(softtimer_t* timer, uint32_t periodInMilliseconds, TimeOutCallbackPtr callback);
void Timer_StartPeriodic(softtimer_t* timer, uint32_t periodInMilliseconds, TimeOutCallbackPtr callback);
void Timer_StopTimer(softtimer_t* timer);
uint32_t Timer_GetRemainingMs(softtimer_t* timer);
void Callback_Timer_TickISR(void);

#endif	/* TIMERS_H */
