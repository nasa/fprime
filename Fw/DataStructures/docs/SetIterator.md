# SetIterator

`SetIterator` is an abstract class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an iterator for a set.

## 1. Template Parameters

`SetIterator` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an element in the set|

## 2. Private Constructors and Destructors

### 2.1. Copy Constructor

```c++
SetIterator(const SetIterator<T>& set)
```

Defined as `= delete`.

## 3. Protected Constructors and Destructors

### 3.1. Zero-Argument Constructor

```c++
SetIterator()
```

Defined as `= default`.

### 3.2. Destructor

```c++
virtual ~SetIterator()
```

Defined as `= default`.

## 4. Private Member Functions

### 4.1. operator=

```c++
SetIterator& operator=(const SetIterator<T>& setIterator)
```

Defined as `= delete`.

## 5. Public Member Functions

### 5.1. getElement

```c++
virtual const T& getElement() const = 0
```

Return a reference to the set element stored in the iterator.

### 5.2. getNextSetIterator

```c++
virtual const SetIterator<T>* getNextSetIterator() = 0
```

Return a pointer to the next iterator for the set, or `nullptr` if
there is none.
