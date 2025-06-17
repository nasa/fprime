# ExternalArrayMap

`ExternalArrayMap` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an array-based map with external storage.
Internally it maintains an [`ExternalArray`](ExternalArray.md) for
storing the entries in the map.

## 1. Template Parameters

`ExternalArrayMap` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Base Class

`ExternalArrayMap<K, V>` is publicly derived from
[`MapBase<K, V>`](MapBase.md).

## 3. Private Member Variables

`ExternalArrayMap` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_entries`|[`ExternalArray<Entry>`](ExternalArray.md)|The array for storing the map entries|C++ default initialization|
|`m_size`|`FwSizeType`|The number of entries in the map|0|

The type `Entry` is defined [in the base class](MapBase.md#2-types).

## 4. Public Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
ExternalArrayMap()
```

Initialize each member variable with its default value.

_Example:_
```c++
ExternalArrayMap<U16, U32> queue;
```

### 4.2. Constructor Providing Typed Backing Storage

```c++
ExternalArrayMap(Entry* entries, FwSizeType capacity)
```

1. Call `setStorage(entries, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
MapEntry<U16, U32> entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
```

### 4.3. Constructor Providing Untyped Backing Storage

```c++
ExternalArrayMap(ByteArray data, FwSizeType capacity)
```

`data` must be correctly aligned for `Entry` and must
contain at least `ExternalArrayMap<K, V>::getByteArraySize(capacity)` bytes.

1. Call `setStorage(data, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr FwSizeType byteArraySize = ExternalArrayMap<U16, U32>::getByteArraySize();
alignas(MapEntry<U16, U32>) U8 bytes[byteArraySize];
ExternalArrayMap<U16, U32> map(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 4.4. Copy Constructor

```c++
ExternalArrayMap(const ExternalArrayMap<T>& queue)
```

Set `*this = queue`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 entries[capacity];
// Call the constructor providing backing storage
ExternalArrayMap<U32> q1(entries, capacity);
// Enqueue an item
U32 value = 42;
(void) q1.enqueue(value);
// Call the copy constructor
ExternalArrayMap<U32> q2(q1);
ASSERT_EQ(q2.getSize(), 1);
```

### 4.5. Destructor

```c++
~ExternalArrayMap() override
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator=

```c++
ExternalArrayMap<T>& operator=(const ExternalArrayMap<T>& queue)
```

1. If `&queue != this`

    1. Set `m_entries = queue.m_entries`.

    1. Set `m_enqueueIndex = queue.m_enqueueIndex`.

    1. Set `m_dequeueIndex = queue.m_dequeueIndex`.

    1. Set `m_size = queue.m_size`.

1. Return `*this`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 entries[capacity];
// Call the constructor providing backing storage
ExternalArrayMap<U32> q1(entries, capacity);
// Enqueue an item
U32 value = 42;
(void) q1.enqueue(value);
// Call the default constructor
ExternalArrayMap q2;
ASSERT_EQ(q2.getSize(), 0);
// Call the copy assignment operator
q2 = q1;
ASSERT_EQ(q2.getSize(), 1);
```

### 5.2. clear

```c++
void clear() override
```

1. Call `m_enqueueIndex.setValue(0)`.

1. Call `m_dequeueIndex.setValue(0)`.

1. Set `m_size = 0`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 entries[capacity] = {};
ExternalArrayMap<U32> queue(entries, capacity);
const auto status = queue.enqueue(3);
ASSERT_EQ(queue.getSize(), 1);
queue.clear();
ASSERT_EQ(queue.getSize(), 0);
```

### 5.3. setStorage (Typed Data)

```c++
void setStorage(T* entries, FwSizeType capacity)
```

1. Call `m_entries.setStorage(entries, capacity)`.

1. If `capacity > 0`

    1. Call `this->m_enqueueIndex.setModulus(capacity)`.

    1. Call `this->m_dequeueIndex.setModulus(capacity)`.

1. Call `this->clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U32> queue;
U32 entries[capacity];
queue.setStorage(entries, capacity);
```

### 5.4. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be correctly aligned for `Entry` and must
contain at least `ExternalArrayMap<K, V>::getByteArraySize(capacity)` bytes.

1. Call `m_entries.setStorage(data, capacity)`.

1. If `capacity > 0`

    1. Call `this->m_enqueueIndex.setModulus(capacity)`.

    1. Call `this->m_dequeueIndex.setModulus(capacity)`.

1. Call `this->clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr FwSizeType byteArraySize = ExternalArrayMap<U16, U32>::getByteArraySize();
alignas(MapEntry<U16, U32>) U8 bytes[byteArraySize];
ExternalArrayMap<U16, U32> map;
map.setStorage(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 5.5. enqueue

```c++
Success enqueue(const T& e) override
```

1. Set `status = Success::FAILURE`.

1. If `m_size < getCapacity()` then

    1. Set `i = m_enqueueIndex.getValue()`.

    1. Set `m_entries[i] = e`.

    1. Call `m_enqueueIndex.increment()`.

    1. Increment `m_size`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 entries[capacity];
ExternalArrayMap<U32> queue(entries, capacity);
ASSERT_EQ(queue.getSize(), 0);
auto status = queue.enqueue(42);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(queue.getSize(), 1);
```

### 5.6. at

```c++
const T& at(FwSizeType index) const override
```

1. Assert `index < m_size`.

1. Set `ci = m_dequeueIndex`.

1. Set `i = ci.increment(index)`.

1. Return `m_entries[i]`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 entries[capacity];
ExternalArrayMap<U32> queue(entries, capacity);
const auto status = queue.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(queue.at(0), 3);
ASSERT_DEATH(queue.at(1), "Assert");
```

### 5.7. dequeue

```c++
Success dequeue(T& e) override
```

1. Set `status = Success::FAILURE`.

1. If `m_size > 0` then

    1. Set `i = m_dequeueIndex.getValue()`.

    1. Set `e = m_entries[i]`.

    1. Call `m_dequeueIndex.increment()`.

    1. Decrement `m_size`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 entries[capacity];
ExternalArrayMap<U32> queue(entries, capacity);
U32 val;
auto status = queue.dequeue(val);
ASSERT_EQ(status, Success::FAILURE);
status = queue.enqueue(42);
ASSERT_EQ(status, Success::SUCCESS);
status = queue.dequeue(val);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(val, 42);
```

### 5.8. getSize

```c++
FwSizeType getSize() const override
```

Return `m_size`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 entries[capacity];
ExternalArrayMap<U32> queue(entries, capacity);
auto size = queue.getSize();
ASSERT_EQ(size, 0);
const auto status = queue.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
size = queue.getSize();
ASSERT_EQ(size, 1);
```

### 5.9. getCapacity

```c++
FwSizeType getCapacity() const override
```

Return `m_entries.getSize()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 entries[capacity];
ExternalArrayMap<U32> queue(entries, capacity);
ASSERT_EQ(queue.getCapacity(), capacity);
```

## 6. Public Static Functions

### 6.1. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType capacity)
```

Return `ExternalArray<T>::getByteArraySize(capacity)`.

_Example:_
```c++
const FwSizeType size = 10;
const FwSizeType byteArraySize = ExternalFifoQueue<U32>::getByteArraySize(size);
ASSERT_EQ(byteArraySize, 10 * sizeof(U32));
```
