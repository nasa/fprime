# Fw::Ds: Basic Data Structures

This directory contains a library of basic data structures.

## 1. Arrays

An **array** _A_ stores _n_ elements for _n > 0_ at indices
0, 1, ... _n - 1_.
The elements are stored in **backing memory** _M_.
An array provides bounds-checked access to the array elements.

### 1.1. Array

`Array` is a class template representing an array
with internal storage.
It maintains the backing memory _M_ as a member variable.

#### 1.1.1. Template Parameters

`Array` has two template parameters:

1. The type `typename T`

1. The size `FwSizeType S`

#### 1.1.2. Private Member Variables

`Array` has one private variable `m_elements` for
storing the array elements.
It is a native C++ array of type `T[S]`.

#### 1.1.3. Construction and Destruction

**Zero-argument constructor:**

```c++
Array()
```

Construct an array _A_ of type `T` and size `S`.
Initialize each element of `m_elements` with the default value for `T`.

Example:
```
Array<U32, 3> a;
```

**Initializer list constructor:**

```c++
Array(const std::initializer_list<T>& il)
```

Construct an array _A_ of type `T` and size `S`.
Initialize the first _n_ elements of `m_elements` from `il`, where
_n_ is the minimum of `S` and the size of `il`.
Initialize any other elements of `m_elements` with default values.

Example:
```
Array<U32, 3> a({ 1, 2, 3 });
```

**Single-item constructor:**

```c++
Array(const T& elt)
```

Construct an array _A_ of type `T` and size `S`.
Initialize each element of `m_elements` with `elt`.

Example:
```
Array<U32, 3> a(10);
```

**Destructor (implicitly declared):**

```c++
~Array()
```

Destroy all the elements of `m_elements`.

#### 1.1.4. Public Member Functions

**operator[]:**

```c++
T& operator[](FwSizeType i)
const T& operator[](FwSizeType i) const
```

This operator asserts that `i < S`.
If the assertion is true, then it returns a reference to `m_elements[i]`.

Example:
```
const U32 x = a[0];
a[0]++;
```

**operator=:**

```c++
Array<T,S>& operator=(const Array<T,S>& a)
```

If `&a == this` then this operator does nothing.
Otherwise it overwrites each element of `m_elements` with the corresponding
element of `a`.

Example:
```
Array<U32, 10> a1(1);
Array<U32, 10> a2(2);
a1 = a2;
```

**getElements:**

```c++
T[S]& getElements()
const T[S]& getElements() const
```

This function returns a reference to `m_elements`.

Example:
```
Array<U32, 10> a;
auto& elements1 = a.getElements();
FW_ASSERT(elements1[0] == 0);
elements1[0] = 1;
const auto& elements2 = a.getElements();
FW_ASSERT(elements2[0] == 1);
```

#### 1.1.5. Public Static Functions

**getSize:**

```c++
static constexpr FwSizeType getSize()
```

This function returns the size `S` of the array.

Example:
```
Array<U32, 10> a;
const auto size = a.getSize();
FW_ASSERT(size == 10);
```

### 1.2. External Array

TODO

## 2. Queues

TODO

## 3. Linked List

TODO

## 4. Sets and Maps

### 4.1. Array-Based Set and Map

TODO

### 4.2. Hash Set and Map

TODO

### 4.3. Balanced Binary Tree Set and Map

TODO
