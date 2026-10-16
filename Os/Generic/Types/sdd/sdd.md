# AtomicQueue Software Design Document

## 1. Overview

The `AtomicQueue` class provides a lock-free MPMC FIFO queue using a circular buffer with atomic sequence numbers. It achieves O(1) enqueue and O(1) dequeue operations and does not require DWCAS atomics, making it portable across most real time operating systems.

### 1.1 Purpose

Provide a bounded FIFO queue that:
- Operates without locks or mutexes (suitable for ISR context)
- Supports multiple concurrent producers and consumers (MPMC)
- Has O(1) bounded, deterministic worst-case execution time
- Uses only standard word-size atomics (no DWCAS required)
- Avoids ABA errors via sequence number coordination
- Provides memory for storing message data (buffer count & size set at queue create)

## 2. Algorithm

### 2.1 Data Structure

**Circular Buffer:**
```
[Slot 0] [Slot 1] [Slot 2] ... [Slot N-1]
  seq=0    seq=1    seq=2         seq=N-1
  
Enqueue at m_enqueuePos → → → → → → → wraps around
                                      ↓
Dequeue at m_dequeuePos ← ← ← ← ← ← ← wraps around
```

**Each Slot contains:**
- `U8* buffer` - Embedded message buffer (data storage)
- `FwSizeType size` - Actual message size stored
- `atomic<FwSizeType> sequence` - Coordination sequence number

### 2.2 Sequence Number States

Each slot's sequence number encodes its current state:

| Sequence Value | State | Who Can Access |
|----------------|-------|----------------|
| `seq == pos` | Ready for write | Producer can enqueue |
| `seq == pos + 1` | Ready for read | Consumer can dequeue |
| `seq == pos + capacity` | After read, next cycle | Producer can enqueue again |

**Example (capacity=4):**
```
Initial:       slot[0].seq=0, slot[1].seq=1, slot[2].seq=2, slot[3].seq=3
After enq @0:  slot[0].seq=1 (ready for read)
After deq @0:  slot[0].seq=4 (next cycle, 0+capacity)
Later enq @0:  slot[0].seq=5 (4+1, ready for read again)
```

**NOTE**: With FwSizeType being a U64, the wrap-around is functionally impossible (~584 years at 1 GHz). However, on 32-bit platforms (FwSizeType=U32), wrap-around occurs after 2^32 operations (~1.2 hours at 1M ops/sec). The algorithm remains correct after wrap (sequence numbers prevent ABA), but applications with sustained high throughput on 32-bit systems should be aware. FwSizeType is used (instead of directly using U64) to support platforms with 32-bit native word size where 64-bit atomics may not be lock-free or require expensive emulation. 

### 2.3 Dequeue Algorithm (O(1))

```
1. Loop (bounded to MAX_CAS_RETRIES = 100):
   a. Load current dequeue position (relaxed)
   b. Calculate slot index = pos & mask
   c. Load slot sequence number (acquire)
   d. Calculate diff = seq - (pos+1)
   
   e. If diff == 0:  // Slot has data
      - CAS dequeue position: pos → pos+1 (release on success)
      - If CAS succeeds:
        * Read slot->size (non-atomic, synchronized via sequence acquire)
        * Copy message data via memcpy from slot->buffer
        * Store seq = pos+capacity (release) - marks available for next cycle
        * Post semaphore (if enabled) - wake blocked enqueuers
        * Return success
   
   f. If diff < 0:  // Queue is empty
      - Return failure
   
   g. Else:  // Another consumer claimed slot
      - Retry
   
2. If loop exhausted, return failure
```

**Failure modes:**
- Queue is empty (enqueue position = dequeue position)
- MAX_CAS_RETRIES exceeded (extreme contention, highly unlikely)
- Recovery: Failures are transient under contention. Caller should retry or back off.

### 2.4 Enqueue Algorithm (O(1))

