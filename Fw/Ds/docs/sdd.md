# Fw::Ds: Basic Data Structures

This directory contains a library of basic data structures.

## 1. Arrays

An **array** _A_ stores _n_ elements for _n > 0_ at indices
0, 1, ..., _n - 1_.
The elements are stored in **backing memory** _M_.
An array provides bounds-checked access to the array elements.

### 1.1. External Array

`ExternalArray` is a `final` class template representing an array with external 
storage.
It stores a pointer to the backing memory _M_.

#### 1.1.1. Template Parameters

`ExternalArray` has one template parameter:

1. The type `typename T`

#### 1.1.2. Private Member Variables

`ExternalArray` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_elements`|`T*`|Points to the backing memory|`nullptr`|
|`m_size`|`FwSizeType`|Stores the size (number of elements) of the array|0|

#### 1.1.3. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
ExternalArray()
```

Initialize the member variables with their default values.

_Example:_
```c++
ExternalArray<U32> a;
```

**Constructor providing backing storage:**

```c++
ExternalArray(T* elements, FwSizeType size)
```

Initialize `m_elements` with `elements` and `m_size` with `size`.

_Example:_
```c++
U32 elements[3];
ExternalArray a(elements, 3);
```

**Copy constructor:**

```c++
ExternalArray(const ExternalArray<T>& a)
```

Set `m_elements = a.elements` and `m_size = a.size`.

_Example:_
```c++
U32 elements[3];
// Call the constructor providing backing storage
ExternalArray<U32> a1(elements, 3);
// Call the copy constructor
ExternalArray<U32, 3> a2(a1);
```

**Destructor:**

```c++
~ExternalArray()
```

Do nothing.

#### 1.1.4. Public Member Functions

**operator[]:**

```c++
T& operator[](FwSizeType i)
const T& operator[](FwSizeType i) const
```

1. Assert that `m_elements != nullptr`.

1. Assert that `i < m_size`.

1. Return a reference to `m_elements[i]`.

_Example:_
```c++
U32 elements[3];
ExternalArray<U32> a(elements, 3);
// Constant access
ASSERT_EQ(a[0], 0);
// Mutable access
a[0]++;
ASSERT_EQ(a[0], 1);
// Out-of-bounds access
ASSERT_DEATH(a[3], "Assert");
```

**Copy assignment operator:**

```c++
ExternalArray<T>& operator=(const ExternalArray<T>& a)
```

1. If `&a == this` then do nothing.

1. Otherwise set `m_elements = a.elements` and `m_size = a.size`.

_Example:_
```c++
U32 elements[3];
ExternalArray<U32> a1(elements, 3);
ExternalArray<U32> a2;
a2 = a1;
```

**getElements:**

```c++
T* getElements()
const T* getElements() const
```

Return `m_elements`.

_Example:_
```c++
U32 elements[3];
ExternalArray<U32> a(elements, 3);
// Mutable pointer
auto& elements1 = a.getElements();
ASSERT_EQ(elements1[0], 0);
elements1[0] = 1;
// Constant pointer
const auto& elements2 = a.getElements();
ASSERT_EQ(elements2[0], 1);
```

**getSize:**

```c++
FwSizeType getSize()
```

Return `m_size`.

_Example:_
```c++
U32 elements[3];
ExternalArray<U32> a(elements, 3);
const auto size = a.getSize();
ASSERT_EQ(size, 3);
```

**setStorage:**

```c++
void setStorage(T* elements, FwSizeType size)
```

Set `m_elements = elements` and `m_size = size`.

_Example:_
```c++
ExternalArray<U32> a;
U32 elements[3];
a.setStorage(elements, 3);
```

### 1.2. Array

`Array` is a `final` class template representing an array
with internal storage.
It maintains the backing memory _M_ as a member variable.

#### 1.2.1. Template Parameters

`Array` has two template parameters:

1. The type `typename T`

1. The size `FwSizeType S`

`Array` statically asserts that `S > 0`.

#### 1.2.2. Types

`Array` defines the type `Elements`.
It is an alias of `T[S]`.

#### 1.2.3. Private Member Variables

`Array` has one private variable `m_elements` for
storing the array elements.
It is a primitive C++ array of type `Elements`.

