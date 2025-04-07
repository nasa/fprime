# Svc::FpySequencer

TODO say this is in development

The FpySequencer loads, validates and runs up to one Fpy sequence at a time. The Fpy sequencing language currently supports executing commands and delays. In the future, it will support checks against telemetry and parameters, variables, functions and arguments.

The FpySequencer is primarily composed of a state machine

TODO how to use, in a practical way

## Commands
| Name | Description |
| RUN | Loads, validates and runs a sequence |
| VALIDATE | Loads and validates a sequence |
| RUN_VALIDATED | Must be called after VALIDATE. Runs the sequence that was validated. |
| CANCEL | Cancels a running or validated sequence. After running CANCEL, the sequence should return to IDLE |

## Events
| Name | Description |

## Telemetry
| Name | Description |

## Unit Tests
Add unit test descriptions in the chart below
| Name | Description |

## Requirements
Add requirements in the chart below
| Name | Description | Validation |