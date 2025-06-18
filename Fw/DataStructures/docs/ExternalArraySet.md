# ExternalArraySet

`ExternalArraySet` is a `final` class template
defined in [`Fw/DataStructures`](sdd.md).
It represents an array-based set with external storage.
Internally it maintains an [`ArraySetOrMapImpl<Entry>`](ArraySetOrMapImpl.md)
as the set implementation.

## 1. Template Parameters

`ExternalArraySet` has the following template parameters.

|Kind|Name|Purpose|
|----|----|-------|
|`typename`|`T`|The type of an element in the set|

## 2. Base Class

`ExternalArraySet` is publicly derived from
[`SetBase<T>`](SetBase.md).

## 3. Public Types

`ExternalArraySet` defines the following public types:

```c++
using Entry = SetOrMapIterator<K, V>
```

The type `SetOrMapIterator` is defined [here](SetOrMapIterator.md).

## 4. Private Member Variables

`ExternalArraySet` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_setImpl`|[`ArraySetOrMapImpl<Entry>`](ArraySetOrMapImpl.md)|The set implementation|C++ default initialization|

```mermaid
classDiagram
    ExternalArraySet *-- ArraySetOrMapImpl
```

## 5. Public Constructors and Destructors

### 5.1. Zero-Argument Constructor

```c++
ExternalArraySet()
```

Initialize each member variable with its default value.

_Example:_
```c++
ExternalArraySet<U16, U32> set;
```

### 5.2. Constructor Providing Typed Backing Storage

```c++
ExternalArraySet(Entry* entries, FwSizeType capacity)
```

1. Call `m_setImpl.setStorage(entries, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
```

### 5.3. Constructor Providing Untyped Backing Storage

```c++
ExternalArraySet(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#71-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#72-getbytearraysize) bytes.

1. Call `m_setImpl.setStorage(data, capacity)`.

1. Initialize the other member variables with their default values.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = ExternalArraySet<U16, U32>::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalArraySet<U16, U32>::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
ExternalArraySet<U16, U32> set(ByteArray(&bytes[0], sizeof bytes), capacity);
```

### 5.4. Copy Constructor

```c++
ExternalArraySet(const ExternalArraySet<K, V>& set)
```

Set `*this = set`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
ExternalArraySet<U16, U32>::Entry entries[capacity];
// Call the constructor providing backing storage
ExternalArraySet<U16, U32> m1(entries, capacity);
// Insert an item
const U16 key = 0;
const U32 value = 42;
const auto status = m1.insert(key, value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the copy constructor
ExternalArraySet<U16, U32> m2(m1);
ASSERT_EQ(m2.getSize(), 1);
```

### 5.5. Destructor

```c++
~ExternalArraySet() override
```

Defined as `= default`.

## 6. Public Member Functions

### 6.1. operator=

```c++
ExternalArraySet<K, V>& operator=(const ExternalArraySet<K, V>& set)
```

1. If `&set != this`

    1. Set `m_setImpl = set.m_setImpl`.

1. Return `*this`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
ExternalArraySet<U16, U32>::Entry entries[capacity];
// Call the constructor providing backing storage
ExternalArraySet<U16, U32> m1(entries, capacity);
// Insert an item
U16 key = 0;
U32 value = 42;
const auto status = m1.insert(value);
ASSERT_EQ(status, Success::SUCCESS);
// Call the default constructor
ExternalArraySet m2;
ASSERT_EQ(m2.getSize(), 0);
// Call the copy assignment operator
m2 = m1;
ASSERT_EQ(m2.getSize(), 1);
```

### 6.2. at

```c++
const V& at(FwSizeType index) const
```

Return `m_setImpl[index]`.

_Example:_
```c++
constexpr FwSizeType capacity = 3;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
const auto status = set.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(set.at(0), 1);
ASSERT_DEATH(set.at(1), "Assert");
```

### 6.3. clear

```c++
void clear() override
```

Call `m_setImpl.clear()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
const auto status = set.enqueue(3);
ASSERT_EQ(set.getSize(), 1);
set.clear();
ASSERT_EQ(set.getSize(), 0);
```

### 6.4. find

```c++
Success find(const K& key, V& value) override
```

1. Set `status = Success::FAILURE`.

1. Set `iterator = m_setImpl.find(key)`.

1. If `iterator != nullptr`

    1. Set `value = iterator.getValue()`.

    1. Set `status = Success::SUCCESS`.

1. Return `status`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
U32 value = 0;
auto status = set.find(0, value);
ASSERT_EQ(status, Success::FAILURE);
status = set.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
status = set.find(0, value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(value, 1);
```

### 6.5. getCapacity

```c++
FwSizeType getCapacity() const override
```

Return `m_setImpl.getCapacity()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
ASSERT_EQ(set.getCapacity(), capacity);
```

### 6.6. getHeadIterator

```c++
const Iterator* getHeadIterator const override
```

The type `Iterator` is defined [in the base class](SetBase.md#2-publictypes).

Return `m_impl.getHeadIterator()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
const auto* e = set.getHeadIterator();
FW_ASSERT(e == nullptr);
set.insert(0, 1);
e = set.getHeadIterator();
FW_ASSERT(e != nullptr);
ASSERT_EQ(e.getKey(), 0);
ASSERT_EQ(e.getValue(), 1);
```

### 6.7. getSize

```c++
FwSizeType getSize() const override
```

Return `m_setImpl.getSize()`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
auto size = set.getSize();
ASSERT_EQ(size, 0);
const auto status = set.enqueue(3);
ASSERT_EQ(status, Success::SUCCESS);
size = set.getSize();
ASSERT_EQ(size, 1);
```

### 6.8. insert

```c++
Success insert(const K& key, const V& value) override
```

Return `m_setImpl.insert(key, value)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
auto size = set.getSize();
ASSERT_EQ(size, 0);
const auto status = set.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
size = set.getSize();
ASSERT_EQ(size, 1);
```

```c++
Success insert(const Iterator& e) override
```

Call `insert(e.getKey(), e.getValue())`.

### 6.9. remove

```c++
Success remove(const K& key, V& value) override
```

Return `m_setImpl.remove(key, value)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32>::Entry entries[capacity];
ExternalArraySet<U16, U32> set(entries, capacity);
auto size = set.getSize();
ASSERT_EQ(size, 0);
auto status = set.insert(0, 1);
ASSERT_EQ(status, Success::SUCCESS);
size = set.getSize();
ASSERT_EQ(size, 1);
// Key does not exist
U32 value = 0;
status = set.remove(10, value);
ASSERT_EQ(status, Success::FAILURE);
ASSERT_EQ(size, 1);
// Key exists
status = set.remove(0, value);
ASSERT_EQ(status, Success::SUCCESS);
ASSERT_EQ(size, 0);
ASSERT_EQ(value, 1);
```

### 6.10. setStorage (Typed Data)

```c++
void setStorage(Entry* entries, FwSizeType capacity)
```

1. Call `m_setImpl.setStorage(entries, capacity)`.

_Example:_
```c++
constexpr FwSizeType capacity = 10;
ExternalArraySet<U16, U32> set;
ExternalArraySet<U16, U32>::Entry entries[capacity];
set.setStorage(entries, capacity);
```

### 6.11. setStorage (Untyped Data)

```c++
void setStorage(ByteArray data, FwSizeType capacity)
```

`data` must be aligned according to 
[`getByteArrayAlignment()`](#71-getbytearrayalignment) and must
contain at least [`getByteArraySize(size)`](#72-getbytearraysize) bytes.

1. Call `m_entries.setStorage(data, capacity)`.

1. Call `clear()`.

```c++
constexpr FwSizeType capacity = 10;
constexpr U8 alignment = ExternalArraySet<U16, U32>::getByteArrayAlignment();
constexpr FwSizeType byteArraySize = ExternalArraySet<U16, U32>::getByteArraySize(capacity);
alignas(alignment) U8 bytes[byteArraySize];
ExternalArraySet<U16, U32> set;
set.setStorage(ByteArray(&bytes[0], sizeof bytes), capacity);
```

## 7. Public Static Functions

### 7.1. getByteArrayAlignment

```c++
static constexpr U8 getByteArrayAlignment()
```

Return `ExternalArray<T>::getByteArrayAlignment()`.

### 7.2. getByteArraySize

```c++
static constexpr FwSizeType getByteArraySize(FwSizeType capacity)
```

Return `ExternalArray<T>::getByteArraySize(capacity)`.
