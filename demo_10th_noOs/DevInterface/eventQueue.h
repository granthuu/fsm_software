#ifndef EVENTQUEUE_H
#define	EVENTQUEUE_H

//#include "Common.h"
#include "stm32f10x.h"

// ------------------------------------------------------------------
// grant add here. to avoid compile warning.

//#define Common_AtomicSection_Begin()    __disable_irq()
//#define Common_AtomicSection_End()      __enable_irq()
#define Common_AtomicSection_Begin()    
#define Common_AtomicSection_End()      


#ifndef NULL
#define NULL 0
#endif

#ifndef bool

#define bool bool_type

#endif



//----------------------------------------------------------------------
// copy from source code.

#define EVENT_INIT(func)      {func, false, NULL, NULL}

typedef void (*EventExecutePtr)(void* data);

typedef struct EventQueue_Event
{
    EventExecutePtr execute;
    volatile bool pending;
    void* volatile data;
    struct EventQueue_Event* volatile next;
} event_t;

void EventQueue_Reset(void);
event_t*  EventQueue_GetPendingEvent(void);
void EventQueue_Enqueue(event_t* event);
void EventQueue_Enqueue_ISR(event_t* event);
void EventQueue_EnqueueWithData(event_t* event, void* data);
uint8_t EventQueue_NrOfPendingEvents(void);

#endif	/* EVENTQUEUE_H */


