# Os::Generic::LocklessPriorityQueue — Software Design Document

## 1. Purpose

`Os::Generic::LocklessPriorityQueue` is an implementation of `Os::QueueInterface`
that provides ISR-safe, lockless message passing with strict-priority delivery.
It is intended for flight-software contexts where a producer or consumer may run
in interrupt context and therefore cannot block on an OS-level mutex or
condition variable, but where the existing priority semantics of
`Os::Generic::PriorityQueue` must still be preserved. Unlike the existing
ISR-safe `Os::Generic::PriorityMemQueue`, this implementation supports the
full `FwQueuePriorityType` priority domain with no per-priority configuration
and uses no OS primitive (semaphore) on any nonblocking path.

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

State values are stored in the low `STATE_BITS` of an atomic word named
`m_stateTag`, whose unsigned integral type `LocklessStateTagType` is
configurable in `config/LocklessQueueCfg.hpp` (default `U64`). The remaining
high bits hold a per-slot epoch tag that is
incremented on **every** transition. The epoch tag prevents ABA hazards: a
consumer that observes `(READY, tag=T)`, performs a non-atomic read of the
slot's priority and sequence, and then attempts to CAS to
`(READING, tag=T+1)` is guaranteed to fail if any other thread completed even
one round-trip on this slot in the meantime.

The tag occupies `TAG_BITS = bits(LocklessStateTagType) - STATE_BITS` bits
(60 with the default `U64` word) and wraps after `2^TAG_BITS` transitions of
a single slot. A stale CAS could therefore succeed only if a thread stalls
between its scan and its CAS while other threads drive that same slot through
an exact multiple of `2^TAG_BITS` transitions and the slot returns to the
same state. With the default 60-bit tag this requires a thread to remain
preempted across ~10^18 queue operations on one slot; with a `U32` word
(28-bit tag) the bound drops to tens of millions of operations.

**Consequence of a wrapped-tag stale CAS.** If this coincidence occurs, the
consequence is bounded: the stale CAS can only succeed against a slot that is
once again `READY`, so the consumer claims a *valid, complete* message — just
not necessarily the one it selected during its scan. The claimed message is
delivered intact and exactly once; no memory corruption, message loss, or
duplication is possible. The observable effect is a single dequeue that may
violate the strict priority/FIFO selection order (a one-time priority
inversion). Lock-freedom of `std::atomic<LocklessStateTagType>` is a
prerequisite for the ISR-safety guarantee: it is statically asserted from the
`ATOMIC_*_LOCK_FREE` macros where the platform guarantees it, and
runtime-asserted in `create()`. Platforms without lock-free 64-bit atomics
(some 32-bit targets) must configure a narrower type in
`config/LocklessQueueCfg.hpp` (`U32`, `U16`, or `U8`, with 28-, 12-, or 4-bit
tags respectively). This residual window is documented in §15.

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
  for pass = 0 .. (NONBLOCKING ? MAX_RETRY_PASSES - 1 : infinity):
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
              count = m_count.fetch_add(1, acq_rel) + 1
              raise_high_mark_to(count)  // CAS until mark >= count; bounded by depth
              m_slots[i].m_stateTag.store(pack(READY, tag(desired) + 1), release)
              return OP_OK
  return FULL
```

The count is incremented *before* the `release` store that publishes `READY`.
A consumer can only decrement after observing `READY`, so the decrement is
ordered after the increment and `m_count` can never transiently underflow.
The cost is that `getMessagesAvailable` may briefly over-report by counting a
message that is not yet receivable, which is benign for an observability
counter.

The outer loop is bounded for non-blocking callers and unbounded for blocking
callers. The blocking spin is the explicit contract of `BlockingType::BLOCKING`
and is not safe to call from ISR context.

**Spurious `FULL` under contention.** A slot held mid-operation by a concurrent
producer (`WRITING`) or consumer (`READING`) is not claimable. A non-blocking
send that exhausts its pass budget while every unclaimed slot is transiently
held returns `FULL` even though fewer than `depth` messages are logically
queued. This differs from the mutex-based `Os::Generic::PriorityQueue`, which
reports fullness exactly under its lock; it is the price of bounded, lock-free
progress. Callers for which a spurious `FULL` is unacceptable (e.g. async
ports configured to assert on overflow) should size the queue with margin or
select the mutex-based implementation.

## 6. Receive Algorithm

```
receive(destination, capacity, blockType, &actualSize, &priority):
  for pass = 0 .. (NONBLOCKING ? MAX_RETRY_PASSES - 1 : infinity):
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
          continue  // returns EMPTY when the NONBLOCKING pass budget is exhausted
      desired = pack(READING, tag(best.packed) + 1)
      if CAS(m_slots[best.i].m_stateTag, best.packed -> desired):
          stored_size = m_slots[best.i].m_size
          assert stored_size <= capacity
          memcpy(destination, data + best.i * messageSize, stored_size)
          actualSize = stored_size
          priority   = m_slots[best.i].m_priority
          m_count.fetch_sub(1, acq_rel)
          m_slots[best.i].m_stateTag.store(
              pack(FREE, tag(desired) + 1), release)
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

