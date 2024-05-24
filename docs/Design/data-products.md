# Data Products

## 1. Introduction

A **data product** is any data that is produced by an embedded
system, stored on board the system, and transmitted to the ground,
typically in priority order.
F Prime provides several features for managing the generation, storage,
and downlink of data products.
In this section, we document those features.

## 2. Basic Concepts

First we explain some basic concepts.

### 2.1. Records, Containers, and Dictionaries

F Prime data products are based on **records** and **containers**.
A record is a basic unit of data.
For example, it may be a struct, an array of typed objects of
statically known size, or an array of bytes of statically unknown size.
A container has an identifier and a priority and stores records.
In C++, a container is represented as a class object with member fields that
(1) store header data and (2) store an `Fw::Buffer` object pointing
to the memory that stores the records.

The set of all container specifications forms the **data product dictionary**.
To manage the data product dictionary, F Prime uses the same general approach
as for commands, telemetry, events, and parameters:

1. Each component _C_ specifies records and containers.
The container IDs are local to _C_.
Typically they have the values 0, 1, 2, ... .

2. Each instance _I_ of _C_ contributes one container _I.c_ to the
dictionary for each container _c_ defined in _C_.
The global identifier for _I.c_ is the base identifier of _I_ plus
the local identifier for _c_.
For example, if the base identifier is 0x1000, then the global identifiers
might be 0x1000, 0x1001, 0x1002, ... .

3. For any topology _T_, the global identifiers _I.c_ for all the component
instances of _T_ form the data product dictionary for _T_.

### 2.2. F Prime Components

Typically a data product system in an F Prime application consists of the following
components:

1. One or more **data product producers**.
   These components produce data products and are typically mission-specific.
   For example, they may produce science data.

1. Standard F Prime components for managing data products.

   1. A **data product manager**.
      This component allocates memory for empty containers.
      It also forwards filled containers to the data product writer.
      See [`Svc::DpManager`](../../Svc/DpManager/docs/sdd.md).

   1. A **data product writer**.
      This component receives filled containers from data product
      producers. It writes the contents of the containers to non-volatile
      storage. See [`Svc::DpWriter`](../../Svc/DpWriter/docs/sdd.md).

   1. A **data product catalog**.
      This component maintains a database of available data
      products. By command, it downlinks and deletes data products.
      See [`Svc::DpCatalog`](../../Svc/DpCatalog/docs/sdd.md).

   1. A **data product processor**.
      This component is not yet developed.
      When it is developed, it will perform in-memory processing on data
      product containers.

Note that when using data products, you need to develop only the
producer components. The other components are provided by F Prime.

## 3. Producer Components

In this section we provide more detail about producer components.

### 3.1. Activities

A producer component typically repeats the following activities,
as often as necessary:

1. Request a container from a data manager component.

2. When the container is received, serialize records into the
container.
This action fills the container with data.

3. When the container is full, send the container to the
data product manager, which forwards it to the data
product writer.

The FPP model and the autocoded C++ have several features that
support these activities.
We discuss these features in the following sections.

### 3.2. FPP Modeling

