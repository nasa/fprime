\page FwBufferSerializableBufferGetBufferSend Fw::Buffer Serializable / Fw::BufferGet Port / Fw::BufferSend Port
# Fw::Buffer Serializable / Fw::BufferGet Port / Fw::BufferSend Port

## 1 Introduction

This module provides the following elements:

* A type `Fw::Buffer` representing a wrapper around a variable-size buffer. This allows for passing a reference to the
allocated memory around without a copy. Typically the memory is allocated in a buffer manager or similar component but 
this is not required.
* A port `Fw::BufferGet` for requesting a buffer of type `Fw::Buffer` from
a [`BufferManager`](../../../Svc/BufferManager/docs/sdd.md) and similar components.

* A port `Fw::BufferSend` for sending a buffer of type `Fw::Buffer` from one component to another.

## 2 Design

The `Fw::Buffer` type wraps a pointer to memory and the size of that memory region. Thus, allowing users to pass the
pointer and size around as a pair without incurring a copy of the data at each step. **Note:** `Fw::Buffer` is not safe
to pass outside a given address space.

### 2.1 The Type Fw::Buffer

`Fw::Buffer` is a serializable class defining the following (private) fields. These fields are accessed through accessor functions.

Name | Type | Accessors | Purpose
---- | ---- | --------- | -------
`m_bufferData` | `U8*` | `getData()`/`setData()`       | Pointer to the raw memory wrapped by this buffer
`m_size`       | `U32` | `getSize()`/`setSize()`       | Size of the raw memory region wrapped by this buffer
`m_context`    | `U32` | `getContext()`/`setContext()` | Context of buffer's origin. Used to track buffers created by [`BufferManager`](../../../Svc/BufferManager/docs/sdd.md)
`m_serialize_repr` | `Fw::ExternalSerializeBuffer` | `getSerializeRepr()` | Interface for serialization to internal buffer

If the size of the data is set to 0, the pointer returned by `getData()` and the serialization interface returned by
`getSerializeRepr()` are considered invalid and should not be used.

The `getSerializeRepr()` function may be used to interact with the wrapped data buffer by serializing types to and from
the data region.


### 2.2 The Port Fw::BufferGet

As shown in the following diagram, `Fw::BufferGet` has one argument `size` of type `U32`. It returns a value of type
`Fw::Buffer`. The returned `Fw::Buffer`'s size must be checked for validity before using.

![`Fw::BufferGet` Diagram](img/BufferGetBDD.jpg "Fw::BufferGet Port")

### 2.3 The Port Fw::BufferSend

As shown in the following diagram, `Fw::BufferSend` has one argument `fwBuffer` of type `Fw::Buffer`.

![`Fw::BufferSend` Diagram](img/BufferSendBDD.jpg "Fw::BufferSend Port")

## 3 Usage Notes

Components allocating `Fw::Buffer` objects may use the `m_context` field at their discretion. This field is typically
used to track the origin of the buffer for eventual allocation. When a component fails to allocate memory, it must set
the `m_size` field to zero to indicate that the buffer is invalid.

Receivers of `Fw::Buffer` objects are expected to check the `m_size` field before using the buffer.

### Serializing and Deserializing with `Fw::Buffer`

Users can obtain a SerializeBuffer, `sb`, by calling `getSerializeRepr()`. This serialize buffer is backed by the memory
of the `Fw::Buffer` and is initially empty.  Users can serialize and deserialize through `sb` to copy to/from the backed
memory. 

The state of `sb` persists as long as the current `Fw::Buffer` object exists as it is stored as a member. However, all
`Fw::Buffer` constructors initialize `sb` to an empty state including the `Fw::Buffer` copy constructor. Thus, if an
`Fw::Buffer` is sent through a port call, passed by-value to a function call, or otherwise copied, the state of the new
buffer's `sb` member is empty.

**Serializing to `Fw::Buffer`**
```c++
U32 my_data = 10001;
U8  my_byte = 2;
Fw::ExternalSerializeBuffer& sb = my_fw_buffer.getSerializeRepr();
sb.resetSer();  // Return the serialization to the beginning of the memory region
sb.serialize(my_data);
sb.serialize(my_byte);
```

Since the initial state of `sb` is empty, deserialization requires setting the size of the data available for
deserialization. This can be done with the `sb.setBuffLen()` method passing in the `Fw::Buffer` size.

**Deserializing from `Fw::Buffer`**
```c++
U32 my_data = 0;
U8  my_byte = 0;
Fw::ExternalSerializeBuffer& sb = my_fw_buffer.getSerializeRepr();
sb.setBuffLen(buffer.getSize());  // Set available data for deserialization to the whole memory region
sb.deserialize(my_data);
sb.deserialize(my_byte);
```
