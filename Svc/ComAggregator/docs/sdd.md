# Svc::ComAggregator

Aggregates buffers in the downlink chain. This is for use with systems that have fixed size frames (e.g. CCSDS TM) that needed internal aggregation.

> [!CAUTION]
> `Svc::ComAggregator` does not preserve context.

## Requirements

| ID                    | Description                                                                                                                                                   | Verification |
| --------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------ |
| Svc-ComAggregator-001 | ComAggregator shall accept incoming downlink data as `Fw::Buffer`, `ComCfg::FrameContext` pairs and append the buffer into the aggregate space permitting     | Unit-Test    |
| Svc-ComAggregator-002 | ComAggregator shall hold the incoming buffer when there is insufficient space in the aggregate buffer.                                                        | Unit-Test    |
| Svc-ComAggregator-003 | ComAggregator shall send the current aggregate buffer when the incoming buffer is held due to overflow.                                                       | Unit-Test    |
| Svc-ComAggregator-004 | ComAggregator shall send the current aggregate buffer when it receives a timeout trigger if and only if the aggregate is non-empty.                           | Unit-Test    |
| Svc-ComAggregator-005 | ComAggregator shall clear aggregation state when a SUCCESS communication status is received back.                                                             | Unit-Test    |
| Svc-ComAggregator-006 | ComAggregator shall preserve the order of received buffers when forming each aggregate and across aggregate sends.                                            | Unit-Test    |
| Svc-ComAggregator-007 | ComAggregator shall inter operate with the [Communication Adapter Interface comStatus protocol](../../../docs/reference/communication-adapter-interface.md)   | Unit-Test    |


## Design

![Component Block Diagram](./img/diagram.svg)

`Svc.ComAggregator` implements `Svc.Framer`.  Additionally, it has a `Svc.Sched` timeout port enabling timeout to be driven via a rate group.
