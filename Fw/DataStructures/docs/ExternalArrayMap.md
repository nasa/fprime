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
ExternalArrayMap<U16, U32> map;
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
constexpr FwSizeType byteArraySize = ExternalArrayMap<U16, U32>::getByteArraySize(capacity);
alignas(MapEntry<U16, U32>) U8 bytes[byteArraySize];
ExternalArrayMap<U16, U32> map(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 4.4. Copy Constructor

```c++
ExternalArrayMap(const ExternalArrayMap<T>& map)
```

Set `*this = map`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 entries[capacity];
// Call the constructor providing backing storage
ExternalArrayMap<U16, U32> m1(entries, capacity);
// Insert an item
const U16 key = 0;
const U32 value = 42;
const auto status = m1.insert(key, value);
ASSERT_EQ(status, Fw::Success::SUCCESS);
// Call the copy constructor
ExternalArrayMap<U16, U32> m2(m1);
ASSERT_EQ(m2.getSize(), 1);
```

### 4.5. Destructor

```c++
~ExternalArrayMap() override
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator=

```c++
ExternalArrayMap<T>& operator=(const ExternalArrayMap<T>& map)
```

1. If `&map != this`

    1. Set `m_entries = map.m_entries`.

    1. Set `m_size = map.m_size`.

1. Return `*this`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
MapEntry<U16, U32> entries[capacity];
// Call the constructor providing backing storage
ExternalArrayMap<U16, U32> m1(entries, capacity);
// Insert an item
U16 key = 0;
U32 value = 42;
const auto status = m1.insert(value);
ASSERT_EQ(status, Fw::Success::SUCCESS);
// Call the default constructor
ExternalArrayMap m2;
ASSERT_EQ(m2.getSize(), 0);
// Call the copy assignment operator
m2 = m1;
ASSERT_EQ(m2.getSize(), 1);
```

### 5.2. clear

```c++
void clear() override
```

1. Set `m_size = 0`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
MapEntry<U16, U32> entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
const auto status = map.enqueue(3);
ASSERT_EQ(map.getSize(), 1);
map.clear();
ASSERT_EQ(map.getSize(), 0);
```

### 5.3. find

```c++
Fw::Success find(const K& key, V& value) override
```

TODO

### 5.4. getCapacity

```c++
FwSizeType getCapacity() const override
```

Return `m_entries.getSize()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
ASSERT_EQ(map.getCapacity(), capacity);
```

### 5.5. getHeadEntry

TODO

### 5.6. getSize

```c++
FwSizeType getSize() const override
```

Return `m_size`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

### 5.7. insert

```c++
Fw::Success insert(const K& key, const V& value) override
Fw::Success insert(const Entry& e) override
```

TODO

### 5.8. remove

```c++
Fw::Success remove(const K& key, V& value) override
```

TODO

### 5.9. setStorage (Typed Data)

```c++
void setStorage(T* entries, FwSizeType capacity)
```

1. Call `m_entries.setStorage(entries, capacity)`.

1. Call `this->clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32> map;
MapEntry<U16, U32> entries[capacity];
map.setStorage(entries, capacity);
```

### 5.10. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be correctly aligned for `Entry` and must
contain at least `ExternalArrayMap<K, V>::getByteArraySize(capacity)` bytes.

1. Call `m_entries.setStorage(data, capacity)`.

1. Call `this->clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr FwSizeType byteArraySize = ExternalArrayMap<U16, U32>::getByteArraySize(capacity);
alignas(MapEntry<U16, U32>) U8 bytes[byteArraySize];
ExternalArrayMap<U16, U32> map;
map.setStorage(ByteArray(&bytes[0], sizeof bytes), capacity);
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
const FwSizeType byteArraySize = ExternalFifoQueue<U16, U32>::getByteArraySize(size);
ASSERT_EQ(byteArraySize, 10 * sizeof(U32));
```
