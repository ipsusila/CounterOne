#ifndef __CounterOne_h__
#define __CounterOne_h__

#include "Arduino.h"

/*
#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#endif
*/

class CounterOne
{
public:
    static const int minorTick = 8;
    static const int prescaler = 1024;
    static const int majorTick = 1000;
    static const int bufCount = 60;
private:
    bool mRising;
    int mBufPos;
    uint16_t mBuf[bufCount];
    uint32_t mTotalCount;
    uint16_t mTotalSec;
	unsigned long mWholeCount;
	unsigned long mWholeSecs;

    #define FREQ (F_CPU / 1000)
public: 

    CounterOne();
    void initialize(uint8_t pin = 5, bool rising = true) __attribute__((always_inline)) {
        mTotalSec = 0;
        mBufPos = 0;
        mTotalCount = 0;
		mWholeCount = 0;
		mWholeSecs = 0;
        mRising = rising;
        memset(mBuf, 0, sizeof(uint16_t) * bufCount);

        //setup pin as input
        pinMode(pin, INPUT);

        //------------------------------------------------------------------
        //setup timer2, ctc mode
        TCCR2A = (1 << WGM21);
        TCCR2B = 0;

        //prescaler 1024, time 8 ms <- minorTick
        //setup max compare number
        OCR2A = ((minorTick * FREQ) / prescaler) - 1;
    
        //compare match
        TIMSK2 = (1 << OCIE2A);
        TCNT2 = 0;

        //setup counter 1, count on rising edge
        TCCR1B = 0;
        TCNT1 = 0;
    }

    const uint16_t * buffer() const __attribute__((always_inline)) {
        return mBuf;
    }

    int bufferCount() const __attribute__((always_inline)) {
        return bufCount;
    }
    int bufferPos() const __attribute__((always_inline)) {
        return mBufPos;
    }

    uint32_t totalSeconds() const __attribute__((always_inline)) {
        return mTotalSec;
    }
	
	unsigned long wholeCount() const {
		return mWholeCount;
	}
	
	unsigned long wholeSeconds() const {
		return mWholeSecs;
	}

    uint32_t totalCount() const __attribute__((always_inline)) {
        return mTotalCount;
    }
    uint16_t latestCount() const __attribute__((always_inline)) {
        return mBuf[mBufPos];
    }
    uint16_t oldestCount() const __attribute__((always_inline)) {
        return (mBufPos == (bufCount - 1) ? mBuf[0] : mBuf[mBufPos+1]);
    }
    float cps() const __attribute__((always_inline)) {
       return mBuf[mBufPos];
    }
    float cpm() const __attribute__((always_inline)) {
        if (mTotalSec == 0){
            return 0;
        } else if (mTotalSec < 60) {
            float rate = mTotalCount * 60.0f;
            return (rate / mTotalSec);
        } else {
            return mTotalCount;
        }          
    }

    void stop() __attribute__((always_inline)) {
        TCCR1B = 0;
        TCCR2B = 0;
    }
    void resume() __attribute__((always_inline)) {
        //set clock source to external pin, rising/falling
        if (mRising)
            TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);
        else
            TCCR1B = (1 << CS12) | (1 << CS11);

        //start timer2
        TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
    }
    void start() __attribute__((always_inline)) {
        //reset counter
        TCNT1 = 0;
        TCNT2 = 0;
		mWholeCount = 0;
		mWholeSecs = 0;
        resume();
    }
	
	void setCallback(void (*cb)()) __attribute__((always_inline)) {
		tickCallback = cb;
	}
	static void (*tickCallback)();
	
protected:
    void tickTimerTwo();

    friend void isrTimerTwoTicker();
};

//predefined class
extern CounterOne Counter1;

#endif
