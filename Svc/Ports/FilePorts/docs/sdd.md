# Svc/FilePorts SDD

## 1 Introduction

The `Svc/Ports/FilePorts` directory has ports related to file handling between components.

## 2 Port Descriptions

Port Type | Purpose | Arguments 
---- | ---- |
[FileAnnounce](../FileAnnounce.fpp) | This port is used to announce the production of a new file to interested components | `file_name` - name of uplinked file

## 3 Example Usage

[Svc/FileUplink](../../../FileUplink/docs/sdd.md) uses this port to announce the completion of a file uplink. 