```
1. Loop (bounded to MAX_CAS_RETRIES = 100):
   a. Load current enqueue position (relaxed)
   b. Load dequeue position (relaxed) - for full-queue detection
   c. Calculate slot index = pos & mask
   d. Load slot sequence number (acquire)
   e. Calculate diff = seq - pos
   
   f. If diff == 0:  // Slot is available
      - CAS enqueue position: pos → pos+1 (release on success)
      - If CAS succeeds:
        * Copy message data via memcpy to slot->buffer
        * Store slot->size (non-atomic, synchronized via sequence)
        * Store seq = pos+1 (release) - marks ready for read
        * Return success
   
   g. If diff < 0:  // Queue is full
      - Return failure
   
   h. Else:  // Another producer claimed slot
      - Retry
   
2. If loop exhausted, return failure

3. If enqueue succeeded and semaphore exists:
   a. tryWait() on semaphore (non-blocking decrement)
   b. Ignore result (may fail due to concurrent races - acceptable)
```

**Semaphore synchronization:**
- Non-blocking enqueue() attempts to decrement semaphore after successful enqueue
- This maintains the invariant: `semaphore count ≈ free slots` for blocking operations
- `tryWait()` may fail due to race conditions when multiple threads enqueue concurrently
- Failure is acceptable - semaphore is a best-effort hint; lock-free atomics are authoritative
- The window between enqueue and tryWait allows other threads to decrement first

**Failure modes:**
- Queue is full (enqueue position = dequeue position + capacity)
- MAX_CAS_RETRIES exceeded (extreme contention, highly unlikely)
- Recovery: Failures are transient under contention. Caller should retry or back off.

### 2.5 Blocking Enqueue Algorithm (O(1))

**Wait-first pattern** prevents semaphore count drift under contention:

```
1. If blockIfFull == false or no semaphore:
   - Use non-blocking enqueue()
   
2. Loop (bounded to MAX_CAS_RETRIES = 100):
   a. Wait on semaphore (blocks until space available)
   b. Try internal enqueue (lock-free only, no semaphore operations)
   c. If enqueue succeeds:
      - Return success
   d. Else (extremely rare race - slot stolen):
      - Post semaphore to return permit
      - Retry
      
3. If loop exhausted, return failure
```

**Key invariant:** Semaphore count ≈ number of free slots

**Why wait-first?** Previous try-wait pattern allowed drift:
- Thread A tries enqueue → full
- Thread B dequeues, posts semaphore (count=1)
- Thread C enqueues before A wakes (steals slot)
- Thread A wakes, consumes permit but can't enqueue
- Result: semaphore count drifts from actual free slots

**Wait-first guarantees:** Permit reservation before enqueue attempt eliminates drift and spurious wakeups.

### 2.6 Size Reporting (O(1))

**Operation:** Returns approximate queue occupancy

```
1. Load current enqueue position (relaxed)
2. Load current dequeue position (relaxed)
3. Return difference: enqueuePos - dequeuePos
```

**Characteristics:**
- **Approximate:** Returns snapshot of position difference
- **Race condition:** May be slightly stale in highly concurrent scenarios
- **Monotonic:** Size never decreases incorrectly (conservative estimate)
- **Use case:** Monitoring/diagnostics, not for critical logic decisions

## 3. ABA Problem and Mitigation

### 3.1 ABA Problem in Circular Buffers

**Potential scenario:**
1. Thread A reads slot sequence `seq=100` at position 100
2. Thread A is preempted
3. Queue wraps around completely (capacity operations)
4. Thread B writes and reads same slot multiple times
5. Slot sequence becomes `100 + N*capacity` (looks like 100 modulo capacity)
6. Thread A resumes - could CAS succeed incorrectly?

### 3.2 Sequence Number Solution

**Why ABA cannot occur:**

1. **Position CAS protects slot claiming:**
   - Threads CAS the position counter, not the sequence
   - Position counter monotonically increases (no wrap-around reuse)
   - If Thread A's CAS succeeds, it has exclusive access to that slot

2. **Sequence enforces ordering:**
   - After Thread A claims position N, it checks `seq == N`
   - If queue wrapped around, sequence would be `N + capacity` (or higher)
   - The difference check `(seq - pos)` detects this: diff > 0 means slot already advanced

