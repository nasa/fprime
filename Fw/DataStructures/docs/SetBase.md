# SetBase

`SetBase` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a set.

## 1. Template Parameters

`SetBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an element in the set|

## 2. Protected Types

`SetBase` defines the following protected types:

|Name|Definition|
|----|----------|
|`Nil`|`struct Nil {}`|
|`MapImpl`|`MapBase<T, Nil>`|
|`Entry`|`SetEntry<T>`|

## 3. Protected Member Variables

`SetBase` has the following protected member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_mapImpl`|`MapImpl&`|The implementation of the set as a map|

## 4. Protected Constructors and Destructors

### 4.1. Constructor Providing the Map Implementation

```c++
SetBase(MapImpl& mapImpl)
```

Set `m_mapImpl = mapImpl`.

## 5. Private Constructors and Destructors

### 5.1. Copy Constructor

```c++
SetBase(const SetBase<K, V>& set)
```

Defined as `= delete`.

### 5.2. Destructor

```c++
virtual ~SetBase()
```

Defined as `= default`.

## 6. Private Member Functions

### 6.1. operator=

```c++
SetBase& operator=(const SetBase&)
```

Defined as `= delete`.

## 7. Public Member Functions

### 7.1. clear

```c++
virtual void clear() = 0
```

Call `m_mapImpl.clear()`.

_Example:_
```c++
void f(SetBase<U32>& set) {
    set.clear();
    ASSERT_EQ(set.getSize(), 0);
}
```

### 7.2. copyDataFrom

```c++
void copyDataFrom(const SetBase<T>& set)
```

Call `m_mapImpl.copyDataFrom(set.m_mapImpl)`.

_Example:_
```c++
void f(SetBase<U32>& m1, SetBase<U32>& m2) {
    m1.clear();
    // Insert an entry
    const auto status = m1.insert(42);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(m1.getSize(), 1);
    m2.clear();
    ASSERT_EQ(m2.getSize(), 0);
    m2.copyDataFrom(q1);
    ASSERT_EQ(m2.getSize(), 1);
}
```

### 7.3. find

```c++
Success find(const T& element) = 0
```

1. Let `Nil nil = {}`.

1. Return `m_mapImpl.find(element, nil);

_Example:_
```c++
void f(const SetBase<U32>& set) {
    set.clear();
    auto status = set.find(42);
    ASSERT_EQ(status, Success::FAILURE);
    status = set.insert(42);
    ASSERT_EQ(status, Success::SUCCESS);
    status = set.find(42);
    ASSERT_EQ(status, Success::SUCCESS);
}
```

### 7.4. getCapacity

```c++
virtual FwSizeType getCapacity() const = 0
```

Return `m_mapImpl.getCapacity()`.

_Example:_
```c++
void f(const SetBase<U32>& set) {
    const auto size = set.getSize();
    const auto capacity = set.getCapacity();
    ASSERT_LE(size, capacity);
}
```

### 7.5. getHeadEntry

```c++
const Entry* getHeadEntry const = 0
```

Return `m_mapImpl.getHeadEntry()`.

_Example:_
```c++
void f(const SetBase<U32>& set) {
    set.clear();
    const auto* e = set.getHeadEntry();
    FW_ASSERT(e == nullptr);
    set.insert(42);
    e = set.getHeadEntry();
    FW_ASSERT(e != nullptr);
    ASSERT_EQ(e.getElement(), 42);
}

```

### 7.6. getSize

```c++
virtual FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
See [**getCapacity**](SetBase.md#55-getCapacity).

### 7.7. insert

```c++
Success insert(const K& key, const V& value) = 0
Success insert(const Entry& e) = 0
```

1. If an entry `e` exists with the specified key, then update the 
   value in `e` and return `SUCCESS`.

1. Otherwise if there is room in the set, then add a new entry `e` with the
specified key-value pair and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(SetBase<U32>& set) {
    set.clear();
    auto size = set.getSize();
    ASSERT_EQ(size, 0);
    const auto status = set.insert(0, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    size = set.getSize();
    ASSERT_EQ(size, 1);
}
```

### 7.8. remove

```c++
Success remove(const K& key, V& value) = 0
```

1. If an entry `e` exists with key `key`, then
store the value of `e` into `value`,
remove `e` from the set, and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(SetBase<U32>& set) {
    set.clear();
    auto size = set.getSize();
    ASSERT_EQ(size, 0);
    auto status = set.insert(0, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    size = set.getSize();
    ASSERT_EQ(size, 1);
    // Key does not exist
    U32 value = 0;
    status = set.remove(10, value);
    ASSERT_EQ(status, Success::FAILURE);
    ASSERT_EQ(size, 1);
    // Key exists
    status = set.remove(0, value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(size, 0);
    ASSERT_EQ(value, 1);
}
```

