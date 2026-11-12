# Svc/TlmPacketizerPorts SDD

## 1 Introduction

The `Svc/Ports/TlmPacketizerPorts` directory has ports for the `Svc::TlmPacketizer`.

## 2 Port Descriptions

Port Type | Purpose | Arguments
---- | ---- | ----
[EnableSection](../TlmPacketizerPorts.fpp) | Enable / disable an output section | `section`, `enabled`
[ConfigureGroupRate](../TlmPacketizerPorts.fpp) | Configure a section/group's rate logic and thresholds | `section`, `tlmGroup`, `rateLogic`, `minDelta`, `maxDelta`

The per-packet override record type `PacketConfigEntry` is defined with the component in [Svc/TlmPacketizer](../../TlmPacketizer/TlmPacketizer.fpp); see its [SDD](../../TlmPacketizer/docs/sdd.md) for the `packetOverrideOut`/`packetOverrideIn` exchange.

## 3 Example Usage

[Svc/TlmPacketizer](../../TlmPacketizer/docs/sdd.md) uses `EnableSection` for its `controlIn` port and `ConfigureGroupRate` for its `configureSectionGroupRate` port.
