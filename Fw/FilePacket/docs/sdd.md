\page FwFilePacketClasses Fw::FilePacket Classes
# Fw::FilePacket Classes

## 1 Introduction

This module provides a type `Fw::FilePacket`.
It represents an ISF file packet.

## 2 Design

The file packet format is similar to the Protocol Data Unit (PDU) format
defined in the CCSDS File Delivery Protocol (CFDP).
See &sect; 5 of the
[CCSDS File Delivery Protocol (CFDP) Recommended Standard](https://public.ccsds.org/Pubs/727x0b4s.pdf).

Each file packet contains the following data:

* The packet type (1 byte): one of START (0), DATA (1), END (2), 
or CANCEL (3).

* The sequence index (4 bytes): an unsigned integer that
identifies each packet.
The sequence index starts at zero for each file and increases
by one for each successive packet within the file.

* The packet data (variable).
The packet data format depends upon the packet type.
The following subsections describe the formats for the different
types.

### 2.1 START Packets

A start packet has packet type START and sequence index zero.
Its data consists of the following:

* The file size in bytes (4 bytes).

* The length of the source path in bytes (1 byte).

* The source path (variable).

* The length of the destination path in bytes (1 byte).

* The destination path (variable).

### 2.2 DATA Packets

A data packet has packet type DATA.
Its data consists of the following:

* The byte offset into the entire file of the file data in this
packet (4 bytes).

* The length of the file data in bytes (2 bytes).

* The file data (variable).

### 2.3 END Packets

An end packet has packet type END.
Its data consists of the following:

* The 32-bit hash value, computed from the file data as described in 
the CFDP protocol (4 bytes).

### 2.4 CANCEL Packets

A cancel packet has packet type CANCEL.
It has no data.
