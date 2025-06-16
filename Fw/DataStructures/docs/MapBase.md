# MapBase

`MapBase` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a map.

## 1. Template Parameters

`MapBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Private Constructors

### 2.1. Copy Constructor

```c++
MapBase(const MapBase<K, V>& map)
```

Defined as `= delete`.

## 3. Protected Constructors and Destructors

### 3.1. Zero-Argument Constructor

```c++
MapBase()
```

Defined as `= default`.

### 3.2. Destructor

```c++
virtual ~MapBase()
```

Defined as `= default`.

## 4. Private Member Functions

### 4.1. operator=

```c++
MapBase& operator=(const MapBase&)
```

Defined as `= delete`.

## 5. Public Member Functions

### 5.1. clear

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

### 5.2. copyDataFrom

```c++
void copyDataFrom(const MapBase<K, V>& map)
```

1. If `&queue != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `map.getSize()` and `getCapacity()`.

    1. Set `e = map.getHeadEntry`.

    1. For `i` in [0, `size`)

        1. Assert `e != nullptr`.

        1. Set `e1 = insert(*e)`.

        1. Assert `status == Success::SUCCESS`.

        1. Set `e = e.getNextEntry()`

_Example:_
```c++
void f(MapBase<U16, U32>& m1, MapBase<U16, U32>& m2) {
    m1.clear();
    // Insert an entry
    const U16 key = 0
    const U32 value = 42;
    const auto status = m1.enqueue(key, value);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    m2.clear();
    ASSERT_EQ(m2.getSize(), 0);
    m2.copyDataFrom(q1);
    ASSERT_EQ(m2.getSize(), 1);
}
```

### 5.3. find

TODO

### 5.4. getHeadEntry

```c++
MapEntry<K, V>* getHeadEntry() = 0
const MapEntry<K, V>* getHeadEntry const = 0
```

Get the head entry of the map.

_Example:_
```c++
void f(const MapBase<U16, U32>& map) {
  map.clear();
  const auto* e = map.getHeadEntry();
  FW_ASSERT(e == nullptr);
  map.insert(0, 1);
  e = map.getHeadEntry();
  FW_ASSERT(e != nullptr);
  ASSERT_EQ(e.getKey(), 0);
  ASSERT_EQ(e.getValue(), 1);
}

```

### 5.5. getCapacity

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

### 5.6. getSize

```c++
virtual FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
See [**getCapacity**](MapBase.md#55-getCapacity).

### 5.7. insert

```c++
Fw::Success insert(const K& key, const V& value) = 0
Fw::Success insert(const MapEntry<K, V>& e) = 0
```

1. If an entry `e` exists with the specified key, then update the 
   value in `e` and return `SUCCESS`.

1. Otherwise if there is room in the map, then add a new entry `e` with the
specified key-value pair and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(const MapBase<U16, U32>& map) {
    map.clear();
    auto size = map.getSize();
    ASSERT_EQ(size, 0);
    const auto status = map.insert(0, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    size = map.getSize();
    ASSERT_EQ(size, 1);
}
```

### 5.8. remove

```c++
Fw::Success remove(const K& key) = 0
```

1. If an entry `e` exists, then remove `e` and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(const MapBase<U16, U32>& map) {
    map.clear();
    auto size = map.getSize();
    ASSERT_EQ(size, 0);
    auto status = map.insert(0, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    size = map.getSize();
    ASSERT_EQ(size, 1);
    // Key does not exist
    status = map.remove(10);
    ASSERT_EQ(status, Success::FAILURE);
    ASSERT_EQ(size, 1);
    // Key exists
    status = map.remove(0);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(size, 0);
}
```

