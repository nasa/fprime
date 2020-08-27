# Utils::TokenBucket

## 1 Introduction

`TokenBucket` is a pure utility class used for throttling actions, such as
components that want to throttle events.

It is similar to `RateLimiter`, but supports a different throttling behavior,
specifically time-based throughput throttling with burstiness. For instance,
`TokenBucket` allows a burst of 5 triggers, before throttling them to 1 trigger
per second after. Unused triggers are collected up to the burst maximum.

## 2 Usage

### 2.1 Basic

TokenBucket is a class that can be included and instantiated with the initial
replenish interval and max tokens.

```cpp
#include <Utils/TokenBucket.hpp>
...
U32 replenishIntervalMicroSecs = 1000000;
U32 maxTokens = 5;
TokenBucket bucket(replenishIntervalMicroSecs, maxTokens);
```

This instantiates `TokenBucket` with 5 initial tokens out of 5 maximum.
Updating and testing if an action should trigger is done in one method:

```cpp
if (bucket.trigger(this->getTime())) {
  // perform action, e.g. log EVR
}
```

Your component must provide it the current `Fw::Time` in the trigger. It will
then use that time to evaluate how many tokens to replenish between the last
trigger and now, and evaluate whether a token can be consumed.

With 5 initial tokens, the first five triggers are guaranteed to return true,
no matter what the time given is. After that, trigger will return true only if
time has elapsed enough for a token to be replenished (in this example, once
per second).

Note that the max tokens is capped at 1000, currently defined as
`MAX_TOKEN_BUCKET_TOKENS` in TokenBucket.hpp.

### 2.2 Advanced

`TokenBucket` can be constructed with 3 more parameters:

```cpp
U32 replenishIntervalMicroSecs = 1000000;
U32 maxTokens = 5;
U32 replenishRate = 2;
U32 startTokens = 2;
Fw::Time startTime(5, 0);
TokenBucket bucket(replenishIntervalMicroSecs, maxTokens, replenishRate, startTokens, startTime);
```

- Replenish rate is the number of tokens to increment by per interval. By default, this is 1.
- Start tokens is the number of tokens to start with. Note that the first trigger will nonetheless attempt to replenish tokens from the start time until the given time, so it is best used with the start time parameter. By default, start tokens is max tokens, and start time is 0.
