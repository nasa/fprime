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
|`m_nextEntry`|`MapEntry<K, V>`|The next map entry|`nullptr`|

## 3. Private Constructors

### 3.1. Copy Constructor

```c++
MapEntry(const MapEntry<K,V>& map)
```

Defined as `= default`.

## 4. Public Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
MapEntry()
```

Defined as `= default`.

### 4.2. Constructor Providing Members

```c++
MapEntry(const K& key, const V& value, MapEntry<K, V>* nextEntry = nullptr)
```

1. Set `m_key = key`.

2. Set `m_value = value`.

3. Set `m_nextEntry = nextEntry`.

### 4.3. Destructor

```c++
MapEntry()
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator=

```c++
MapEntry& operator=(const MapEntry&)
```

Defined as `= default`.

### 5.2. getKey

```c++
const K& getKey() const
```

Return a reference to the `m_key`.

### 5.3. getValue

```c++
const V& getValue() const
```

Return a reference to `m_value`.

### 5.4. getNextEntry

```c++
MapEntry<K, V>* getNextEntry()
```

Return `m_nextEntry`.

### 5.5. setKey

```c++
void setKey(const K& key) const
```

Set `m_key = key`.

### 5.6. setValue

```c++
setValue(const V& value) const
```

Set `m_value = value`.

### 5.7. setNextEntry

```c+
setNextEntry(MapEntry<K, V>* nextEntry)
```

Set `m_nextEntry = nextEntry`.
