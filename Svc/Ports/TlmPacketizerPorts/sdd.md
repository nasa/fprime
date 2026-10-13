# Svc/TlmPacketizerPorts SDD

## 1 Introduction

The `Svc/Ports/TlmPacketizerPorts` directory has ports and types for the `Svc::TlmPacketizer`.

## 2 Port Descriptions

Port Type | Purpose | Arguments
---- | ---- | ----
[EnableSection](../TlmPacketizerPorts.fpp) | Enable / disable an output section | `section`, `enabled`
[ConfigureGroupRate](../TlmPacketizerPorts.fpp) | Configure a section/group's rate logic and thresholds | `section`, `tlmGroup`, `rateLogic`, `minDelta`, `maxDelta`

The `PacketConfigEntry` type (packetId, section, config, source) is also defined here. Per-packet overrides are exchanged with an external persistence component as `PacketConfigEntry` records serialized into an `Fw.Buffer` over the packetizer's `configOut`/`configIn` ports (`Fw.BufferSend`).

## 3 Example Usage

[Svc/TlmPacketizer](../../TlmPacketizer/docs/sdd.md) uses `EnableSection` for its `controlIn` port and `ConfigureGroupRate` for its `configureSectionGroupRate` port.
