# Fw::Ds: Basic Data Structures

This directory contains a library of basic data structures.

## 1. Arrays

An **array** _A_ stores _n_ elements for _n > 0_ at indices
0, 1, ..., _n - 1_.
The elements are stored in **backing memory** _M_.
An array provides bounds-checked access to the array elements.

### 1.1. Array

`Array` is a `final` class template representing an array
with internal storage.
It maintains the backing memory _M_ as a member variable.

#### 1.1.1. Template Parameters

`Array` has two template parameters:

1. The type `typename T`

1. The size `FwSizeType S`

`Array` statically asserts that `S > 0`.

#### 1.1.2. Private Member Variables

`Array` has one private variable `m_elements` for
storing the array elements.
It is a primitive C++ array of type `T[S]`.

#### 1.1.3. Construction and Destruction

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

_Example:_
```c++
Array<U32, 3> a({ 1, 2, 3 });
```

**Single-item constructor:**

```c++
Array(const T& elt)
```

Initialize each element of `m_elements` with `elt`.

_Example:_
```c++
Array<U32, 3> a(10);
```

**Copy constructor:**

```c++
Array(const Array<S,T>& a)
```

Initialize the elements of `m_elements` with the
elements of `a.m_elements`.

_Example:_
```c++
Array<U32, 3> a1(10);
Array<U32, 3> a2(a1);
```

**Destructor:**

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

1. Assert that `i < S`.

1. Return a reference to `m_elements[i]`.

_Example:_
```c++
Array<U32, 10> a;
ASSERT_EQ(a[0], 0);
a[0]++;
ASSERT_EQ(a[0], 1);
```

**operator=:**

```c++
Array<T,S>& operator=(const Array<T,S>& a)
```

1. If `&a == this` then do nothing.

1. Otherwise overwrite each element of `m_elements` with the corresponding
element of `a`.

_Example:_
```c++
Array<U32, 10> a1(1);
Array<U32, 10> a2(2);
a1 = a2;
```

**getElements:**

```c++
T[S]& getElements()
const T[S]& getElements() const
```

Return a reference to `m_elements`.

_Example:_
```c++
Array<U32, 10> a;
auto& elements1 = a.getElements();
ASSERT_EQ(elements1[0], 0);
elements1[0] = 1;
const auto& elements2 = a.getElements();
ASSERT_EQ(elements2[0], 1);
```

#### 1.1.5. Public Static Functions

**getSize:**

```c++
static constexpr FwSizeType getSize()
```

Return the size `S` of the array.

_Example:_
```c++
const auto size = Array<U32, 10>::getSize();
ASSERT_EQ(size, 10);
```

### 1.2. External Array

TODO

## 2. Queues

### 2.1. FIFO Queue

TODO

### 2.2. LIFO Queue

TODO

## 3. Linked List

TODO

## 4. Sets and Maps

### 4.1. Array Set and Map

TODO

### 4.2. Hash Set and Map

TODO

### 4.3. Balanced Binary Tree Set and Map

TODO
