# ComFprime (F´ Framing) Subtopology — Software Design Document (SDD)

The **ComFprime subtopologies** implement F´’s lightweight communications stack for framing/deframing bytes on the spacecraft side. There are **two variants** in the same module:

1. A variant that **supplies a `Svc::ComStub`** implementation of `Svc.ComInterface` and expects to be wired to a **`Drv::ByteStreamDriverModel`** (TCP/UDP/UART, etc.), and
2. A variant that **expects an external implementation of [`Svc.ComInterface`](https://fprime.jpl.nasa.gov/latest/docs/reference/communication-adapter-interface/)** provided by the deployment.

Both variants provide the standard **F´ framer/deframer + router + ComQueue** path and are tuned through **ComFprimeConfig** instance properties.

---

## 1. Requirements

| ID                | Description                                                                                                     | Validation |
| ----------------- | --------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-COMFPRIME-001 | Provide an F´ **framer** to convert COM buffers into framed packets for downlink using the F Prime format.      | Inspection |
| SVC-COMFPRIME-002 | Provide an F´ **deframer** to parse incoming packets into com buffers for uplink.                               | Inspection |
| SVC-COMFPRIME-003 | Provide an F´ **router** to route deframed packets (e.g., commands/files) into the flight software.             | Inspection |
| SVC-COMFPRIME-004 | Provide a **subtopology variant that supplies `Svc::ComStub`** designed to connect to a ByteStream driver.      | Inspection |
| SVC-COMFPRIME-005 | Provide a **subtopology variant that expects an external `Svc::ComInterface`** supplied by the deployment.      | Inspection |
| SVC-COMFPRIME-006 | Support **configurable instance properties** (IDs, queue sizes, stack sizes, priorities) via `ComFprimeConfig`. | Inspection |

---

## 2. Design & Core Functions

### 2.1 Instance Summary (common building blocks)

| Instance name  | Type (Svc/Drv)               | Kind    | Purpose (core function)                                                                |
| -------------- | ---------------------------- | ------- | -------------------------------------------------------------------------------------- |
| `framer`       | `Svc.FprimeFramer`           | Passive | Frames fprime data into packets for transport.                                         |
| `deframer`     | `Svc.FprimeDeframer`         | Passive | Deframes fprime data from incoming packets.                                            |
| `fprimeRouter` | `Svc.FprimeRouter`           | Passive | Routes deframed packets (e.g., commands/files) into the flight software.               |
| `comQueue`     | `Svc.ComQueue`               | Active  | Queues data for framing (telemetry, events, file, etc.); has `Run`.                    |
| `commsBufferManager` | `Svc.BufferManager`    | Active  | Provides memory allocation to the subtopology.                                         |
| `frameAccumulator`   | `Svc.FrameAccumulator` | Passive | Accumulates whole frames before deframing.                                             |
| `comStub`      | `Svc.ComStub`                | Passive | (Variant A only) Adapts  a `Drv::ByteStreamDriverModel` driver for use as downlink.    |

> **Two variants:**
> **A. “With ComStub”:** Subtopology **includes** `Svc::ComStub` and exposes **ByteStream** ports to your driver.
> **B. “With External ComInterface”:** Subtopology **does not include** `Svc::ComStub`; you **provide** one in the deployment.

### 2.2 Required Inputs for Operation

* **Rate Groups:** Connect a scheduler output to **`comQueue.run`**.
* **Transport Endpoint:**

  * **Variant A:** Wire **ByteStream send/recv** between your **`Drv::ByteStreamDriverModel`** and the subtopology’s **`ComStub`**.
  * **Variant B:** Provide your own **`Svc::ComInterface`** and wire it to the **framer/deframer ports** in the subtopology.
* **Flight-side hookups:** Wire the **router** outputs (commands/files) into your CDH stack (e.g., command dispatcher, file uplink), and feed **packet sources** (telemetry/events/file downlink) into the **ComQueue**.

### 2.3 Limitations

These subtopologies focus on the **F´ communications protocol**. They do **not** provide CCSDS framing/deframing nor CDH capabilities.

---

## 3. Usage

Below are **two usage patterns**, one for each variant. Replace identifiers/ports with the **exact names in `ComFprime.fpp`**.

### 3.1 Variant A — ComFprime **with** `Svc::ComStub` (expects a ByteStream driver)

```fpp
topology Flight {
  import ComFprime.Subtopology

  # (A1) Provide a ByteStreamDriver interface (e.g. Drv.TcpClient)
  instance comDriver: ...

  # (A2) Schedule ComQueue
  connections RateGroups {
    rg.RateGroupMemberOut[0] -> ComFprime.comQueue.run
  }

  # (A3) Wire ByteStream driver <-> ComStub supplied by the subtopology
      comDriver.$recv                     -> ComFprime.comStub.drvReceiveIn
      ComFprime.comStub.drvReceiveReturnOut -> comDriver.recvReturnIn
      ComFprime.comStub.drvSendOut      -> comDriver.$send
      comDriver.ready         -> ComFprime.comStub.drvConnected
  }
}
```

> [!TIP]
> `ComFprime.commsBufferManager` can be reused if the `ByteStreamDriver` requires buffer management.

### 3.2 Variant B — ComFprime **without** `Svc::ComStub`

```fpp
topology Flight {
  import ComFprime.FramingSubtopology

  # (B1) Provide your own ComInterface
  instance radio: ...

  # (B2) Schedule ComQueue
  connections RateGroups {
    rg.RateGroupMemberOut[0] -> ComFprime.comQueue.run
  }

  # (B3) Wire your ComStub between the driver and the ComFprime framer/deframer
  connections Link {
        # Framer <-> ComStub (Downlink)
        ComFprime.framer.dataOut -> radio.dataIn
        radio.dataReturnOut    -> ComFprime.framer.dataReturnIn
        radio.comStatusOut     -> ComFprime.framer.comStatusIn

        # ComStub <-> FrameAccumulator (Uplink)
        radio.dataOut -> ComFprime.frameAccumulator.dataIn
        ComFprime.frameAccumulator.dataReturnOut -> radio.dataReturnIn
  }
}
```

---

## 4. Configuration

> Configure **only the instance properties** owned by the ComFprime subtopologies. All knobs live under:
> `Svc/Subtopologies/ComFprime/ComFprimeConfig/ComFprimeConfig.fpp`

### 4.1 Component properties (`ComFprimeConfig.fpp`)

* **Base ID** — Base identifier for the subtopologies; instance IDs are offset from this base.
* **Queue sizes** — Depths for **`ComQueue`** and any other active/queued elements defined by the subtopology.
* **Stack sizes** — Task stack allocations for active components (if any beyond `ComQueue`).
* **Priorities** — RTOS priorities for active/queued components as applicable.

### 4.2 Buffer Manager Bin Configuration

`module BuffMgr` provides constants for the bins configured for `commsBufferManager`.

## 5. Traceability Matrix

| Requirement ID    | Satisfied by (instance/type)                                      |
| ----------------- | ----------------------------------------------------------------- |
| SVC-COMFPRIME-001 | `framer` — `Svc.FprimeFramer`                                     |
| SVC-COMFPRIME-002 | `deframer` — `Svc.FprimeDeframer`                                 |
| SVC-COMFPRIME-003 | `router` — `Svc.FprimeRouter`                                     |
| SVC-COMFPRIME-004 | `Subtopology`                                                     |
| SVC-COMFPRIME-005 | `FramingSubtopology`                                              |
| SVC-COMFPRIME-006 | `comQueue` — `Svc.ComQueue` (`run` scheduling port)               |
| SVC-COMFPRIME-007 | `ComFprimeConfig`                                                 
