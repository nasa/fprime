# Svc::WasmSequencer

A sequence engine based around a WebAssembly interpreter.

## Introduction

The WasmSequencer is a component that implements the F Prime sequencing paradigm to dispatch commands and event, read telemetry parameters and more. It is based around the [WebAssembly](https://webassembly.org/) standard.

The component integrates a [WebAssembly interpreter](https://github.com/nasa/spacewasm) capable of loading, validating and executing WebAssembly modules as well as a state machine to manage the interpreter.

## Requirements

| Name | Description | Rationale | Validation |
|---|---|---|---|
|   |   |   |   |

## Design

The design of this component is tightly coupled with the design of the SpaceWasm WebAssembly engine.

## Configuration

<!-- If the component requires configuration at initialization, document here -->
