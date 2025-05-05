# Utils::RateLimiter

## 1 Introduction

`RateLimiter` is a pure utility class used for throttling actions, such as
components that want to throttle events.

It is mainly used for managing counter state, and providing a simple interface
for both updating and evaluating if the action should be performed. The two
criteria it supports is simple iteration counter and time-based throttling.

## 2 Usage

### 2.1 Basic

RateLimiter is a class that can be included and instantiated with the initial thresholds ("cycle"):

```cpp
#include <Utils/RateLimiter.hpp>
...
U32 counterThreshold = 10;
U32 timeThresholdSecs = 5;
RateLimiter limiter(counterThreshold, timeThresholdSecs);
```

Either throttling methods (counter or time) can be disabled by giving a value of 0.

Updating and testing if an action should trigger is done in one method:

```cpp
if (limiter.trigger(this->getTime())) {
  // perform action, e.g. log EVR
}
```

If you use time-throttling, your component must provide it the current
`Fw::Time` in the trigger. It will then update its internal state, and return
true if the threshold is reached.

### 2.2 Using both criteria

The semantics of using both criteria is as follows: once it triggers, it should
not trigger again until counterThreshold (from 0) is reached or timeThreshold
(from last trigger) is reached, whichever happens first.

In other words, it is either or, rather than both and.
