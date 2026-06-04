# How-To: Load Parameters in Batch


## Overview

Readers of this guide are encouraged to read through the documentation for [**PrmDb component**](https://github.com/nasa/fprime/blob/devel/Svc/PrmDb/docs/sdd.md) (F´ framework standard), which stores and manages parameters in F´ deployments. This guide will go over two methods of updating parameters: through a `.dat` file or through a `.seq` file. Both types of files are created by the `fprime-prm-write tool` using a JSON file with the parameters. The PrmDb component loads parameters from binary `.dat` files that contain a CRC32 header followed by parameter records. CmdSeq loads parameters from a binary created from a `.seq` file.

This guide uses the [**Ref**](https://github.com/nasa/fprime/tree/devel/Ref) (reference) F´ project as an example.

*Contents:*

1. [Why Import Parameters?](#why-import-parameters)
2. [Creating a Parameters JSON File](#creating-a-parameters-json-file)
3. [Method 1: Generate a .dat File](#method-1-generate-a-dat-file)
4. [Method 2: Generate a .seq File](#method-2-generate-a-seq-file)
5. [Troubleshooting](#troubleshooting)

## Why Import Parameters?

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

> [!NOTE]
> Component instance names must be fully qualified (e.g., `Ref.recvBuffComp`). Parameter values support complex F´ types:
> - **Primitives**: Numbers (`20`, `99.99`), booleans, strings
> - **Enums**: String constants (e.g., `"RED"`, `"BLUE"`)
> - **Arrays**: JSON arrays (e.g., `["ONE", "BLUE"]`)
> - **Structs**: Nested objects with field names as keys (e.g., `{"firstChoice": "RED", "secondChoice": "BLUE"}`)

> [!TIP]
> **Using the `--defaults` flag:** By default, the `fprime-prm-write` tool only includes parameters explicitly listed in your JSON file. If you add the `--defaults` flag, the tool will include all parameters that have default values defined in the dictionary. This effectively resets all parameters to their defaults, except for those you specify in the JSON file. This flag works for both `.dat` and `.seq` file generation.

All steps from this point on either take place in the terminal or the GDS GUI.

## Method 1: Generate a `.dat` File

Use this method to load a binary parameter file into the `PrmDb` component at any time. 

### Step 1: Generate the `.dat` File

Run the tool in the terminal:

**General syntax:**
```bash
fprime-prm-write dat <json file> --dictionary <path to compiled FPrime dict>
```

**Example: Ref project:**
```bash
fprime-prm-write dat params.json \
  --dictionary build-artifacts/Linux/Ref/dict/RefTopologyDictionary.json
```

This creates `params.dat` in the same directory as your JSON file.

> [!NOTE]
> The output file is named based on your input JSON file. If your JSON file is named `my_params.json`, the output will be `my_params.dat`.

### Step 2: Start the GDS

**With GUI:**
```bash
fprime-gds
```

**Without GUI (Command Line):**
```bash
fprime-gds -g none --log-directly
```

### Step 3: Load the Parameter File

**Using GUI:**
1. Launch the GDS using `fprime-gds` and open up the browser window
2. Navigate to the **Commanding** tab
3. Find and select `FileHandling.prmDb.PRM_LOAD_FILE`
4. Fill in the arguments:
   - **fileName**: `"params.dat"`
   - **mode**: `MERGE` (merge with existing) or `CLEAR` (replace all)
5. Send the command
6. Check the **Events** tab for `PrmFileLoadComplete`

**Command Line:**
```bash
# Load the .dat file into staging area 
fprime-cli command-send FileHandling.prmDb.PRM_LOAD_FILE "params.dat" MERGE
# Check events for success
grep -q "PrmFileLoadComplete" gds.log
```

**Expected events:**

| Event | Status | Description |
|-------|--------|-------------|
| `PrmFileLoadComplete` | ✓ | File loaded successfully |
| `PrmDbFileLoadFailed` | ✗ | File not found |
| `PrmFileBadCrc` | ✗ | CRC checksum error |

### Step 4: Commit the Staged Parameters

After verifying the load succeeded, commit to make parameters active:

**Using GUI:**

Send `FileHandling.prmDb.PRM_COMMIT_STAGED` (no arguments). Check the **Events** tab for `PrmDbCopyAllComplete` to confirm the commit succeeded.

**Command Line:**
```bash
# Commit staged parameters
fprime-cli command-send FileHandling.prmDb.PRM_COMMIT_STAGED
# Verify the commit operation succeeded
grep -q "PrmDbCopyAllComplete" gds.log
```

### Step 5 (optional): Verify Parameter Changes

**Downlink and decode PrmDb.dat**

If your deployment has file downlink configured (using `Svc.FileDownlink`), you can verify by downloading and decoding the parameter database:

1. Use your deployment's file downlink commands to retrieve `PrmDb.dat` from the FSW
```bash fileDownlink.FileDownlink_SendFile```
2. Decode the downloaded file to a JSON file for human readability
   
   **General syntax:** Using the inverse `fprime-prm-decode` tool
   ```bash
   fprime-prm-decode PrmDb.dat \
     --dictionary build-fprime-automatic-<platform>/<YourDeployment>/dict/<YourDeployment>TopologyDictionary.json \
     --format json \
     --output downlinked_params.json
   ```

3. Compare with your input
   ```bash
   diff params.json downlinked_params.json
   ```

## Method 2: Generate a `.seq` File

Sometimes you need to update parameters while F´ is running without loading files. This can be accomplished with a sequence of _PRM_SET commands, which the `fprime-prm-write` tool can automatically create for you. This creates `.seq` which can be compiled and executed by `Svc::CmdSequencer`.

> [!TIP]
> **Using the `--save` flag:** The generated `.seq` file contains `_PRM_SET` commands that update parameter values in memory. If you want the changes to persist across FSW restarts, add the `--save` flag to include `_PRM_SAVE` commands after each `_PRM_SET`, which writes the parameters to the PrmDb.dat file on the FSW.

### Step 1: Generate the Sequence File

**General syntax:**
```bash
fprime-prm-write seq <json file> --dictionary <path to compiled FPrime dict>
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
   fprime-seqgen params.seq --dictionary build-fprime-automatic-<platform>/<YourDeployment>/dict/<YourDeployment>TopologyDictionary.json
   ```
   This creates `params.bin`.

2. Load the sequence into `CmdSequencer`:
   - **GUI**: Send `FileHandling.cmdSeq.CS_RUN` command with the binary file name
   - **Command Line**: 
     ```bash
     fprime-cli commands send FileHandling.cmdSeq.CS_RUN "params.bin" RUN_NOW
     ```

3. Verify the sequence executed successfully by checking events in the **Events** tab or command line

For more details, follow the standard [F´ sequencing workflow](https://nasa.github.io/fprime/UsersGuide/user/cmd-seq.html).

## Troubleshooting

### Error: `PrmFileBadCrc`

**Cause:** CRC checksum mismatch.

**Fix:** Regenerate with an up-to-date version of `fprime-prm-write`.

### Error: `PrmDbFileLoadFailed`

**Cause:** File not found.

**Fix:** Verify file path and ensure it's accessible to the FSW.

### Error: `RuntimeError: Unable to find param <name> in dictionary`

**Cause:** Parameter name doesn't match dictionary.

**Fix:**
- Use fully qualified component names (e.g., `Ref.recvBuffComp`)
- Check parameter names are exact (case-sensitive)
- Verify dictionary path is correct

## References

1. [fprime-prm-write Tool README](https://github.com/nasa/fprime-gds/blob/devel/docs/prm-write-README.md)
2. [Svc::PrmDb Component SDD](https://github.com/nasa/fprime/blob/devel/Svc/PrmDb/docs/sdd.md)
3. [Ref Deployment](https://github.com/nasa/fprime/tree/devel/Ref)
4. [F´ Command Sequencing Guide](https://nasa.github.io/fprime/UsersGuide/user/cmd-seq.html)
