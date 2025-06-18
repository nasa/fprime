# SetOrMapIterator

`SetOrMapIterator` is a class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an iterator for a set or a map.

## 1. Template Parameters

`SetOrMapIterator` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`KE`|The type of a key in a map or the element of a set|
|`typename`|`V`|The type of a value in a map; unused in a set|

## 2. Base Class

`SetOrMapIterator<KE, V>` is publicly derived from the following
templates:

1. [`MapIterator<KE, V>`](MapIterator.md).

1. [`SetIterator<KE>`](SetIterator.md).

## 3. Private Member Variables

`SetOrMapIterator` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_keyOrElement`|`KE`|The map key or set element|C++ default initialization|
|`m_value`|`V`|The value|C++ default initialization|
|`m_next`|`SetOrMapIterator<KE, V>*`|Pointer to the next iterator or `nullptr` if none|`nullptr`|

## 4. Public Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
SetOrMapIterator()
```

Defined as `= default`.

### 4.2. Constructor Providing Members

```c++
SetOrMapIterator(const KE& keyOrElement, const V& value, SetOrMapIterator<KE, V>* next = nullptr)
```

1. Set `m_keyOrElement = keyOrElement`.

2. Set `m_value = value`.

3. Set `m_next = next`.

### 4.3. Copy Constructor

```c++
SetOrMapIterator(const SetOrMapIterator<KE, V>& map)
```

Defined as `= default`.

### 4.4. Destructor

```c++
~SetOrMapIterator() override
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator=

```c++
SetOrMapIterator& operator=(const SetOrMapIterator&<KE, V>)
```

Defined as `= default`.

### 5.3. getElement

```c++
const KE& getElement() const
```

Return a reference to `m_keyOrElement`.

### 5.2. getKey

```c++
const KE& getKey() const override
```

Return a reference to `m_keyOrElement`.

### 5.3. getValue

```c++
const V& getValue() const
```

Return a reference to `m_value`.

### 5.4. getNextIterator

```c++
SetOrMapIterator<KE, V> getNextIterator()
```

Return `m_next`.

### 5.4. getNextMapIterator

```c++
MapIterator<KE, V> getNextMapIterator() override
```

Return `m_next`.

### 5.5. getNextSetIterator

```c++
SetIterator<KE>* getNextSetIterator()
```

Return `m_next`.

### 5.6. setKeyOrElement

```c++
void setKeyOrElement(const KE& keyOrElement) const
```

Set `m_keyOrElement = keyOrElement`.

### 5.8. setNextIterator

```c++
void setNextIterator(SetOrMapIterator<KE, V>* next)
```

Set `m_next = next`.

### 5.7. setValue

```c++
void setValue(const V& value) const
```

Set `m_value = value`.
