# Os::Generic::LocklessPriorityQueue — Software Design Document

## 1. Purpose

`Os::Generic::LocklessPriorityQueue` is an implementation of `Os::QueueInterface`
that provides ISR-safe, lockless message passing with strict-priority delivery.
It is intended for flight-software contexts where a producer or consumer may run
in interrupt context and therefore cannot block on an OS-level mutex or
condition variable, but where the existing priority semantics of
`Os::Generic::PriorityQueue` must still be preserved.

## 2. Requirements

| ID | Description | Verification |
|----|-------------|--------------|
| OS-LOCKLESSQUEUE-001 | All memory is allocated exactly once during `create` through the registered `Fw::MemAllocator`. No allocation occurs during `send`, `receive`, `getMessagesAvailable`, or `getMessageHighWaterMark`. | Unit Test |
| OS-LOCKLESSQUEUE-002 | All non-blocking control paths terminate in bounded time proportional to the configured queue depth. | Unit Test, Inspection |
| OS-LOCKLESSQUEUE-003 | The non-blocking variants of `send` and `receive` are safe to call from interrupt context. They use only lock-free atomic operations and bounded `memcpy`. They do not invoke any OS service. | Inspection |
| OS-LOCKLESSQUEUE-004 | Consumers receive the highest-priority message. Equal-priority messages are delivered in FIFO order, matching the ordering contract of `Os::Generic::PriorityQueue`. | Unit Test |
| OS-LOCKLESSQUEUE-005 | A successful `send` followed by any number of `receive` calls delivers each message exactly once. No message is dropped, duplicated, or corrupted under concurrent producers and consumers. | Unit Test (TSan stress) |
| OS-LOCKLESSQUEUE-006 | The public surface conforms to `Os::QueueInterface` and is selectable as the default `Os::Queue` implementation through `register_fprime_implementation` in CMake. | Unit Test |

> [!CAUTION]
> This implementation is **not** a general-purpose lock-free O(log n)
> priority queue. Linear scans are accepted because flight queues are small
> (tens of slots) and because the predictable, depth-bounded scan time is
> exactly the property flight software wants.

## 3. Design Overview

The queue is a fixed pool of slots. Each slot owns its own atomic state machine
and the data backing it. Producers and consumers coordinate through per-slot
compare-and-swap (CAS) operations.

The absence of mutexes and condition variables is a deliberate design choice:
OS-level synchronization primitives cannot be used from interrupt context. By
relying exclusively on lock-free atomics, the non-blocking send/receive paths
satisfy the ISR-safety constraint without restricting the calling context.

```
+---------------------+         +-------------------------+
| LocklessSlot[0]     |  --->   | data[0 .. messageSize-1]|
|  m_stateTag (atomic)|         +-------------------------+
|  m_priority         |         | data[messageSize .. ]   |
|  m_size             |         |       ...               |
|  m_sequence         |         +-------------------------+
+---------------------+
| LocklessSlot[1]     |
+---------------------+
| ...                 |
+---------------------+
| LocklessSlot[depth] |
+---------------------+
```

Two separate allocations are made at `create` time:

1. `LocklessSlot[depth]` — one slot per message capacity.
2. `U8[depth * messageSize]` — the message-payload region.

Both allocations are released to the same `Fw::MemAllocator` at `teardown`.
After `create` succeeds, no further allocation occurs.

## 4. Slot State Machine

Each slot has a four-state lifecycle:

```
   FREE  --(producer CAS)-->  WRITING  --(producer release)-->  READY
    ^                                                              |
    |                                                              v
    +-----(consumer release)------  READING  <--(consumer CAS)-----+
```

State values are stored in the low `STATE_BITS` of an `std::atomic<U32>` named
`m_stateTag`. The remaining high bits hold a per-slot epoch tag that is
incremented on **every** transition. The epoch tag prevents ABA hazards: a
consumer that observes `(READY, tag=T)`, performs a non-atomic read of the
slot's priority and sequence, and then attempts to CAS to
`(READING, tag=T+1)` is guaranteed to fail if any other thread completed even
one round-trip on this slot in the meantime.

