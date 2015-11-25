#include "Timer_intf.h"
#include "eventQueue.h"
#include "stdio.h"

//#include "hal_time.h"


static void OnTimerTickPassed(void);
static void OnTimerTicksEvent(void* data);

static event_t hwTimerExpired = EVENT_INIT(OnTimerTicksEvent);
static volatile uint8_t timerTicks = 0;

static softtimer_t sentinelAtEnd = TIMER_INIT;
static softtimer_t* timerList = &sentinelAtEnd;

// Private functions


static uint32_t Timer_ConvertMillisecondsToTicks(uint32_t periodInMilliseconds)
{
    return (periodInMilliseconds + TIMER_MILLISECONDS_PER_TICK - 1) / TIMER_MILLISECONDS_PER_TICK;
}

static void Timer_StartTimer(softtimer_t* timer, uint32_t periodInMilliseconds, TimeOutCallbackPtr callback)
{
    Timer_StopTimer(timer);
    timer->currentTicksRemaining = Timer_ConvertMillisecondsToTicks(periodInMilliseconds) + (uint32_t)timerTicks; // add backlog
    timer->timeOutCallback = callback;
    timer->isRunning = true;
    if (timer->next == NULL)
    {
        timer->next = timerList;
        timerList = timer;
    }
}

// Public functions

void Timer_Initialize(void)
{
#ifdef UNIT_TESTING
    // to ease unit testing, deconstruct the timer list
    // and reinitialize the timers that were in it.
    // In production code, everything is initialized statically
    // and Init must not be called more then once.
    while(timerList != NULL)
    {
        softtimer_t* tmp = timerList;
        timerList = timerList->next;

        // re-initialize the timer object
        TIMER_DYNINIT((*tmp));
    }
    timerList = &sentinelAtEnd;
    timerTicks = 0;
#endif
}

void Timer_StartSingleShot(softtimer_t* timer, uint32_t periodInMilliseconds, TimeOutCallbackPtr callback)
{
    timer->periodInTicks = 0;
    Timer_StartTimer(timer, periodInMilliseconds, callback);
}

void Timer_StartPeriodic(softtimer_t* timer, uint32_t periodInMilliseconds, TimeOutCallbackPtr callback)
{
    timer->periodInTicks = Timer_ConvertMillisecondsToTicks(periodInMilliseconds);
    Timer_StartTimer(timer, periodInMilliseconds, callback);
}

void Timer_StopTimer(softtimer_t* timer)
{
    timer->isRunning = false;
}

uint32_t Timer_GetRemainingMs(softtimer_t* timer)
{
    return (timer->isRunning ? timer->currentTicksRemaining * TIMER_MILLISECONDS_PER_TICK : 0);
}

void Callback_Timer_TickISR(void)
{
    timerTicks++;
    EventQueue_Enqueue(&hwTimerExpired);
}

static void OnTimerTickPassed(void)
{
    softtimer_t* timer;

    for (timer = timerList; timer != NULL; timer = timer->next)
    {
        if (timer->isRunning )
        {
            timer->currentTicksRemaining--;

            if (timer->currentTicksRemaining == 0)
            {
                if (timer->periodInTicks == 0)  // one shot timer
                {
                    Timer_StopTimer(timer);
                }
                else    // periodic timer
                {
                    timer->currentTicksRemaining = timer->periodInTicks;
                }

                timer->timeOutCallback(NULL);
            }
        }
    }
}

static void OnTimerTicksEvent(void* data)
{
    //static uint16_t cnt = 0;
    uint8_t elapsedTicks;
    UNUSED_PARAMETER(data);

    // atomically copy and reset timer ticks
    Common_AtomicSection_Begin();
    elapsedTicks = timerTicks;
    timerTicks = 0;
    Common_AtomicSection_End();

//    if (elapsedTicks > 0)
//    {
//        printf ("e: %d\n", elapsedTicks);
//    }
    
    
//    printf("cnt: %d \n", cnt);
//    if(cnt ++ >= 1000)
//    {  
//        cnt = 0;
//    }
    
    while ( elapsedTicks > 0 )
    {
        OnTimerTickPassed();
        elapsedTicks--;
    }
}
