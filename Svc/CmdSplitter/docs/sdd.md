\page SvcCmdSplitter Svc::CmdSplitter Component
# Svc::CmdSplitter Component

## 1. Introduction

The `Svc::CmdSplitter` splits an uplinked command execution to two separate `Svc::CmdDispatcher` components: one "local" and the other "remote". This splitting is done by opcode where local commands are commands whose opcode is less than a configured base and remote commands are those opcodes equal to or larger than that configurated base setting. `Svc::CmdSplitter` is intended to be used as part of the hub pattern to route command to a command dispatcher in the remote deployment.

All input and output are done through port arrays that mirror the port array of input commands to the command dispatcher. In this way `Svc::CmdSplitter` may support multiple command sources in the same manner that `Svc::CmdDispatcher` does.

## 2. Requirements

The requirements for `Svc::CmdSplitter` are as follows:

| Requirement          | Description                                                                                          | Verification Method |
|----------------------|------------------------------------------------------------------------------------------------------|---------------------|
| SVC-CMD-SPLITTER-000 | The `Svc::CmdSplitter` component shall support multiple command sources.                             | Unit Test           |
| SVC-CMD-SPLITTER-001 | The `Svc::CmdSplitter` component shall accept incoming command buffers.                              | Unit Test           |
| SVC-CMD-SPLITTER-002 | The `Svc::CmdSplitter` component shall route commands under a configured value to the "local" port.  | Unit Test           |
| SVC-CMD-SPLITTER-003 | The `Svc::CmdSplitter` component shall route commands under a configured value to the "remote" port. | Unit Test           |
| SVC-CMD-SPLITTER-004 | The `Svc::CmdSplitter` component shall route commands to the "local" port when an error occurs.      | Unit Test           |
| SVC-CMD-SPLITTER-005 | The `Svc::CmdSplitter` component shall forward command status responses.                             | Unit Test           |

## 3. Design

### 3.1 Ports

| Name                | Type           | Kind             | Description                                                              |
|---------------------|----------------|------------------|--------------------------------------------------------------------------|
| CmdBuff             | Fw.Com         | sync input array | Array of incoming command buffer.                                        |
| seqCmdStatus        | Fw.CmdResponse | sync input array | Array of incoming command status from both local and remote dispatchers. |
| LocalCmd            | Fw.Com         | output array     | Array of outgoing command buffer for local command dispatcher.           |
| RemoteCmd           | Fw.Com         | output array     | Array of outgoing command buffer for remote command dispatcher.          |
| forwardSeqCmdStatus | Fw.CmdResponse | output array     | Array of outgoing forwarded command status.                              |

> Port arrays mirror those of the command dispatcher in order to support multiple sources in the same fashion that command dispatcher does.

### 3.2 Functional Description

The `Svc::CmdSplitter` routes an incoming command buffer of type `Fw::ComBuffer` to a local or remote command dispatcher.  This is done by comparing the command's opcode to the configured base opcode value. All command responses are forwarded through.

### 3.1 State

`Svc::CmdSplitter` has only one internal state variable: `m_remoteBase` the configured base opcode for remote opcodes. This is set by a call to the `configure()` method at system start-up.

### 3.2 Algorithms

`Svc::CmdSplitter` has no significant algorithms.

## 4. Unit Testing

To see unit test coverage run `fprime-util check --coverage` in the `Svc::CmdSplitter` directory

## 5. Change Log

| Date       | Description                     |
|------------|---------------------------------|
| 2023-06-12 | Initial                         |
| 2023-09-27 | Support multiple command inputs |