The slot's `m_size` field is written by the producer while the slot is in `WRITING` and read
by the consumer while the slot is in `READING`; it is only ever accessed under exclusive
ownership and is therefore non-atomic. The `m_priority` and `m_sequence` fields are
`std::atomic` with `relaxed` ordering because consumers read them during the scan phase
(while the slot is `READY`) without exclusive ownership — see §9. All three fields are written
by the producer during `WRITING` and become visible to consumers through the `release` store
that publishes `READY`. Consumers acquire those fields through a matching `acquire` load of
`m_stateTag`.

## 5. Send Algorithm

```
send(buffer, size, priority, blockType):
  if size > m_messageSize:
      return SIZE_MISMATCH
  for pass = 0 .. (NONBLOCKING ? MAX_RETRY_PASSES : infinity):
      for i = 0 .. depth - 1:
          packed = m_slots[i].m_stateTag.load(acquire)
          if state(packed) != FREE:
              continue
          desired = pack(WRITING, tag(packed) + 1)
          if CAS(m_slots[i].m_stateTag, packed -> desired):
              memcpy(data + i * messageSize, buffer, size)
              m_slots[i].m_size     = size
              m_slots[i].m_priority = priority
              m_slots[i].m_sequence = m_sequence.fetch_add(1, relaxed)
              m_slots[i].m_stateTag.store(pack(READY, tag(desired) + 1), release)
              count = m_count.fetch_add(1, acq_rel) + 1
              update_high_mark_with_bounded_cas(count)
              return OP_OK
      if NONBLOCKING and pass == MAX_RETRY_PASSES:
          return FULL
```

The outer loop is bounded for non-blocking callers and unbounded for blocking
callers. The blocking spin is the explicit contract of `BlockingType::BLOCKING`
and is not safe to call from ISR context.

## 6. Receive Algorithm

```
receive(destination, capacity, blockType, &actualSize, &priority):
  for pass = 0 .. (NONBLOCKING ? MAX_RETRY_PASSES : infinity):
      best = none
      for i = 0 .. depth - 1:
          packed = m_slots[i].m_stateTag.load(acquire)
          if state(packed) != READY:
              continue
          candidate_priority = m_slots[i].m_priority
          candidate_sequence = m_slots[i].m_sequence
          packed_recheck = m_slots[i].m_stateTag.load(acquire)
          if packed != packed_recheck:
              continue
          if best is none or
             candidate_priority > best.priority or
             (candidate_priority == best.priority and
              modular_less(candidate_sequence, best.sequence)):
              best = {i, candidate_priority, candidate_sequence, packed}
      if best is none:
          if NONBLOCKING and pass == MAX_RETRY_PASSES:
              return EMPTY
          continue
      desired = pack(READING, tag(best.packed) + 1)
      if CAS(m_slots[best.i].m_stateTag, best.packed -> desired):
          stored_size = m_slots[best.i].m_size
          assert stored_size <= capacity
          memcpy(destination, data + best.i * messageSize, stored_size)
          actualSize = stored_size
          priority   = m_slots[best.i].m_priority
          m_slots[best.i].m_stateTag.store(
              pack(FREE, tag(desired) + 1), release)
          m_count.fetch_sub(1, acq_rel)
          return OP_OK
      // CAS failed; loop and retry
```

The selection scan is bounded by `depth`. The recheck of `m_stateTag`
immediately after reading the `relaxed`-atomic priority/sequence fields catches the
case where a concurrent consumer has already begun draining the slot; in that
case the slot is skipped for this pass.

The CAS at the end of each pass uses the captured `packed` (state + tag) value
as the expected operand. Any concurrent transition on the slot — even one that
returned the slot to `READY` with a different message — increments the tag,
causing the CAS to fail and the pass to retry.

## 7. Sequence-Number Ordering and Wrap

