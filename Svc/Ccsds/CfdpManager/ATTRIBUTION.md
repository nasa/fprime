# CFDP Manager Attribution

This component implements the CCSDS File Delivery Protocol (CFDP) for F-Prime (F'). It includes both ported code from NASA's Core Flight System (cFS) CFDP application and new F' implementations.

## Source Attribution

Portions of this code are derived from the NASA Core Flight System (cFS) CFDP (CF) Application:
- **Repository**: https://github.com/nasa/CF
- **Version**: 3.0.0
- **License**: Apache License 2.0
- **Copyright**: Copyright (c) 2019 United States Government as represented by the Administrator of the National Aeronautics and Space Administration
- **NASA Docket**: GSC-18,447-1

## Files Ported from CF

The following files are ports/adaptations from CF source code and retain the original NASA copyright:

### Core Engine & Transaction Management
- `CfdpEngine.hpp` / `.cpp` - from `cf_cfdp.c` / `cf_cfdp.h`
- `CfdpTransaction.hpp` - from `cf_cfdp_r.h` / `cf_cfdp_s.h` / `cf_cfdp_dispatch.h`
- `CfdpTxTransaction.cpp` - from `cf_cfdp_s.c` / `cf_cfdp_dispatch.c`
- `CfdpRxTransaction.cpp` - from `cf_cfdp_r.c` / `cf_cfdp_dispatch.c`

### Data Structures & Utilities
- `CfdpTypes.hpp` - from `cf_cfdp_types.h`
- `CfdpUtils.hpp` / `.cpp` - from `cf_utils.h` / `cf_utils.c`
- `CfdpChannel.hpp` / `.cpp` - from channel functions in `cf_cfdp.c` / `cf_utils.c`
- `CfdpChunk.hpp` / `.cpp` - from `cf_chunks.h` / `cf_chunks.c`
- `CfdpClist.hpp` / `.cpp` - from `cf_clist.h` / `cf_clist.c`
- `CfdpPdu.hpp` - from `cf_cfdp_pdu.h`

Each of these files includes the full NASA copyright notice and Apache 2.0 license text in its header.

## New F-Prime Implementations

The following files are new implementations for F-Prime and do not contain CF-derived code:

### Integration Layer
- `CfdpManager.hpp` / `.cpp` - F-Prime component wrapper
- `CfdpTimer.hpp` / `.cpp` - F-Prime timer implementation

### PDU Object-Oriented Implementation
All files in the `Types/` directory are new F' serializable implementations based on the CFDP Blue Book specification (CCSDS 727.0-B-5):
- `Types/Pdu.hpp` / `.cpp`
- `Types/PduHeader.cpp`
- `Types/MetadataPdu.cpp`
- `Types/FileDataPdu.cpp`
- `Types/EofPdu.cpp`
- `Types/FinPdu.cpp`
- `Types/AckPdu.cpp`
- `Types/NakPdu.cpp`

These files implement CFDP PDU encoding/decoding based on the specification rather than porting CF's C-style codec.

## License

This component as a whole is licensed under the Apache License 2.0. See the top-level [LICENSE.txt](../../../LICENSE.txt) for the full license text.

The CF-derived portions retain their original NASA copyright and Apache 2.0 license as documented in their file headers.
