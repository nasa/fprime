# Ref::SignalGen Component

## 1. Introduction

The `Ref::SignalGen` is a demonstration component that generates different waveforms. It is a source for testing plots in the ground software. It also provides a demonstration of data products.

## 2. Requirements

The requirements for `Ref::SignalGen` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
REF-SGN-001 | The `Ref::SignalGen` component shall generate telemetry with tunable waveforms | System test
REF-SGN-002 | The `Ref::SignalGen` component shall store waveform data using data products | System test

## 3. Design

### 3.1 Context

#### 3.1.1 Component Diagram

The `Ref::SignalGen` component has the following component diagram:

![`Ref::SignalGen` Diagram](img/SignalGenBDD.jpg "Ref::SignalGen")

#### 3.1.2 Data Products

The `Ref::SignalGen` component will generate data products using the `Dp` command.
It will demonstrate the two different ways to request data product buffers and will generate
a data product based on storing a commanded number of 

## 4. Dictionaries

## 5. Module Checklists

## 6. Unit Testing

## 7. Change Log

Date | Description
---- | -----------
4/20/2017 | Initial Version
4/26/2024 | Added data product demonstration