3. **64-bit position counter:**
   - Wrap-around requires 2^64 operations (~10^19)
   - At 1 GHz (1 operation/cycle), takes ~584 years
   - Acceptable for flight software mission duration

**Example:**
```
Initial:     pos=100, slot[4].seq=100 (capacity=8)
Thread 1:    Reads pos=100, sees seq=100
             [Preempted before CAS]
Queue wraps: 8+ operations complete, pos=108, slot[4].seq=108
Thread 1:    Attempts CAS pos 100→101
             CAS FAILS - position already advanced to 108
```

## 4. Safety and Compliance

### 4.1 Thread Safety and Memory Ordering

**Thread Safety:** Yes - MPMC with atomic coordination
**SMP Safety:** Yes - acquire/release memory ordering prevents stale reads on weakly-ordered CPUs (ARM, POWER)
**Reentrancy:** Yes - no shared mutable state outside atomics

**ISR Safety:**

enqueue() & dequeue() both call semaphore operations (tryWait() and post()) - therefore, they're only ISR safe on platforms which support non-blocking semaphore operations in ISR context. 
- ISR-safe: VxWorks, FreeRTOS, INTEGRITY, ThreadX, RTEMS, QNX Neutrino, Zephyr, embOS, µC/OS-II/III, SafeRTOS, Azure RTOS
- NOT ISR-safe: POSIX RT (strict spec), Linux (standard/non-RT), Embedded Linux without RT patches

**Memory ordering:**
- **Slot sequence load:** acquire (synchronizes with enqueue/dequeue release, ensures size/buffer visibility)
- **Slot sequence store:** release (publishes data availability/slot availability)
- **Position CAS operations:** release on success (publishes position claim)
- **Enqueue/dequeue position loads:** relaxed (stale reads cause harmless retries)
- **Non-atomic size field/memcpy data:** Synchronized via sequence number acquire/release pair (C++ memory model guarantees visibility)

### 4.2 Atomic Requirements

**Only word-size atomics needed:**
- `atomic<FwSizeType>` for sequence numbers (typically 64-bit)
- `atomic<FwSizeType>` for enqueue/dequeue positions (typically 64-bit)
- **No DWCAS (128-bit) required** - improves portability vs tagged pointer approaches

**Lock-free guarantee:** Runtime assertion verifies `is_lock_free()` during create()

**Platform Support**

| Architecture | Support | Native Instructions |
|--------------|---------|---------------------|
| **ARM (all variants)** | ✅ Full | LDREX/STREX (32-bit), LDXR/STXR (64-bit) |
| **x86-64** | ✅ Full | LOCK CMPXCHG8B |
| **PowerPC** | ✅ Full | LDARX/STDCX |
| **RISC-V** | ✅ Full | LR.D/SC.D |
| **MIPS** | ✅ Full | LL/SC |
| **Any with C++11** | ✅ Full | Compiler guarantees `atomic<uint64_t>` |

## 5. Performance Characteristics

### 5.1 Time Complexity

| Operation | Complexity | Notes |
|-----------|------------|-------|
| **Enqueue** | O(1) | No traversals, bounded retries |
| **Dequeue** | O(1) | No traversals, bounded retries |
| **isFull** | O(1) | Simple position difference |
| **isEmpty** | O(1) | Simple position equality check |

## 6. Verification Strategy

### 6.1 Testing

**Unit tests:**
- Single producer/single consumer
- Multi producer/single consumer
- Multi producer/multi consumer (stress test)
- Boundary conditions (empty, full, wrap-around)
- Power-of-2 capacities (8, 16, 32, 64, 128, 256)
- Non-power-of-2 capacities (10, 100, 500)

**Stress tests:**
- Sustained high throughput (verify no O(n) degrade)
- Alternating enqueue/dequeue bursts
- ISR preemption simulation
- Capacity wrap-around (position counter near 2^64)

## 7. References

- Dmitry Vyukov (2012). "Bounded MPMC Queue" - Sequence number coordination algorithm
- Michael, M. M., & Scott, M. L. (1996). "Simple, Fast, and Practical Non-Blocking Algorithms"

## 8. Revision History

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 1.0 | 2026-05-26 | B. Duckett | Initial implementation documentation |
