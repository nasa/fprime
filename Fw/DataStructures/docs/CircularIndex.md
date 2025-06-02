# 1. CircularIndex

`CircularIndex` represents an index value that
wraps around modulo an integer.

## 1. Private Member Variables

`CircularIndex` has the following private member variables.

|Name|Type|Purpose|Default Value|
|----|----|-------|-------------|
|`m_value`|`FwSizeType`|The index value|0|
|`m_modulus`|`FwSizeType`|The modulus|1|

## 2. Public Constructors and Destructors

**Zero-argument constructor:**

```c++
CircularIndex()
```

Initialize the member variables with their default values.

**Constructor with specified members:**

```c++
CircularIndex(FwSizeType modulus, FwSizeType value = 0)
```

1. Assert `modulus > 0`.

1. Set `m_modulus = modulus`.

1. Call `setValue(value)`.

## 3. Public Member Functions

**getValue:**

```c++
FwSizeType getValue() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_value`.

**setValue:**

```c++
void setValue(FwSizeType value)
```

Set `m_value = m_value % m_modulus`.

**getModulus:**

```c++
FwSizeType CircularIndex::getModulus() const
```

1. Assert `m_value < m_modulus`.

1. Return `m_modulus`.

**setModulus:**

```c++
void setModulus(FwSizeType modulus)
```

1. Set `m_modulus = modulus`.

2. Call `setValue(m_value)`.

**increment:**

```c++
FwSizeType increment(FwSizeType amount = 1)
```

1. Set `offset = amount % m_modulus`.

1. Call `setValue(m_value + offset)`.

1. Return `m_value`.

**decrement:**

```c++
FwSizeType decrement(FwSizeType amount = 1)
```

1. Set `offset = amount % m_modulus`.

1. Call `setValue(m_value + m_modulus - offset)`.

1. Return `m_value`.

