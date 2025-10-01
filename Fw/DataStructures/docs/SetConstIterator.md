# SetConstIterator

`SetConstIterator` is a class for performing immutable iteration over a set.
The iteration order is not specified.

## 1. Template Parameters

`SetConstIterator` has the following template parameters:

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an element in the set|

## 2. Constructors and Destructors

`SetConstIterator` provides the following constructors and destructors:

1. One constructor for each set implementation.
   The set implementations use these constructors to provide iterators.

1. A copy constructor.

1. A destructor.

## 3. Public Member Functions

`SetConstIterator` provides the following member functions.

### 3.1. operator=

Defined as `= default`.

### 3.2. operator==

```c++
bool operator==(const SetConstIterator& it)
```

Compare two `SetConstIterator` instances for equality.

1. If the implementations differ, then return `false`.

1. Otherwise check whether the implementations have equal values.

### 3.3. operator !=

```c++
bool operator!=(const SetConstIterator& it)
```

Return the negation of `operator=`.

### 3.4. operator++

```c++
SetConstIterator& operator++()
SetConstIterator& operator++(int)
```

Increment the iterator.

### 3.5. isInRange()

```c++
bool isInRange() const
```

Check whether the iterator is in range.
It is a runtime error to attempt to access a set element through an iterator
for which `isInRange` evaluates to `false`.
In this case an assertion failure will occur.

### 3.6. operator*

```c++
const EntryBase& operator*() const
```

Return a `const` reference to the `T` element
pointed to by the iterator.
If the iterator is not in range for the map, an assertion failure will occur.
It is not recommended to use this operation
after updating the set that the iterator points to.

### 3.7. operator->

```c++
const EntryBase* operator->() const
```

Return a pointer to the `const T` element
pointed to by the iterator.
If the iterator is not in range for the map, an assertion failure will occur.
It is not recommended to use this operation
after updating the set that the iterator points to.
