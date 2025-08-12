# ExternalArray

`ExternalArray` is a `final` class template 
defined in [`Fw/DataStructures`](sdd.md).
It represents an
[array](sdd.md#1-arrays) with external storage.
It stores a pointer to the backing memory _M_.

## 1. Template Parameters

`ExternalArray` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an array element|

`Array` statically asserts that `T` is assignable to `T&`.

## 2. Private Member Variables

`ExternalArray` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_elements`|`T*`|Pointer to backing memory or `nullptr`|`nullptr`|
|`m_size`|`FwSizeType`|Stores the size (number of elements) of the array|0|

<a name="Public-Constructors-and-Destructors"></a>
## 3. Public Constructors and Destructors

### 3.1. Zero-Argument Constructor

```c++
ExternalArray()
```

Initialize the member variables with their default values.

_Example:_
```c++
ExternalArray<U32> a;
```

### 3.2. Constructor Providing Typed Backing Storage

```c++
ExternalArray(T* elements, FwSizeType size)
```

`elements` must point to a primitive array of at least `size`
elements of type `T`.

Call `setStorage(elements, size)`.

_Example:_
```c++
constexpr FwSizeType size = 3;
U32 elements[size];
ExternalArray<U32> a(elements, size);
```

### 3.3. Constructor Providing Untyped Backing Storage

```c++
ExternalArray(ByteArray data, FwSizeType size)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#51-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#52-getbytearraysize) bytes.

Call `setStorage(data, size)`.

_Example:_
```c++
constexpr FwSizeType size = 3;
constexpr U8 alignment = ExternalArray<U32>::byteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalArray<U32>::getByteArraySize(size);
alignas(alignment) U8 bytes[byteArraySize];
ExternalArray<U32> a(ByteArray(&bytes[0], sizeof bytes), size);
```

### 3.4. Copy Constructor

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

### 3.5. Destructor

```c++
~ExternalArray()
```

Defined as `= default`.

## 4. Public Member Functions

### 4.1. operator[]

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

### 4.2. operator=

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

### 4.3. copyDataFrom

```c++
void copyDataFrom(const ExternalArray<T>& a)
```

1. If `&a != this`

    1. Let `size` be the minimum of `m_size` and `a.m_size`

    1. For each `i` in [0, `size`), set `m_elements[i] = a.m_elements[i]`

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
a2.copyDataFrom(a1);
for (FwSizeType i = 0; i < size; i++) {
    ASSERT_EQ(a2[i], a1[i]);
}
```

### 4.4. getElements

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

### 4.5. getSize

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

### 4.6. setStorage (Typed Data)

```c++
void setStorage(T* elements, FwSizeType size)
```

`elements` must point to a primitive array of at least `size`
elements of type `T`.

Set `m_elements = elements` and `m_size = size`.

_Example:_
```c++
ExternalArray<U32> a;
constexpr FwSizeType size = 3;
U32 elements[size];
a.setStorage(elements, size);
```

### 4.7. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType size)
```

`data` must be correctly aligned according to [`getByteArrayAlignment`](#51-getbytearrayalignment)
and must contain at least [`getByteArraySize(size)`](#52-getbytearraysize) bytes.

1. Assert that `data.bytes != nullptr`.

1. Assert that `data.bytes` is correctly aligned for type `T`.

1. Assert that `size * sizeof(T) <= data.size`.

1. Initialize `m_elements` with `data.bytes`.

1. Construct the objects pointed to by `m_elements` in place.

1. Initialize `m_size` with `size`.

_Example:_
```c++
constexpr FwSizeType size = 3;
constexpr U8 alignment = ExternalArray<U32>::byteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalArray<U32>::getByteArraySize(size);
alignas(alignment) U8 bytes[byteArraySize];
ExternalArray<U32> a;
a.setStorage(ByteArray(&bytes[0], sizeof bytes), size);
```

## 5. Public Static Functions

### 5.1. getByteArrayAlignment

```c++
static constexpr U8 getByteArrayAlignment()
```

Return `alignof(T)`.

_Example:_
```c++
const U8 byteArrayAlignment = ExternalArray<U32>::getByteArrayAlignment(size);
ASSERT_EQ(byteArrayAlignment, alignof(U32));
```

### 5.2. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType size)
```

Return `size * sizeof(T)`.

_Example:_
```c++
const FwSizeType size = 10;
const FwSizeType byteArraySize = ExternalArray<U32>::getByteArraySize(size);
ASSERT_EQ(byteArraySize, 10 * sizeof(U32));
```
