# Utils::Types

This directory contains a library of helper types.

## Circular Buffer

This type uses a circular buffer to implement a bounded FIFO stream,
i.e., a logical store that can grow to a maximum
size and can shrink.
The logical store is byte addressable with addresses
0 through _n - 1_, where _n_ is the current store size.
It grows by increasing the top address from _n - 1_
to _n + m - 1_ and copying _m_ bytes of data into the
logical memory so allocated.
It shrinks by deleting the lowest _m_ addresses
and renumbering the logical addresses of the
remaining bytes starting at zero.

The implementation uses a fixed-size physical store.
The logical store is represented as a base or head index into
the physical store and an allocated size.
Initially both are zero.
When data is added to the circular buffer, the allocated size grows.
When data is removed from the circular buffer, the allocated
size shrinks, and the head pointer advances.

The allocated store size never exceeds the physical store size.
However, when the head index is greater than zero, the sum of
the head index and the allocated size may exceed
the physical store size.
In this case, the logical store wraps around to the beginning
of the physical store.
Further, deleting data while in this state may cause
the head index to wrap around.

`CircularBuffer` does not provide concurrency control.
If multiple threads use the buffer, the uses must
be guarded by other concurrency control, e.g.,
a queue or lock.

The `CircularBuffer` type provides the following operations.

### Constructor

```c++
CircularBuffer(U8* const buffer, const NATIVE_UINT_TYPE size)
```

Construct a circular buffer with the given physical store,
specified as a starting pointer and a size in bytes.

### Adding Data

```c++
Fw::SerializeStatus serialize(const U8* const buffer, const NATIVE_UINT_TYPE size);
```

If the current logical store size plus `size` exceeds
the maximum logical store size, then return an error.
Otherwise increase the logical store size by
`size` bytes and copy `size` bytes starting at `buffer`
into the new logical memory.

The operation is called `serialize` following F Prime practice.
No data is actually serialized (the data is copied byte for byte).

### Reading Data

```c++
Fw::SerializeStatus peek(char& value, NATIVE_UINT_TYPE offset = 0) const;
```

If `offset` is not a valid address of the logical store,
then return an error.
Otherwise read a `char` value at address `offset` of the logical store
and store the result into `value`.

```c++
Fw::SerializeStatus peek(U8& value, NATIVE_UINT_TYPE offset = 0) const;
```

Same as previous, but read a `U8` value.

```c++
Fw::SerializeStatus peek(U32& value, NATIVE_UINT_TYPE offset = 0) const;
```

If `offset` through `offset` + 3 are not valid addresses
in the logical store, then return an error.
Otherwise read four bytes of the logical store starting at `offset`,
interpret them as an unsigned 32-bit integer in big endian order,
and store the result into `value`.

```c++
Fw::SerializeStatus peek(U8* buffer, NATIVE_UINT_TYPE size, NATIVE_UINT_TYPE offset = 0) const;
```

If `offset` through `offset + size - 1` are not all valid
addresses in the logical store, then return an error.
Otherwise copy `size` bytes starting at `offset` into
the memory starting at `buffer`.

### Deleting Data

```c++
Fw::SerializeStatus rotate(NATIVE_UINT_TYPE amount);
```

If the logical store size _s_ is less than `amount`, then
return an error.
Otherwise delete `amount` bytes from the bottom of the
logical store: reassign the bytes at addresses `amount`
through _s_ - 1 to addresses zero through _s_ - `amount` - 1,
and set the logical store size to _s_ - `amount`.

### Querying Buffer State

```c++
NATIVE_UINT_TYPE get_allocated_size() const;
```

Return the number of allocated bytes, i.e., the
current logical store size.
This is the maximum number of bytes that may be read from
the logical store without adding data.

```c++
NATIVE_UINT_TYPE get_free_size() const;
```

Return the number of free bytes, i.e., the
maximum logical store size minus the current logical store size.
This is the number of bytes that may be added to the logical
store without deleting data.

```c++
NATIVE_UINT_TYPE get_capacity() const;
```

Return the maximum logical store size (equal to the physical store size).
This is the total number of bytes that may be added to an empty
circular buffer.
