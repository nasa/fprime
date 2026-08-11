# CFDP Utilities

## Overview

This directory contains CFDP utility classes used by the F' Framework.

## Contents

- **Checksum/**: CCSDS CFDP-compliant 32-bit checksum implementation

## Where is CfdpManager?

The main CFDP protocol implementation is located at:

**`Svc/Ccsds/CfdpManager/`**

If you're looking for the CFDP file transfer component, you'll find it there.

## Why is Checksum here?

The Checksum utility is used by the F' Framework's FilePacket class (`Fw/FilePacket/FilePacket.hpp`), which predates the CfdpManager component and supports legacy file transfer components (FileUplink, FileDownlink). To maintain framework independence, Checksum cannot be moved into CfdpManager.

## Future

A long-term solution would be to relocate Checksum to a framework utilities directory (e.g., `Fw/Utils/`), further clarifying the architectural separation.
