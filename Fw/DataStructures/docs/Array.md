# 1.2. Array

`Array` is a `final` class template representing an array
with internal storage.
It maintains the backing memory _M_ as a member variable.

## 1. Template Parameters

`Array` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an array element|
|`FwSizeType`|`S`|The array size in elements|

`Array` statically asserts that `S > 0`.

## 2. Types

`Array` defines the type `Elements`.
It is an alias of `T[S]`.

## 3. Private Member Variables

`Array` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_elements`|`Elements`|The array elements|C++ default initialization|

## 4. Public Constructors and Destructors

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

1. Assert that `il.m_size == S`.

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

Defined as `= default`.

## 5. Public Member Functions

**operator[]:**

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

**asExternalArray:**

```c++
ExternalArray<T> asExternalArray()
```

Return `ExternalArray<T>(m_elements, S)`.

_Example:_
```c++
constexpr FwSizeType size = 3;
Array<U32, size> a = { 1, 2, 3 };
ExternalArray<U32> ea = a.asExternalArray();
ASSERT_EQ(ea[0], 1);
```

## 6. Public Static Functions

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

