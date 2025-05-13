# Svc::TCDeframer

Deframer for the TC Space Data Link Protocl (CCSDS Standard)

- Service Types: Use "Expedited Service" Type-B Frames (meaning no sequence control)
- This means FARM flag is expected to be 0, and sequence control is set to all 0s

Implements the following service:
- VIRTUAL CHANNEL PACKET SERVICE

Implements the following functions:
- ALL FRAMES RECEPTION FUNCTION
- VC PACKET EXTRACTION FUNCTION (Router is the user to which frame data unit is delivered) ---- OR ------ use VCA_SDU with no Space Packet
- FRAME DELIVERY FUNCTION (Type-BD -> to Router)
- Frame Validation Check Procedure


Not implemented:
- Sequence Control (Type-A frames) and Control Commands
- 


## Usage Examples
Add usage examples here

### Diagrams
Add diagrams here

### Typical Usage
And the typical usage of the component here

## Class Diagram
Add a class diagram here

## Port Descriptions
| Name | Description |
|---|---|
|---|---|

## Component States
Add component states in the chart below
| Name | Description |
|---|---|
|---|---|

## Sequence Diagrams
Add sequence diagrams here

## Parameters
| Name | Description |
|---|---|
|---|---|

## Commands
| Name | Description |
|---|---|
|---|---|

## Events
| Name | Description |
|---|---|
|---|---|

## Telemetry
| Name | Description |
|---|---|
|---|---|

## Unit Tests
Add unit test descriptions in the chart below
| Name | Description | Output | Coverage |
|---|---|---|---|
|---|---|---|---|

## Requirements
Add requirements in the chart below
| Name | Description | Validation |
|---|---|---|
|---|---|---|

## Change Log
| Date | Description |
|---|---|
|---| Initial Draft |