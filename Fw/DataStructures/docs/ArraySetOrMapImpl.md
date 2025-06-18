# ArraySetOrMapImpl

`ArraySetOrMapImpl` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an array-based implementation of a set or map.
Internally it maintains an [`ExternalArray`](ExternalArray.md) for
storing the entries in the set or map.

## 1. Template Parameters

`ArraySetOrMapImpl` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`KE`|The type of a key in a map or the element of a set|
|`typename`|`V`|The type of a value in a map; unused in a set|

## 2. Public Types

`ArraySetOrMapImpl` defines the following types:
```c++
using Iterator = SetOrMapIterator<KE, V>
```

The type `SetOrMapIterator` is defined [here](SetOrMapIterator.md).

## 3. Private Member Variables

`ArraySetOrMapImpl` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_entries`|[`ExternalArray<Iterator>`](ExternalArray.md)|The array for storing the set or map entries|C++ default initialization|
|`m_size`|`FwSizeType`|The number of entries in the map|0|

## 4. Public Constructors and Destructors

### 4.1. Zero-Argument Constructor

```c++
ArraySetOrMapImpl()
```

Initialize each member variable with its default value.

### 4.2. Constructor Providing Typed Backing Storage

```c++
ArraySetOrMapImpl(Iterator* entries, FwSizeType capacity)
```

1. Call `setStorage(entries, capacity)`.

1. Initialize the other member variables with their default values.

### 4.3. Constructor Providing Untyped Backing Storage

```c++
ArraySetOrMapImpl(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#61-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#62-getbytearraysize) bytes.

1. Call `setStorage(data, capacity)`.

1. Initialize the other member variables with their default values.

### 4.4. Copy Constructor

```c++
ArraySetOrMapImpl(const ArraySetOrMapImpl<K, V>& map)
```

Set `*this = map`.

### 4.5. Destructor

```c++
~ArraySetOrMapImpl()
```

Defined as `= default`.

## 5. Public Member Functions

### 5.1. operator=

```c++
ArraySetOrMapImpl<K, V>& operator=(const ArraySetOrMapImpl<K, V>& map)
```

1. If `&map != this`

    1. Set `m_entries = map.m_entries`.

    1. Set `m_size = map.m_size`.

1. Return `*this`.

### 5.2. at

```c++
const Iterator& at(FwSizeType index) const
```

1. Assert `index < m_size`.

1. Return `m_entries[index]`.

### 5.3. clear

```c++
void clear()
```

Set `m_size = 0`.

### 5.4. find

```c++
const Iterator* find(const K& key)
```

1. Set `result = nullptr`.

1. For `i` in `[0, m_size)`

    1. Let `const auto& e = &m_entries[i]`.

    1. If `e.getKey() == key`

        1. Set `result = e`.

        1. Break out of the loop.

1. Return `result`.

### 5.5. getCapacity

```c++
FwSizeType getCapacity() const
```

Return `m_entries.getSize()`.

### 5.6. getHeadIterator

```c++
const Iterator* getHeadIterator const
```

1. Set `result = nullptr`.

1. If `m_size > 0`

    1. Assert `m_entries != nullptr`.

    1. Set `result = &m_entries[0]`.

1. Return `result`.

### 5.7. getSize

```c++
FwSizeType getSize()
```

Return `m_size`.

### 5.8. insert

```c++
Success insert(const KE& keyOrElement, const V& value)
```

1. Set `status = Success::FAILURE`.

1. For `i` in `[0, m_size)`

    1. Let `auto& e = m_entries[i]`.

    1. If `e.getKey() == e.keyOrElement`

        1. Call `e.setValue(value)`.

        1. Set `status = Success::SUCCESS`.

        1. Break out of the loop

1. If `(status == Success::FAILURE) && (m_size < getCapacity())`

    1. Set `m_entries[m_size] = Iterator(keyOrElement, value)`.

    1. If `m_size > 0` then
       call `m_entries[m_size - 1].setNextIterator(&m_entries[m_size])`.

    1. Increment `m_size`.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

### 5.9. remove

```c++
Success remove(const K& key, V& value)
```

1. Set `status = Success::FAILURE`.

1. For `i` in `[0, m_size)`

    1. If `m_entries[i].getKey() == key`

        1. If `i < m_size - 1` then

            1. `m_entries[i] = m_entries[m_size - 1]`.

            1. Call `m_entries[i].setNextIterator(&m_entries[i + 1])`.

        1. Otherwise call `m_entries[i].setNextIterator(nullptr)`.

        1. Decrement `size`.

        1. Set `status = Success::SUCCESS`.

        1. Break out of the loop.

1. Return `status`.

### 5.10. setStorage (Typed Data)

```c++
void setStorage(T* entries, FwSizeType capacity)
```

1. Call `m_entries.setStorage(entries, capacity)`.

1. Call `clear()`.

### 5.11. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#61-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#62-getbytearraysize) bytes.

1. Call `m_entries.setStorage(data, capacity)`.

1. Call `clear()`.

## 6. Public Static Functions

### 6.1. getByteArrayAlignment

```c++
static constexpr U8 getByteArrayAlignment()
```

Return `ExternalArray<Iterator>::getByteArrayAlignment()`.

### 6.2. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType capacity)
```

Return `ExternalArray<Iterator>::getByteArraySize(capacity)`.
