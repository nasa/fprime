# ExternalStack

`ExternalStack` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents a stack with external storage.
Internally it maintains an [`ExternalArray`](ExternalArray.md) for 
storing the items on the stack.

## 1. Template Parameters

`ExternalStack` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an item on the stack|

## 2. Base Class

`ExternalStack<T>` is publicly derived from 
[`StackBase<T>`](StackBase.md).

## 3. Private Member Variables

`ExternalStack` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_items`|[`ExternalArray<T>`](ExternalArray.md)|The array for storing the stack items|C++ default initialization|
|`m_size`|`FwSizeType`|The number of items on the stack|0|

```mermaid
classDiagram
    Stack *-- ExternalArray
```

## 4. Public Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
ExternalStack()
```

Initialize each member variable with its default value.

_Example:_
```c++
ExternalStack<U32> stack;
```

### 4.2. Constructor Providing Typed Backing Storage

```c++
ExternalStack(T* items, FwSizeType capacity)
```

`items` must point to a primitive array of at least `capacity`
items of type `T`.

1. Call `setStorage(items, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[capacity];
ExternalStack<U32> stack(items, capacity);
```

### 4.3. Constructor Providing Untyped Backing Storage

```c++
ExternalStack(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#getByteArrayAlignment) and must
contain at least [`getByteArraySize(capacity)`](#getByteArraySize) bytes.

1. Call `setStorage(data, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = ExternalStack<U32>::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalStack<U32>::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
ExternalStack<U32> stack(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 4.4. Copy Constructor

```c++
ExternalStack(const ExternalStack<T>& stack)
```

Set `*this = stack`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
// Call the constructor providing backing storage
ExternalStack<U32> q1(items, capacity);
// Push an item
U32 value = 42;
(void) q1.push(value);
// Call the copy constructor
ExternalStack<U32> q2(q1);
ASSERT_EQ(q2.getSize(), 1);
```

### 4.5. Destructor

```c++
~ExternalStack() override
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator=

```c++
ExternalStack<T>& operator=(const ExternalStack<T>& stack)
```

1. If `&stack != this`

    1. Set `m_items = stack.m_items`.

    1. Set `m_size = stack.m_size`.

1. Return `*this`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
// Call the constructor providing backing storage
ExternalStack<U32> q1(items, capacity);
// Push an item
U32 value = 42;
(void) q1.push(value);
// Call the default constructor
ExternalStack q2;
ASSERT_EQ(q2.getSize(), 0);
// Call the copy assignment operator
q2 = q1;
ASSERT_EQ(q2.getSize(), 1);
```

### 5.2. clear

```c++
void clear() override
```

Set `m_size = 0`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[capacity];
ExternalStack<U32> stack(items, capacity);
const auto status = stack.push(3);
ASSERT_EQ(stack.getSize(), 1);
stack.clear();
ASSERT_EQ(stack.getSize(), 0);
```

### 5.3. setStorage (Typed Data)

```c++
void setStorage(T* items, FwSizeType capacity)
```

`items` must point to a primitive array of at least `capacity`
items of type `T`.

1. Call `m_items.setStorage(items, capacity)`.

1. Call `this->clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalStack<U32> stack;
U32 items[capacity];
stack.setStorage(items, capacity);
```

### 5.4. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#getByteArrayAlignment) and must
contain at least [`getByteArraySize(capacity)`](#getByteArraySize) bytes.

1. Call `m_items.setStorage(data, capacity)`.

1. Call `this->clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = ExternalStack<U32>::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalStack<U32>::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
ExternalStack<U32> stack;
stack.setStorage(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 5.6. at

```c++
const T& at(FwSizeType index) const override
```

1. Assert `index < m_size`.

1. Return `m_items[m_size - 1 - index]`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
ExternalStack<U32> stack(items, capacity);
const auto status = stack.push(3);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(stack.at(0), 3);
ASSERT_DEATH(stack.at(1), "Assert");
```

### 5.8. getSize

```c++
FwSizeType getSize() const override
```

Return `m_size`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[capacity];
ExternalStack<U32> stack(items, capacity);
auto size = stack.getSize();
ASSERT_EQ(size, 0);
const auto status = stack.push(3);
ASSERT_EQ(status, Success::SUCCESS);
size = stack.getSize();
ASSERT_EQ(size, 1);
```

### 5.9. getCapacity

```c++
FwSizeType getCapacity() const override
```

Return `m_items.getSize()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
U32 items[capacity];
ExternalStack<U32> stack(items, capacity);
ASSERT_EQ(stack.getCapacity(), capacity);
```

### 5.7. pop

```c++
Success pop(T& e) override
```

1. Set `status = Success::FAILURE`.

1. If `m_size > 0` then

    1. Decrement `m_size`.

    1. Set `e = m_items[m_size]`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
ExternalStack<U32> stack(items, capacity);
U32 val;
auto status = stack.pop(val);
ASSERT_EQ(status, Success::FAILURE);
status = stack.push(42);
ASSERT_EQ(status, Success::SUCCESS);
status = stack.pop(val);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(val, 42);
```

### 5.5. push

```c++
Success push(const T& e) override
```

1. Set `status = Success::FAILURE`.

1. If `m_size < getCapacity()` then

    1. Set `m_items[m_size] = e`.

    1. Increment `m_size`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
U32 items[capacity];
ExternalStack<U32> stack(items, capacity);
ASSERT_EQ(stack.getSize(), 0);
auto status = stack.push(42);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(stack.getSize(), 1);
```

## 6. Public Static Functions

<a name="getByteArrayAlignment"></a>
### 6.1. getByteArrayAlignment

```c++
static constexpr U8 getByteArrayAlignment()
```

Return `ExternalArray<T>::getByteArrayAlignment()`.

<a name="getByteArraySize"></a>
### 6.2. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType capacity)
```

Return `ExternalArray<T>::getByteArraySize(capacity)`.
