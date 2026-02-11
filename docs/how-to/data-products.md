# How-To: Generate Data Products

This How-To describes **when to use data products**, **how to generate them in flight software**, **how to test them**, and provides guidance for **topology integration** and **ground decoding**. It is intended for engineers who are comfortable with F Prime components and want to add structured, store-and-forward mission data to their system.

This guide uses the [DataProduct/Producer](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/DataProduct/Producer) example from `fprime-examples` as a concrete example.

## Applicability

Use **data products** when you need to generate **structured mission data** that is:

- Produced on-board and **stored for later downlink**
- Potentially **large or bursty**, making it unsuitable for real-time telemetry
- Intended for **post-processing or scientific analysis**
- Managed by the system with **priorities, and cataloging**

Data products complement, rather than replace, other F Prime data mechanisms:

- **Telemetry** is best for continuous monitoring and trending.
- **Events** are best for discrete system state changes.
- **Data products** are best for *mission data* that is generated, accumulated, and delivered asynchronously.

Typical examples include:

- Science samples
- Image tiles or processed results
- Batched sensor data
- Periodic analysis outputs
- Stored system health data

## Design

A typical system using data products includes:

- One or more data product **producer components** (mission-specific)
- Framework services that manage storage and delivery:
    - `Svc.DpManager` for allocating containers
    - `Svc.DpWriter` for storing products to disk
    - `Svc.DpCatalog` for tracking products
    - `Svc.FileDownlink` for downlinking products

The producer itself is intentionally simple: it requests a container, fills it with records, and sends it off.  To model a producer, we need to define the following for the component:

> [!TIP]
> Most design work happens in your component's `.fpp` file where you define types, records, containers, and ports. Implementation happens in your component's `.cpp` file where you allocate, fill, and send containers.

- Define types
- Product records
- Product containers
- Product ports

### Define Types

A **record** is the smallest unit of data stored in a data product container. A record has a type that defines its structure and is any one of the FPP modeled [data types](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Types).

In our example, our record uses a FPP modeled struct containing a time tag and value:

```fpp
struct SinusoidDataType {
  timeTag: Fw.TimeValue
  value: F32
}
```

> [!TIP]
> Records do not have implicit time. If the time of the container is insufficient, users must include time fields in their record types.

### Declare Product Records

Each record type that may appear in a container must be declared as a **product record**.  These records must have a record name (e.g. `SineRecord`) and a type (e.g. `SinusoidDataType`). Each record must also have a unique numeric ID within the component this can be explicit or implicit (auto-assigned).

```python
active component Producer {
    # [ ... other code ... ]

    product record SineRecord: SinusoidDataType id 0
    product record CosineRecord: SinusoidDataType id 1
}
```

> [!NOTE]
> These IDs are serialized into the container so that ground software can interpret the record type correctly.

### Declare a Product Container

A **product container** groups records into a single "product" and defines how they are prioritized and managed. A container can be filled with any mix of records.

A container must have a name (e.g. SinusoidContainer), a unique numeric ID within the component that is explicit or implicit (auto-assigned), and a default priority level that influences storage and downlink behavior.

This example declares a container with ID `0` and default priority `10`:
```fpp
product container SinusoidContainer id 0 default priority 10
```

> [!TIP]
> Priority is only a default and may be overridden at runtime.

### Add Product Ports

A producer must include ports for:

* **Allocating** container backing memory
* **Sending** completed containers

Allocation supports both synchronous and asynchronous modes. In synchronous mode, the producer blocks until the container is returned. In asynchronous mode, the producer requests memory and is provided the container via callback.

In this example, synchronous allocation is used:

```fpp
product get port productGetOut
product send port productSendOut
```

## Implementation: Generating Data Products

Generating data products involves allocating a container, instantiating a record, assigning record value(s), and serializing the record into the container. The container may have any number of records of any type, in any order, and can be sent once it is "ready" according to some user policy.

### Track Container State

Producers typically track:

* Whether a container is currently allocated and successful
* If the container is "full" according to some policy

This creates a simple internal flow:

* Allocate → Fill → Send → Reset

Containers are of the type `DpContainer` in the component's base class, which derives from `Fw::Container`. Users typically instantiate the container in their header as a member variable.

```cpp
// In Component.hpp
DpContainer m_container; //! Tracked container state
```

This member variable can be filled by container allocation calls, and set via container send calls.

> [!CAUTION]
> It is very important to use the local `DpContainer` type provided by the autocoding and not the `Fw::DpContainer`.

### Compute Container Size

Container allocation requires an explicit data size (think: just like an `Fw::Buffer` allocation). When serializing records into a container, the record ID is also serialized (of size `sizeof(FwDpIdType)`). Therefore, for precise allocation, users must compute the size of all records to be stored in the container, including record IDs.

Our example intends to store `RECORD_COUNT` sine and cosine records per container:

```cpp
containerSize = 2 * RECORD_COUNT * // times two for both sine and cosine records
        // each record needs space for data size + size of record ID
        (SinusoidDataType::SERIALIZED_SIZE + sizeof(FwDpIdType))
```

