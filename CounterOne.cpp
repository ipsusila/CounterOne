#include "CounterOne.h"

CounterOne Counter1; // pre-allocated
static void nop()
{
}
void (*CounterOne::tickCallback)() = nop;

volatile long gCount = 0;
// called every 8 ms
volatile byte tick = 0;

void __co_initVars()
{
    tick = 0;
    gCount = 0;
}

void isrTimerTwoTicker()
{
    // called every 8 ms * (1000/8),
    // thus, will be called every 1000 ms
    int cnt = TCNT1;
    TCNT1 = 0;

    gCount += cnt;
    const byte tickLimit = CounterOne::majorTick / CounterOne::minorTick;
    tick++;
    if (tick >= tickLimit)
    {
        tick = 0;
        Counter1.tickTimerTwo();
    }
}

// overflow every 8ms
ISR(TIMER2_COMPA_vect) // timer2 overflow interrupt
{
    // call tick handler
    isrTimerTwoTicker();
}

CounterOne::CounterOne()
{
    mTotalSec = 0;
    mBufPos = 0;
    mTotalCount = 0;
}

// called every 1 secs
void CounterOne::tickTimerTwo()
{
    // read count
    // uint16_t cnt = TCNT1;
    // TCNT1 = 0;
    uint16_t cnt = gCount;
    gCount = 0;

    // save to circular buffer
    int idx = mBufPos + 1;
    if (idx >= bufCount)
    {
        idx = 0;
    }
    else if (mTotalSec < bufCount) // less than one minute
    {
        mTotalSec += (majorTick / 1000);
    }
    // remember old count (in the circular buffer)
    uint16_t oldCount = mBuf[idx];
    mBuf[idx] = cnt;
    mBufPos = idx;

    // TODO: how if overflow?
    mTotalCount += cnt;
    mTotalCount -= oldCount;

    mWholeCount += cnt;
    mWholeSecs += (majorTick / 1000);

    // data updated
    tickCallback();
}