The count is decremented *before* the `release` store that frees the slot. A
producer can only re-claim (and re-count) the slot after observing `FREE`, so
`m_count` — and therefore the high-water mark — never exceeds `depth`.

**Spurious `EMPTY` under contention.** Symmetrically to send, a non-blocking
receive can return `EMPTY` while messages are transiently held in `WRITING`
or `READING` by concurrent threads.

## 7. Sequence-Number Ordering and Wrap

`m_sequence` is a `std::atomic<U32>` global counter, incremented once per
successful publication. Per-message sequences are compared with unsigned modular
subtraction, so two values `a` and `b` with `(a - b)` having its top bit set are
ordered as `a < b` regardless of wrap.

The comparison is exact only while the sequence values of queued
equal-priority messages span less than half the `U32` domain. `create()`
enforces `depth < 2^31`, which bounds the number of queued messages but not
the *spread* of their sequences: a message that remains queued while `2^31`
or more intervening sends occur (≈ 2.5 days of continuous 10 kHz traffic
through one queue) would compare as newest rather than oldest, perturbing
FIFO order among equal-priority messages until it drains. Priority ordering
is unaffected. A `U64` counter would remove the window but `std::atomic<U64>`
is not lock-free on all supported 32-bit flight targets, so the `U32`
counter is retained and the window is documented as a limitation (§15).

## 8. Counters and the High-Water Mark

`m_count` tracks the current number of messages and is updated atomically by
producers (`fetch_add`) and consumers (`fetch_sub`). It is read atomically by
`getMessagesAvailable`.

`m_highMark` is raised to the post-increment count by producers using a
compare-exchange loop that runs until the mark reflects the observed count.
The mark only increases and never exceeds `depth` (the count is incremented
before `READY` and decremented before `FREE`), so every strong-CAS failure
strictly raises the observed mark and the loop is bounded by `depth`
iterations. The high-water mark is therefore exact: it equals the maximum
number of messages the queue has held, and never exceeds `depth`.

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
  `std::atomic<LocklessStateTagType>` (the per-slot state-tag word, default
  `U64`; see `config/LocklessQueueCfg.hpp`), `std::atomic<U32>` (sequence,
  count, high-water mark), and `std::atomic<FwQueuePriorityType>` (per-slot
  priority).
- `memcpy` over a region of size `m_messageSize`.

Runtime assertions in `create` reject platforms where
`std::atomic<LocklessStateTagType>`, `std::atomic<U32>`, or
`std::atomic<FwQueuePriorityType>` is not lock-free. (The check is runtime rather than `static_assert` because the
`is_always_lock_free` constexpr is C++17 and the project targets C++14;
`static_assert`s derived from the width-matched `ATOMIC_*_LOCK_FREE` macros
cover all three types on platforms with a compile-time guarantee. On every supported flight
target the atomics are in fact lock-free.)

There are no system calls, no OS-level synchronization primitives, and no
allocations on these paths. Both calls therefore satisfy the ISR-safety
contract for the platforms F Prime currently supports.

The blocking variants poll the same atomics but call `Os::Task::delay`
between bounded scans when no progress is possible (queue empty for a
consumer, queue full for a producer). The delay is
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
| `teardown`       | n/a         | `depth` (slot destruction scan)    |
| high-water-mark  | n/a         | `depth` (mark increases monotonically) |

The unbounded blocking loop terminates only when the caller's condition is
satisfied, which is the explicit `BlockingType::BLOCKING` contract; unlike
`Os::Generic::PriorityQueue` it polls with a configurable backoff
(`LOCKLESS_QUEUE_BLOCKING_BACKOFF_US` in `config/LocklessQueueCfg.hpp`) rather
than blocking on a condition variable (see §15). This is a deliberate
deviation from the JPL fixed-loop-bound rule: the blocking contract has no
static bound, and the poll-with-backoff design is the price of keeping the
implementation free of OS synchronization primitives. Note that every idle
`BLOCKING` caller wakes once per backoff period, so deployments that select
this queue for their active components trade idle CPU for ISR safety.
`MAX_RETRY_PASSES` is likewise configurable as
`LOCKLESS_QUEUE_MAX_RETRY_PASSES` in `config/LocklessQueueCfg.hpp` (default 4).

