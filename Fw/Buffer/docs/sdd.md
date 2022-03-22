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
m_bufferData | U8* | `getData()`/`setData()`       | Pointer to the raw memory wrapped by this buffer
m_size       | U32 | `getSize()`/`setSize()`       | Size of the raw memory region wrapped by this buffer
m_context    | U32 | `getContext()`/`setContext()` | Context of buffer's origin. Used to track buffers created by [`BufferManager`](../../../Svc/BufferManager/docs/sdd.md)
m_serialize_repr |  Fw::ExternalSerializeBuffer | `getSerializeRepr()` | Interface for serialization to internal buffer

If the size of the data is set to 0, the pointer returned by `getData()` and the the serialization interface returned by
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

### Serializing to `Fw::Buffer`

A given `Fw::Buffer`'s data can be serialized to using the serialization interface returned by `getSerializeRepr()`. The
serialization interface **is not** stored when passed between components and thus serialization should be performed
within one context.  The following C++ snippet shows the process for serializing data to a buffer.

```c++
U32 my_data = 10001;
U8  my_byte = 2;
Fw::ExternalSerializeBuffer& serializer = my_fw_buffer.getSerializeRepr();
serializer.resetSer();  // Return the serialization to the beginning of the memory region
serializer.serialize(my_data);
serializer.serialize(my_byte);
```

### Deserializing from `Fw::Buffer`

A given `Fw::Buffer`'s data can be deserialized using the serialization interface returned by `getSerializeRepr()`. The
serialization interface **is not** stored when passed between components and thus deserialization should be performed
within one context.  The following C++ snippet shows the process for deserializing data to a buffer.

```c++
U32 my_data = 0;
U8  my_byte = 0;
Fw::ExternalSerializeBuffer& deserializer = my_fw_buffer.getSerializeRepr();
deserializer.sbb.setBuffLen(buffer.getSize());  // Set available data for deserialization to the whole memory region
deserializer.deserialize(my_data);
deserializer.deserialize(my_byte);
```

**Note:** this interface is given as a convenience for working with `Fw::Buffers`, but does not guarantee that the
serialization location persists through a `Fw::BufferSend` call.  The typically pattern of serialize, send, deserialize
works as expected but multi-stage serialization or deserialization does not work without external management of the
serialization and deserialization pointers.