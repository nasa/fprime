# ComCcsds (CCSDS Framing) Subtopology — Software Design Document (SDD)

The **ComCcsds subtopologies** implement F´’s **CCSDS** communications stack for framing/deframing on the flight side. There are **two variants** in the same module:

1. A variant that **supplies a `Svc::ComStub`** implementation of `Svc.ComInterface` and expects to be wired to a **`Drv::ByteStreamDriverModel`** (TCP/UDP/UART, etc.), and
2. A variant that **expects an external implementation of [`Svc.ComInterface`](https://fprime.jpl.nasa.gov/latest/docs/reference/communication-adapter-interface/)** provided by the deployment.

Both variants provide the standard **router + ComQueue + CCSDS framers/deframers** path. `ComCcsds` is **composed from two subtopologies**: the packet layer comes from **`SpacePacket`** (`Svc/Subtopologies/SpacePacket`) and the transfer-frame layer from **`CcsdsFraming`** (`Svc/Subtopologies/CcsdsFraming`); `ComCcsds` itself contributes only the composition wiring and the optional `Svc::ComStub`.

> [!IMPORTANT]
> `ComCcsds` provides framing/deframing for CCSDS SpacePackets inside TM/TC data transfer frames.

---

## 1. Requirements

| ID               | Description                                                                                                    | Validation |
| ---------------- | -------------------------------------------------------------------------------------------------------------- | ---------- |
| SVC-COMCCSDS-001 | Provide a **CCSDS framer** to convert COM buffers into CCSDS frames for downlink.                             | Inspection |
| SVC-COMCCSDS-002 | Provide a **CCSDS deframing path** to parse incoming CCSDS frames into COM buffers for uplink.                 | Inspection |
| SVC-COMCCSDS-003 | Provide an F´ **router** to route deframed packets (e.g., commands/files) into the flight software.            | Inspection |
| SVC-COMCCSDS-004 | Provide a **subtopology variant that supplies `Svc::ComStub`** designed to connect to a ByteStream driver.     | Inspection |
| SVC-COMCCSDS-005 | Provide a **subtopology variant that expects an external `Svc::ComInterface`** supplied by the deployment.     | Inspection |
| SVC-COMCCSDS-006 | Support **configurable instance properties** (IDs, queue sizes, stack sizes, priorities) via the `SpacePacketConfig`, `CcsdsFramingConfig`, and `ComCcsdsConfig` modules. | Inspection |
| SVC-COMCCSDS-007 | Be **composed from** the `SpacePacket` (packet layer) and `CcsdsFraming` (transfer-frame layer) subtopologies. | Inspection |

---

## 2. Design & Core Functions

### 2.1 Instance Summary

| Instance name                      | Type (Svc/Drv)                  | Kind    | Purpose (core function)                                                                         |
| ---------------------------------- | ------------------------------- | ------- | ----------------------------------------------------------------------------------------------- |
| `SpacePacket.fprimeRouter`         | `Svc.FprimeRouter`              | Passive | Routes deframed packets (e.g., commands/files) into the flight software.                        |
| `SpacePacket.comQueue`             | `Svc.ComQueue`                  | Active  | Queues categorized COM data for framing (telemetry, events, file, etc.); exposes `run`.         |
| `SpacePacket.spacePacketFramer`    | `Svc.Ccsds.SpacePacketFramer`   | Passive | Builds **CCSDS Space Packets** from COM buffers (downlink step 1).                              |
| `CcsdsFraming.framer`              | `Svc.Ccsds.TmFramer`            | Passive | Builds **CCSDS TM Transfer Frames** from space packets and sends to the link (downlink step 2). |
| `SpacePacket.spacePacketDeframer`  | `Svc.Ccsds.SpacePacketDeframer` | Passive | Deframes F Prime data from **CCSDS Space Packets** (uplink step 2).                             |
| `CcsdsFraming.tcDeframer`          | `Svc.Ccsds.TcDeframer`          | Passive | Deframes **CCSDS Space Packets** from  **CCSDS TM Transfer Frames** (uplink step 1).            |
| `CcsdsFraming.frameAccumulator`    | `Svc.FrameAccumulator`          | Passive | Collects bytes from the link and emits complete frames/packets for deframing (uplink path).     |
| `comStub`                          | `Svc.ComStub`                   | Passive | (Variant A only) Implementation of `Svc.ComInterface`, adapting a `Drv::ByteStreamDriverModel`. |

> **Two variants:**
> **A. “With ComStub”:** Subtopology **includes** `Svc::ComStub` and exposes **ByteStream** ports to your driver.
> **B. “With External ComInterface”:** Subtopology **does not include** `Svc::ComStub`; you **provide** one in the deployment.

### 2.2 Required Inputs for Operation

* **Rate Groups:** Connect a rate group to **`comQueue.run`**. This is not required for the subtopology to function, but defines the rate at which ComQueue will send telemetry.
* **Transport Endpoint:**

  * **Variant A:** Wire **ByteStream send/recv** between your **`Drv::ByteStreamDriverModel`** and the subtopology’s **`ComStub`**.
  * **Variant B:** Provide your own **`Svc::ComInterface`** and wire it to the **CCSDS framer/deframer ports** in the subtopology.
* **Flight-side hookups:** Wire the **router** outputs (commands/files) into your CDH stack (e.g., command dispatcher, file uplink), and feed **packet sources** (telemetry/events/file downlink) into **`comQueue`**.

### 2.3 Limitations

These subtopologies focus on the **CCSDS framing and deframing setup** and does not provide wider CDH.

---

## 3. Usage

Below are **two usage patterns**, one for each variant. Replace identifiers/ports with the **exact names in `ComCcsds.fpp`**.

### 3.1 Variant A — ComCcsds **with** `Svc::ComStub` (expects a ByteStream driver)

```fpp
topology Flight {
  instance ComCcsds.Subtopology

instance comDriver: <ByteStreamDriverInterface>

# (A1) Schedule ComQueue telemetry downlink (optional)
  connections RateGroups {
    rg.RateGroupMemberOut[0] -> ComCcsds.Subtopology.comQueueRun
  }

  # (A2) Wire ByteStream driver <-> ComStub supplied by the subtopology
  connections Link {
    comDriver.$recv                                -> ComCcsds.Subtopology.drvReceiveIn
    ComCcsds.Subtopology.drvReceiveReturnOut       -> comDriver.recvReturnIn
    ComCcsds.Subtopology.drvSendOut                -> comDriver.$send
    comDriver.ready                                -> ComCcsds.Subtopology.drvConnected
  }
}
```

> [!TIP]
> `ComCcsds.Subtopology.commsBufferGetCallee` and `ComCcsds.Subtopology.commsBufferSendIn` can be used if the `ByteStreamDriver` requires buffer management.

### 3.2 Variant B — ComCcsds **without** `Svc::ComStub`

```fpp
topology Flight {
  import ComCcsds.FramingSubtopology

  # (B1) Provide your own ComInterface
  instance radio: <YourComInterface>

  # (B2) Schedule ComQueue
  connections RateGroups {
    rg.RateGroupMemberOut[0] -> SpacePacket.comQueue.run
  }

  # (B3) Wire your ComInterface between the driver and the CCSDS framer/deframer
  connections Link {
    # Downlink: TM framer -> your ComInterface
    CcsdsFraming.framer.dataOut       -> radio.dataIn
    radio.dataReturnOut               -> CcsdsFraming.framer.dataReturnIn
    radio.comStatusOut                -> CcsdsFraming.framer.comStatusIn

    # Uplink: your ComInterface -> frame accumulator
    radio.dataOut                     -> CcsdsFraming.frameAccumulator.dataIn
    CcsdsFraming.frameAccumulator.dataReturnOut -> radio.dataReturnIn
  }
}
```

---

## 4. Configuration

> Configuration is split across the composed subtopologies:
>
> * `Svc/Subtopologies/ComCcsds/ComCcsdsConfig/ComCcsdsConfig.fpp` — base ID for the `comStub` instance owned by `ComCcsds` itself.
> * `Svc/Subtopologies/SpacePacket/SpacePacketConfig/SpacePacketConfig.fpp` — packet-layer properties (ComQueue sizes/depths/priorities, aggregator, buffer manager bins).
> * `Svc/Subtopologies/CcsdsFraming/CcsdsFramingConfig/CcsdsFramingConfig.fpp` — transfer-frame-layer properties (frame accumulator buffer size).

---

## 5. Traceability Matrix

| Requirement ID   | Satisfied by (instance/type)                                                           |
| ---------------- | -------------------------------------------------------------------------------------- |
| SVC-COMCCSDS-001 | `SpacePacket.spacePacketFramer` — `Svc.Ccsds.SpacePacketFramer`, `CcsdsFraming.framer` — `Svc.Ccsds.TmFramer` |
| SVC-COMCCSDS-002 | `CcsdsFraming.frameAccumulator` — `Svc.FrameAccumulator`                               |
| SVC-COMCCSDS-003 | `SpacePacket.fprimeRouter` — `Svc.FprimeRouter`                                        |
| SVC-COMCCSDS-004 | `Subtopology` (variant including `Svc.ComStub`)                                        |
| SVC-COMCCSDS-005 | `FramingSubtopology` (variant expecting external `Svc.ComInterface`)                   |
| SVC-COMCCSDS-006 | `SpacePacketConfig`, `CcsdsFramingConfig`, and `ComCcsdsConfig` modules                |
| SVC-COMCCSDS-007 | `import SpacePacket.FramingSubtopology`, `import CcsdsFraming.Subtopology`             |

