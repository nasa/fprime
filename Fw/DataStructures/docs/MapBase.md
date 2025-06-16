# MapBase

`MapBase` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an abstract base class for a map.

## 1. Template Parameters

`MapBase` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Private Constructors

### 2.1. Copy Constructor

```c++
MapBase(const MapBase<K,V>& map)
```

Defined as `= delete`.

## 3. Protected Constructors and Destructors

### 3.1. Zero-Argument Constructor

```c++
MapBase()
```

Defined as `= default`.

### 3.2. Destructor

```c++
virtual MapBase()
```

Defined as `= default`.

## 4. Private Member Functions

### 4.1. operator=

```c++
MapBase& operator=(const MapBase&)
```

Defined as `= delete`.

## 5. Public Member Functions

### 5.2. clear

```c++
virtual void clear() = 0
```

Clear the map.

_Example:_
```c++
void f(MapBase<U32>& map) {
    map.clear();
    ASSERT_EQ(map.getSize(), 0);
}
```

### 5.3. copyDataFrom

TODO

### 5.6. getCapacity

TODO

### 5.7. getSize

TODO
