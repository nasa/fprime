# MapBase

`MapBase` is an abstract class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a map.

## 1. Template Parameters

`MapBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Deleted Elements

The following elements are private and are defined `= delete`:

1. The copy constructor.
    ```c++
    MapBase(const MapBase<K, V>& map)
    ```

1. The assignment operator.
   ```c++
   MapBase& operator=(const MapBase&)
   ```

## 3. Public Types

`MapBase` defines the following public types:

|Name|Definition|
|----|----------|
|`ConstItrator`|Alias of [`MapConstIterator<K, V>`](MapConstIterator.md)|

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

## 5. Public Member Functions

### 5.1. begin

```c++
virtual ConstIterator begin() const = 0.
```

Return the iterator for the implementation.

_Example:_
```c++
void f(MapBase<U16, U32>& map) {
    map.clear();
    // Insert an entry in the map
    auto status = map.insert(0, 1);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    // Get a map const iterator object
    auto it = map.begin();
    // Use the iterator to access the underlying map const entry
    const key = it->getKey();
    const value = it->getValue();
    ASSERT_EQ(key, 0);
    ASSERT_EQ(value, 1);
}
```

### 5.2. clear

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

### 5.3. copyDataFrom

```c++
void copyDataFrom(const MapBase<K, V>& map)
```

1. If `&map != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `map.getSize()` and `getCapacity()`.

    1. Set `it = map.begin()`.

    1. For `i` in [0, `size`)

        1. Set `status = insert(it->getKey(), it->getValue())`.

        1. Assert `status == Success::SUCCESS`.

        1. Call `it++`.

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

### 5.4. end

```c++
virtual ConstIterator end() const = 0
```

Return the iterator for the implementation.

_Example:_
```c++
void f(MapBase<U16, U32>& map) {
    map.clear();
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
}
``

### 5.5. find

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

### 5.6. getCapacity

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

### 5.7. getSize

```c++
virtual FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
See [**getCapacity**](MapBase.md#64-getcapacity).

### 5.8. insert

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

### 5.9. remove

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

