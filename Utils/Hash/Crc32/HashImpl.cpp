// ======================================================================
// \title  CRC32.cpp
// \author dinkel
// \brief  cpp file for CRC32 implementation of Hash class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Utils/Hash/Hash.hpp>
#include "Crc32.hpp"

namespace Utils {

Hash ::Hash() {
    this->init();
}

Hash ::~Hash() = default;

void Hash ::hash(const void* const data, const FwSizeType len, HashBuffer& buffer) {
    Hash crc32;
    crc32.update(data, len);
    crc32.finalize(buffer);
}

void Hash ::init() {
    this->hash_handle = 0xffffffffL;
}

void Hash ::update(const void* const data, FwSizeType len) {
    static_assert(sizeof(Utils::Hash::hash_handle) == sizeof(U32), "hash handle size must match CRC32 size");
    FW_ASSERT(data);
    this->hash_handle = crc32_ieee802_3_update(static_cast<const U8*>(data), len, this->hash_handle);
}

void Hash ::finalize(HashBuffer& buffer) const {
    HashBuffer bufferOut;
    // For CRC32 we need to return the one's complement of the result:
    Fw::SerializeStatus status = bufferOut.serializeFrom(~(this->hash_handle));
    FW_ASSERT(Fw::FW_SERIALIZE_OK == status);
    buffer = bufferOut;
}

void Hash ::finalize(U32& hashvalue) const {
    // For CRC32 we need to return the one's complement of the result:
    hashvalue = ~(this->hash_handle);
}

void Hash ::setHashValue(HashBuffer& value) {
    Fw::SerializeStatus status = value.deserializeTo(this->hash_handle);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == status);
    // Expecting `value` to already be one's complement; so doing one's complement
    // here for correct hash updates
    this->hash_handle = ~this->hash_handle;
}

void Hash ::setHashValue(U32 value) {
    this->hash_handle = ~value;
}

}  // namespace Utils
