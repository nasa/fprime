# SetConstIterator

TODO

`SetConstIterator` is a class for performing immutable iteration over a map.

## 1. Template Parameters

`SetConstIterator` has the following template parameters:

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Public Types

`SetConstIterator` defines the following public types:

|Name|Definition|
|----|----------|
|`EntryBase`|Alias of [`SetEntryBase<K, V>`](SetEntryBase.md)|

## 3. Constructors and Destructors

`SetConstIterator` provides the following constructors and destructors:

1. One constructor for each map implementation.
   The map implementations use these constructors to provide iterators.

1. A copy constructor.

1. A destructor.

## 4. Public Member Functions

`SetConstIterator` provides the following member functions.

### 4.1. operator=

Defined as `= default`.

### 4.2. operator==

```c++
bool operator==(const SetConstIterator& it)
```

Compare two `SetConstIterator` instances for equality.

1. If the implementations differ, then return `false`.

1. Otherwise check whether the implementations have equal values.

### 4.3. operator !=

```c++
bool operator!=(const SetConstIterator& it)
```

Return the negation of `operator=`.

### 4.4. operator++

```c++
SetConstIterator& operator++()
SetConstIterator& operator++(int)
```

Increment the iterator.

### 4.5. isInRange()

```c++
bool isInRange() const
```

Check whether the iterator is in range.

### 4.6. operator*

```c++
const EntryBase& operator*() const
```

Return a `const` reference to the `EntryBase` object
pointed to by the iterator.

### 4.7. operator->

```c++
const EntryBase* operator->() const
```

Return a pointer to the `const EntryBase` object
pointed to by the iterator.
