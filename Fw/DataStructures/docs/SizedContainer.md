# SizedContainer

`SizedContainer` is a class
defined in [`Fw/DataStructures`](sdd.md).
It is an abstract class representing a sized container.

## 1. Private Constructors

### 1.1. Copy Constructor

```c++
SizedContainer(const SizedContainer<T>& c)
```

Defined as `= delete`.

## 2. Protected Constructors and Destructors

### 2.1. Zero-Argument Constructor

```c++
SizedContainer()
```

Use default initialization of members.

### 2.2. Destructor

```c++
virtual ~SizedContainer()
```

Defined as `= default`.

## 3. Private Member Functions

### 3.1. operator=

```c++
SizedContainer& operator=(const SizedContainer&)
```

Defined as `= delete`.

## 4. Public Member Functions

### 4.1. clear

```c++
virtual void clear() = 0
```

Clear the container.

_Example:_
```c++
void f(SizedContainer& c) {
    c.clear();
    ASSERT_EQ(c.getSize(), 0);
}
```

### 4.2. getCapacity

```c++
virtual FwSizeType getCapacity() const = 0
```

Return the current capacity.

_Example:_
```c++
void f(const SizedContainer& c) {
    const auto size = c.getSize();
    const auto capacity = c.getCapacity();
    ASSERT_LE(size, capacity);
}
```

### 4.3. getSize

```c++
virtual FwSizeType getSize() const = 0
```

Return the current size.

_Example:_
```c++
void f(const SizedContainer& c) {
    c.clear();
    auto size = c.getSize();
    ASSERT_EQ(size, 0);
}
```

### 4.4. isEmpty

```c++
bool isEmpty() const
```

Return `true` if the container is empty.

_Example:_
```c++
void f(const SizedContainer& c) {
    if (c.size() == 0) {
        ASSERT_TRUE(c.isEmpty());
    } else {
        ASSERT_FALSE(c.isEmpty());
    }
}
```

### 4.5. isFull

```c++
bool isFull() const
```

Return `true` if the container is full.

_Example:_
```c++
void f(const SizedContainer& c) {
    if (c.size() >= c.capacity()) {
        ASSERT_TRUE(c.isFull());
    } else {
        ASSERT_FALSE(c.isFull());
    }
}
```

