# MapEntryBase

`MapEntryBase` is an abstract class template
defined in [`Fw/DataStructures`](sdd.md).
It provides the user-facing interface for a map entry.

## 1. Template Parameters

`MapEntryBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Deleted elements

Because this is an abstract base class,
the copy constructor and copy assignment operator are deleted.

## 3. Protected Constructors and Destructors

`MapEntryBase` provides a protected zero-argument constructor
and a protected virtual destructor.
It uses the default implementations.

## 4. Public Member Functions

### 4.1. getKey

```c++
virtual const K& getKey() const = 0
```

Return a `const` reference to the key stored in the entry.

### 4.2. getValue

```c++
virtual const V& getValue() const = 0
```

Return a `const` reference to the value stored in the entry.
