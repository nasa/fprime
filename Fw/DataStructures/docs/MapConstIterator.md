# MapConstIterator

`MapConstIterator` is a class for iterating over a map.

## 1. Template Parameters

`MapBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Constructors and Destructors

`MapConstIterator` provides the following constructors and destructors:

1. One constructor for each map implementation.
1. A copy constructor.
1. A destructor.

## 3. Public Member Functions

`MapConstIterator` provides the following member functions.

### 3.1. operator=

Defined as `= default`.

### 3.2. operator==

```c++
bool operator==(const MapConstIterator& it)
```

Compare two `MapConstIterator` instances for equality.

1. If the implementations differ, then return `false`.

1. Otherwise delegate the comparison to the common implementation.

### operator !=

```c++
bool operator!=(const MapConstIterator& it)
```

Defined as the negation of `operator=`.


### 3.3. operator++

```c++
MapConstIterator& operator++()
MapConstIterator& operator++(int)
```

Increment the iterator.

### 3.4. isInRange()

```c++
bool isInRange() const
```

Check whether the iterator is in range.
