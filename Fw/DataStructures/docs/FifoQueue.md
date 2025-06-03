# FifoQueue

`FifoQueue` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents a FIFO queue with internal storage.
Internally it maintains an <a href="#array">`Array`</a>
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

## 4. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
FifoQueue()
```

Initialize each member with default initialization.

_Example:_
```c++
FifoQueue<U32, 10> queue;
```

**Copy constructor:**

```c++
FifoQueue(const FifoQueue<T, S>& queue)
```

Call `m_extQueue.copyItemsFrom(queue.m_extQueue)`.

_Example:_
```c++
FifoQueue<U32, 10> q1;
auto status = q1.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
FifoQueue<U32, 10> q2;
q2 = q1;
U32 value = 0;
status = q2.dequeue(value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(value, 3);
```

**Destructor:**

```c++
~FifoQueue()
```

Defined as `= default`.

## 5. Public Member Functions

**operator[]:**

```c++
T& operator[](FwSizeType i)
const T& operator[](FwSizeType i) const
```

TODO

**Copy assignment operator:**

```c++
FifoQueue<T>& operator=(const FifoQueue<T>& queue)
```

TODO

**clear:**

```c++
void clear()
```

TODO

**enqueue:**

```c++
Success enqueue(const T& e)
```

TODO

**peek:**

```c++
Success peek(T& e)
```

TODO

**dequeue:**

```c++
Success dequeue(T& e)
```

TODO

**getSize:**

```c++
FwSizeType getSize() const
```

TODO

**getCapacity:**

```c++
FwSizeType getCapacity() const
```

TODO

**asExternalFifoQueue:**

```c++
ExternalFifoQueue<T> asExternalFifoQueue()
```

TODO

