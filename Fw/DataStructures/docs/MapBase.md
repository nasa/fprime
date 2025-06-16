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
MapBase(const MapBase<K,V>& map)
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
virtual MapBase()
```

Defined as `= default`.

## 4. Private Member Functions

### 4.1. operator=

```c++
MapBase& operator=(const MapBase&)
```

Defined as `= delete`.

## 5. Public Member Functions

### 5.2. clear

```c++
virtual void clear() = 0
```

Clear the map.

_Example:_
```c++
void f(MapBase<U32>& map) {
    map.clear();
    ASSERT_EQ(map.getSize(), 0);
}
```

### 5.3. copyDataFrom

```c++
void copyDataFrom(const MapBase<T>& map)
```

1. If `&map != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `map.getSize()` and `getCapacity()`.

    1. For `i` in [0, `size`)

        1. Set `e = at(i)`.

        1. Set `status = enmap(e)`.

        1. Assert `status == Success::SUCCESS`.


_Example:_
```c++
void f(MapBase<U32>& q1, MapBase<U32>& q2) {
    q1.clear();
    // Enmap an item
    U32 value = 42;
    (void) q1.enmap(value);
    q2.clear();
    ASSERT_EQ(q2.getSize(), 0);
    q2.copyDataFrom(q1);
    ASSERT_EQ(q2.getSize(), 1);
}
```

### 5.4. demap

```c++
virtual Success demap(T& e) = 0
```

1. Set `status = Success::FAILURE`.

1. If `size > 0`

    1. Remove the leftmost item from the map and store it into `e`.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
void f(MapBase<U32>& map) {
    map.clear();
    U32 val = 0;
    auto status = map.demap(val);
    ASSERT_EQ(status, Success::FAILURE);
    status = map.enmap(3);
    ASSERT_EQ(status, Success::SUCCESS);
    status = map.demap(val);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(val, 3);
}
```

### 5.5. enmap

```c++
virtual Success enmap(const T& e) = 0
```

1. Set `status = Success::FAILURE`.

1. If there is room on the map for a new item, then

    1. Add `e` to the right of the map.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
void f(MapBase<U32>& map) {
    map.clear();
    const auto status = map.enmap(3);
    ASSERT_EQ(status, Success::SUCCESS);
}
```

### 5.6. getCapacity

```c++
virtual FwSizeType getCapacity() const = 0
```

Return the current capacity.

_Example:_
```c++
void f(const MapBase<U32>& map) {
    const auto size = map.getSize();
    const auto capacity = map.getCapacity();
    ASSERT_LE(size, capacity);
}
```

### 5.7. getSize

```c++
virtual FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
```c++
void f(const MapBase<U32>& map) {
    map.clear();
    auto size = map.getSize();
    ASSERT_EQ(size, 0);
    const auto status = map.enmap(3);
    ASSERT_EQ(status, Success::SUCCESS);
    size = map.getSize();
    ASSERT_EQ(size, 1);
}
```

