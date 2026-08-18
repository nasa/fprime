---
hide:
  - toc
---

# System Functional Documentation

The system functional documentation provides a functional or capability description of F Prime and its components. This includes both a high level description of the functionality and corresponding requirements.


### Core

- __[Framework](framework.md)__ - Core types, ports, components, serialization, commands, events, telemetry, parameters, time, buffers, and data products
- __[OSAL](osal.md)__ - Operating system abstraction layer for tasks, files, mutexes, queues, and system resources

### Capabilities

- __[Command Dispatch](command-dispatch.md)__ - Command decoding, routing, and status tracking
- __Dictionary__ - Dictionary (commands, telemetry, events, data products, parameters)
- __Sequencing__ - Command sequencing functionality
- __Telemetry__ - [Channel-based (TlmChan)](telemetry-chan.md) | [Packetized (TlmPacketizer)](telemetry-packetizer.md)
- __[Event Management](event-management.md)__ - Event collection, distribution, text logging, and fatal handling
- __[Rate Group Scheduling](rate-group-scheduling.md)__ - Periodic execution of components at configurable rates
- __[Health Monitoring](health-monitoring.md)__ - Component liveness verification and watchdog stroking
- __[File Management](file-management.md)__ - File transfer (uplink/downlink), file operations, and file dispatch
- __[Communication Stack](communication.md)__ - Data path between flight software and external systems
- __[CCSDS Protocol](ccsds-protocol.md)__ - CCSDS-standard space communication protocols
- __[Data Products](data-products.md)__ - Structured data collection, storage, and priority-driven downlink
- __[Buffer Management](buffer-management.md)__ - Memory allocation and buffer lifecycle services
- __[Parameter Management](parameters.md)__ - Persistent configuration value storage and retrieval
- __[Time Services](time-services.md)__ - System time sources and cycle generation
- __[System Services](system-services.md)__ - Resource monitoring, version reporting, assertion handling, and polymorphic database
- __[Hardware Drivers](hardware-drivers.md)__ - Byte stream, network, serial, GPIO, I2C, and SPI driver abstractions

### Subtopologies

- __[CDH Core](subtopology-cdh-core.md)__ - Reusable bundle of command dispatch, event management, health, version, and text logging
- __[ComFprime](subtopology-com-fprime.md)__ - F Prime protocol communication stack subtopology
- __[ComCcsds](subtopology-com-ccsds.md)__ - CCSDS protocol communication stack subtopology
- __[ComLoggerTee](subtopology-com-logger-tee.md)__ - Communication data splitter and logger subtopology
- __[Data Products](subtopology-data-products.md)__ - Data product lifecycle management subtopology
- __[File Handling](subtopology-file-handling.md)__ - File transfer, file management, and parameter storage subtopology


