# ExternalFifoQueue

`ExternalFifoQueue` is a `final` class template representing a
FIFO queue with external storage.
Internally it maintains an [`ExternalArray`](ExternalArray.md) for 
storing the items on the queue.

## 1. Template Parameters

`ExternalFifoQueue` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the queue|

## 2. Private Member Variables

`ExternalFifoQueue` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_items`|`ExternalArray<T>`|The array for storing the queue items|C++ default initialization|
|`m_enqueueIndex`|`CircularIndex`|The enqueue index|0|
|`m_dequeueIndex`|`CircularIndex`|The dequeue index|0|
|`m_size`|`FwSizeType`|The number of items on the queue|0|

## 3. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
ExternalFifoQueue()
```

`ExternalFifoQueue` is a `final` class template representing a FIFO queue with 
internal storage.
It maintains an `Array` for storing the items on the queue.

**Constructor providing backing storage:**

```c++
ExternalFifoQueue(T* items, FwSizeType size)
```

Initialize `m_items` with `items` and `size`.

_Example:_
```c++
constexpr FwSizeType size = 10;
U32 items[size];
ExternalFifoQueue queue(items, size);
```

**Copy constructor:**

```c++
ExternalFifoQueue(const ExternalFifoQueue<T>& queue)
```

1. Set `m_items = queue.m_items`.

1. Set `m_enqueueIndex = queue.m_enqueueIndex`.

1. Set `m_dequeueIndex = queue.m_dequeueIndex`.

1. Set `m_size = queue.size`.

_Example:_
```c++
constexpr FwSizeType size = 3;
U32 items[size];
// Call the constructor providing backing storage
ExternalFifoQueue<U32> q1(elements, 10);
// Enqueue an element
U32 value = 42;
(void) q1.enqueue(value);
// Call the copy constructor
ExternalFifoQueue<U32> a2(a1);
```

**Destructor:**

```c++
ExternalFifoQueue()
```

Defined as `= default`.

## 4. Public Member Functions

**operator[]:**

```c++
T& operator[](FwSizeType i)
const T& operator[](FwSizeType i) const
```

TODO

**clear:**

```c++
void clear()
```

TODO

**setStorage:**

```c++
void setStorage(T* items, FwSizeType size)
```

TODO

**copyItemsFrom:**

```c++
void copyItemsFrom(const ExternalFifoQueue<T>& queue)
```

TODO

**enqueue:**

```c++
Fw::Success enqueue(const T& e)
```

TODO

**peek:**

```c++
Fw::Success peek(T& e)
```

TODO

**dequeue:**

```c++
Fw::Success dequeue(T& e)
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

