# Utils::LockGuard

## 1 Introduction

`LockGuard` is a RAII-wrapper around Os::Mutex, essentially allowing scoped
lock and unlock. It works the same way as C++11 `std::lock_guard`.

If a section of code has only basic lock/unlock requirements, `LockGuard`
ensures that early exits from that section will always release the lock.

## 2 Usage

### 2.1 Basic

LockGuard is a class that can be included and instantiated with an associated
Os::Mutex reference.

```cpp
#include <Utils/LockGuard.hpp>
...
Os::Mutex mutex;
LockGuard lock(mutex);
```

The moment LockGuard is instantiated, it will attempt to lock the mutex that's
given to it.

LockGuard is meant for scoped use, i.e. it should always live on a function
call stack. It allows a sometimes more convenient way of using mutex, e.g. you
want this entire function locked from this point forward, and early returns
should be guaranteed to release the lock.

```cpp
LockGuard _lock(mutex);
...
if (xyz) {
  return 0; // will unlock mutex
}
if (abc) {
  return 1; // will unlock mutex
}
return 2; // will unlock mutex
```

It can also be used for a smaller scope

```cpp
if (xyz) {
  LockGuard _lock(mutex);
  ...
} // will unlock mutex
