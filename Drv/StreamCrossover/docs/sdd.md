\page DrvStreamCrossover Drv::StreamCrossover Stream Crossover Component
# Drv::StreamCrossover Stream Crossover Component

The Stream Crossover component allows a connection of byte stream driver model ports of type ByteStreamRecv and
ByteStreamSend.

## Design

The Drv::StreamCrossover utilizes the byte stream driver model to handle the incoming stream of bytes. Upon calling
the "streamIn" port, the `Fw::Buffer` containing the data will be forwarded to the "streamOut" port. This enables a
connection from a ByteStreamRecv port to a ByteStreamSend port.

## Port Descriptions
| Name | Description |
|---|---|
| streamOut       | A ByteStreamSend port for outgoing data stored in `Fw::Buffer` |
| streamIn        | A ByteStreamRecv port for incoming data stored in `Fw::Buffer` |
| errorDeallocate | Deallocate a `Fw::Buffer` on error |

## Requirements
Add requirements in the chart below
| Name | Description | Validation |
|---|---|---|
| STREAM-CROSSOVER-COMP-001 | The stream crossover component shall provide the capability to forward bytes | Unit Test |

## Change Log
| Date | Description |
|---|---|
| 2023-06-05 | Initial Draft |
| 2023-06-09 | Implement Error Handling |
