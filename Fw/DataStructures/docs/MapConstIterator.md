# MapConstIterator

TODO

`MapBase` is an abstract class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a map.

## 1. Template Parameters

`MapBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

<a name="Public-Types"></a>
## 2. Public Types

<a name="Public-Type-Aliases"></a>
### 2.1. Type Aliases

`MapBase` defines the following public type aliases.

|Name|Definition|
|----|----------|
|`MapEntry`|Alias of [`MapEntry<K, V>`](MapEntry.md)|

### 2.2. ConstIterator

`ConstIterator` is a public inner class of `MapBase`.  It provides
non-modifying iteration over the elements of a `MapBase` instance.

#### 2.2.1. Public Type Aliases

`ConstIterator` defines the following public type aliases.

|Name|Definition|
|----|----------|
|ArrayIterator|An alias for `ArraySetOrMapImpl<K, V>::ConstIterator`.
|RedBlackTreeIterator|An alias for `RedBlackTreeSetOrMapImpl<K, V>::ConstIterator`.

#### 2.2.2. Private Enumerations

`ConstIterator` defines the following private enumerations.

|Name|Definition|
|----|----------|
|ImplKind|An enumeration with values `ARRAY` and `RED_BLACK_TREE`|

#### 2.2.3. Private Type Aliases

`ConstIterator` defines the following private type aliases.

|Name|Definition|
|----|----------|
|ImplIterator|A union with member variables `arrayIterator` of type `ArrayIterator` and `redBlackTreeIterator` of type `RedBlackTreeIterator`|

#### 2.2.4. Private Member Variables

`ConstIterator` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_implKind`|`ImplKind`|The implementation kind|None (must be set by the constructor)|
|`m_impl`|`ImplIterator`|The iterator for the implementation|C++ default initialization|

#### 2.2.5. Public Constructors and Destructors

##### 2.2.5.1. Constructor Providing an Array Implementation

```c++
ConstIterator(const ArraySetOrMapImpl<K, V>& impl)
```

1. Set `m_implKind = ARRAY`.

1. Set `m_implIterator.arrayIterator = ArrayIterator(impl);`

##### 2.2.5.2. Constructor Providing a Red-Black Tree Implementation

```c++
ConstIterator(const RedBlackTreeSetOrMapImpl<K, V>& impl)
```

1. Set `m_implKind = RED_BLACK_TREE`.

1. Set `m_implIterator.array = RedBlackTreeIterator(impl);`

##### 2.2.5.3. Copy Constructor

```c++
ConstIterator(const ConstIterator<KE, VN>& it)
```

Defined as `= delete`.

##### 2.2.5.4. Destructor

```c++
~ConstIterator()
```

Defined as `= default`.

#### 2.2.6. Public Member Functions

##### 2.2.6.1. operator=

```c++
ConstIterator<KE, VN>& operator=(const ConstIterator<KE, VN>& it)
```

Defined as `= delete`.

##### 2.2.6.2. operator==

```c++
bool operator==(const ConstIterator& it)
```

1. If the implementations don't match, then return `false`.

1. Otherwise delegate to the implementations.

##### 2.2.6.3. operator++

```c++
ConstIterator& operator++()
```

Delegated to the implementation.

##### 2.2.6.4. getKeyOrElement

```c++
const KE& getKeyOrElement() const
```

Delegated to the implementation.

##### 2.2.6.5. getValueOrNil

```c++
const KE& getValueOrNil() const
```

Delegated to the implementation.

##### 2.2.6.6. isInRange()

```c++
bool isInRange() const
```

Delegated to the implementation.

##### 2.2.6.7. reset

```c++
void reset()
```

Delegated to the implementation.

## 3. Private Constructors

### 3.1. Copy Constructor

```c++
MapBase(const MapBase<K, V>& map)
```

Defined as `= delete`.

