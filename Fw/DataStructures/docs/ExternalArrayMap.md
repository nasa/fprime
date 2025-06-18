# ExternalArrayMap

`ExternalArrayMap` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an array-based map with external storage.
Internally it maintains an [`ArraySetOrMapImpl`](ArraySetOrMapImpl.md)
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

## 3. Private Member Variables

`ExternalArrayMap` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_impl`|[`ArraySetOrMapImpl<K, V>`](ArraySetOrMapImpl.md)|The map implementation|C++ default initialization|

```mermaid
classDiagram
    ExternalArrayMap *-- ArraySetOrMapImpl
```

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

The type `Entry` is defined [in the base class](MapBase.md#2-public-types).

Call `m_impl.setStorage(entries, capacity)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
```

### 4.3. Constructor Providing Untyped Backing Storage

```c++
ExternalArrayMap(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#61-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#62-getbytearraysize) bytes.

Call `m_impl.setStorage(data, capacity)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = ExternalArrayMap<U16, U32>::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalArrayMap<U16, U32>::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
ExternalArrayMap<U16, U32> map(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 4.4. Copy Constructor

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

### 4.5. Destructor

```c++
~ExternalArrayMap() override
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator=

```c++
ExternalArrayMap<K, V>& operator=(const ExternalArrayMap<K, V>& map)
```

1. If `&map != this`

    1. Set `m_impl = map.m_impl`.

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

### 5.2. at

```c++
const V& at(FwSizeType index) const
```

Return `m_impl.at(index)`.

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

### 5.3. clear

```c++
void clear() override
```

Call `m_impl.clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
const auto status = map.insert(0, 3);
ASSERT_EQ(map.getSize(), 1);
map.clear();
ASSERT_EQ(map.getSize(), 0);
```

### 5.4. find

```c++
Success find(const K& key, V& value) override
```

Return `m_impl.find(key, value)`.

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

### 5.5. getCapacity

```c++
FwSizeType getCapacity() const override
```

Return `m_impl.getCapacity()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
ASSERT_EQ(map.getCapacity(), capacity);
```

### 5.6. getHeadIterator

```c++
const Iterator* getHeadIterator const override
```

The type `Iterator` is defined [in the base class](MapBase.md#2-public-types).

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
ASSERT_EQ(e->getKey(), 0);
ASSERT_EQ(e->getValue(), 1);
```

### 5.7. getSize

```c++
FwSizeType getSize() const override
```

Return `m_impl.getSize()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
ExternalArrayMap<U16, U32> map(entries, capacity);
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.insert(0, 3);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

### 5.8. insert

```c++
Success insert(const K& key, const V& value) override
```

Return `m_impl.insert(key, value)`.

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

### 5.9. remove

```c++
Success remove(const K& key, V& value) override
```

Return `m_impl.remove(key, value)`.

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

### 5.10. setStorage (Typed Data)

```c++
void setStorage(Entry* entries, FwSizeType capacity)
```

The type `Entry` is defined [in the base class](MapBase.md#2-public-types).

Call `m_impl.setStorage(entries, capacity)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArrayMap<U16, U32> map;
ExternalArrayMap<U16, U32>::Entry entries[capacity];
map.setStorage(entries, capacity);
```

### 5.11. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#61-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#62-getbytearraysize) bytes.

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

## 6. Public Static Functions

### 6.1. getByteArrayAlignment

```c++
static constexpr U8 getByteArrayAlignment()
```

Return `ArraySetOrMapImpl<Entry>::getByteArrayAlignment()`.

### 6.2. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType capacity)
```

Return `ArraySetOrMapImpl<Entry>::getByteArraySize(capacity)`.
