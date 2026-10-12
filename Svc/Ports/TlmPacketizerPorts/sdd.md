# Svc/TlmPacketizerPorts SDD

## 1 Introduction

The `Svc/Ports/TlmPacketizerPorts` directory has ports and types for the `Svc::TlmPacketizer`.

## 2 Port Descriptions

Port Type | Purpose | Arguments
---- | ---- | ----
[EnableSection](../TlmPacketizerPorts.fpp) | Enable / disable an output section | `section`, `enabled`
[ConfigureGroupRate](../TlmPacketizerPorts.fpp) | Configure a section/group's rate logic and thresholds | `section`, `tlmGroup`, `rateLogic`, `minDelta`, `maxDelta`
[TlmPacketConfigUpdate](../TlmPacketizerPorts.fpp) | Mirror (`configOut`) or reload (`configIn`) per-packet overrides to/from an external persistence component | `count`, `batch`

## 3 Example Usage

[Svc/TlmPacketizer](../../TlmPacketizer/docs/sdd.md) uses these ports for its `controlIn`, `configureSectionGroupRate`, `configOut`, and `configIn` ports.