# Svc::ComRetry

## 1. Introduction

The `Svc::ComRetry` active component forwards messages from upstream to downstream components, resending messages on failure. Any topology requiring retry capabilities must place this component in the pipeline before a `ComStub` or `Radio` component. This component expects a `ComStatus` response per the [Communication Adapter Protocol](../../../docs/reference/communication-adapter-interface.md#communication-adapter-protocol). It acts as a pass-through component in case of a successful delivery, i.e. when it receives `Fw::Success::SUCCESS`. On receiving `Fw::Success::FAILURE`, it resends the message until it exceeds the maximum number of retries. After all retries are exhausted, it emits `Fw::Success::FAILURE` upstream, and the downstream communication adapter is responsible for eventually emitting a recovery `Fw::Success::SUCCESS` to resume data flow.

All `Svc::ComRetry` inputs are asynchronous: callers only enqueue a message, and all sends to the communication adapter (the first attempt and every resend) are issued from the `Svc::ComRetry` thread. Neither the upstream sender nor a thread delivering a status from the communication adapter is held while a buffer is delivered.

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
| SVC-COMRETRY-009 | `Svc::ComRetry` shall issue every send to the communication adapter, including resends, from its own thread, and its inputs shall not block the caller beyond enqueuing | Decouple the upstream and adapter threads from delivery and retry | Unit Test |
| SVC-COMRETRY-010 | `Svc::ComRetry` shall process its inputs in arrival order | Preserve the ownership-return-before-status ordering of the communication adapter protocol | Unit Test |
| SVC-COMRETRY-011 | `Svc::ComRetry` shall support communication adapters that answer from within the send call and adapters that answer later from another thread | Adapters such as radios return status synchronously and recover asynchronously | Unit Test |
| SVC-COMRETRY-012 | `Svc::ComRetry` input queuing shall be bounded, and overflow of the data and status inputs shall be a fatal assertion | Overflow indicates a protocol violation; silently dropping a status or buffer would stall data flow or leak a buffer | Unit Test |
| SVC-COMRETRY-013 | `Svc::ComRetry` shall respond to health pings by returning the ping key, dropping pings when its queue is full | Health monitoring of the component thread | Unit Test |

## 3. Design

### 3.1 Ports

`Svc::ComRetry` provides the same port names and types as the `Svc.Framer` interface, but its inputs are asynchronous.

| Kind | Name | Port Type | Usage |
|---|---|---|---|
| `async input` | `dataIn` | `Svc.ComDataWithContext` | Data to send, with its context |
| `output` | `dataOut` | `Svc.ComDataWithContext` | Data sent to the communication adapter |
| `output` | `dataReturnOut` | `Svc.ComDataWithContext` | Returns buffer ownership to the sender of `dataIn` |
| `async input` | `dataReturnIn` | `Svc.ComDataWithContext` | Receives buffer ownership back from the communication adapter |
| `async input` | `comStatusIn` | `Fw.SuccessCondition` | Status from the communication adapter |
| `output` | `comStatusOut` | `Fw.SuccessCondition` | Status forwarded upstream |
| `async input` | `pingIn` | `Svc.Ping` | Health ping (dropped when the queue is full) |
| `output` | `pingOut` | `Svc.Ping` | Health ping response |

`dataIn`, `dataReturnIn`, and `comStatusIn` use the default `assert` queue-full behavior. These inputs are commonly invoked by the communication adapter from within the `dataOut` call, i.e. on the `Svc::ComRetry` thread itself, so a `block` policy would deadlock and a `drop` policy would lose a buffer or status.

### 3.2 Behavior

`Svc::ComRetry` holds at most one buffer. When no buffer is pending, statuses are forwarded upstream unchanged. A buffer received on `dataIn` is sent on `dataOut` and its ownership is stored when it returns on `dataReturnIn`. On `Fw::Success::SUCCESS` the buffer is returned on `dataReturnOut` and the status is forwarded on `comStatusOut`. On `Fw::Success::FAILURE` with retries remaining, `Svc::ComRetry` waits; the next `Fw::Success::SUCCESS` from the communication adapter (its recovery status) triggers the resend. When retries are exhausted, the buffer is returned on `dataReturnOut` and `Fw::Success::FAILURE` is forwarded on `comStatusOut`.

### 3.3 Queue Depth

The [Communication Adapter Protocol](../../../docs/reference/communication-adapter-interface.md#communication-adapter-protocol) allows only one buffer in flight, which bounds the queue. A `dataIn` message is never queued alongside the adapter's answers to a previous buffer: `Svc::ComRetry` dequeues `dataIn` before calling `dataOut`, and the upstream sends the next buffer only after `Svc::ComRetry` forwards a status. Likewise, the initial start-up status is dequeued and forwarded before the upstream sends its first buffer. The largest backlog the protocol can produce is therefore three messages: `dataReturnIn`, a `comStatusIn` `Fw::Success::FAILURE`, and the adapter's recovery `Fw::Success::SUCCESS`. `Svc.Health` keeps at most one ping outstanding per component, adding one more.

**The queue depth of a `Svc::ComRetry` instance shall be at least 4.** A communication adapter that emits more than one status per send (e.g. duplicate recovery statuses) violates the protocol and requires additional depth.

### 3.4 Configuration

`configure(U32 num_retries)` sets the maximum number of retries (default 3). Instances need a queue size, stack size, and priority:

```
instance comRetry: Svc.ComRetry base id 0x1000 \
  queue size 4 \
  stack size Default.STACK_SIZE \
  priority 5
```

The `Svc::ComRetry` thread performs the call to the communication adapter's `dataIn` port, so its stack must accommodate that adapter's send path.
