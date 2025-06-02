# ExternalArray

`ExternalArray` is a `final` class template representing an 
[array](sdd.md#1-arrays) with external storage.
It stores a pointer to the backing memory _M_.

## 1. Template Parameters

`ExternalArray` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an array element|

## 2. Private Member Variables

`ExternalArray` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_elements`|`T*`|Points to the backing memory|`nullptr`|
|`m_size`|`FwSizeType`|Stores the size (number of elements) of the array|0|

## 3. Public Constructors and Destructors

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
constexpr FwSizeType size = 3;
U32 elements[size];
ExternalArray a(elements, size);
```

**Copy constructor:**

```c++
ExternalArray(const ExternalArray<T>& a)
```

Set `m_elements = a.m_elements` and `m_size = a.m_size`.

_Example:_
```c++
constexpr FwSizeType size = 3;
U32 elements[size];
// Call the constructor providing backing storage
ExternalArray<U32> a1(elements, size);
// Call the copy constructor
ExternalArray<U32> a2(a1);
```

**Destructor:**

```c++
~ExternalArray()
```

Defined as `= default`.

## 4. Public Member Functions

**operator[]:**

```c++
T& operator[](FwSizeType i)
const T& operator[](FwSizeType i) const
```

1. Assert that `m_elements != nullptr`.

1. Assert that `i < m_size`.

1. Return `m_elements[i]`.

_Example:_
```c++
constexpr FwSizeType size = 3;
U32 elements[size] = {};
ExternalArray<U32> a(elements, size);
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
ExternalArray<T>& operator=(const ExternalArray<T>& a)
```

1. If `&a == this` then do nothing.

1. Otherwise set `m_elements = a.m_elements` and `m_size = a.m_size`.

_Example:_
```c++
constexpr FwSizeType size = 3;
U32 elements[size];
ExternalArray<U32> a1(elements, size);
ExternalArray<U32> a2;
a2 = a1;
```

**copyElementsFrom:**

```c++
void copyElementsFrom(const ExternalArray<T>& a)
```

1. If `&a == this` then do nothing.

1. Otherwise 

    1. Let `size` be the minimum of `this->m_size` and `a.m_size`

    1. For each `i` from 0 through `size - 1`, set `m_elements[i] = a.m_elements[i]`

_Example:_
```c++
constexpr FwSizeType size = 10;
U32 elements1[size];
ExternalArray<U32> a1(elements, size);
for (FwSizeType i = 0; i < size; i++) {
    a1[i] = i;
}
U32 elements2[size];
ExternalArray<U32> a2(elements, size);
a2.copyElementsFrom(a1);
for (FwSizeType i = 0; i < size; i++) {
    ASSERT_EQ(a2[i], a1[i]);
}
```

**getElements:**

```c++
T* getElements()
const T* getElements() const
```

Return `m_elements`.

_Example:_
```c++
constexpr FwSizeType size = 3;
U32 elements[size];
ExternalArray<U32> a(elements, size);
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
constexpr FwSizeType size = 3;
U32 elements[size];
ExternalArray<U32> a(elements, size);
const auto size1 = a.getSize();
ASSERT_EQ(size1, size);
```

**setStorage:**

```c++
void setStorage(T* elements, FwSizeType size)
```

Set `m_elements = elements` and `m_size = size`.

_Example:_
```c++
constexpr FwSizeType size = 3;
ExternalArray<U32> a;
U32 elements[size];
a.setStorage(elements, size);
```
