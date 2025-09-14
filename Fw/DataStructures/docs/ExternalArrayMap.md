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

<a name="Public-Types"></a>
## 3. Public Types

`ExternalArrayMap` defines the following public types:

|Name|Definition|
|----|----------|
|`ConstIterator`|Alias of [`MapConstIterator<K, V>`](MapConstIterator.md)|
|`Entry`|Alias of [`SetOrMapImplEntry<K, V>`](SetOrMapImplEntry.md)|

## 4. Private Member Variables

`ExternalArrayMap` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_impl`|[`ArraySetOrMapImpl<K, V>`](ArraySetOrMapImpl.md)|The map implementation|C++ default initialization|

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

`entries` must point to a primitive array of at least `capacity`
elements of type [`Entry`](ExternalArrayMap.md#Public-Types).

Call `setStorage(entries, capacity)`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
Map map(entries, capacity);
```

### 5.3. Constructor Providing Untyped Backing Storage

```c++
ExternalArrayMap(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#getByteArrayAlignment) and must
contain at least [`getByteArraySize(capacity)`](#getByteArraySize) bytes.

Call `setStorage(data, capacity)`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = Map::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = Map::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
Map map(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 5.4. Copy Constructor

```c++
ExternalArrayMap(const ExternalArrayMap<K, V>& map)
```

Set `*this = map`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 3;
Map::Entry entries[capacity];
// Call the constructor providing backing storage
Map m1(entries, capacity);
// Insert an item
const U16 key = 0;
const U32 value = 42;
const auto status = m1.insert(key, value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the copy constructor
Map m2(m1);
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

    1. Set `m_impl = map.m_impl`.

1. Return `*this`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 3;
Map::Entry entries[capacity];
// Call the constructor providing backing storage
Map m1(entries, capacity);
// Insert an item
U16 key = 0;
U32 value = 42;
const auto status = m1.insert(key, value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the default constructor
ExternalArrayMap m2;
ASSERT_EQ(m2.getSize(), 0);
// Call the copy assignment operator
m2 = m1;
ASSERT_EQ(m2.getSize(), 1);
value = 0;
status = m2.find(key, value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(value, 42);
```

### 6.2. begin

```c++
ConstIterator begin() const
```

Return `m_impl.begin()`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
// Call the constructor providing backing storage
Map map(entries, capacity);
// Insert an entry in the map
const auto status = map.insert(0, 1);
ASSERT_EQ(status, Fw::Success::SUCCESS);
// Get a map const iterator object
auto it = map.begin();
// Use the iterator to access the underlying map const entry
const auto key = it->getKey();
const auto value = it->getValue();
ASSERT_EQ(key, 0);
ASSERT_EQ(value, 1);
```

### 6.3. clear

```c++
void clear() override
```

Call `m_impl.clear()`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
Map map(entries, capacity);
const auto status = map.insert(0, 3);
ASSERT_EQ(map.getSize(), 1);
map.clear();
ASSERT_EQ(map.getSize(), 0);
```

### 6.4. end

```c++
ConstIterator end() const
```

Return `m_impl.end()`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
// Call the constructor providing backing storage
Map map(entries, capacity);
// Insert an entry in the map
auto status = map.insert(0, 1);
ASSERT_EQ(status, Fw::Success::SUCCESS);
// Get a map const iterator object
auto iter = map.begin();
// Check that iter is not at the end
ASSERT_NE(iter, map.end());
// Increment iter
it++;
// Check that iter is at the end
ASSERT_EQ(iter, map.end());
```

### 6.5. find

```c++
Success find(const K& key, V& value) override
```

Return `m_impl.find(key, value)`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
Map map(entries, capacity);
U32 value = 0;
auto status = map.find(0, value);
ASSERT_EQ(status, Success::FAILURE);
status = map.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
status = map.find(0, value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(value, 1);
```

### 6.6. getCapacity

```c++
FwSizeType getCapacity() const override
```

Return `m_impl.getCapacity()`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
Map map(entries, capacity);
ASSERT_EQ(map.getCapacity(), capacity);
```

### 6.7. getSize

```c++
FwSizeType getSize() const override
```

Return `m_impl.getSize()`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
Map map(entries, capacity);
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.insert(0, 3);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

### 6.8. insert

```c++
Success insert(const K& key, const V& value) override
```

Return `m_impl.insert(key, value)`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
Map map(entries, capacity);
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

### 6.9. remove

```c++
Success remove(const K& key, V& value) override
```

Return `m_impl.remove(key, value)`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map::Entry entries[capacity];
Map map(entries, capacity);
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

`entries` must point to a primitive array of at least `capacity`
elements of type `Entry`.
The type `Entry` is defined [in this section](ExternalArrayMap.md#Public-Types).

Call `m_impl.setStorage(entries, capacity)`.

_Example:_
```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
Map map;
Map::Entry entries[capacity];
map.setStorage(entries, capacity);
```

### 6.11. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#getByteArrayAlignment) and must
contain at least [`getByteArraySize(capacity)`](#getByteArraySize) bytes.

1. Call `m_entries.setStorage(data, capacity)`.

1. Call `clear()`.

```c++
using Map = ExternalArrayMap<U16, U32>;
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = Map::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = Map::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
Map map;
map.setStorage(ByteArray(&bytes[0], sizeof bytes), capacity);
```

## 7. Public Static Functions

<a name="getByteArrayAlignment"></a>
### 7.1. getByteArrayAlignment

```c++
static constexpr U8 getByteArrayAlignment()
```

Return `ArraySetOrMapImpl<K, V>::getByteArrayAlignment()`.

<a name="getByteArraySize"></a>
### 7.2. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType capacity)
```

Return `ArraySetOrMapImpl<K, V>::getByteArraySize(capacity)`.
