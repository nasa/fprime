# FifoQueueBase

`FifoQueueBase` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a FIFO queue.

## 1. Template Parameters

`FifoQueueBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the queue|

## 2. Protected Constructors and Destructors

### 2.1. Zero-argument Constructor

```c++
FifoQueueBase()
```

Defined as `= default`.

### 2.2. Copy Constructor

```c++
FifoQueueBase(const FifoQueueBase<T>& queue)
```

Defined as `= delete`.

### 2.3. Destructor

```c++
virtual FifoQueueBase()
```

Defined as `= default`.

## 3. Public Member Functions

### 3.1. at

```c++
virtual const T& at(FwSizeType i) const = 0
```

1. Assert that `i < getSize()`.

1. Get a reference to the element of the queue at index `i`.
Index 0 is the first element inserted in the queue.

_Example:_
```c++
void f(FifoQueueBase& queue) {
    queue.clear();
    auto status = queue.enqueue(10);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    auto status = queue.enqueue(11);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    ASSERT_EQ(queue.at(0), 10);
    ASSERT_EQ(queue.at(1), 11);
    // Out-of-bounds access
    ASSERT_DEATH(queue.at(2), "Assert");
}
```

### 3.2. clear

```c++
virtual void clear() = 0
```

Clear the queue.

_Example:_
```c++
void f(FifoQueueBase& queue) {
    queue.clear();
    ASSERT_EQ(queue.getSize(), 0);
}
```

### 3.3. copyDataFrom

**copyDataFrom:**

```c++
void copyDataFrom(const FifoQueueBase<T>& queue) override
```

1. Call `clear()`.

1. For `i` from 0 to `queue.getSize() - 1`

    1. Set `const auto status = enqueue(queue.at(i))`

    1. Assert `status == Fw::Success::SUCCESS`.

_Example:_
```c++
void f(FifoQueueBase<U32>& q1, FifoQueueBase<U32>& q2) {
    q1.clear();
    // Enqueue an item
    U32 value = 42;
    (void) q1.enqueue(value);
    q2.clear();
    ASSERT_EQ(q2.getSize(), 0);
    q2.copyDataFrom(q1);
    ASSERT_EQ(q2.getSize(), 1);
}
```

### 3.4. enqueue

```c++
virtual void Fw::Success enqueue(const T& e) = 0
```

1. If there is no room on the queue for a new item, then return `Fw::Success::FAILURE`.

1. Otherwise

    1. Enqueue `e`.

    1. Return `Fw::Success::SUCCESS`.

_Example:_
```c++
void f(FifoQueueBase& queue) {
    queue.clear();
    status = queue.enqueue(3);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
}
```

### 3.5. peek

```c++
void Fw::Success peek(T& e)
```

1. If `getSize() == 0` return `Fw::Success::FAILURE`.

1. Otherwise

    1. Set `e = (*this)[getSize() - 1]`.

    1. Return `Fw::Success::SUCCESS`.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    U32 value = 0;
    auto status = queue.peek(value);
    ASSERT_EQ(status, Fw::Success::FAILURE);
    status = queue.enqueue(3);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    status = queue.peek(value);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    ASSERT_EQ(value, 3);
}
```

### 3.6. dequeue

```c++
virtual void Fw::Success dequeue(T& e) = 0
```

1. Set `status = peek(e)`.

1. If `status == Fw::Success::SUCCESS` then dequeue an item and store it into 
   `e`.

1. Return `status`.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    U32 val = 0;
    auto status = queue.dequeue(val);
    ASSERT_EQ(status, Fw::Success::FAILURE);
    status = queue.enqueue(3);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    status = queue.dequeue(val);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    ASSERT_EQ(val, 3);
}
```

### 3.7. getSize

```c++
virtual void FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
```c++
void f(const FifoQueueBase& queue) {
    queue.clear();
    auto size = queue.getSize();
    ASSERT_EQ(size, 0);
    const auto status = queue.enqueue(3);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    size = queue.getSize();
    ASSERT_EQ(size, 1);
}
```

### 3.8. getCapacity

```c++
virtual FwSizeType getCapacity() const = 0
```

Return the current capacity.

_Example:_
```c++
void f(const FifoQueueBase& queue) {
    const auto size = queue.getSize();
    const auto capacity = queue.getCapacity();
    ASSERT_LE(size, capacity);
}
```
