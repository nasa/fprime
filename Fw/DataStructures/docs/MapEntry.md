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

## 2. Private Member Variables

`MapEntry` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_key`|`K`|The key|C++ default initialization|
|`m_value`|`V`|The value|C++ default initialization|

## 3. Private Constructors

### 3.1. Copy Constructor

```c++
MapEntry(const MapEntry<K,V>& map)
```

Defined as `= default`.

## 4. Protected Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
MapEntry()
```

Defined as `= default`.

### 4.2. Destructor

```c++
MapEntry()
```

Defined as `= default`.

## 5. Private Member Functions

### 5.1. operator=

```c++
MapEntry& operator=(const MapEntry&)
```

Defined as `= default`.

## 6. Public Member Functions

### 6.1. getKey

```c++
const K& getKey() const
```

Get a reference to the key.

### 6.2. getValue

```c++
const V& getValue() const
```

Get a reference to the value.

### 6.3. getNextEntry

```c++
Fw::Success getNextEntry(MapEntry<K,V>& entry)
```

If the map has a next entry _E_, then set `entry`
to _E_ and return `SUCCESS`.
Otherwise return `FAILURE`.
