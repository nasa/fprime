# CircularIndex

`CircularIndex` is a `final` class defined in
[`Fw/DataStructures`](sdd.md).
It represents an index value that
wraps around modulo an integer.

## 1. Private Member Variables

`CircularIndex` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_value`|`FwSizeType`|The index value|0|
|`m_modulus`|`FwSizeType`|The modulus|1|

## 2. Public Constructors and Destructors

### 2.1. Zero-Argument Constructor

```c++
CircularIndex()
```

Use default initialization of members.

### 2.2. Constructor with Specified Members

```c++
explicit CircularIndex(FwSizeType modulus, FwSizeType value = 0)
```

1. Assert `modulus > 0`.

1. Set `m_modulus = modulus`.

1. Call `setValue(value)`.

### 2.3. Copy Constructor

```c++
CircularIndex(const CircularIndex& ci)
```

Set `*this = ci`.

### 2.4. Destructor

```c++
~CircularIndex()
```

Defined as `= default`.

## 3. Public Member Functions

### 3.1. operator=

```c++
CircularIndex& operator=(const CircularIndex& ci)
```

1. If `this != &ci`

    1. Set `m_value = ci.m_value`.

    1. Set `m_modulus = ci.m_modulus`.

1. Return `*this`.

### 3.2. getValue

```c++
FwSizeType getValue() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_value`.

### 3.3. setValue

```c++
void setValue(FwSizeType value)
```

1. Assert `m_modulus > 0`.

2. Set `m_value = m_value % m_modulus`.

### 3.4. getModulus

```c++
FwSizeType CircularIndex::getModulus() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_modulus`.

### 3.5. setModulus

```c++
void setModulus(FwSizeType modulus)
```

1. Set `m_modulus = modulus`.

2. Call `setValue(m_value)`.

### 3.6. increment

```c++
FwSizeType increment(FwSizeType amount = 1)
```

1. Assert `m_modulus > 0`.

1. Set `offset = amount % m_modulus`.

1. Call `setValue(m_value + offset)`.

1. Return `m_value`.

### 3.7. decrement

```c++
FwSizeType decrement(FwSizeType amount = 1)
```

1. Assert `m_modulus > 0`.

1. Set `offset = amount % m_modulus`.

1. Call `setValue(m_value + m_modulus - offset)`.

1. Return `m_value`.

