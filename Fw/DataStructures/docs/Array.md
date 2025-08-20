# 1.2. Array

`Array` is a `final` class template defined in
[`Fw/DataStructures`](sdd.md).
It represents an array with internal storage.
It maintains the backing memory _M_ as a member variable.

## 1. Template Parameters

`Array` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an array element|
|`FwSizeType`|`S`|The array size in elements|

`Array` statically asserts the following:

* `T` is default constructible.
* `S > 0`.

## 2. Types

`Array` defines the type `Elements`.
It is an alias of `T[S]`.

## 3. Private Member Variables

`Array` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_elements`|`Elements`|The array elements|C++ default initialization|

## 4. Public Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
Array()
```

Defined as `= default`.

_Example:_
```c++
Array<U32, 3> a;
```

### 4.2. Initializer List Constructor

```c++
Array(const std::initializer_list<T>& il)
```

1. Assert that `il.size() == S`.

1. Initialize `m_elements` from `il`.

_Examples:_
```c++
Array<U32, 3> a({ 1, 2, 3 });
```

### 4.3. Primitive Array Constructor

```c++
Array(const Elements& elements)
```

1. Statically assert that `S1 == S`.

1. Set `*this = elements`.

_Example:_
```c++
U32 elements[3] = { 1, 2, 3 };
Array<U32, 3> a(elements);
```

### 4.4. Single-Element Constructor

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

### 4.5. Copy Constructor

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

### 4.6. Destructor

```c++
~Array()
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator[]

```c++
T& operator[](FwSizeType i)
const T& operator[](FwSizeType i) const
```

1. Assert that `i < S`.

1. Return `m_elements[i]`.

_Example:_
```c++
constexpr FwSizeType size = 3;
Array<U32, size> a;
// Constant access
ASSERT_EQ(a[0], 0);
// Mutable access
a[0]++;
ASSERT_EQ(a[0], 1);
// Out-of-bounds access
ASSERT_DEATH(a[size], "Assert");
```

### 5.2. operator= (Initializer List)

```c++
Array<T, S>& operator=(const std::initializer_list<T>& il)
```

1. Assert that `il.size() == S`.

1. Copy each element of `il` into `m_elements`.

1. Return `*this`.

_Example:_
```c++
Array<U32, 3> a;
a = { 1, 2, 3 };
```

### 5.3. operator= (Primitive Array)

```c++
Array<T, S>& operator=(const Elements& elements)
```

1. Copy each element of `elements` into `m_elements`.

1. Return `*this`.

_Example:_
```c++
U32 elements[3] = { 1, 2, 3 };
Array<U32, 3> a;
a = elements;
```

### 5.4. operator= (Single Element)

```c++
Array<T, S>& operator=(const T& element)
```

1. Copy `element` into each element of `m_elements`.

1. Return `*this`.

_Example:_
```c++
Array<U32, 3> a;
a = 5;
```

### 5.5. operator= (Copy Assignment)

```c++
Array<T, S>& operator=(const Array<T, S>& a)
```

1. If `&a != this`, overwrite each element of `m_elements` with the 
corresponding element of `a`.

1. Return `*this`.

_Example:_
```c++
Array<U32, 3> a1(1);
Array<U32, 3> a2(2);
a1 = a2;
```

### 5.6. getElements

```c++
Elements& getElements()
const Elements& getElements() const
```

Return `m_elements`.

_Example:_
```c++
constexpr FwSizeType size = 3;
Array<U32, size> a;
// Mutable reference
auto& elements1 = a.getElements();
ASSERT_EQ(elements1[0], 0);
elements1[0] = 1;
// Constant reference
const auto& elements2 = a.getElements();
ASSERT_EQ(elements2[0], 1);
```

### 5.7. asExternalArray

```c++
ExternalArray<T> asExternalArray()
```

Return [`ExternalArray<T>(m_elements, S)`](ExternalArray.md#Public-Constructors-and-Destructors)

_Example:_
```c++
constexpr FwSizeType size = 3;
Array<U32, size> a = { 1, 2, 3 };
ExternalArray<U32> ea = a.asExternalArray();
ASSERT_EQ(ea[0], 1);
```