`m_sequence` is a `std::atomic<U32>` global counter, incremented once per
successful publication. Per-message sequences are compared with unsigned modular
subtraction, so two values `a` and `b` with `(a - b)` having its top bit set are
ordered as `a < b` regardless of wrap. Because the queue holds at most `depth`
messages at once and `depth` is far less than `2^31`, the active set of
sequence values at any moment is always within the wrap-aware window. Wrap of
the global counter therefore never corrupts ordering.

## 8. Counters and the High-Water Mark

`m_count` tracks the current number of messages and is updated atomically by
producers (`fetch_add`) and consumers (`fetch_sub`). It is read atomically by
`getMessagesAvailable`.

`m_highMark` is updated by producers using a bounded compare-exchange loop.
In the worst case the loop terminates after a fixed number of iterations,
preserving the bounded-loop requirement. The high-water mark may lag the true
maximum by at most one if the loop exhausts its budget under extreme
contention; the existing
`Os::Generic::PriorityQueue::getMessageHighWaterMark` contract permits this
because it is an observability counter, not a correctness invariant.

## 9. Memory Ordering

The queue uses standard `std::atomic` operations with explicit memory orders:

- The transition `WRITING -> READY` uses `release` so that the payload
  (`memcpy`), `m_size` (non-atomic), `m_priority` (relaxed atomic), and
  `m_sequence` (relaxed atomic) become visible to any consumer that
  observes `READY` via an `acquire` load of `m_stateTag`.
- The transition `READING -> FREE` uses `release` so that subsequent producers
  observing `FREE` see the consumed slot as fully released.
- All scan loads use `acquire`. All CAS operations use `acq_rel` on success and
  `relaxed` on failure.
- `m_sequence`, `m_count`, and `m_highMark` use `relaxed` or `acq_rel` as
  appropriate; their values are not used to publish data, only to track
  counters and break priority ties.

## 10. ISR Safety

Non-blocking `send` and `receive` use only:

- Lock-free atomic loads, stores, fetch-add, fetch-sub, and CAS on
  `std::atomic<U32>`.
- `memcpy` over a region of size `m_messageSize`.

A runtime assertion in `create` rejects platforms where `std::atomic<U32>` is
not lock-free. (The check is runtime rather than `static_assert` because the
`is_always_lock_free` constexpr is C++17 and the project targets C++14. On
every supported flight target the atomic is in fact lock-free.)

There are no system calls, no OS-level synchronization primitives, and no
allocations on these paths. Both calls therefore satisfy the ISR-safety
contract for the platforms F Prime currently supports.

The blocking variants poll the same atomics but call
`std::this_thread::sleep_for` between bounded scans when no progress is
possible (queue empty for a consumer, queue full for a producer). The sleep is
a scheduling hint, not a synchronization primitive: it does not acquire any lock
and does not change the memory ordering of any subsequent atomic operation. It
is **only** reached on the BLOCKING path, so ISR callers — which must use
`BlockingType::NONBLOCKING` — never execute it.

The blocking spin must not be invoked from ISR context because ISR contexts
cannot make forward progress while preempting the threads they are waiting on.

## 11. Bounded Loops

| Operation        | Mode        | Bound                              |
| ---------------- | ----------- | ---------------------------------- |
| `send`           | NONBLOCKING | `depth * MAX_RETRY_PASSES`         |
| `send`           | BLOCKING    | unbounded by user contract         |
| `receive`        | NONBLOCKING | `depth * MAX_RETRY_PASSES`         |
| `receive`        | BLOCKING    | unbounded by user contract         |
| `create`         | n/a         | `depth` (initialization scan)      |
| `teardown`       | n/a         | `depth` (destructor scan)          |
| high-water-mark  | n/a         | `HIGH_MARK_CAS_BOUND`              |

The unbounded blocking spin is identical in nature to the
`condition_variable.wait()` loop used by `Os::Generic::PriorityQueue` and is
covered by the same caller-side discipline.

## 12. Memory Allocation