In this section we summarize the features of the FPP modeling
language used in constructing data product producer components.
Each of these features is fully documented in
[_The FPP User's Guide_](https://nasa.github.io/fpp/fpp-users-guide.html)
and [_The FPP Language Specification_](https://nasa.github.io/fpp/fpp-spec.html).

#### 3.2.1. Ports

FPP provides the following special ports for managing data products:

1. A **product get port** of type [`Fw::DpGet`](../../Fw/Dp/docs/sdd.md).
   This is an output port for synchronously requesting
   memory from a buffer manager.
   The request is served on the thread that invokes the port
   and causes a mutex lock to be taken on that thread.
   Example syntax:
   ```
   product get port productGetOut
   ```

1. A **product request port** of type [`Fw::DpRequest`](../../Fw/Dp/docs/sdd.md).
   This is an output port for asynchronously requesting memory
   from a data product manager.
   The request is served on the thread of the data product manager.
   This approach incurs the overhead of a separate thread, but it
   does not require the requesting thread to take a lock.
   Example syntax:
   ```
   product request port productRequestOut
   ```

1. A **product receive port** of type [`Fw::DpResponse`](../../Fw/Dp/docs/sdd.md).
   This is an input port for receiving an empty container in response
   to an asynchronous request. Example syntax:
   ```
   async product recv port productRecvIn
   ```

1. A **product send port** of type [`Fw::DpSend`](../../Fw/Dp/docs/sdd.md).
   This is an output port for sending a filled container
   to a data product writer. Example syntax:
   ```
   product send port productSendOut
   ```

Each data product producer component must have the following
ports in its component model:

1. One or both of a `product` `get` port and a `product` `request` port.

1. A `product` `send` port.

 A component that has a `product` `request` port must also have
 a `product` `receive` port.

#### 3.2.2. Records

A record is a unit of data.
When defining a producer component, you can specify one or more
records.
A record specification consists of a name, a type specifier, and an optional identifier.
The type specifier may be one of the following:

1. An FPP type _T_. In this case, the record contains a single value of type
   _T_. _T_ may be any FPP type, including a struct or array type.

1. An FPP type _T_ followed by the keyword `array`.
   In this case, the record is an array of values of type _T_
   of statically unknown size.
   The size of the array is stored in the record.

In either case, _T_ may be any FPP type, including a struct or array type.

Example syntax:
```
@ A struct with a fixed-size member array
struct FixedSizeData {
  data: [1024] F32
}
@ A record containing fixed-size data
product record FixedSizeDataRecord: FixedSizeData id 0x00
@ A record containing a variable-size array
product record F32ArrayRecord: F32 array id 0x01
```

#### 3.2.3. Containers

A container is a data structure that stores records.
When defining a producer component, you can specify one or more containers.
Each container specified in a component can store
any of the records specified in the component.

A container specification consists of a name, an optional
identifier, and an optional default priority.
The default priority is the priority to use if no
other priority is specified for the container
during operations.
Example syntax:
```
product container C1
product container C2 id 0x01 default priority 10
```

### 3.3. Autocoded C++

The autocoded C++ base class for a producer component _C_ provides
the following API elements:

1. Enumerations defining the available container IDs, container
priorities, and record IDs.

1. A member class _C_ `::DpContainer`. This class is derived from
[`Fw::DpContainer`](../../Fw/Dp/docs/sdd.md) and represents a container
specialized to the data products defined in _C_.
Each instance of _C_ `::DpContainer` is a wrapper for an `Fw::Buffer` _B_,
which points to allocated memory.
The class provides operations for serializing the records
defined in _C_ into the memory pointed to by _B_.
There is one operation _C_ `::DpContainer::serialize_` _R_
for each record _R_ defined in _C_.
For the serialized format of each record, see the documentation
for [`Fw::DpContainer`](../../Fw/Dp/docs/sdd.md).

1. If _C_ has a `product` `get` port, a member function `dpGet_`
_c_ for each container _c_ defined in _C_.
This function takes a data size and a reference
to a data product container _D_.
It invokes `productGetOut`, which is typically connected
to a data product manager component.
In the nominal case, the invocation returns an `Fw::Buffer` _B_ large enough
to store a data product packet with the requested data size.
The `dpGet` function then uses the ID and _B_ to initialize _D_.
It returns a status value indicating whether the buffer
allocation succeeded.

1. If _C_ has a `product` `request` port, a member function
`dpRequest_` _c_ for each container _c_ defined in _C_.
This function takes a data size.
It sends out a request on `productRequestOut`, which is
typically connected to a data product manager component.
The request is for a buffer large enough to store a data
product packet with the requested data size.

1. If _C_ has a `product` `recv` port, a pure virtual
member function `dpRecv_` _c_ `_handler` for each container _c_
defined in _C_.
When a fresh container arrives in response to a
`dpRequest` invocation, the autocoded C++ uses the container ID to
select and invoke the appropriate `dpRecv` handler.
The implementation of _C_ must override each handler
to provide the mission-specific behavior for filling
in the corresponding container.
The arguments to `dpRecv_` _c_ `_handler` provide
(1) a reference to the container, which the implementation can fill in;
and (2) a status value indicating whether the container
is valid. An invalid container can result if the buffer
allocation fails.

1. A member function `dpSend` for sending a filled
data product container.
This function takes a reference to a container _c_ and an
optional time tag.
It does the following:

   1. If no time tag is provided, then invoke `timeGetOut`
      to get the system time and use it to set the time tag.

   1. Store the time tag into _c_.

   1. Send _c_ on `productSendOut`.

1. Constant expressions representing the sizes of the records.

   1. If a record _R_ holds a single value, then
      the expression `SIZE_OF_` _R_ `_RECORD`
      evaluates to the size of that record.

   1. Otherwise _R_ is an array record. In this case
      the expression `SIZE_OF_` _R_ `_RECORD(` _size_ `)`
      evaluates to the size of an array record _R_ with
      _size_ array elements.

   You can use these expressions to compute data sizes
   when requesting data product buffers. For example,
   if a component specifies a record `Image`,
   then inside the component implementation the expression
   `10 * SIZE_OF_Image_RECORD` represents the size of the
   storage necessary to hold 10 `Image` records.

### 3.4. Unit Test Support

In F Prime, each component _C_ comes with auto-generated
classes _C_ `TesterBase` and _C_ `GTestBase` for writing
unit tests against _C_.
_C_ `GTestBase` is derived from _C_ `TesterBase`; it
provides test macros based on the Google Test framework.

To write unit tests, you construct a class _C_ `Tester`.
Typically _C_ `Tester` is derived from _C_ `GTestBase` and
uses the Google Test framework macros.
If for some reason you can't use the Google Test framework
(e.g., because you are running on a platform that does not support it),
then your _C_ `Tester` class can be derived from _C_ `TesterBase`.

This section documents the unit test support for producer components.

#### 3.4.1. The TesterBase Class

**History data structures:**
The class _C_ `TesterBase` provides the following histories:

1. If _C_ has a product get port,
then _C_ `TesterBase` has a history called `productGetHistory`.
Each element in the history is of type `DpGet`.
`DpGet` is a struct with fields storing the container ID and the
size emitted on the product get port.

1. If _C_ has a product request port, then _C_ `TesterBase` has a
corresponding history called `productRequestHistory`.
Each element in the history is of type `DpRequest`.
`DpRequest` is a struct with fields storing the container ID and the
size emitted on the product request port.

1. _C_ `TesterBase` has a history called `productSendHistory`.
Each element in the history is of type `DpSend`.
`DpSend` is a struct with fields storing the container ID and
a shallow copy of the buffer emitted on the product send port.

**History functions:**
The class _C_ `TesterBase` provides the following functions
for managing the histories:

1. If _C_ has a product get port, then _C_ `TesterBase` provides
   the following functions:

   1. `pushProductGetEntry`: This function takes a container ID and
      a size. It constructs the corresponding `DpGet` history object
      and pushes it on `productGetHistory`. Typically this function is
      called by `productGet_handler` (see below).

   1. `productGet_handler`: This function is called when the tester
      component receives data emitted on the product get port of the
      component under test. It takes a container ID, a size, and a
      mutable reference to a buffer _B_. By default it calls
      `pushProductGetEntry` with the ID and size and returns `FAILURE`,
      indicating that no memory was allocated and _B_ was not updated.
      This function is virtual, so you can override it with your own
      behavior. For example, your function could call `pushProductGetEntry`,
      allocate a buffer, store the allocated buffer into _B_, and return
      `SUCCESS`.

1. If _C_ has a product request port, then _C_ `TesterBase` provides
   the following functions:

   1. `pushProductRequestEntry`: This function takes a container ID and
      a size. It constructs the corresponding `DpRequest` history object
      and pushes it on `productRequestHistory`. Typically this function is
      called by `productRequest_handler` (see below).

   1. `productRequest_handler`: This function is called when the tester
      component receives data emitted on the product request port of the
      component under test. It takes a container ID and a size. By default
      it calls `pushProductRequestEntry` with the ID and size. This function
      is virtual, so you can override it with your own behavior.

1. _C_ `TesterBase` provides the following functions:

   1. `pushProductSendEntry`: This function takes a container ID and a
      const reference to a buffer.  It constructs the corresponding
      `DpSend` history object and pushes it on `productSendHistory`.
      Typically this function is called by `productSend_handler` (see below).

   1. `productSend_handler`: This function is called when the tester
      component receives data emitted on the product send port of the
      component under test. It takes a container ID and a const reference
      to a buffer. By default it calls `pushProductSendEntry` with the
      ID and buffer. This function is virtual, so you can override it
      with your own behavior.

#### 3.4.2. The GTestBase Class

**Testing macros:**
The class _C_ `GTestBase` provides the following macros for
verifying the histories managed by _C_ `TesterBase`.

1. If _C_ defines data products and has a product get port, then _C_
   `GTestBase` provides the following macros:

   1. `ASSERT_PRODUCT_GET_SIZE(size)`: This macro checks that `productGetHistory`
      has the specified size (number of entries).

   1. `ASSERT_PRODUCT_GET(index, id, size)`: This macro checks that
      `productGetHistory` has the specified container ID and size
      at the specified history index.

1. If _C_ defines data products and has a product request port,
   then _C_ `GTestBase` provides the following macros:

   1. `ASSERT_PRODUCT_REQUEST_SIZE(size)`: This macro checks that
      `productRequestHistory` has the specified size (number of entries).

   1. `ASSERT_PRODUCT_REQUEST(index, id, size)`: This macro checks that
      `productRequestHistory` has the specified container ID and size
      at the specified history index.

1. If _C_ defines data products, then _C_ `GTestBase` provides
   the following macros:

   1. `ASSERT_PRODUCT_SEND_SIZE(size)`: This macro checks that
      `productSendHistory` has the specified size (number of entries).

   1. `ASSERT_PRODUCT_SEND(index, id, priority, timeTag, procType, userData, dataSize, buffer)`:
      All the arguments of this macro are inputs (read-only) except `buffer`, which is
      a by-reference output and must be a variable of type `Fw::Buffer&`.
      This macro verifies the entry `entry` stored at the specified
      index of `productSendHistory`. It does the following:

      1. Check that `entry.id` matches the specified ID.

      1. Deserialize the data product header stored in `entry.buffer`.

      1. Check that the container ID, priority, time tag, processor type,
         user data, and data size stored in the deserialized header
         match the specified values.

      1. Assign `entry.buffer` to `buffer`. After this macro runs,
         the deserialization pointer of `buffer` points into the start
         of the data payload of `entry.buffer`. You can write additional
         code to deserialize and check the data payload.

**Container IDs:**
The container IDs emitted by the component under test are global
IDs.
Therefore, when constructing specified IDs you must add
the ID base specified in the tester component to the local
ID specified in the component under test.
For example, for container `CONTAINER` in component `Component`,
you would write
```cpp
ID_BASE + Component::ContainerId::CONTAINER
```
`ID_BASE` is a standard constant defined in each Tester implementation
and provided to the Tester base classes in their constructors.

## 4. Use Cases

In this section we discuss several common use cases involving
data products.

**Requesting and sending data products:**
See the example uses in the documentation for
[`Svc::DpManager`](../../Svc/DpManager/docs/sdd.md#5-example-uses).
The component referred to as `producer` in that document
is a data product producer.

**Writing data products to non-volatile storage:**
See the example uses in the documentation for
[`Svc::DpWriter`](../../Svc/DpWriter/docs/sdd.md#6-example-uses).
The component referred to as `producer` in that document
is a data product producer.

**Cataloging and downlinking data products:**
For a preliminary implementation of the data product catalog,
see [`Svc::DpCatalog`](../../Svc/DpCatalog/docs/sdd.md).

**Processing data products:**
TODO
