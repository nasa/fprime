\page MicroFs SDD
# Os::MicroFs Library

## 1. Introduction

F Prime has a number of file-based service components like `Svc::CmdSequencer`, `Svc::FileDownLink`, and `Svc::FileUplink`. These components supply widely-used core features,
but they require a file system to operate. Many deeply embedded microcontrollers don't have file storage or a file system because of highly contrianted resources. This library
was written to provide a very basic RAM-based file system that implements the F PRime `Os::File`, `Os::FileSystem`, and `Os::Directory` interfaces. Since it is RAM-based, any
data stored will be lost when the system is rebooted, but it will allow files to be stored, used, and uplinked/downlinked for as long as the system is up. Services that rely on
non-volatile files (like `Svc/PrmDb`) wouldn't make sense in this context.

## 2. Requirements

The requirements for `Os::MicroFs` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
MFS-001 | The `Os::MicroFs` library shall utilize only RAM for implementing the file system | Analysis
MFS-002 | The `Os::MicroFs` library shall utilize the `Fw::MemAllocator` base class to allocate memory | Analysis
MFS-003 | The `Os::MicroFs` library shall implement the `Os::File` class implementation | Analysis
MFS-004 | The `Os::MicroFs` library shall implement the `Os::FileSystem` class implementation | Analysis
MFS-005 | The `Os::MicroFs` library shall implement the `Os::Directory` class implementation | Analysis

## 3. Design

### 3.1 Context

#### 3.1.1 Class Diagram

The `Os::MicroFs` component has the following class diagram:

![`Svc::ActiveLogger` Diagram](img/ActiveLoggerBDD.jpg "Svc::ActiveLogger")

### 3.2 Design Description

#### 3.2.1 Overview

The file system creates fixed size buffers to hold the file contents. The files can grow to contain data within the size of the buffer, but no larger. Any attempts to write beyond the end of the buffer will be truncated. The user can specify multiple "bins" of files, where each bin can be configured to hold a specified number of files a specified length. The files have specific names that map the file requested to a particular bin and file buffer. The user cannot open arbitrary files. They must match the naming schemes.

During initialization, the library will request a block of memory from an allocator. The memory is then popoluted with:

1\) A copy of the file system

`Os::MicroFs` requests memory from the `Fw::MemAlloctor` instance passed to the following call:

```c++
void MicroFsInit(const MicroFsConfig& cfg,      //!< the configuration of the memory space
                 const PlatformUIntType id,     //!< The memory id. Value doesn't matter if allocator doesn't need it
                 Fw::MemAllocator& allocator);  //!< Memory allocator to use for memory
``` 



#### 3.2.1 Data Structures


### 3.3 Scenarios

#### 3.3.1 Receive Events

The `Svc::ActiveLogger` component accepts events from other components.

### 3.4 State

`Svc::ActiveLogger` has no state machines, but stores the state of the event severity and event ID filters.

### 3.5 Algorithms

`Svc::ActiveLogger` has no significant algorithms.

## 4. Dictionaries

TBD

## 5. Module Checklists

Document | Link
-------- | ----
Design Checklist | [Link](Checklist_Design.xlsx)
Code Checklist | [Link](Checklist_Code.xlsx)
Unit Test Checklist | [Link](Checklist_Unit_test.xlsx)

## 6. Unit Testing

To see unit test coverage run fprime-util check --coverage

## 7. Change Log

Date | Description
---- | -----------
6/25/2015 | Design review edits
7/22/2015 | Design review actions
9/7/2015 | Unit Test updates 
10/28/2015 | Added FATAL announce port
12/1/2020 | Removed event buffers and post-filter



