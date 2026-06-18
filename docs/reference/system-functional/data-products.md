
# Data Products Functionality

## References

- [F Prime DpManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/DpManager/docs/sdd.md)
- [F Prime DpWriter SDD](https://github.com/nasa/fprime/blob/devel/Svc/DpWriter/docs/sdd.md)
- [F Prime DpCatalog SDD](https://github.com/nasa/fprime/blob/devel/Svc/DpCatalog/docs/sdd.md)
- [Fw::Dp SDD](https://github.com/nasa/fprime/blob/devel/Fw/Dp/docs/sdd.md)
- [FPP User Guide — Data Products](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Data-Products)

## Overview

Data products provide a mechanism for components to collect structured data into named containers and write them to the file system for later downlink. Unlike telemetry channels that represent real-time state, data products capture larger datasets (science data, diagnostic dumps, logs) that are stored on-board and downloaded on a priority-driven schedule. The capability is provided by the Data Product Manager (buffer allocation), Data Product Writer (file storage), and Data Product Catalog (downlink management).

### Container and Record Model

Data products are defined in FPP models as containers and records:

- **Containers** — Named, prioritized wrappers for data product content. Each container has a unique ID, a priority, a time tag, optional processing flags, and user-configurable header data.
- **Records** — Individual data items within a container. Records can be single-value (one item of any FPP type) or array-type (a sequence of items).

The serialized container format includes a header, a header hash for integrity verification, the data records, and a data hash.

### Buffer Management

When a component needs to write a data product, it requests a buffer from the Data Product Manager. The manager allocates a buffer of the requested size and returns it to the component. The component populates the buffer with its container and record data, then sends the filled buffer to the Data Product Writer. Buffers can be requested either synchronously (blocking) or asynchronously (via request/response).

### Writing to Disk

The Data Product Writer receives filled data product buffers and writes them to the file system. It applies any configured processing (such as compression) before writing. Each container is written as an individual file in a project-specified directory structure. The writer reports successful writes via events.

### Catalog and Downlink

The Data Product Catalog manages the inventory and downlink of stored data products. Upon command, it scans the data product directories and builds a catalog of available products. It then initiates downlink of the cataloged products, ordered by the priority stored in each product file and by generation time. Additional commands allow operators to:

- Reprioritize data products for downlink
- Delete data products from storage
- Query the catalog status

### Configuration

- Data product directories are project-specific and configured at deployment time.
- Container IDs, priorities, and record definitions are specified in FPP models.
- Processing types (e.g., compression) are configurable via a bit mask in the container header.
- User-configurable header data provides a fixed-size area for mission-specific metadata.

### Off Nominal

- Buffer allocation failures result in an invalid buffer being returned; the requesting component must handle this case.
- File write failures are reported via events.
- If the catalog scan finds corrupt or unreadable data product files, they are skipped with a warning.
