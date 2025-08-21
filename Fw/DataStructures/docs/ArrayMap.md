# ArrayMap

`ArrayMap` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an array-based map with internal storage.

## 1. Template Parameters

`ArrayMap` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|
|`FwSizeType`|`C`|The capacity, i.e., the maximum number of keys that the map can store|

`ArrayMap` statically asserts that `C > 0`.

## 2. Base Class

`ArrayMap` is publicly derived from
[`MapBase<K, V>`](MapBase.md).

<a name="Public-Types"></a>
## 3. Public Types

`ArrayMap` defines the following public types:

|Name|Definition|
|----|----------|
|`ConstIterator`|Alias of [`MapConstIterator<K, V>`](MapConstIterator.md)|
|`Entry`|Alias of [`SetOrMapImplEntry<K, V>`](SetOrMapImplEntry.md)|
|`Entries`|Alias of `Entry[C]`|

## 4. Private Member Variables

`ArrayMap` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_extMap`|[`ExternalArrayMap<K, V>`](ExternalArrayMap.md)|The external map implementation|C++ default initialization|
|`m_entries`|`Entries`|The array providing the backing memory for `m_extMap`|C++ default initialization|

```mermaid
classDiagram
    ArrayMap *-- ExternalArrayMap
```

## 5. Public Constructors and Destructors

### 5.1. Zero-Argument Constructor

```c++
ArrayMap()
```

Initialize `m_extMap` with `ExternalArrayMap<K, V>(m_entries, C)`.

_Example:_
```c++
ArrayMap<U16, U32, 10> map;
```

### 5.2. Copy Constructor

```c++
ArrayMap(const ArrayMap<K, V, C>& map)
```

1. Initialize `m_extMap` with `ExternalArrayMap<K, V>(m_entries, C)`.

1. Set `*this = map`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map m1;
// Insert an item
const U16 key = 0;
const U32 value = 42;
const auto status = m1.insert(key, value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the copy constructor
Map m2(m1);
ASSERT_EQ(m2.getSize(), 1);
```

### 5.3. Destructor

```c++
~ArrayMap() override
```

Defined as `= default`.

## 6. Public Member Functions

### 6.1. operator=

```c++
ArrayMap<K, V, C>& operator=(const ArrayMap<K, V, C>& map)
```

Return `m_extMap.copyDataFrom(map)`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map m1;
// Insert an item
U16 key = 0;
U32 value = 42;
auto status = m1.insert(key, value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the default constructor
Map m2;
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

Return `m_extMap.begin()`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map map;
// Insert an entry in the map
const auto status = map.insert(0, 1);
ASSERT_EQ(status, Fw::Success::SUCCESS);
// Get a map const iterator object
auto it = map.begin();
// Use the iterator to access the underlying map const entry
const key = it->getKey();
const value = it->getValue();
ASSERT_EQ(key, 0);
ASSERT_EQ(value, 1);
```

### 6.3. clear

```c++
void clear() override
```

Call `m_extMap.clear()`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map map;
const auto status = map.insert(0, 3);
ASSERT_EQ(map.getSize(), 1);
map.clear();
ASSERT_EQ(map.getSize(), 0);
```

### 6.4. end

```c++
ConstIterator end() const
```

Return `m_extMap.end()`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
// Call the constructor providing backing storage
Map map;
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

Return `m_extMap.find(key, value)`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map map;
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

Return `m_extMap.getCapacity()`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map map;
ASSERT_EQ(map.getCapacity(), 10);
```

### 6.8. getSize

```c++
FwSizeType getSize() const override
```

Return `m_extMap.getSize()`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map map;
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.insert(0, 3);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

### 6.9. insert

```c++
Success insert(const K& key, const V& value) override
```

Return `m_extMap.insert(key, value)`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map map;
auto size = map.getSize();
ASSERT_EQ(size, 0);
const auto status = map.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
size = map.getSize();
ASSERT_EQ(size, 1);
```

### 6.10. remove

```c++
Success remove(const K& key, V& value) override
```

Return `m_extMap.remove(key, value)`.

_Example:_
```c++
using Map = ArrayMap<U16, U32, 10>;
Map map;
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
