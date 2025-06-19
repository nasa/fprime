# MapIterator

`MapIterator` is an abstract class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an iterator for a map.

## 1. Template Parameters

`MapIterator` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Private Constructors and Destructors

### 2.1. Copy Constructor

```c++
MapIterator(const MapIterator<K, V>& map)
```

Defined as `= delete`.

## 3. Protected Constructors and Destructors

### 3.1. Zero-Argument Constructor

```c++
MapIterator()
```

Defined as `= default`.

### 3.2. Destructor

```c++
virtual ~MapIterator()
```

Defined as `= default`.

## 4. Private Member Functions

### 4.1. operator=

```c++
MapIterator<K, V>& operator=(const MapIterator<K, V>&)
```

Defined as `= delete`.

## 5. Public Member Functions

### 5.1. getKey

```c++
virtual const K& getKey() const = 0
```

Return a reference to the key.

### 5.2. getValue

```c++
virtual const V& getValue() const = 0
```

Return a reference to the value.

### 5.3. getNextMapIterator

```c++
virtual const MapIterator<K, V>* getNextMapIterator() = 0
```

Return a pointer to the next map iterator, or `nullptr` if there is none.
