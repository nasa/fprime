# FifoQueueBase

`FifoQueueBase` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a FIFO queue.

## 1. Template Parameters

`FifoQueueBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the queue|

## 2. Base Class

`FifoQueueBase<T>` is publicly derived from [`SizedContainer`](SizedContainer.md).

## 3. Private Constructors

### 3.1. Copy Constructor

```c++
FifoQueueBase(const FifoQueueBase<T>& queue)
```

Defined as `= delete`.

## 4. Protected Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
FifoQueueBase()
```

Use default initialization of members.

### 4.2. Destructor

```c++
virtual ~FifoQueueBase()
```

Defined as `= default`.

## 5. Private Member Functions

### 5.1. operator=

```c++
FifoQueueBase& operator=(const FifoQueueBase&)
```

Defined as `= delete`.

## 6. Public Member Functions

### 6.1. at

```c++
virtual const T& at(FwSizeType index) const = 0
```

Return the item at the specified index.
Index 0 is the leftmost (earliest) element in the queue.
Increasing indices go from left to right.
Fails an assertion if the index is out of range.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    const auto status = queue.enqueue(3);
    ASSERT_EQ(status, Success::SUCCESS);
    const auto status = queue.enqueue(4);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(queue.at(0), 3);
    ASSERT_EQ(queue.at(1), 4);
    ASSERT_DEATH(queue.at(2), "Assert");
}
```

### 6.2. copyDataFrom

```c++
void copyDataFrom(const FifoQueueBase<T>& queue)
```

1. If `&queue != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `queue.getSize()` and `getCapacity()`.

    1. For `i` in [0, `size`)

        1. Set `e = at(i)`.

        1. Set `status = enqueue(e)`.

        1. Assert `status == Success::SUCCESS`.


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

### 6.3. dequeue

```c++
virtual Success dequeue(T& e) = 0
```

1. Set `status = Success::FAILURE`.

1. If `size > 0`

    1. Remove the leftmost item from the queue and store it into `e`.

    1. Set `status = Success::SUCCESS`.

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

### 6.4. enqueue

```c++
virtual Success enqueue(const T& e) = 0
```

1. Set `status = Success::FAILURE`.

1. If there is room on the queue for a new item, then

    1. Add `e` to the right of the queue.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    const auto status = queue.enqueue(3);
    ASSERT_EQ(status, Success::SUCCESS);
}
```

### 6.5. peek

```c++
Success peek(T& e, FwSizeType index = 0) const
```

1. Set `status = Success::FAILURE`.

1. If `index < getSize()`

    1. Set `e = at(index)`.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
void f(FifoQueueBase<U32>& queue) {
    queue.clear();
    U32 value = 0;
    auto status = queue.peek(value);
    ASSERT_EQ(status, Success::FAILURE);
    status = queue.enqueue(3);
    status = queue.peek(value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 3);
    status = queue.peek(value, 1);
    ASSERT_EQ(status, Success::FAILURE);
    status = queue.enqueue(4);
    status = queue.peek(value, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 4);
}
```

