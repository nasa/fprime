/**
 * \file
 * \author T. Canham
 * \brief Definitions for ISF type serial IDs
 *
 * NOTE: Not currently being used
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */
#ifndef _FW_SER_IDS_HPP_
#define _FW_SER_IDS_HPP_

// Definitions of provided types serialized IDs
// Should fit in 16 bits

namespace Fw {
    enum {

        // Built-in types

        FW_TYPEID_U8 =          10, //!< U8 serialized type id
        FW_TYPEID_18 =          11, //!< I8 serialized type id
        FW_TYPEID_U16 =         12, //!< U16 serialized type id
        FW_TYPEID_I16 =         13, //!< I16 serialized type id
        FW_TYPEID_U32 =         14, //!< U32 serialized type id
        FW_TYPEID_I32 =         15, //!< I32 serialized type id
        FW_TYPEID_U64 =         16, //!< U64 serialized type id
        FW_TYPEID_I64 =         17, //!< I64 serialized type id
        FW_TYPEID_F32 =         18, //!< F32 serialized type id
        FW_TYPEID_F64 =         19, //!< F64 serialized type id
        FW_TYPEID_BOOL =        20, //!< boolean serialized type id
        FW_TYPEID_PTR =         21, //!< pointer serialized type id
        FW_TYPEID_BUFF =        22, //!< buffer serialized type id

        // PolyType

        FW_TYPEID_POLY =        30, //!< PolyType serialized type id

        // Command/Telemetry types

        FW_TYPEID_CMD_BUFF  =    40, //!< Command Buffer type id
        FW_TYPEID_CMD_STR   =    41, //!< Command string type id
        FW_TYPEID_TLM_BUFF  =    42, //!< Telemetry Buffer type id
        FW_TYPEID_TLM_STR   =    43, //!< Telemetry string type id
        FW_TYPEID_LOG_BUFF  =    44, //!< Log Buffer type id
        FW_TYPEID_LOG_STR   =    45, //!< Log string type id
        FW_TYPEID_PRM_BUFF  =    46, //!< Parameter Buffer type id
        FW_TYPEID_PRM_STR   =    47, //!< Parameter string type id
        FW_TYPEID_FILE_BUFF =    48, //!< File piece Buffer type id

        // Other types

        FW_TYPEID_EIGHTY_CHAR_STRING = 50, //!< 80 char string Buffer type id
        FW_TYPEID_INTERNAL_INTERFACE_STRING =   51, //!< interface string Buffer type id
        FW_TYPEID_FIXED_LENGTH_STRING = 52, //!< 256 char string Buffer type id
        FW_TYPEID_OBJECT_NAME = 53, //!< ObjectName string Buffer type id
        FW_TYPEID_FILE_NAME_STRING = 54, //!< FileName string Buffer type id
    };
}

#endif
