# SetBase

`SetBase` is an abstract class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a set.

## 1. Template Parameters

`SetBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an element in the set|

## 2. Public Types

`SetBase` defines the following public types:

|Name|Definition|
|----|----------|
|`Iterator`|[`SetIterator<T>`](SetIterator.md)|

## 3. Private Constructors

### 3.1. Copy Constructor

```c++
SetBase(const SetBase<T>& set)
```

Defined as `= delete`.

## 4. Protected Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
SetBase()
```

Defined as `= default`.

### 4.2. Destructor

```c++
virtual ~SetBase()
```

Defined as `= default`.

## 5. Private Member Functions

### 5.1. operator=

```c++
SetBase& operator=(const SetBase&)
```

Defined as `= delete`.

## 6. Public Member Functions

### 6.1. clear

```c++
virtual void clear() = 0
```

Call `m_setImpl.clear()`.

_Example:_
```c++
void f(SetBase<U32>& set) {
    set.clear();
    ASSERT_EQ(set.getSize(), 0);
}
```

### 6.2. copyDataFrom

```c++
void copyDataFrom(const SetBase<T>& set)
```

1. If `&set != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `set.getSize()` and `getCapacity()`.

    1. Set `e = set.getHeadIterator()`.

    1. For `i` in [0, `size`)

        1. Assert `e != nullptr`.

        1. Set `status = insert(e->getElement())`.

        1. Assert `status == Success::SUCCESS`.

        1. Set `e = e->getNextSetIterator()`


_Example:_
```c++
void f(SetBase<U32>& s1, SetBase<U32>& s2) {
    s1.clear();
    // Insert an entry
    const auto status = s1.insert(42);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(s1.getSize(), 1);
    s2.clear();
    ASSERT_EQ(s2.getSize(), 0);
    s2.copyDataFrom(q1);
    ASSERT_EQ(s2.getSize(), 1);
}
```

### 6.3. find

```c++
Success find(const T& element) = 0
```

1. If an entry `e` with element `element` exists in the set,
then set `element = e.getElement()` and return `SUCCESS`.

1. Otherwise return `FAILURE`.

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
    ASSERT_EQ(value, 1);
}
```

### 6.4. getCapacity

```c++
virtual FwSizeType getCapacity() const = 0
```

Return the current capacity.

_Example:_
```c++
void f(const SetBase<U32>& set) {
    const auto size = set.getSize();
    const auto capacity = set.getCapacity();
    ASSERT_LE(size, capacity);
}
```

### 6.5. getHeadIterator

```c++
const Iterator* getHeadIterator const = 0
```

Get the head iterator for the set.

_Example:_
```c++
void f(const SetBase<U32>& set) {
    set.clear();
    const auto* e = set.getHeadIterator();
    ASSERT_EQ(e, nullptr);
    set.insert(42);
    e = set.getHeadIterator();
    ASSERT_NE(e, nullptr);
    ASSERT_EQ(e->getElement(), 42);
}

```

### 6.6. getSize

```c++
virtual FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
See [**getCapacity**](SetBase.md#65-getcapacity).

### 6.7. insert

```c++
Success insert(const T& element) = 0
```

1. If an entry `e` exists with the specified element, then return `SUCCESS`.

1. Otherwise if there is room in the set, then add a new entry `e` with the
   specified element and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(SetBase<U16, U32>& set) {
    set.clear();
    auto size = set.getSize();
    ASSERT_EQ(size, 0);
    const auto status = set.insert(0, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    size = set.getSize();
    ASSERT_EQ(size, 1);
}
```

### 6.8. remove

```c++
Success remove(const T& element) = 0
```

1. If an entry `e` exists with element `element`, then
remove `e` from the set, and return `SUCCESS`.

1. Otherwise return `FAILURE`.

_Example:_
```c++
void f(SetBase<U32>& set) {
    set.clear();
    auto size = set.getSize();
    ASSERT_EQ(size, 0);
    auto status = set.insert(0);
    ASSERT_EQ(status, Success::SUCCESS);
    size = set.getSize();
    ASSERT_EQ(size, 1);
    // Element does not exist
    status = set.remove(42);
    ASSERT_EQ(status, Success::FAILURE);
    ASSERT_EQ(size, 1);
    // Key exists
    status = set.remove(0);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(size, 0);
}
```
