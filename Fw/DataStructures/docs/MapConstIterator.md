# MapConstIterator

`MapConstIterator` is a class for performing immutable iteration over a map.
The iteration order is not specified.

## 1. Template Parameters

`MapConstIterator` has the following template parameters:

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Public Types

`MapConstIterator` defines the following public types:

|Name|Definition|
|----|----------|
|`EntryBase`|Alias of [`MapEntryBase<K, V>`](MapEntryBase.md)|

## 3. Constructors and Destructors

`MapConstIterator` provides the following constructors and destructors:

1. One constructor for each map implementation.
   The map implementations use these constructors to provide iterators.

1. A copy constructor.

1. A destructor.

## 4. Public Member Functions

`MapConstIterator` provides the following member functions.

### 4.1. operator=

Defined as `= default`.

### 4.2. operator==

```c++
bool operator==(const MapConstIterator& it)
```

Compare two `MapConstIterator` instances for equality.

1. If the implementations differ, then return `false`.

1. Otherwise check whether the implementations have equal values.

### 4.3. operator !=

```c++
bool operator!=(const MapConstIterator& it)
```

Return the negation of `operator=`.

### 4.4. operator++

```c++
MapConstIterator& operator++()
MapConstIterator& operator++(int)
```

Increment the iterator.

### 4.5. isInRange()

```c++
bool isInRange() const
```

Check whether the iterator is in range.
It is a runtime error to attempt to access a map entry through an iterator
for which `isInRange` evaluates to `false`.
In this case an assertion failure will occur.

### 4.6. operator*

```c++
const EntryBase& operator*() const
```

Return a `const` reference to the `EntryBase` object
pointed to by the iterator.
If the iterator is not in range for the map, an assertion failure will occur.
It is not recommended to use this operation
after updating the map that the iterator points to.

### 4.7. operator->

```c++
const EntryBase* operator->() const
```

Return a pointer to the `const EntryBase` object
pointed to by the iterator.
If the iterator is not in range for the map, an assertion failure will occur.
It is not recommended to use this operation
after updating the map that the iterator points to.
