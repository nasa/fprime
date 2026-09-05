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
| Svc-ComAggregator-005 | ComAggregator shall clear aggregation state when a `Fw::Success::SUCCESS` communication status is received back.                                                             | Unit-Test    |
| Svc-ComAggregator-006 | ComAggregator shall preserve the order of received buffers when forming each aggregate and across aggregate sends.                                            | Unit-Test    |
| Svc-ComAggregator-007 | ComAggregator shall interoperate with the [Communication Adapter Interface protocol](../../../docs/reference/communication-adapter-interface.md). Specifically, it shall pass through `Fw::Success::SUCCESS` and `Fw::Success::FAILURE` statuses per the [Framer Status Protocol](../../../docs/reference/communication-adapter-interface.md#framer-status-protocol), including the initial start-up SUCCESS and any recovery SUCCESS following a FAILURE.   | Unit-Test    |
| Svc-ComAggregator-008 | ComAggregator shall provide a packet spanning configuration, disabled by default; with spanning disabled, behavior shall be unchanged.                        | Unit-Test    |
| Svc-ComAggregator-009 | With spanning enabled, when an incoming buffer does not fit in the remaining aggregate space, ComAggregator shall fill the remaining space with the buffer's leading bytes, send the full aggregate, and retain the remainder for subsequent aggregates. | Unit-Test    |
| Svc-ComAggregator-010 | With spanning enabled, a retained remainder shall be able to span one or more complete subsequent aggregates, which are sent as continuation-only aggregates. | Unit-Test    |
| Svc-ComAggregator-011 | With spanning enabled, ComAggregator shall report the CCSDS TM First Header Pointer for each aggregate via `ComCfg::FrameContext.firstHeaderPointer`: the offset of the first packet header starting in the aggregate, or `0x7FF` when no packet header starts in the aggregate (CCSDS 132.0-B-3 4.1.2.7.6). | Unit-Test    |
| Svc-ComAggregator-012 | With spanning enabled, ComAggregator shall fill residual aggregate space with an SPP idle packet before sending, spanning the idle packet into the next aggregate when the residual space is smaller than a minimum idle packet. | Unit-Test    |


## Design

![Component Block Diagram](./img/diagram.svg)

`Svc.ComAggregator` implements `Svc.Framer`.  Additionally, it has a `Svc.Sched` timeout port enabling timeout to be driven via a rate group.

### Threading Model

`Svc.ComAggregator` is an active component whose input ports are all `sync`: port handlers run on the caller's thread and do no work beyond sending a signal to the `AggregationMachine` state machine instance. State machine signals are internally enqueued on the component's message queue, so all state machine actions and guards execute serially on the component's own thread. This keeps callers non-blocking while ensuring the aggregation state (frame buffer, held buffer, last context) is only touched from one thread. The only state shared across threads is the buffer ownership flag, which is an atomic exchanged in `dataReturnIn_handler` (caller thread) and `doSend` (component thread).

### Packet Spanning

Calling `configure(true)` before startup enables CCSDS TM packet spanning. In this mode the aggregation capacity
expands to the full TM data field (`ComCfg::AggregationSpanningSize`) and every emitted aggregate is exactly that
size:

- A packet that does not fit in the remaining space is split: its leading bytes complete the current aggregate and
  the remainder is retained. Retention of the underlying buffer (and its return) follows normal buffer ownership;
  the buffer is only returned once fully consumed.
- After a successful send, if the retained remainder completely fills the next aggregate, that aggregate is sent
  immediately as a continuation-only aggregate (First Header Pointer = `0x7FF`), allowing a packet to span any
  number of complete frames.
- Residual space at send time (e.g. on timeout) is filled with an SPP idle packet (APID `0x7FF`). When fewer bytes
  remain than a minimum idle packet (header + 1 byte), the idle packet itself spans into the next aggregate.
- The First Header Pointer (the data-field offset of the first packet header starting in the aggregate) is
  reported through `ComCfg::FrameContext.firstHeaderPointer` and written into the TM Data Field Status by
  `Svc::Ccsds::TmFramer`, per CCSDS 132.0-B-3 section 4.1.2.7.6.

With spanning disabled (the default), incoming buffers are never split and behavior is unchanged; a buffer larger
than `ComCfg::AggregationSize` is rejected by assertion rather than truncated. `configure()` must be called before
any data is aggregated and asserts otherwise.