> [!CAUTION]
> Record ID size is not automatically included in container data size because the number and types of records are up to the developer. The total allocated size is adjusted by the product get call to add the container overhead.

Allocation is then performed through an autocoded helper generated from the FPP model of the form `this->dpGet_<ContainerName>`. In our example, this becomes to:

```cpp
Fw::Success status = this->dpGet_SinusoidContainer(containerSize, this->m_container);
```

> [!CAUTION]
> Allocation may fail due to memory pressure. Producers must handle this case gracefully. [The example](https://github.com/nasa/fprime-examples/tree/devel/DataProduct/Producer/Producer.cpp) skips record generation when allocation fails and tries again on the next cycle.


### Serialize Records

Records must be serialized into the container payload. This is done by instantiating a record's data type, assigning values, and invoking the appropriate autocoded serialization method (`serializeRecord_<RecordName>(container, record)`). This serializes both the record ID and data into the container.

In our example, we create and serialize sine and cosine records as follows:

```cpp
SinusoidDataType sineRecord, cosineRecord;

sineRecord.timeTag = ...;
sineRecord.value = ...;
cosineRecord.timeTag = ...;
cosineRecord.value = ...;

this->m_container.serializeRecord_SineRecord(sineRecord);
this->m_container.serializeRecord_CosineRecord(cosineRecord);
```


### Send the Container

Once the container is "full" (by count, size, time, or other policy) it should be sent using the autocoded `dpSend` method. In our example, this becomes:

```cpp
this->dpSend(this->m_container);
```

> [!WARNING]
> Users must allocate a new container and avoid using the sent container again. The sent container is now owned by the framework and must not be modified.

## Testing

The F Prime unit test framework makes it straightforward to test data product producers. Users can assert on calls to product allocation and sending. They can also override the allocation handler to control allocation behavior for testing off-nominal behavior.

Unit tests should verify:

* Containers are allocated only when needed
* Records accumulate across multiple calls
* Containers are sent exactly when expected
* State resets correctly after send
* Allocation failures are handled gracefully

The unit test framework provides several helper assertions for this purpose:

- `ASSERT_PRODUCT_GET_SIZE` to verify a container allocation was requested (synchronous)
- `ASSERT_PRODUCT_GET` to verify a container allocation request parameters (asynchronous)
- `ASSERT_PRODUCT_SEND_SIZE` to verify a container was sent
- `ASSERT_PRODUCT_SET` to verify the sent container data

Users may also override the allocation handler to simulate allocation failures. For example, driving allocation failure via a member variable:

```cpp
Fw::Success::T ProducerTester ::productGet_handler(FwDpIdType id, FwSizeType dataSize, Fw::Buffer& buffer) {
    EXPECT_EQ(dataSize, sizeof(this->m_buffer));
    buffer.set(this->m_buffer, dataSize);
    this->pushProductGetEntry(id, dataSize);
    return (this->m_allocation_failure) ? Fw::Success::FAILURE : Fw::Success::SUCCESS;
}
```

## Topology Integration

Data product producers should be integrated with a `Svc.DpManager`, which is typically supplied by the `DataProducts` subtopology. Users should import the topology, connect their producers, connect the catalog to file downlink, and (optionally) configure the `DataProducts` subtopology parameters.

For example, in the project topology FPP file:

```fpp
topology ... {
    import FileHandling.Subtopology # For file downlink
    import DataProducts.Subtopology
    instance producer

    # Connect producers to DpManager
    connections DataProducers {
        producer.productGetOut  -> DataProducts.dpMgr.productGetIn
        producer.productSendOut -> DataProducts.dpMgr.productSendIn
    }

    # For connecting the catalog to the file downlink
    connections FileHandling_DataProducts{
        DataProducts.dpCat.fileOut             -> FileHandling.fileDownlink.SendFile
        FileHandling.fileDownlink.FileComplete -> DataProducts.dpCat.fileDone
    }
}
```

## Ground Decoding

Once a data product has been sent to the ground, ground software can decode it into a more easily readable JSON format. This can then be run through any number of tools:

```bash
fprime-dp decode --bin-file <data_product_file> --dictionary <path_to_dictionary> --output <output.json>
```

## Other Considerations

* **Synchronous vs asynchronous allocation**: synchronous allocation is simpler but may block; asynchronous allocation trades simplicity for responsiveness. Users should consider carefully which type of allocation is needed.
* **Container sizing strategy**: count-based, time-based, or size-based policies all work. Users should pick the strategy that best fits their data generation patterns.
* **Container Size**: Larger containers reduce the overhead of allocation and sending, but increase the amount of data stored in RAM. This data is at risk from unexpected restarts. Smaller containers reduce data loss during resets, but increase overhead.

## Conclusion

Data products provide a structured, scalable way to generate and manage mission data in F Prime. By clearly modeling records and containers, carefully managing allocation and serialization, and thoroughly testing both nominal and failure paths, producers can remain simple, deterministic, and flight-worthy—while enabling powerful ground-side analysis.

To dive deeper into the broader data products system in F´, see the [Data Products User Guide](../user-manual/framework/data-products.md) document.
