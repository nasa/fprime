\page SvcBufferRepeaterComponent Svc::BufferRepeater Component
# Svc::BufferRepeater: Buffer Repeater (Passive Component)

## 1. Introduction

The BufferRepeater component is designed to take in an `Fw::buffer` port call and repeat it to multiple output ports.
New memory is allocated from a `Svc::BufferManager` and the data is copied to ensure that each downstream component has
full ownership of its data. The original buffer is deallocated.  Users call `configure` to set the response of the
component when allocation requests fail.

## 2. Requirements

| Name               | Description                                                                   | Validation |
|--------------------|-------------------------------------------------------------------------------|------------|
| BUFFER_REPEATER_01 | The buffer repeater shall accept incoming `Fw::Buffer` port calls             | Unit Test  |
| BUFFER_REPEATER_02 | The buffer repeater shall copy the incoming data before each repeated send    | Unit Test  |
| BUFFER_REPEATER_03 | The buffer repeater shall only send a copy to connected components            | Unit Test  |
| BUFFER_REPEATER_04 | The buffer repeater shall have a configurable response to allocation failures | Unit Test  |

## 3. Design

The buffer repeater has a single `portIn` port of type `Fw.BufferSend` and an array of `portOut` ports of the same type.
Each port call received on `portIn` is repeated to each connected `portOut` port. These repetitions emit a copy of the
`Fw::Buffer` data.

## 3.1 Ports

`Svc::BufferRepeater` has the following ports:

| Kind         | Name         | Port Type                                   | Usage                                 |
|--------------|--------------|---------------------------------------------|---------------------------------------|
| `sync input` | `portIn`     | `Fw.BufferSend`                             | Port for receiving initial Fw::Buffer |
| `output`     | `portOut`    | `[BufferRepeaterOutputPorts] Fw.BufferSend` | Port array for repeating Fw::Buffers  |
| `output`     | `deallocate` | `Fw.BufferSend`                             | Deallocate the original buffer        |
| `output`     | `allocate`   | `Fw.BufferGet`                              | Port for allocating new memory        |
| `event`      | `Log`        | `Fw.Log`                                    | Port for emitting events              |
| `text event` | `LogText`    | `Fw.LogText`                                | Port for emitting text events         |
| `time get`   | `Time`       | `Fw.Time`                                   | Port for getting the time             |

#### 3.1.1 portIn handler

The `portIn` port handler receives an `Fw::Buffer` data type and a port number. For each connected `portOut` port it
allocates new memory large enough to hold a copy of the `Fw::Buffer` data via the `allocate` port, copies the data to
the new invocation, and sends it to the active `portOut` port. This continues until each `portOut` port has been tried
and at the end it returns the original buffer via `deallocate`.

Should an allocation fail, `portOut` is not called and an event may be emitted (see below).

### 3.2 Events

| Name                  | Description                                                                          |
|-----------------------|--------------------------------------------------------------------------------------|
| AllocationSoftFailure | WARNING_HI indicating allocation failure when configured to WARNING_ON_OUT_OF_MEMORY |
| AllocationHardFailure | FATAL indicating allocation failure when configured to FATAL_ON_OUT_OF_MEMORY        |

## 4. Configuration

Buffer repeater maximum output ports are configured using `AcConstants.fpp` as shown below:

```ini
constant BufferRepeaterOutputPorts = 10
```

The component's response to allocation failures is configured with a call to the configure method during system
initialization. Possible values include:

| Setting                      | Description                                                                   |
|------------------------------|-------------------------------------------------------------------------------|
| NO_RESPONSE_ON_OUT_OF_MEMORY | The component drops the `portOut` call and otherwise takes no response action |
| WARNING_ON_OUT_OF_MEMORY     | The component drops the `portOut` call and emits a WARNING_HI event           |
| FATAL_ON_OUT_OF_MEMORY       | The component drops the `portOut` call and emits a FATAL event                |

```c++
bufferRepeater.configure(Svc::BufferRepeater::FATAL_ON_OUT_OF_MEMORY);
```

