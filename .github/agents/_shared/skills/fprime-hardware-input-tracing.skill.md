---
name: fprime-hardware-input-tracing
description: Use when tracing a value in F Prime code back to determine whether it originates from hardware input (driver receive paths, hardware registers, interrupt context, or POSIX/Linux device reads).
---

# Skill: Trace a value back to determine if it is hardware-controlled

Counterpart to `fprime-ground-input-tracing.skill.md`. The security
agent uses this skill to determine whether a predicate, an
arithmetic operand, or an arbitrary value originates from a hardware
input — i.e., from data crossing a `Drv` driver interface, a
hardware register, a bus read, or an interrupt context.

A `**must fix**` from this skill is severity-identical to the
ground-input equivalent: an assert reachable by hardware input
DoS-es the spacecraft when the hardware misbehaves, an overflow
reachable by hardware input is a DDoS vector, and a missing
validation on hardware input is an exploitable surface.

---

## 1. Hardware-input entry points

The following F Prime constructs receive hardware input directly:

| Entry point | Where the data enters |
|---|---|
| **`Drv::*` driver components** | Anything in `Drv/` whose `.fpp` declares an input port labeled as a driver / hardware boundary. Examples: `Drv::I2cDriver::read()`, `Drv::SpiDriver::transact()`, `Drv::UartDriver::recv()`, `Drv::TcpClient::recv()`, `Drv::TcpServer::recv()`, `Drv::IpSocket::recv()`. |
| **Hub / serializer ports receiving from off-device** | `Svc::Hub`, `Svc::Framer`, `Svc::Deframer` input from a physical link. |
| **Linux/POSIX HAL reads** | `Os::File::read()` against a device path (e.g., `/dev/...`), `read()` from a file descriptor backed by hardware. |
| **Interrupt handlers / signal handlers** | Code paths entered from an OS-level ISR or POSIX signal. (Rare in F Prime application code; common in `Drv/` internals.) |
| **Memory-mapped registers** | Direct `volatile *` accesses to register addresses (typically only in low-level `Drv/`). |
| **Telemetry / control link data received from another spacecraft component** | Inter-component data over a physical wire. |

Each handler parameter at one of these entry points is
**hardware-input** at the moment it enters the agent's component.

---

## 2. Forward trace within a component

Same procedure as `fprime-ground-input-tracing.skill.md` §2. The
only difference is the classification result and the entry-point
table in §1 above.

`m_state` written from a hardware-source class is sticky: it is
treated as `hardware-input` for the remainder of the trace.

---

## 3. Library and primitive source classes

| Construct | Source class |
|---|---|
| Literal, `constexpr`, `enum` value, `static const` | `programmer-constant` |
| `Fw::Time::getTimeBase()`, `Os::IntervalTimer` | `internal-state` |
| `Os::File::read()` against a device path | `hardware-input` |
| `Drv::*` recv / read / transact return values, payload buffers | `hardware-input` |
| `Drv::*` callback parameters | `hardware-input` |
| `recv()` / `read()` system call result on a socket / file
descriptor connected to a physical link | `hardware-input` |
| Volatile register reads | `hardware-input` |

---

## 4. Cross-component flow

Same algorithm as the ground-input skill §4. The trace follows the
topology wiring; the difference is that the chain of origin ends at
a hardware-input entry point (§1) instead of a ground-input entry
point.

A buffer that flows `Drv::UartDriver::recv` → `Svc::Deframer` →
`Svc::Hub` → application component is `hardware-input` at every hop
unless the data is overwritten with internal state along the way.

---

## 5. Combined ground + hardware reachability

Some predicates are reachable from BOTH ground input and hardware
input — for example, a buffer that ground can uplink AND that
hardware can also push. In that case:

- Classify the predicate as `ground-input + hardware-input`.
- For triage: this is at least the most dangerous of the two
  applicable severities. In practice, this is `**must fix**` for an
  assert and `**must fix**` for unbounded copy / overflow / missing
  validation.

The agent's category for the finding is the more specific one
(e.g., category 4–6 — hardware variants — if the assert is in a
driver layer that is primarily fed by hardware, even if a ground
path also exists). When both apply equally, the agent picks one
category and notes the dual reachability in the comment body.

---

## 6. Worked example — hardware-reachable assert

```cpp
// In Drv/I2cDriver/I2cDriver.cpp (illustrative)
void I2cDriver::Recv_handler(NATIVE_INT_TYPE port, Fw::Buffer& buf) {
    FW_ASSERT(buf.getSize() <= MAX_PAYLOAD);   // L
    // ...
}
```

Trace from line `L`:

1. Variable: `buf.getSize()`.
2. `buf` is the handler parameter at the `Recv` input port of an
   `Drv::*` component → hardware-input.
3. `buf.getSize()` is a property of a hardware-input buffer →
   hardware-input.

The assert is hardware-reachable → category 4 → `**must fix**`.

Suggestion: replace with a validation that returns the buffer to
the sender with an error code per the F Prime `Fw::Buffer` ownership
contract:

```suggestion
if (buf.getSize() > MAX_PAYLOAD) {
    this->bufferReturn_out(0, buf);
    this->log_WARNING_HI_OversizedFrame(buf.getSize());
    return;
}
```

---

## 7. Worked example — hardware register read without bounds check

```cpp
this->m_dma_len = REG_DMA_LEN;   // memory-mapped register read
// ...
memcpy(this->m_payload, src, this->m_dma_len);  // overflow vector
```

Tracing `m_dma_len`:

1. `m_dma_len` is internal-state, written from `REG_DMA_LEN`.
2. `REG_DMA_LEN` is a volatile register read → hardware-input.
3. `m_dma_len` is sticky hardware-input.

The `memcpy` length parameter is hardware-input. Without a bound
check against `sizeof(m_payload)`, this is a hardware-driven
overflow → category 5 → `**must fix**`.

Suggestion adds the bound check (the exact form depends on the
desired error response — drop, log, return-to-sender).

---

## 8. Confidence calibration

The agent reports `high confidence` when:

- The trace reaches a §1 entry point within 3 hops.
- The driver is well-known F Prime stock (`I2cDriver`, `SpiDriver`,
  `UartDriver`, etc.).

The agent reports `low confidence` when:

- The driver is custom and the agent cannot fully read its
  interfaces.
- The hardware boundary involves callback-based dispatch that the
  agent cannot resolve.
- The autocoded portion of the component is large and the agent
  cannot fully follow generated dispatch.

Low confidence → tag at the right severity + maintainer ping per
`maintainer-lookup.skill.md`.

---

## 9. One-line summary

`Walk backwards from the offending variable; if origin is a Drv::*
port, a hardware register, a /dev/ read, or an interrupt context,
classify as hardware-input. Combined ground+hardware reachability
inherits the more dangerous severity. Low confidence pings the
maintainer; severity drives the tag.`
