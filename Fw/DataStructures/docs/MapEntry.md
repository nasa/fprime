# MapEntry

`MapEntry` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an entry in a map.

## 1. Template Parameters

`MapEntry` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`K`|The type of a key in the map|
|`typename`|`V`|The type of a value in the map|

## 2. Private Constructors

### 2.1. Copy Constructor

```c++
MapEntry(const MapEntry<K,V>& map)
```

Defined as `= default`.

## 3. Protected Constructors and Destructors

### 3.1. Zero-Argument Constructor

```c++
MapEntry()
```

Defined as `= default`.

### 3.2. Destructor

```c++
MapEntry()
```

Defined as `= default`.

## 4. Private Member Functions

### 4.1. operator=

```c++
MapEntry& operator=(const MapEntry&)
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. getKey

```c++
const K& getKey() const
```

Get a reference to the key.

### 5.2. getValue

```c++
const K& getValue() const
```

Get a reference to the value.

### 5.3. getNextEntry

```c++
Fw::Success getNextEntry(MapEntry<K,V>& entry)
```

If the map has a next entry _E_, then set `entry`
to _E_ and return `SUCCESS`.
Otherwise return `FAILURE`.
