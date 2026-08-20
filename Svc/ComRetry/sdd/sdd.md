# Svc::ComRetry

## 1. Introduction

The `Svc::ComRetry` component forwards messages from upstream to downstream components, resending messages on failure. Any topology requiring retry capabilities must place this component in the pipeline before a `ComStub` or `Radio` component. This component expects a `ComStatus` response per the [Communication Adapter Protocol](../../../docs/reference/communication-adapter-interface.md#communication-adapter-protocol). It acts as a pass-through component in case of a successful delivery, i.e. when it receives `Fw::Success::SUCCESS`. On receiving `Fw::Success::FAILURE`, it resends the message until it exceeds the maximum number of retries. After all retries are exhausted, it emits `Fw::Success::FAILURE` upstream, and the downstream communication adapter is responsible for eventually emitting a recovery `Fw::Success::SUCCESS` to resume data flow.

`Svc::ComRetry` can be used alongside the other F´ communication components (`Svc::Framer`, `Svc::Deframer`, `Svc::ComQueue`).

## 2. Requirements

| Requirement     | Description                                                                                                                 | Rationale                                                   | Verification Method |
|-----------------|-----------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|---------------------|
| SVC-COMRETRY-001 | `Svc::ComRetry` shall accept incoming downlink data as `Fw::Buffer` and pass them to an `Svc.ComDataWithContext` port                    | The component must forward messages without modifying them | Unit Test           |
| SVC-COMRETRY-002 | `Svc::ComRetry` shall store `Fw::Buffer` and its context on receiving buffer ownership through `dataReturnIn` | Store the buffer in case a retry is required  | Unit test           |
| SVC-COMRETRY-003 | `Svc::ComRetry` shall pause delivery on receiving `Fw::Success::FAILURE` | `Svc::ComRetry` should not send to a failing communication adapter.  | Unit test           |
| SVC-COMRETRY-004 | `Svc::ComRetry` shall resend `Fw::Buffer` on receiving `Fw::Success::SUCCESS` after prior failure if retries are available | Retry delivery of buffer  | Unit test           |
| SVC-COMRETRY-005 | `Svc::ComRetry` shall pass through the initial start-up `Fw::Success::SUCCESS` and any non-data statuses upstream when no buffer is pending  | The initial SUCCESS must reach `Svc::ComQueue` to initiate data flow per the [Communication Queue Protocol](../../../docs/reference/communication-adapter-interface.md#communication-queue-protocol)  | Unit test           |
| SVC-COMRETRY-006 | The maximum number of retries shall be configurable | The number of retries should be adaptable for projects  | Inspection           |
| SVC-COMRETRY-007 | `Svc::ComRetry` shall return buffer ownership to the upstream component on receiving `Fw::Success::SUCCESS` or after all retry attempts fail | Memory management       | Unit Test           |
| SVC-COMRETRY-008 | `Svc::ComRetry` shall send `ComStatus` upstream on successful delivery or after all retry attempts fail                             | Status of message delivery must be passed up the stack      | Unit Test           |

## 3. Design

`Svc::ComRetry` implements `Svc.Framer`.
