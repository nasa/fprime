
# Data Products Subtopology

## References

- [F Prime DpManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/DpManager/docs/sdd.md)
- [F Prime DpWriter SDD](https://github.com/nasa/fprime/blob/devel/Svc/DpWriter/docs/sdd.md)
- [F Prime DpCatalog SDD](https://github.com/nasa/fprime/blob/devel/Svc/DpCatalog/docs/sdd.md)
- [Fw::Dp SDD](https://github.com/nasa/fprime/blob/devel/Fw/Dp/docs/sdd.md)

## Overview

The Data Products subtopology packages the complete data product lifecycle — buffer allocation, data writing, and catalog-driven downlink — as a reusable building block. It provides the infrastructure for components to collect structured data products and store them on the file system for priority-ordered downlink.

### Included Components

- **Data Product Manager** (active) — Allocates buffers for data product containers in response to requests from producer components. Supports both synchronous (get) and asynchronous (request/response) allocation.
- **Data Product Writer** (active) — Receives filled data product buffers and writes them to the file system. Applies configured processing (e.g., compression) before writing.
- **Data Product Catalog** (active) — Manages the inventory of stored data products. On command, scans data product directories and downloads products in priority order.
- **Buffer Manager** (passive) — Provides dedicated buffer allocation for the data product path, configured with appropriate bin sizes and counts for data product containers.

### Internal Wiring

Within the subtopology:

1. The Data Product Manager requests buffers from the Buffer Manager.
2. Filled buffers are sent from the Manager to the Writer.
3. The Writer deallocates buffers back to the Buffer Manager after writing.
4. The Writer notifies the Catalog when new data products are written, so the catalog stays current.

### External Interfaces

The subtopology exposes ports for:

- **Product get/request/send** — Ports for producer components to request, receive, and send data product buffers.
- **File downlink** — Output port for the Catalog to request file downlink, and input port for completion notifications.
- **Schedule inputs** — Ports for rate group connections to drive telemetry output from the Manager, Writer, and Buffer Manager.

### Configuration

- Data product directory paths and state file location are configured at setup time.
- Base IDs, queue sizes, stack sizes, and priorities via DataProductsConfig.
- Buffer Manager bin sizes and counts are configured for the expected data product sizes.
