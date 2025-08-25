# SetBase

`SetBase` is an abstract class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a set.

## 1. Template Parameters

`SetBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an element in the set|

## 2. Base Class

`SetBase<T>` is publicly derived from [`SizedContainer`](SizedContainer.md).

## 3. Deleted Elements

The following elements are private and are defined `= delete`:

1. The copy constructor.
    ```c++
    SetBase(const SetBase<T>& map)
    ```

1. The assignment operator.
   ```c++
   SetBase& operator=(const SetBase&)
   ```

## 4. Public Types

`SetBase` defines the following public types:

|Name|Definition|
|----|----------|
|`ConstIterator`|Alias of [`SetConstIterator<T>`](SetConstIterator.md)|

## 5. Protected Constructors and Destructors

### 5.1. Zero-Argument Constructor

```c++
SetBase()
```

Use default initialization of members.

### 5.2. Destructor

```c++
virtual ~SetBase()
```

Defined as `= default`.

## 6. Public Member Functions

### 6.1. begin

```c++
virtual ConstIterator begin() const = 0.
```

Return the begin value of the iterator for the implementation.

_Example:_
```c++
void f(SetBase<U32>& set) {
    set.clear();
    // Insert an element in the set
    const auto status = set.insert(42);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    // Get a set const iterator object
    auto it = set.begin();
    // Use the iterator to access the element
    ASSERT_EQ(*it, 42);
}
```

### 6.2. copyDataFrom

```c++
void copyDataFrom(const SetBase<T>& set)
```

1. If `&set != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `set.getSize()` and `getCapacity()`.

    1. Set `it = map.begin()`.

    1. For `i` in [0, `size`)

        1. Let `status = insert(*it)`.

        1. Assert `status == Success::SUCCESS`.

        1. Call `it++`.

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

### 6.3. end

```c++
virtual ConstIterator end() const = 0
```

Return the end value of the iterator for the implementation.

_Example:_
```c++
void f(SetBase<U32>& set) {
    set.clear();
    // Insert an element in the set
    auto status = set.insert(42);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    // Get a set const iterator object
    auto iter = set.begin();
    // Check that iter is not at the end
    ASSERT_NE(iter, set.end());
    // Increment iter
    it++;
    // Check that iter is at the end
    ASSERT_EQ(iter, set.end());
}
```

### 6.4. find

```c++
virtual Success find(const T& element) = 0
```

1. If an entry `e` with element `element` exists in the set,
then return `SUCCESS`.

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

### 6.5. insert

```c++
virtual Success insert(const T& element) = 0
```

1. If an entry `e` exists with the specified element, then return `SUCCESS`.

1. Otherwise if there is room in the set, then add a new entry `e` with the
   specified element and return `SUCCESS`.

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

### 6.6. remove

```c++
virtual Success remove(const T& element) = 0
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
