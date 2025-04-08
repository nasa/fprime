# Svc::AMPCSSequence Class

`AMPCSSequence` is a derived class of `CmdSequencer::Sequence`.
It loads, validates, and runs AMPCS-compatible sequence files.

<a name="File_Format"></a>
## 1 File Format

A sequence file consists of a 32-bit sequence header followed by
up to 32,768 bytes of record data.

The record data consists of zero or more records.
Each record has the following format:

Record Field | Size in Bytes | Description
------------ | ------------- | -----------
Time Flag    | 1             | Either 0x0 denoting absolute time or 0xFF denoting relative time
Time         | 4             | The time in seconds
CMD Length   | 2             | The size in bytes of the Command field
Command      | < 1017    | A serialized command in F Prime format, with a 16-bit opcode

## 2 Implementation

The `AMPCSSequence` class implements the virtual functions of
`CmdSequencer::Sequence` as follows.

<a name="loadFile"></a>
### 2.1 `loadFile(`*filename*`)`

1. Read the stored CRC value from *filename*`.CRC32`.

2. Ask the OS for the size of the sequence file, and use it to compute the size
of the command record data.

3. Read the 32-bit sequence header.

4. Read the command record data into a serial buffer *B*. 

5. Check that the format of the records is valid according to the binary format
specified in [File Format](#File_Format).

6. Compute the CRC value of the binary file and check it against the stored value.

7. Fill in `m_header` as follows:

    * `m_fileSize`: The size of the command record data, computed in item 2 above.

    * `m_numRecords`: Count the records when validating them.

    * `m_timeBase`: Use 0xFFFF (don't care).

    * `m_timeContext`: Use 0xFF (don't care).

### 2.2 `hasMoreRecords`

Return `true` if and only if *B* has more data.

### 2.3 `nextRecord`

Deserialize and return the next record stored in the serial buffer.
The deserialized record contains the following binary data in its 
argument field:

Field             | Size in Bytes | Value
----------------- | ------------- | -----------
Packet Descriptor | 4             | `Fw::ComPacket::FW_PACKET_COMMAND`
Opcode            | 4             | The opcode stored in bytes 0-1 of the serial record, zero-extended to four bytes
Arguments         | Variable      | Bytes 2-end of the serial record

### 2.4 `reset`

Reset *B* for deserialization.

### 2.5 `clear`

Reset *B* for serialization.
