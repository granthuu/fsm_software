#include "EventQueue.h"

//Private functions

static struct
{
    event_t* volatile head;
    event_t* volatile tail;
    uint8_t volatile nrOfPending;
} eventQueue = {NULL, NULL, 0};

//Public functions
void EventQueue_Reset(void)
{
    eventQueue.head = NULL;
    eventQueue.tail = NULL;
    eventQueue.nrOfPending = 0;
}

event_t* EventQueue_GetPendingEvent(void)
{
    event_t* e;

    Common_AtomicSection_Begin();

    if(eventQueue.head == NULL)
    {
        Common_AtomicSection_End();
        return NULL;
    }

    e = eventQueue.head;
    eventQueue.head = eventQueue.head->next;

    e->pending = false;
    e->next = NULL;
    eventQueue.nrOfPending--;

    Common_AtomicSection_End();
    return e;
}

static void EventQueue_EnqueueInternal(event_t* event)
{
    if(!event->pending)
    {
        event->pending = true;
        event->next = NULL;
        eventQueue.nrOfPending++;

        if (eventQueue.head == NULL)
        {
            eventQueue.head = event;
            eventQueue.tail = event;
        }
        else
        {
            eventQueue.tail->next = event;
            eventQueue.tail = event;
        }
    }
}

void EventQueue_Enqueue(event_t* event)
{
    Common_AtomicSection_Begin();
    event->data = NULL;
    EventQueue_EnqueueInternal(event);
    Common_AtomicSection_End();
}

void EventQueue_Enqueue_ISR(event_t* event)
{
    Common_AtomicSection_Begin();
    event->data = NULL;
    EventQueue_EnqueueInternal(event);
    Common_AtomicSection_End();
}

void EventQueue_EnqueueWithData(event_t* event, void* data)
{
    Common_AtomicSection_Begin();
    event->data = data;
    EventQueue_EnqueueInternal(event);
    Common_AtomicSection_End();
}

uint8_t EventQueue_NrOfPendingEvents(void)
{
    return eventQueue.nrOfPending;
}