#### 1.2.4. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
Array()
```

Initialize each element of `m_elements` with the default value for `T`.

_Example:_
```c++
Array<U32, 3> a;
```

**Initializer list constructor:**

```c++
Array(const std::initializer_list<T>& il)
```

1. Assert that `il.size == S`.

1. Initialize `m_elements` from `il`.

_Examples:_
```c++
// Explicit call to constructor
Array<U32, 3> a({ 1, 2, 3 });
// Implicit call to constructor via initialization
Array<U32, 3> b = { 1, 2, 3 };
```

**Single-element constructor:**

```c++
explicit Array(const T& element)
```

Initialize each element of `m_elements` with `element`.

_Example:_
```c++
// Explicit call to constructor in variable declaration
Array<U32, 3> a(1);
// Explicit call to constructor in assignment
a = Array<U32, 3>(2);
```

**Copy constructor:**

```c++
Array(const Array<T, S>& a)
```

Initialize the elements of `m_elements` with the
elements of `a.m_elements`.

_Example:_
```c++
// Call the single-item constructor
Array<U32, 3> a1(3);
// Call the copy constructor
Array<U32, 3> a2(a1);
```

**Destructor:**

```c++
~Array()
```

Destroy each element of `m_elements`.

#### 1.2.5. Public Member Functions

**operator[]:**

```c++
T& operator[](FwSizeType i)
const T& operator[](FwSizeType i) const
```

1. Assert that `i < S`.

1. Return a reference to `m_elements[i]`.

_Example:_
```c++
Array<U32, 3> a;
// Constant access
ASSERT_EQ(a[0], 0);
// Mutable access
a[0]++;
ASSERT_EQ(a[0], 1);
// Out-of-bounds access
ASSERT_DEATH(a[3], "Assert");
```

**Copy assignment operator:**

```c++
Array<T, S>& operator=(const Array<T, S>& a)
```

1. If `&a == this` then do nothing.

1. Otherwise overwrite each element of `m_elements` with the corresponding
element of `a`.

_Example:_
```c++
Array<U32, 3> a1(1);
Array<U32, 3> a2(2);
a1 = a2;
```

**getElements:**

```c++
Elements& getElements()
const Elements& getElements() const
```

Return a reference to `m_elements`.

_Example:_
```c++
Array<U32, 3> a;
// Mutable reference
auto& elements1 = a.getElements();
ASSERT_EQ(elements1[0], 0);
elements1[0] = 1;
// Constant reference
const auto& elements2 = a.getElements();
ASSERT_EQ(elements2[0], 1);
```

**asExternalArray:**

```c++
ExternalArray<T> asExternalArray()
```

Return `ExternalArray(m_elements, S)`.

_Example:_
```c++
Array<U32, 3> a = { 1, 2, 3 };
ExternalArray<U32> ea = a.asExternalArray();
ASSERT_EQ(ea[0], 1);
```

#### 1.2.6. Public Static Functions

**getSize:**

```c++
static constexpr FwSizeType getSize()
```

Return the size `S` of the array.

_Example:_
```c++
const auto size = Array<U32, 3>::getSize();
ASSERT_EQ(size, 3);
```

## 2. CircularIndex

`CircularIndex` represents an index value that
wraps around modulo an integer.

### 2.1. Private Member Variables

`CircularIndex` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_value`|`FwSizeType`|The index value|0|
|`m_modulus`|`FwSizeType`|The modulus|1|

### 2.2. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
CircularIndex()
```

Initialize the member variables with their default values.

**Constructor with specified members:**

```c++
CircularIndex(FwSizeType modulus, FwSizeType value = 0)
```

1. Assert `modulus > 0`.

1. Set `m_modulus = modulus`.

1. Call `setValue(value)`.

### 2.3. Public Member Functions

**getValue:**

```c++
FwSizeType getValue() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_value`.

**setValue:**

```c++
void setValue(FwSizeType value)
```

Set `m_value = m_value % m_modulus`.

**getModulus:**

```c++
FwSizeType CircularIndex::getModulus() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_modulus`.

**setModulus:**

```c++
void setModulus(FwSizeType modulus)
```

1. Set `m_modulus = modulus`.

2. Call `setValue(m_value)`.

**increment:**

```c++
FwSizeType increment(FwSizeType amount = 1)
```

1. Set `offset = amount % modulus`.

1. Call `setValue(m_value + offset)`.

1. Return `m_value`.

**decrement:**

```c++
FwSizeType decrement(FwSizeType amount = 1)
```

1. Set `offset = amount % modulus`.

1. Call `setValue(m_value + modulus - offset)`.

1. Return `m_value`.

## 3. FIFO Queues

A **FIFO queue** is a data structure backed by an array.
It supports enqueue and dequeue operations in
first in first out (FIFO) order.

### 3.1. ExternalFifoQueue

`ExternalFifoQueue` is a `final` class template representing a
first in first out (FIFO) queue with external storage.
Internally it maintains an `ExternalArray` for storing
the elements on the queue.

#### 3.1.1. Template Parameters

`ExternalFifoQueue` has one template parameter:

1. The type `typename T`

#### 3.1.2. Private Member Variables

`ExternalFifoQueue` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_array`|`ExternalArray<T>`|The array for storing the elements|C++ default initialization|
|`m_enqueueIndex`|`CircularIndex`|The enqueue index|0|
|`m_dequeueIndex`|`CircularIndex`|The dequeue index|0|
|`m_size`|`FwSizeType`|The number of elements on the queue|0|

#### 3.1.3. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
ExternalFifoQueue()
```

TODO

**Constructor providing backing storage:**

```c++
ExternalArray(T* elements, FwSizeType size)
```

TODO

**Copy constructor:**

```c++
ExternalFifoQueue(const ExternalFifoQueue<T>& queue)
```

TODO

**Destructor:**

```c++
ExternalFifoQueue()
```

TODO

#### 3.1.4. Public Member Functions

**clear:**

```c++
void clear()
```

TODO

**setStorage:**

```c++
void setStorage(T* elements, FwSizeType size)
```

**enqueue:**

```c++
Fw::Success enqueue(const T& e)
```

TODO

**peek:**

```c++
Fw::Success peek(T& e)
```

TODO

**dequeue:**

TODO

**getSize:**

TODO

**getCapacity:**

TODO

### 3.2. FifoQueue

TODO

## 4. Linked Lists

TODO

## 5. Sets and Maps

### 5.1. Array Set and Map

TODO

### 5.2. Hash Set and Map

TODO

### 5.3. Balanced Binary Tree Set and Map

TODO