The queue uses the existing `OS_GENERIC_PRIORITY_QUEUE` enumerator from
`Fw::MemoryAllocation::MemoryAllocatorType`. No new allocator type is
introduced; system designers can configure either the existing
`Os::Generic::PriorityQueue` or this implementation against the same allocator.

### 12.1 Resource-Management Contract (`teardown()` vs destructor)

`create()` is the only allocating call in the queue's lifetime. Resource
release happens in `teardown()`, which delegates to a non-virtual private
helper `teardownInternal()`. `teardown()` is idempotent: only the first call
returns memory; subsequent calls are no-ops.

The destructor `~LocklessPriorityQueue()` is **intentionally empty**. Owners
must call `teardown()` explicitly before the queue (or its hosting
`Os::Queue`) is destroyed. Two reasons:

1. **Static destruction order.** A `LocklessPriorityQueue` may live inside a
   global / topology-scoped component that is destroyed at process exit.
   `teardownInternal()` calls
   `Fw::MemAllocatorRegistry::getInstance().getAnAllocator(...)` and then
   invokes the virtual `MemAllocator::deallocate`. The registry is itself a
   function-local static and its destruction order with respect to other
   globals is unspecified. If the destructor runs after the registry has
   already been destroyed, the virtual call dispatches through a v-table whose
   most-derived type is gone, and the program aborts with
   `pure virtual method called` (observed in upstream CI's
   `FppTest_topology_special_ports_ut_exe`, see PR nasa/fprime#5076).
2. **Consistency.** `Os::Generic::PriorityQueue::~PriorityQueue()` is also
   empty for the same reason. The lockless queue follows the same contract so
   it can be a drop-in replacement for the existing implementation.

Owners that fail to call `teardown()` leak the slot pool and message-data
region exactly as they would with the existing `Os::Generic::PriorityQueue`.

## 13. Selection at Build Time

The new module is registered through CMake as
`Os_Generic_LocklessPriorityQueue_Implementation`, with a corresponding
`register_fprime_implementation(Os_Generic_LocklessPriorityQueue ...)` block
that supplies a delegate file `DefaultLocklessPriorityQueue.cpp`. A test target
`LocklessPriorityQueueTest` chooses this implementation through
`CHOOSES_IMPLEMENTATIONS`. Any deployment that wants the lockless behavior can
adopt it through the same CMake mechanism the existing `Os_Generic_PriorityQueue`
uses.

## 14. Verification

The implementation is covered by the same shared queue tests used by
`Os::Generic::PriorityQueue`:

- `Os/test/ut/queue/CommonTests.cpp`: interface uninitialized, interface
  invalid, basic create/send/recv/overflow/underflow, and blocking
  send/receive (which uses real OS threads through `AggregatedConcurrentRule`).
- `Os/test/ut/queue/QueueRules.cpp`: a 10,000-step random rules scenario that
  interleaves all queue operations and validates against a reference shadow
  queue.

In addition, `Os/Generic/test/ut/LocklessPriorityQueueTests.cpp` adds two
lockless-specific tests:

- `LocklessConcurrent.MultiProducerMultiConsumer`: four producer threads and
  four consumer threads exchange 4,000 messages through a 64-slot queue. Every
  value must be received exactly once.
- `LocklessConcurrent.PriorityOrderSingleProducer`: 200 batches of 16
  increasing-priority messages are sent and drained; the receive order must
  be strictly non-increasing in priority.

All tests are compiled with AddressSanitizer, UndefinedBehaviorSanitizer, and
LeakSanitizer enabled, and pass under those sanitizers. ThreadSanitizer stress
tests provide additional coverage (see `LocklessPriorityQueueTsanTests.cpp`).

## 15. Limitations

- `receive` is O(`depth`) because it scans the slot array. For the
  flight-typical depths of tens of slots this is preferable to the dynamic
  bookkeeping required by an O(log n) lock-free priority queue.
- The blocking variants spin and are therefore not appropriate for ISR
  callers. ISR callers must use `BlockingType::NONBLOCKING`.
- The high-water mark is observability-only and may lag the true maximum by
  one under extreme contention.
