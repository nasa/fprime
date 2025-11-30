# Svc::ComRetry

## 1. Introduction

The `Svc::ComRetry` component forwards messages from upstream to downstream components, resending messages on failure. Any topology requiring retry capabilities must place this component in the pipeline before a `ComStub` or `Radio` component. This component expects a `ComStatus` response. It acts as a pass-through component in case of a successful delivery, i.e. when it receives `Fw::Success::SUCCESS`. On receiving `Fw::Success::FAILURE`, it resends the message until it exceeds the maximum number of retries.

`Svc::ComRetry` can be used alongside the other F´ communication components (`Svc::Framer`, `Svc::Deframer`, `Svc::ComQueue`).

## 2. Requirements

| Requirement     | Description                                                                                                                 | Rationale                                                   | Verification Method |
|-----------------|-----------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|---------------------|
| SVC-COMRETRY-001 | `Svc::ComRetry` shall accept incoming downlink data as `Fw::Buffer` and pass them to an `Svc.ComDataWithContext` port                    | The component must forward messages without modifying them | Unit Test           |
| SVC-COMRETRY-002 | `Svc::ComRetry` shall store `Fw::Buffer` and its context on receiving buffer ownership through `dataReturnIn` | Store the buffer in case a retry is required  | Unit test           |
| SVC-COMRETRY-003 | `Svc::ComRetry` shall resend the stored `Fw::Buffer` on receiving `Fw::Success::FAILURE` | Retry delivery of message  | Unit test           |
| SVC-COMRETRY-004 | The maximum number of retries shall be configurable | The number of retries should be adaptable for projects  | Inspection           |
| SVC-COMRETRY-005 | `Svc::ComRetry` shall return buffer ownership to the upstream component on receiving `Fw::Success::SUCCESS` or after all retry attempts fail | Memory management       | Unit Test           |
| SVC-COMRETRY-006 | `Svc::ComRetry` shall send `ComStatus` upstream on successful delivery or after all retry attempts fail                                  | Upstream component must receive status of message delivery from downstream                | Unit Test           |

## 3. Design

`Svc::ComRetry` implements `Svc.Framer`.
