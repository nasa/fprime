# ExternalArrayMap

`ExternalArrayMap` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an array-based map with external storage.
Internally it maintains an [`ArraySetOrMapImpl<Entry>`](ArraySetOrMapImpl.md)
as the map implementation.

## 1. Template Parameters

`ExternalArrayMap` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Base Class

`ExternalArrayMap` is publicly derived from
[`MapBase<K, V>`](MapBase.md).

## 3. Public Types

`ExternalArrayMap` defines the following public types:

```c++
using Entry = SetOrMapIterator<K, V>
```

The type `SetOrMapIterator` is defined [here](SetOrMapIterator.md).

## 4. Private Member Variables

`ExternalArrayMap` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_mapImpl`|[`ArraySetOrMapImpl<Entry>`](ArraySetOrMapImpl.md)|The map implementation|C++ default initialization|

```mermaid
classDiagram
    ExternalArrayMap *-- ArraySetOrMapImpl
```

## 5. Public Constructors and Destructors

### 5.1. Zero-Argument Constructor

```c++
ExternalArrayMap()
```

Initialize each member variable with its default value.

_Example:_
```c++
ExternalArrayMap<U16, U32> map;
```

### 5.2. Constructor Providing Typed Backing Storage

```c++
ExternalArrayMap(Entry* entries, FwSizeType capacity)
```

1. Call `m_mapImpl.setStorage(entries, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
```

### 5.3. Constructor Providing Untyped Backing Storage

```c++
ExternalArrayMap(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#71-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#72-getbytearraysize) bytes.

1. Call `m_mapImpl.setStorage(data, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = ExternalArrayMap<U16, U32>::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalArrayMap<U16, U32>::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
ExternalArrayMap<U16, U32> map(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 5.4. Copy Constructor

```c++
ExternalArrayMap(const ExternalArrayMap<K, V>& map)
```

Set `*this = map`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
// Call the constructor providing backing storage
ExternalArrayMap<U16, U32> m1(entries, capacity);
// Insert an item
const U16 key = 0;
const U32 value = 42;
const auto status = m1.insert(key, value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the copy constructor
ExternalArrayMap<U16, U32> m2(m1);
ASSERT_EQ(m2.getSize(), 1);
```

### 5.5. Destructor

```c++
~ExternalArrayMap() override
```

Defined as `= default`.

## 6. Public Member Functions

### 6.1. operator=

```c++
ExternalArrayMap<K, V>& operator=(const ExternalArrayMap<K, V>& map)
```

1. If `&map != this`

    1. Set `m_mapImpl = map.m_mapImpl`.

1. Return `*this`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
// Call the constructor providing backing storage
ExternalArrayMap<U16, U32> m1(entries, capacity);
// Insert an item
U16 key = 0;
U32 value = 42;
const auto status = m1.insert(value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the default constructor
ExternalArrayMap m2;
ASSERT_EQ(m2.getSize(), 0);
// Call the copy assignment operator
m2 = m1;
ASSERT_EQ(m2.getSize(), 1);
```

### 6.2. at

```c++
const V& at(FwSizeType index) const
```

Return `m_mapImpl[index]`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
const auto status = map.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(map.at(0), 1);
ASSERT_DEATH(map.at(1), "Assert");
```

### 6.3. clear

```c++
void clear() override
```

Call `m_mapImpl.clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
const auto status = map.enqueue(3);
ASSERT_EQ(map.getSize(), 1);
map.clear();
ASSERT_EQ(map.getSize(), 0);
```

### 6.4. find

```c++
Success find(const K& key, V& value) override
```

1. Set `status = Success::FAILURE`.

1. Set `iterator = m_mapImpl.find(key)`.

1. If `iterator != nullptr`

    1. Set `value = iterator.getValue()`.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
U32 value = 0;
auto status = map.find(0, value);
ASSERT_EQ(status, Success::FAILURE);
status = map.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
status = map.find(0, value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(value, 1);
```

### 6.5. getCapacity

```c++
FwSizeType getCapacity() const override
```

Return `m_mapImpl.getCapacity()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
ASSERT_EQ(map.getCapacity(), capacity);
```

### 6.6. getHeadIterator

```c++
const Iterator* getHeadIterator const override
```

The type `Iterator` is defined [in the base class](MapBase.md#2-publictypes).

Return `m_impl.getHeadIterator()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
const auto* e = map.getHeadIterator();
FW_ASSERT(e == nullptr);
map.insert(0, 1);
e = map.getHeadIterator();
FW_ASSERT(e != nullptr);
ASSERT_EQ(e.getKey(), 0);
ASSERT_EQ(e.getValue(), 1);
```

### 6.7. getSize

```c++
FwSizeType getSize() const override
```

Return `m_mapImpl.getSize()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

### 6.8. insert

```c++
Success insert(const K& key, const V& value) override
```

Return `m_mapImpl.insert(key, value)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

```c++
Success insert(const Iterator& e) override
```

Call `insert(e.getKey(), e.getValue())`.

### 6.9. remove

```c++
Success remove(const K& key, V& value) override
```

Return `m_mapImpl.remove(key, value)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
auto size = map.getSize();
ASSERT_EQ(size, 0);
auto status = map.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
// Key does not exist
U32 value = 0;
status = map.remove(10, value);
ASSERT_EQ(status, Success::FAILURE);
ASSERT_EQ(size, 1);
// Key exists
status = map.remove(0, value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(size, 0);
ASSERT_EQ(value, 1);
```

### 6.10. setStorage (Typed Data)

```c++
void setStorage(Entry* entries, FwSizeType capacity)
```

1. Call `m_mapImpl.setStorage(entries, capacity)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32> map;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
map.setStorage(entries, capacity);
```

### 6.11. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#71-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#72-getbytearraysize) bytes.

1. Call `m_entries.setStorage(data, capacity)`.

1. Call `clear()`.

```c++
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = ExternalArrayMap<U16, U32>::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalArrayMap<U16, U32>::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
ExternalArrayMap<U16, U32> map;
map.setStorage(ByteArray(&bytes[0], sizeof bytes), capacity);
```

## 7. Public Static Functions

### 7.1. getByteArrayAlignment

```c++
static constexpr U8 getByteArrayAlignment()
```

Return `ExternalArray<T>::getByteArrayAlignment()`.

### 7.2. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType capacity)
```

Return `ExternalArray<T>::getByteArraySize(capacity)`.
