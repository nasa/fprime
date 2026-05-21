
# Parameter Management Functionality

## References

- [F Prime PrmDb SDD](https://github.com/nasa/fprime/blob/devel/Svc/PrmDb/docs/sdd.md)
- [Fw::Prm SDD](https://github.com/nasa/fprime/blob/devel/Fw/Prm/docs/sdd.md)
- [FPP User Guide — Parameters](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Parameters)

## Overview

Parameter management provides persistent storage and retrieval of configuration values used by components throughout the system. Parameters are named values defined in FPP models that can be loaded from a file at startup, queried by components during initialization, and updated by ground command during operation. The Parameter Database (PrmDb) is the central storage service.

### Parameter Storage

The Parameter Database stores parameter values in serialized form in an internal table, indexed by parameter ID. At startup, it loads a set of parameter values from a file on the file system. This file contains the serialized state of all saved parameters from the last save operation.

### Parameter Retrieval

During initialization, components request their parameter values from the Parameter Database. The request specifies a parameter ID, and the response includes the serialized value along with a validity status:

- **Valid** — The parameter was found in the database and returned successfully.
- **Invalid** — The parameter was not found or could not be retrieved.
- **Default** — The parameter was not in the database, so the component uses its FPP-defined default value.
- **Uninitialized** — The component has not yet called its parameter load function.

### Parameter Updates

Parameters can be updated during operation via ground command. The command specifies the component, parameter name, and new value. When a parameter is updated:

1. The new value is sent to the Parameter Database for storage.
2. The component is notified of the update and can act on the new value immediately.

A separate save command writes all current parameter values back to the file system, persisting them for the next startup.

### Off Nominal

- If the parameter file is missing or corrupt at startup, the Parameter Database starts with an empty table and components fall back to their default values.
- If a parameter value cannot be deserialized (e.g., type mismatch), the retrieval returns an invalid status.
