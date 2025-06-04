# FifoQueue

`FifoQueue` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents a FIFO queue with internal storage.
Internally it maintains an [`Array`](Array.md)
for storing the items on the queue.

## 1. Template Parameters

`FifoQueue` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of a queue item|
|`FwSizeType`|`C`|The queue capacity in items|

`FifoQueue` statically asserts that `C > 0`.

## 2. Base Class

`ExternalFifoQueue<T>` is publicly derived from 
[`FifoQueueBase<T>`](FifoQueueBase.md).

## 3. Private Member Variables

`FifoQueue` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_extQueue`|`ExternalFifoQueue<T>`|The external queue implementation|C++ default initialization|
|`m_array`|`Array<T, C>`|The array providing the backing memory for `m_extQueue`|C++ default initialization|

```mermaid
classDiagram
    queue:FifoQueue<T,C> *-- m_extQueue:ExternalFifoQueue<T>
    queue:FifoQueue<T,C> *-- m_array:Array<T, C>
```

## 4. Public Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
FifoQueue()
```

Initialize each member variable with its default value.

_Example:_
```c++
FifoQueue<U32, 10> queue;
```

### 4.2. Copy Constructor

```c++
FifoQueue(const FifoQueue<T, S>& queue)
```

Set `*this = queue`.

_Example:_
```c++
FifoQueue<U32, 10> q1;
auto status = q1.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
FifoQueue<U32, 10> q2(q1);
ASSERT_EQ(q2.size(), 1);
U32 value = 0;
status = q2.dequeue(value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(value, 3);
```

### 4.3. Destructor

```c++
~FifoQueue() override
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. at

```c++
const T& operator[](FwSizeType i) const override
```

Return `m_extQueue(i)`.

### 5.2. operator=

```c++
FifoQueue<T>& operator=(const FifoQueue<T>& queue)
```

1. Set `status = m_extQueue.copyDataFrom(queue.m_extQueue)`.

1. Assert `status == Success::SUCCESS`.

_Example:_
```c++
FifoQueue<U32, 10> q1;
auto status = q1.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
FifoQueue<U32, 10> q2;
ASSERT_EQ(q2.size(), 0);
q2 = q1;
ASSERT_EQ(q2.size(), 1);
U32 value = 0;
status = q2.dequeue(value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(value, 3);
```

### 5.3. clear

```c++
void clear() override
```

Call `m_extQueue.clear()`.

### 5.4. enqueue

```c++
Success enqueue(const T& e) override
```

Return `m_extQueue.enqueue(e)`.

### 5.5. dequeue

```c++
Success dequeue(T& e) override
```

Return `m_extQueue.dequeue(e)`.

### 5.6. getSize

```c++
FwSizeType getSize() const
```

Return `m_extQueue.getSize()`.

### 5.7. getCapacity

```c++
FwSizeType getCapacity() const
```

Return `m_extQueue.getCapacity()`.

### 5.8. asExternalFifoQueue

```c++
ExternalFifoQueue<T> asExternalFifoQueue()
```

Return [`ExternalFifoQueue<T>(m_items, C)`](ExternalFifoQueue.md#4-public-constructors-and-destructors)

_Example:_
```c++
constexpr FwSizeType size = 3;
FifoQueue<U32, size> queue;
(void) queue.enqueue(3);
ExternalFifoQueue<U32> extQueue = queue.asExternalFifoQueue();
ASSERT_EQ(extQueue.size(), 1);
```

## 6. Public Static Functions

### 6.1. getStaticCapacity

```c++
static constexpr FwSizeType getStaticCapacity()
```

Return the static capacity `C`.

_Example:_
```c++
const auto capacity = FifoQueue<U32, 3>::getStaticCapacity();
ASSERT_EQ(capacity, 3);
```
