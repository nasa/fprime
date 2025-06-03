# FifoQueueBase

`FifoQueueBase` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a FIFO queue.

## 1. Template Parameters

`FifoQueueBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the queue|

## 2. Private Constructors

### 2.1. Copy Constructor

```c++
FifoQueueBase(const FifoQueueBase<T>& queue)
```

Defined as `= delete`.

## 3. Protected Constructors and Destructors

### 3.1. Zero-argument Constructor

```c++
FifoQueueBase()
```

Defined as `= default`.

### 3.2. Destructor

```c++
virtual FifoQueueBase()
```

Defined as `= default`.

## 4. Private Member Functions

### 4.1. operator=

```c++
FifoQueueBase& operator=(const FifoQueueBase&)
```

Defined as `= delete`.

## 5. Public Member Functions

### 5.1. at

```c++
virtual const T& at(FwSizeType i) const = 0
```

1. Assert that `i < getSize()`.

1. Get a reference to the element of the queue at index `i`.
Index 0 is the first element inserted in the queue.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    auto status = queue.enqueue(10);
    ASSERT_EQ(status, Success::SUCCESS);
    auto status = queue.enqueue(11);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(queue.at(0), 10);
    ASSERT_EQ(queue.at(1), 11);
    // Out-of-bounds access
    ASSERT_DEATH(queue.at(2), "Assert");
}
```

### 5.2. clear

```c++
virtual void clear() = 0
```

Clear the queue.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    ASSERT_EQ(queue.getSize(), 0);
}
```

### 5.3. copyDataFrom

```c++
void copyDataFrom(const FifoQueueBase<T>& queue)
```

1. If `&queue == this` then do nothing.

1. Otherwise

    1. Let `size` be the minimum of `queue.getSize()` and `getCapacity()`.

    1. For `i` in [0, `size`)

        1. Set `enqueueStatus = enqueue(queue.at(i))`.

        1. Assert `enqueueStatus == Success::SUCCESS`.

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

### 5.4. enqueue

```c++
virtual void Success enqueue(const T& e) = 0
```

1. If there is no room on the queue for a new item, then return `Success::FAILURE`.

1. Otherwise

    1. Enqueue `e`.

    1. Return `Success::SUCCESS`.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    status = queue.enqueue(3);
    ASSERT_EQ(status, Success::SUCCESS);
}
```

### 5.5. peek

```c++
void Success peek(T& e)
```

1. If `getSize() == 0` return `Success::FAILURE`.

1. Otherwise

    1. Set `e = (*this)[getSize() - 1]`.

    1. Return `Success::SUCCESS`.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    U32 value = 0;
    auto status = queue.peek(value);
    ASSERT_EQ(status, Success::FAILURE);
    status = queue.enqueue(3);
    ASSERT_EQ(status, Success::SUCCESS);
    status = queue.peek(value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 3);
}
```

### 5.6. dequeue

```c++
virtual void Success dequeue(T& e) = 0
```

1. Set `status = peek(e)`.

1. If `status == Success::SUCCESS` then dequeue an item and store it into 
   `e`.

1. Return `status`.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    U32 val = 0;
    auto status = queue.dequeue(val);
    ASSERT_EQ(status, Success::FAILURE);
    status = queue.enqueue(3);
    ASSERT_EQ(status, Success::SUCCESS);
    status = queue.dequeue(val);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(val, 3);
}
```

### 5.7. getSize

```c++
virtual void FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
```c++
void f(const FifoQueueBase<U32>& queue) {
    queue.clear();
    auto size = queue.getSize();
    ASSERT_EQ(size, 0);
    const auto status = queue.enqueue(3);
    ASSERT_EQ(status, Success::SUCCESS);
    size = queue.getSize();
    ASSERT_EQ(size, 1);
}
```

### 5.8. getCapacity

```c++
virtual FwSizeType getCapacity() const = 0
```

Return the current capacity.

_Example:_
```c++
void f(const FifoQueueBase<U32>& queue) {
    const auto size = queue.getSize();
    const auto capacity = queue.getCapacity();
    ASSERT_LE(size, capacity);
}
```
