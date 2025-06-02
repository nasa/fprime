# Fw/DataStructures: Basic Data Structures

`Fw/DataStructures` contains a library of basic data structures.
All the definitions in this directory are in the
namespace `Fw`.

The data structures defined here use the following concepts:

* **size:** The number of elements currently stored in a data structure.

* **capacity:** The maximum number of elements stored in a data structure.

For a fixed-size array, the size and the capacity are the same.
For other data structures, the size and the capacity are not
in general the same.
For example, at all times a map has a fixed capacity _C_ and a size between 0 
and _C_.

## 1. Arrays

An **array** _A_ stores _S_ elements for _S > 0_ at indices
0, 1, ..., _S - 1_.
The elements are stored in **backing memory** _M_.
An array provides bounds-checked access to the array elements
stored in _M_.

`Fw/DataStructures` provides the following array templates:

1. [`ExternalArray`](ExternalArray.md)

1. [`Array`](Array.md)

## 2. CircularIndex

`CircularIndex` represents an index value that
wraps around modulo an integer.

### 2.1. Private Member Variables

`CircularIndex` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_value`|`FwSizeType`|The index value|0|
|`m_modulus`|`FwSizeType`|The modulus|1|

### 2.2. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
CircularIndex()
```

Initialize the member variables with their default values.

**Constructor with specified members:**

```c++
CircularIndex(FwSizeType modulus, FwSizeType value = 0)
```

1. Assert `modulus > 0`.

1. Set `m_modulus = modulus`.

1. Call `setValue(value)`.

### 2.3. Public Member Functions

**getValue:**

```c++
FwSizeType getValue() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_value`.

**setValue:**

```c++
void setValue(FwSizeType value)
```

Set `m_value = m_value % m_modulus`.

**getModulus:**

```c++
FwSizeType CircularIndex::getModulus() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_modulus`.

**setModulus:**

```c++
void setModulus(FwSizeType modulus)
```

1. Set `m_modulus = modulus`.

2. Call `setValue(m_value)`.

**increment:**

```c++
FwSizeType increment(FwSizeType amount = 1)
```

1. Set `offset = amount % m_modulus`.

1. Call `setValue(m_value + offset)`.

1. Return `m_value`.

**decrement:**

```c++
FwSizeType decrement(FwSizeType amount = 1)
```

1. Set `offset = amount % m_modulus`.

1. Call `setValue(m_value + m_modulus - offset)`.

1. Return `m_value`.

## 3. FIFO Queues

A **FIFO queue** is a data structure backed by an array.
It supports enqueue and dequeue operations in
first in first out (FIFO) order.

### 3.1. ExternalFifoQueue

`ExternalFifoQueue` is a `final` class template representing a
FIFO queue with external storage.
Internally it maintains an <a href="#external_array">`ExternalArray`</a> for 
storing the items on the queue.

#### 3.1.1. Template Parameters

`ExternalFifoQueue` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the queue|

#### 3.1.2. Private Member Variables

`ExternalFifoQueue` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_items`|`ExternalArray<T>`|The array for storing the queue items|C++ default initialization|
|`m_enqueueIndex`|`CircularIndex`|The enqueue index|0|
|`m_dequeueIndex`|`CircularIndex`|The dequeue index|0|
|`m_size`|`FwSizeType`|The number of items on the queue|0|

#### 3.1.3. Public Constructors and Destructors

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

#### 3.1.4. Public Member Functions

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

### 3.2. FifoQueue

`FifoQueue` is a `final` class template representing a
FIFO queue with internal storage.
Internally it maintains an <a href="#array">`Array`</a>
for storing the items on the queue.

#### 3.2.1. Template Parameters

`FifoQueue` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of a queue item|
|`FwSizeType`|`C`|The queue capacity in items|

`FifoQueue` statically asserts that `C > 0`.

#### 3.2.2. Private Member Variables

`FifoQueue` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_extQueue`|`ExternalFifoQueue<T>`|The external queue implementation|C++ default initialization|
|`m_array`|`Array<T, C>`|The array providing the backing memory for `m_extQueue`|C++ default initialization|

#### 3.2.3. Public Constructors and Destructors

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
ASSERT_EQ(status, Fw::Success::SUCCESS);
FifoQueue<U32, 10> q2;
q2 = q1;
U32 value = 0;
status = q2.dequeue(value);
ASSERT_EQ(status, Fw::Success::SUCCESS);
ASSERT_EQ(value, 3);
```

**Destructor:**

```c++
~FifoQueue()
```

Defined as `= default`.

#### 3.2.4. Public Member Functions

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

**asExternalFifoQueue:**

```c++
ExternalFifoQueue<T> asExternalFifoQueue()
```

TODO

## 4. Maps

### 4.1. Map

TODO

### 4.2. Array Map

TODO

### 4.3. AVL Tree Map

TODO

## 5. Sets

### 5.1. Set

TODO

### 5.2. Array Set

TODO

### 5.3. AVL Tree Set

TODO
