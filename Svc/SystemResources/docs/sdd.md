# SystemResources Component

The system resources component downlinks information about the running F´ system. This information includes:

1. Free Memory
2. CPU load
3. Disk space

These items are downlinked as telemetry channels in response to a rate group port invocation.

**Note:** system resources requires `U64` types to be available on the target architecture.

<!-- fpp-dictionary-begin -->
## Component Dictionary

The following tables are derived from the component's FPP model.

### Commands

| Name | Kind | Description |
|---|---|---|
| `ENABLE` | `guarded` | A command to enable or disable system resource telemetry |

### Telemetry

| Name | Type | Description |
|---|---|---|
| `MEMORY_TOTAL` | `U64` | Total system memory in KB |
| `MEMORY_USED` | `U64` | System memory used in KB |
| `NON_VOLATILE_TOTAL` | `U64` | System non-volatile available in KB |
| `NON_VOLATILE_FREE` | `U64` | System non-volatile available in KB |
| `CPU` | `F32` | System's CPU Percentage |
| `CPU_00` | `F32` | System's CPU Percentage |
| `CPU_01` | `F32` | System's CPU Percentage |
| `CPU_02` | `F32` | System's CPU Percentage |
| `CPU_03` | `F32` | System's CPU Percentage |
| `CPU_04` | `F32` | System's CPU Percentage |
| `CPU_05` | `F32` | System's CPU Percentage |
| `CPU_06` | `F32` | System's CPU Percentage |
| `CPU_07` | `F32` | System's CPU Percentage |
| `CPU_08` | `F32` | System's CPU Percentage |
| `CPU_09` | `F32` | System's CPU Percentage |
| `CPU_10` | `F32` | System's CPU Percentage |
| `CPU_11` | `F32` | System's CPU Percentage |
| `CPU_12` | `F32` | System's CPU Percentage |
| `CPU_13` | `F32` | System's CPU Percentage |
| `CPU_14` | `F32` | System's CPU Percentage |
| `CPU_15` | `F32` | System's CPU Percentage |

<!-- fpp-dictionary-end -->
