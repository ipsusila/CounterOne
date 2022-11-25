# CounterOne
Arduino counter using 16-bit `Timer1` as counter, and `Timer2` as ticker. 
Incoming pulse should be connected to `D5/PD5` pin of the `avr`-based Arduino board.

Counter allocates `uint16_t` ring-buffer with capacity of `60` for storing last 60 `count per second` values.

## Install

Please refer to [https://www.baldengineer.com/installing-arduino-library-from-github.html](https://www.baldengineer.com/installing-arduino-library-from-github.html)

## Usage

Example usage of the library.

```cpp
#include <CounterOne.h>

// Optional: CounterOne ticker callback
void counterOneTicker() {
    // Will be called every second
}


void setup() {
  // Initialize counter1, input from `D5` and count on rising edge
  Counter1.initialize();

  // Initialization parameters:
  // arg1: pin
  // arg2: true for rising edge OR false for falling edge
  // initialize(uint8_t pin = 5, bool rising = true)

  // Optionally: If you need callback from Timer2 ticker
  Counter1.setCallback(counterOneTicker);
  
  // Other initialization codes
  // ...
}

void loop() {
  // A. start counter. Set the counter to 0 and start counting.
  if (...) {
    Counter1.start();
  }

  // B. Stop counter. Stop counting. The counter value will be left as is
  if (...) {
    Counter1.stop();
  }

  // C. Resume counter. This will start counting from last counter value.
  if (...) {
    Counter1.resume();
  }


  // D. Reading counter/timer values
  float cps = Counter1.cps();   // counts per second
  float cpm = Counter1.cpm();   // counts per minute, averaged over 60 cps.

  // Get total count over 1-minute period
  uint32_t tc = Counter1.totalCount();
  // Get total seconds with maximum of 60 seconds or less
  uint32_t ts = Counter1.totalSeconds();

  // Get total count since CounterOne started
  unsigned long wc = Counter1.wholeCount();
  // Get total seconds since Counter started up to now
  unsigned long ws = Counter1.wholeSeconds();

  // Get latest/oldest count in the ring-buffer
  uint16_t lc = Counter1.latestCount();
  uint16_t oc = Counter1.oldestCount();

  // ...
}
```
