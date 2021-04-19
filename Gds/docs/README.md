# Gds Command and Control Tool

## 1. Introduction

This is a tool intended to be used with FPrime deployments to exchange commands, telemetry, events and other messages.
This tool includes logging capability and can be connected to several adapters to communicate with ground station equipment.

## 2. Message Field Configuration

Certain deployments may alter the fields of messages for various reasons including memory conservation.
To make the corresponding changes to the Gds for successful communication, 
please edit the `Gds/src/fprime_gds/common/utils/config_manager.py` file accordingly. 
Each field in the `_set_defaults()` function corresponds to a specific compile time flag in the `/config/FpConfig.hpp` file for an FPrime deployment.
Below is a table of corresponding fields that must match between deployment and Gds in order for successful communication to occur.

Gds Field | FPrime Flag
----------- | ----------- |
self.__prop['types']['msg_len'] | TOKEN_TYPE
self.__prop['types']['msg_desc'] | FwPacketDescriptorType
self.__prop['types']['op_code'] | FwOpcodeType
self.__prop['types']['ch_id'] | FwChanIdType
self.__prop['types']['event_id'] | FwEventIdType
self.__prop['types']['pkt_id'] | Unknown
self.__prop['types']['key_val'] | unknown

It is important to note that the default value of all of these fprime fields is U32, which is the behavior when none of these compile time flags are changed in the `FpConfig.hpp` file.