## 4. Protected Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
MapBase()
```

Use default initialization of members.

### 4.2. Destructor

```c++
virtual ~MapBase()
```

Defined as `= default`.

## 5. Private Member Functions

### 5.1. operator=

```c++
MapBase& operator=(const MapBase&)
```

Defined as `= delete`.

## 6. Public Member Functions

### 6.1. begin

```c++
ConstIterator begin() const = 0.
```

Return the iterator for the implementation.

### 6.2. clear

```c++
virtual void clear() = 0
```

Clear the map.

_Example:_
```c++
void f(MapBase<U16, U32>& map) {
    map.clear();
    ASSERT_EQ(map.getSize(), 0);
}
```

### 6.3. copyDataFrom

```c++
void copyDataFrom(const MapBase<K, V>& map)
```

1. If `&map != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `map.getSize()` and `getCapacity()`.

    1. Set `e = map.getHeadMapEntry()`.

    1. For `i` in [0, `size`)

        1. Assert `e != nullptr`.

        1. Set `status = insert(e->getKey(), e->getValue())`.

        1. Assert `status == Success::SUCCESS`.

        1. Set `e = e->getNextMapEntry()`

_Example:_
```c++
void f(MapBase<U16, U32>& m1, MapBase<U16, U32>& m2) {
    m1.clear();
    // Insert an entry
    const U16 key = 0
    const U32 value = 42;
    const auto status = m1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    m2.clear();
    ASSERT_EQ(m2.getSize(), 0);
    m2.copyDataFrom(q1);
    ASSERT_EQ(m2.getSize(), 1);
}
```

### 6.4. end

```c++
ConstIterator end() const = 0
```

Return the iterator for the implementation.

### 6.5. find

```c++
virtual Success find(const K& key, V& value) const = 0
```

1. If an entry `e` with value `key` exists in the map,
then set `value = e.getValue()` and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(const MapBase<U16, U32>& map) {
    map.clear();
    U32 value = 0;
    auto status = map.find(0, value);
    ASSERT_EQ(status, Success::FAILURE);
    status = map.insert(0, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    status = map.find(0, value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 1);
}
```

### 6.6. getCapacity

```c++
virtual FwSizeType getCapacity() const = 0
```

Return the current capacity.

_Example:_
```c++
void f(const MapBase<U16, U32>& map) {
    const auto size = map.getSize();
    const auto capacity = map.getCapacity();
    ASSERT_LE(size, capacity);
}
```

### 6.7. getHeadMapEntry

```c++
virtual const MapEntry* getHeadMapEntry() const = 0
```

Get a pointer to the head iterator for the map, or `nullptr` if there is none.

_Example:_
```c++
void f(const MapBase<U16, U32>& map) {
    map.clear();
    const auto* e = map.getHeadMapEntry();
    ASSERT_EQ(e, nullptr);
    map.insert(0, 1);
    e = map.getHeadMapEntry();
    ASSERT_NE(e, nullptr);
    ASSERT_EQ(e->getKey(), 0);
    ASSERT_EQ(e->getValue(), 1);
}

```

### 6.8. getSize

```c++
virtual FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
See [**getCapacity**](MapBase.md#64-getcapacity).

### 6.9. insert

```c++
virtual Success insert(const K& key, const V& value) = 0
```

1. If an entry `e` exists with the specified key, then update the 
   value in `e` and return `SUCCESS`.

1. Otherwise if there is room in the map, then add a new entry `e` with the
specified key-value pair and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(MapBase<U16, U32>& map) {
    map.clear();
    auto size = map.getSize();
    ASSERT_EQ(size, 0);
    const auto status = map.insert(0, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    size = map.getSize();
    ASSERT_EQ(size, 1);
}
```

### 6.10. remove

```c++
virtual Success remove(const K& key, V& value) = 0
```

1. If an entry `e` exists with key `key`, then
store the value of `e` into `value`,
remove `e` from the map, and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(MapBase<U16, U32>& map) {
    map.clear();
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
}
```