## 12. Memory Allocation

The queue uses the existing `OS_GENERIC_PRIORITY_QUEUE` enumerator from
`Fw::MemoryAllocation::MemoryAllocatorType`. No new allocator type is
introduced; system designers can configure either the existing
`Os::Generic::PriorityQueue` or this implementation against the same allocator.

### 12.1 Resource-Management Contract (`teardown()` vs destructor)

`create()` is the only allocating call in the queue's lifetime. Resource
release happens in `teardown()`, which is idempotent: only the first call
returns memory; subsequent calls are no-ops.

The destructor `~LocklessPriorityQueue()` is **intentionally empty**. Owners
must call `teardown()` explicitly before the queue (or its hosting
`Os::Queue`) is destroyed. Two reasons:

1. **Static destruction order.** A `LocklessPriorityQueue` may live inside a
   global / topology-scoped component that is destroyed at process exit.
   `teardown()` calls
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
`CHOOSES_IMPLEMENTATIONS`. The implementation is **opt-in**: platform default
queues are unchanged (unix remains on `Os_Generic_PriorityQueue`), and any
deployment or platform that needs ISR-safe queueing can select
`Os_Generic_LocklessPriorityQueue` through the same CMake mechanism the
existing `Os_Generic_PriorityQueue` uses.

## 14. Verification

The implementation is covered by the same shared queue tests used by
`Os::Generic::PriorityQueue`:

- `Os/test/ut/queue/CommonTests.cpp`: interface uninitialized, interface
  invalid, basic create/send/recv/overflow/underflow, and blocking
  send/receive (which uses real OS threads through `AggregatedConcurrentRule`).
- `Os/test/ut/queue/QueueRules.cpp`: a 10,000-step random rules scenario that
  interleaves all queue operations and validates against a reference shadow
  queue.

In addition, `Os/Generic/test/ut/LocklessPriorityQueueTests.cpp` adds
lockless-specific tests, including:

- `LocklessConcurrent.MultiProducerMultiConsumer`: four producer threads and
  four consumer threads exchange 4,000 messages through a 64-slot queue. Every
  value must be received exactly once.
- `LocklessConcurrent.PriorityOrderSingleProducer`: 200 batches of 16
  increasing-priority messages are sent and drained; the receive order must
  be strictly non-increasing in priority.
- `LocklessLifetime` tests covering destruct-without-create,
  create/teardown/destruct ordering, teardown idempotency, oversized-send
  rejection (wrapper and delegate level), and zero-size message round-trip.

All tests are compiled with AddressSanitizer, UndefinedBehaviorSanitizer, and
LeakSanitizer enabled, and pass under those sanitizers. ThreadSanitizer stress
tests provide additional coverage (see `LocklessPriorityQueueTsanTests.cpp`),
run in CI by the dedicated `.github/workflows/tsan-lockless-queue.yml`
workflow on Linux and macOS.

## 15. Limitations

- `receive` is O(`depth`) because it scans the slot array. For the
  flight-typical depths of tens of slots this is preferable to the dynamic
  bookkeeping required by an O(log n) lock-free priority queue.
- The blocking variants poll with a configurable backoff rather than blocking
  on an OS primitive: an idle blocking receiver wakes every
  `LOCKLESS_QUEUE_BLOCKING_BACKOFF_US` (default 100 µs, see
  `config/LocklessQueueCfg.hpp`) instead of sleeping until a message
  arrives, and each message may see up to one backoff period of added
  latency. Deployments whose components idle on blocking receives should
  prefer the condition-variable-based `Os::Generic::PriorityQueue` unless
  they need ISR-safe non-blocking operations from the same queue.
- The blocking variants spin and are therefore not appropriate for ISR
  callers. ISR callers must use `BlockingType::NONBLOCKING`.
- Non-blocking `send`/`receive` may return spurious `FULL`/`EMPTY` under
  contention (§5, §6).
- The ABA epoch tag is `TAG_BITS` wide (60 by default; 28 when
  `LocklessStateTagType` is configured to `U32`); a stale CAS is defeated
  unless a thread stalls between scan and CAS across an exact multiple of
  `2^TAG_BITS` transitions of one slot. Should that coincidence occur, the
  effect is a single out-of-priority-order dequeue of a valid message —
  never corruption, loss, or duplication (§4).
- FIFO tie-breaking among equal-priority messages assumes no message remains
  queued across `2^31` intervening sends (§7).
