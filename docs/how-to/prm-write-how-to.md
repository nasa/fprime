# How-To: Load Parameters in Batch


## Overview

Readers of this guide are encouraged to read through the documentation for the [PrmDb component](https://fprime.jpl.nasa.gov/latest/Svc/PrmDb/docs/sdd/) and the [CmdSequencer component](https://fprime.jpl.nasa.gov/latest/Svc/CmdSequencer/docs/sdd/). This guide will go over two methods of updating parameters: through a `.dat` file that PrmDb loads directly, or by generating a command sequence (`.seq` file) to dispatch a series of commands.

This guide uses the [**Ref**](https://github.com/nasa/fprime/tree/devel/TestDeploymentsProject/Ref) (reference) F´ project as an example.

*Contents:*

1. [Why Import Parameters in Batch?](#why-import-parameters-in-batch)
2. [Creating a Parameters JSON File](#creating-a-parameters-json-file)
3. [Method 1: Generate a .dat File](#method-1-generate-a-dat-file)
4. [Method 2: Generate a .seq File](#method-2-generate-a-seq-file)
5. [Troubleshooting](#troubleshooting)

## Why Import Parameters in Batch?

The `fprime-prm-write` tool allows you to:
- Set initial parameter values for system startup
- Update multiple parameters at once without manual commanding
- Version-control parameter configurations alongside your code
- Share parameter sets between team members or mission phases

## Creating a Parameters JSON File

The `fprime-prm-write` tool accepts parameter values in JSON format. This format is human-readable and supports all F´ parameter types including primitives, enums, arrays, and structs.

### JSON File Format

The JSON should consist of a key-value map of component instance names to an inner key-value map. The inner key-value map should consist of parameter name-to-value map entries. In other words, the JSON file should have a **component name → parameter map** structure.

Create a JSON file (e.g., `params.json`) anywhere in your project directory:

**Generic Format:**
```json
{
  "ComponentInstance": {
    "parameterName": value,
    "anotherParameter": value
  },
  "AnotherComponent": {
    "paramName": value
  }
}
```

### Example: Ref Project Parameters


**`params.json`:**
```json
{
    "Ref.recvBuffComp": {
        "parameter1": 20,
        "parameter2": -5
    },

    "Ref.sendBuffComp": {
        "parameter3": 25,
        "parameter4": 99.99
    },
    "Ref.typeDemo": {
        "CHOICE_PRM": "RED",
        "CHOICES_PRM": ["ONE", "BLUE"],
        "CHOICE_PAIR_PRM": {
            "firstChoice": "RED",
            "secondChoice": "BLUE"
        }
    }
}
```
Component instance names must be fully qualified (e.g., `Ref.recvBuffComp`). 

> [!NOTE]
> The .seq method only handles primitives (strings, numbers, booleans, enums).
> Parameter values may be complex F´ types **only when using the .dat method**.

> [!TIP]
> **Using the `--defaults` flag:** The `fprime-prm-write` tool only automatically includes parameters explicitly listed in your JSON file. If you add the `--defaults` flag, the tool will include all parameters that have default values defined in the dictionary. This effectively resets all parameters to their defaults, except for those you specify in the JSON file. This flag works for both `.dat` and `.seq` file generation.

All steps from this point on either take place in the terminal or the GDS GUI.

## Method 1: Generate a `.dat` File

With this method, the user uplinks a `.dat` file to the FSW and sends the `PRM_LOAD_FILE` and `PRM_COMMIT_STAGED` commands to the PrmDb component, which loads parameter values in batch from the file.

### Step 1: Generate the `.dat` File

Now you will run the `fprime-prm-write` tool in the terminal.

**General syntax:**
```bash
fprime-prm-write dat <path/to/paramFile.json> --dictionary <path/to/TopologyDictionary.json>
```

**Example: Ref project:**
```bash
fprime-prm-write dat params.json 
  --dictionary build-artifacts/Linux/Ref/dict/RefTopologyDictionary.json
```

This creates `params.dat` in the same directory as your JSON file.

> [!NOTE]
> The output file is named based on your input JSON file. If your JSON file is named `my_params.json`, the output will be `my_params.dat`.

### Step 2: Load the Parameter File

1. Launch the GDS using `fprime-gds` and open up the browser window
2. Navigate to the **Uplink** tab to [stage and uplink](https://fprime.jpl.nasa.gov/devel/docs/user-manual/overview/gds-introduction/#navigating-the-gds-gui) the `params.dat` file.
    - Specify an onboard Destination Folder path
3. Navigate to the **Commanding** tab
4. Find and select `FileHandling.prmDb.PRM_LOAD_FILE`
5. Fill in the arguments:
   - **fileName**: `path/params.dat`
   - **mode**: `MERGE` (merge with existing) or `CLEAR` (replace all)
6. Send the command
7. Check the **Events** tab for `PrmFileLoadComplete`

**Expected events:**

| Event | Status | Description |
|-------|--------|-------------|
| `PrmFileLoadComplete` | ✓ | File loaded successfully |
| `PrmDbFileLoadFailed` | ✗ | File not found |
| `PrmFileBadCrc` | ✗ | CRC checksum error |

### Step 3: Commit the Staged Parameters

After verifying the load succeeded, commit to make parameters active:

In the **Commanding** tab, send `FileHandling.prmDb.PRM_COMMIT_STAGED` (no arguments). Check the **Events** tab for `PrmDbCopyAllComplete` to confirm the commit succeeded.

### Step 4 (optional): Verify Individual Parameter Changes

**Downlink and decode PrmDb.dat**

If your deployment has file downlink configured (using `Svc.FileDownlink`), you can verify by downloading and decoding the parameter database:

1. Use your deployment's file downlink commands to retrieve `PrmDb.dat` from the FSW
```bash fileDownlink.FileDownlink_SendFile```
2. Decode the downloaded file to a JSON file for human readability
   
   **General syntax:** Using the inverse `fprime-prm-decode` tool
   ```bash
   fprime-prm-decode PrmDb.dat
     --dictionary <path/to/TopologyDictionary.json>
     --format json
     --output downlinked_params.json
   ```

3. Compare with your input
   ```bash
   diff params.json downlinked_params.json
   ```

## Method 2: Generate a `.seq` File

With this method, the user compiles a `.seq` file into a `.bin` and uplinks it to CmdSequencer, which dispatches `_PRM_SET` commands (and `_PRM_SAVE` if `--save` was used) to each component.

> [!TIP]
> **Using the `--save` flag:** The generated `.seq` file contains `_PRM_SET` commands that update parameter values in memory. If you want the changes to persist across FSW restarts, add the `--save` flag to include `_PRM_SAVE` commands after each `_PRM_SET`, which writes the parameters to the PrmDb.dat file on the FSW.

### Step 1: Generate the Sequence File

**General syntax:**
```bash
fprime-prm-write seq <json file> --dictionary <path/to/TopologyDictionary.json>
```

**Example: Ref project:**
```bash
fprime-prm-write seq params.json \
  --dictionary build-artifacts/Linux/Ref/dict/RefTopologyDictionary.json
```

This creates `params.seq` in the same directory as your JSON file.

### Step 2: Execute the Sequence

1. Compile the sequence file to binary format:
   ```bash
   fprime-seqgen params.seq --dictionary <path/to/TopologyDictionary.json>
   ```
   This creates `params.bin`.

2. Load the sequence into `CmdSequencer` in the GDS GUI:
    1. In the **Uplink** tab, [stage and uplink](https://fprime.jpl.nasa.gov/devel/docs/user-manual/overview/gds-introduction/#navigating-the-gds-gui) the `params.bin` file.
        - Specify the Destination Folder onboard, such as `/seq`
    2. Send the `<project>.cmdSeq.CS_RUN` command with the binary file name
        - File name should include the path (e.g. `/seq/params.bin`)
        - `BLOCK`: The command waits for the sequence to complete before returning status
        - `NO_BLOCK`: The command returns immediately and the sequence runs in the background

3. Verify the sequence executed successfully by checking events in the **Events** tab or command line
    ```bash
    grep -E "CS_Sequence" <path/to/project/log/file>
    ```
    | Event | Status | Description |                                                                                  
    |-------|--------|-------------|                                                                                  
    | `CS_SequenceLoaded` | ✓ | Sequence file loaded successfully |                                                   
    | `CS_CommandComplete` | ✓ | Each command in sequence completed |                                                 
    | `CS_SequenceComplete` | ✓ | Entire sequence finished successfully |                                             
    | `CS_FileNotFound` | ✗ | .bin file not found |                                                                   
    | `CS_FileReadError` | ✗ | Error reading the file |                                                               
    | `CS_FileInvalid` | ✗ | Invalid file format or structure |                                                       
    | `CS_FileCrcFailure` | ✗ | CRC checksum mismatch |                                                               
    | `CS_CommandError` | ✗ | A command in the sequence failed |  
 
    For more details, follow the standard [F´ sequencing workflow](https://fprime.jpl.nasa.gov/devel/docs/user-manual/gds/seqgen/).

## Troubleshooting

### Error: `PrmFileBadCrc`

**Cause:** CRC checksum mismatch.

**Fix:** Regenerate with an up-to-date version of `fprime-prm-write`.

### Error: `PrmDbFileLoadFailed`

**Cause:** File not found.

**Fix:** Verify file path and ensure it's accessible to the FSW (uplinked to the spacecraft, providing the onboard file path).

### Error: `RuntimeError: Unable to find param <name> in dictionary`

**Cause:** Parameter name doesn't match dictionary.

**Fix:**
- Use fully qualified component names (e.g., `Ref.recvBuffComp`)
- Check parameter names are exact (case-sensitive)
- Verify dictionary path is correct

## References
1. [Svc::PrmDb Component SDD](https://fprime.jpl.nasa.gov/latest/Svc/PrmDb/docs/sdd/)
2. [Svc::CmdSequencer Component SDD](https://fprime.jpl.nasa.gov/latest/Svc/CmdSequencer/docs/sdd/)
3. [Ref Deployment](https://github.com/nasa/fprime/tree/devel/TestDeploymentsProject/Ref)
4. [F´ Command Sequencing Guide](https://fprime.jpl.nasa.gov/latest/docs/user-manual/gds/seqgen/)
