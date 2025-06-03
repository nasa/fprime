# ExternalFifoQueue

`ExternalFifoQueue` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents a FIFO queue with external storage.
Internally it maintains an [`ExternalArray`](ExternalArray.md) for 
storing the items on the queue.

## 1. Template Parameters

`ExternalFifoQueue` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the queue|

## 2. Base Class

`ExternalFifoQueue<T>` is publicly derived from 
[`FifoQueueBase<T>`](FifoQueueBase.md).

## 3. Private Member Variables

`ExternalFifoQueue` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_items`|[`ExternalArray<T>`](ExternalArray.md)|The array for storing the queue items|C++ default initialization|
|`m_enqueueIndex`|[`CircularIndex`](CircularIndex.md)|The enqueue index|`CircularIndex(m_items.size(), 0)`|
|`m_dequeueIndex`|[`CircularIndex`](CircularIndex.md)|The dequeue index|`CircularIndex(m_items.size(), 0)`|
|`m_size`|`FwSizeType`|The number of items on the queue|0|

## 4. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
ExternalFifoQueue()
```

Initialize the member variables with their default values.

_Example:_
```c++
ExternalFifoQueue()
```

**Constructor providing backing storage:**

```c++
ExternalFifoQueue(T* items, FwSizeType capacity)
```

1. Initialize `m_items` with `ExternalArray<T>(items, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[capacity];
ExternalFifoQueue queue(items, capacity);
```

**Copy constructor:**

```c++
ExternalFifoQueue(const ExternalFifoQueue<T>& queue)
```

Set `*this = queue`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
// Call the constructor providing backing storage
ExternalFifoQueue<U32> q1(items, capacity);
// Enqueue an item
U32 value = 42;
(void) q1.enqueue(value);
// Call the copy constructor
ExternalFifoQueue<U32> q2(q1);
ASSERT_EQ(q2.getSize(), 1);
```

**Destructor:**

```c++
~ExternalFifoQueue() override
```

Defined as `= default`.

## 5. Public Member Functions

**Copy assignment operator:**

```c++
ExternalFifoQueue<T>& operator=(const ExternalFifoQueue<T>& queue)
```

1. If `&queue == this` then do nothing.

1. Otherwise 

    1. Set `m_items = queue.m_items`.

    1. Set `m_enqueueIndex = queue.m_enqueueIndex`.

    1. Set `m_dequeueIndex = queue.m_dequeueIndex`.

    1. Set `m_size = queue.size`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
// Call the constructor providing backing storage
ExternalFifoQueue<U32> q1(items, capacity);
// Enqueue an item
U32 value = 42;
(void) q1.enqueue(value);
// Call the default constructor
ExternalFifoQueue q2;
ASSERT_EQ(q2.getSize(), 0);
// Call the copy assignment operator
q2 = q1;
ASSERT_EQ(q2.getSize(), 1);
```

**at:**

```c++
const T& at(FwSizeType i) const override
```

1. Assert that `i < m_size`.

1. Return `m_items[(m_enqueueIndex + i) % m_items.size()]`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[size] = {};
ExternalFifoQueue<U32> queue(items, capacity);
const auto status = queue.enqueue(3);
// Constant access
ASSERT_EQ(queue.at(0), 3);
// Out-of-bounds access
ASSERT_DEATH(queue.at(1), "Assert");
```

**clear:**

```c++
void clear() override
```

1. Call `m_enqueueIndex.setValue(0)`.

1. Call `m_dequeueIndex.setValue(0)`.

1. Set `m_size = 0`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[capacity] = {};
ExternalFifoQueue<U32> queue(items, capacity);
const auto status = queue.enqueue(3);
ASSERT_EQ(queue.getSize(), 1);
queue.clear();
ASSERT_EQ(queue.getSize(), 0);
```

**setStorage:**

```c++
void setStorage(T* items, FwSizeType size)
```

Call `m_items.setStorage(items, size)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalFifoQueue<U32> queue;
U32 items[capacity];
queue.setStorage(items, capacity);
```

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
constexpr FwSizeType capacity = 3;
U32 items1[capacity];
// Call the constructor providing backing storage
ExternalFifoQueue<U32> q1(items1, capacity);
// Enqueue an item
U32 value = 42;
(void) q1.enqueue(value);
U32 items2[capacity];
// Call the constructor providing backing storage
ExternalFifoQueue<U32> q2(items2, capacity);
ASSERT_EQ(q2.getSize(), 0);
q2.copyDataFrom(q1);
ASSERT_EQ(q2.getSize(), 1);
```

**enqueue:**

```c++
Fw::Success enqueue(const T& e) override
```

1. Set `status = Fw::Success::FAILURE`.

1. If `m_size < m_capacity` then

    1. Set `i = m_enqueueIndex.getValue()`.

    1. Set `m_items[i] = e`.

    1. Call `m_enqueueIndex.increment()`.

    1. Increment `m_size`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
ExternalFifoQueue<U32> queue(items, capacity);
ASSERT_EQ(queue.getSize(), 0);
auto status = queue.enqueue(42);
ASSERT_EQ(status, Fw::Success::SUCCESS);
ASSERT_EQ(queue.getSize(), 1);
```

**dequeue:**

```c++
Fw::Success dequeue(T& e) override
```

1. Set `status = Fw::Success::FAILURE`.

1. If `m_size > 0` then

    1. Set `i = m_dequeueIndex.getValue()`.

    1. Set `e = m_items[i]`.

    1. Call `m_dequeueIndex.increment()`.

    1. Decrement `m_size`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
ExternalFifoQueue<U32> queue(items, capacity);
U32 val;
auto status = queue.dequeue(val);
ASSERT_EQ(status, Fw::Success::FAILURE);
status = queue.enqueue(42);
ASSERT_EQ(status, Fw::Success::SUCCESS);
status = queue.dequeue(val);
ASSERT_EQ(status, Fw::Success::SUCCESS);
ASSERT_EQ(val, 42);
```

**getSize:**

```c++
FwSizeType getSize() const override
```

Return `m_size`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[capacity];
ExternalFifoQueue<U32> queue(items, capacity);
auto size = queue.getSize();
ASSERT_EQ(size, 0);
const auto status = queue.enqueue(3);
ASSERT_EQ(status, Fw::Success::SUCCESS);
size = queue.getSize();
ASSERT_EQ(size, 1);
```

**getCapacity:**

```c++
FwSizeType getCapacity() const override
```

TODO

