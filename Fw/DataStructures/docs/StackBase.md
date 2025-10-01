# StackBase

`StackBase` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a stack.

## 1. Template Parameters

`StackBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the stack|

## 2. Base Class

`StackBase<T>` is publicly derived from [`SizedContainer`](SizedContainer.md).

## 3. Private Constructors

### 3.1. Copy Constructor

```c++
StackBase(const StackBase<T>& stack)
```

Defined as `= delete`.

## 4. Protected Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
StackBase()
```

Use default initialization of members.

### 4.2. Destructor

```c++
virtual ~StackBase()
```

Defined as `= default`.

## 5. Private Member Functions

### 5.1. operator=

```c++
StackBase& operator=(const StackBase&)
```

Defined as `= delete`.

## 6. Public Member Functions

### 6.1. at

```c++
virtual const T& at(FwSizeType index) const = 0
```

Return the item at the specified index.
Index 0 is the rightmost (latest) element in the stack.
Increasing indices go from right to left.
Fails an assertion if the index is out of range.

_Example:_
```c++
void f(StackBase<U32>& stack) {
    stack.clear();
    auto status = stack.push(3);
    ASSERT_EQ(status, Success::SUCCESS);
    status = stack.push(4);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(stack.at(0), 4);
    ASSERT_EQ(stack.at(1), 3);
    ASSERT_DEATH(stack.at(2), "Assert");
}
```

### 6.2. copyDataFrom

```c++
void copyDataFrom(const StackBase<T>& stack)
```

1. If `&stack != this` then

    1. Call `clear()`.

    1. Let `size` be the minimum of `stack.getSize()` and `getCapacity()`.

    1. For `i` in [0, `size`)

        1. Set `e = at(size - 1 - i)`.

        1. Set `status = push(e)`.

        1. Assert `status == Success::SUCCESS`.


_Example:_
```c++
void f(StackBase<U32>& q1, StackBase<U32>& q2) {
    q1.clear();
    // Push an item
    U32 value = 42;
    (void) q1.push(value);
    q2.clear();
    ASSERT_EQ(q2.getSize(), 0);
    q2.copyDataFrom(q1);
    ASSERT_EQ(q2.getSize(), 1);
}
```

### 6.3. peek

```c++
Success peek(T& e, FwSizeType index = 0) const
```

1. Set `status = Success::FAILURE`.

1. If `index < getSize()`

    1. Set `e = at(index)`.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
void f(StackBase<U32>& stack) {
    stack.clear();
    U32 value = 0;
    auto status = stack.peek(value);
    ASSERT_EQ(status, Success::FAILURE);
    status = stack.push(3);
    status = stack.peek(value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 3);
    status = stack.peek(value, 1);
    ASSERT_EQ(status, Success::FAILURE);
    status = stack.push(4);
    status = stack.peek(value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 4);
    status = stack.peek(value, 1);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 3);
}
```

### 6.4. pop

```c++
virtual Success pop(T& e) = 0
```

1. Set `status = Success::FAILURE`.

1. If `size > 0`

    1. Remove the rightmost item from the stack and store it into `e`.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
void f(StackBase<U32>& stack) {
    stack.clear();
    U32 val = 0;
    auto status = stack.pop(val);
    ASSERT_EQ(status, Success::FAILURE);
    status = stack.push(3);
    ASSERT_EQ(status, Success::SUCCESS);
    status = stack.pop(val);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(val, 3);
}
```

### 6.5. push

```c++
virtual Success push(const T& e) = 0
```

1. Set `status = Success::FAILURE`.

1. If there is room on the stack for a new item, then

    1. Add `e` to the right of the stack.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
void f(StackBase<U32>& stack) {
    stack.clear();
    const auto status = stack.push(3);
    ASSERT_EQ(status, Success::SUCCESS);
}
```
