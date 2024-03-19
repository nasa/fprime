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

namespace Utils {

    Hash ::
        Hash()
    {
        this->init();
    }

    Hash ::
        ~Hash()
    {
    }

    void Hash ::
        hash(const void *const data, const NATIVE_INT_TYPE len, HashBuffer& buffer)
    {
        HASH_HANDLE_TYPE local_hash_handle;
        local_hash_handle = 0xffffffffL;
        FW_ASSERT(data);
        char c;
        for(int index = 0; index < len; index++) {
            c = static_cast<const char*>(data)[index];
            local_hash_handle = static_cast<HASH_HANDLE_TYPE>(update_crc_32(local_hash_handle, c));
        }
        HashBuffer bufferOut;
        // For CRC32 we need to return the one's complement of the result:
        Fw::SerializeStatus status = bufferOut.serialize(~(local_hash_handle));
        FW_ASSERT( Fw::FW_SERIALIZE_OK == status );
        buffer = bufferOut;
    }

    void Hash ::
        init()
    {
        this->hash_handle = 0xffffffffL;
    }

    void Hash ::
        update(const void *const data, NATIVE_INT_TYPE len)
    {
        FW_ASSERT(data);
        char c;
        for(int index = 0; index < len; index++) {
            c = static_cast<const char*>(data)[index];
            this->hash_handle = static_cast<HASH_HANDLE_TYPE>(update_crc_32(this->hash_handle, c));
        }
    }

    void Hash ::
        final(HashBuffer& buffer)
    {
        HashBuffer bufferOut;
        // For CRC32 we need to return the one's complement of the result:
        Fw::SerializeStatus status = bufferOut.serialize(~(this->hash_handle));
        FW_ASSERT( Fw::FW_SERIALIZE_OK == status );
        buffer = bufferOut;
    }

    void Hash ::
      final(U32 &hashvalue)
    {
      FW_ASSERT(sizeof(this->hash_handle) == sizeof(U32));
      // For CRC32 we need to return the one's complement of the result:
      hashvalue = ~(this->hash_handle);
    }

    void Hash ::
      setHashValue(HashBuffer &value)
    {
      Fw::SerializeStatus status = value.deserialize(this->hash_handle);
      FW_ASSERT( Fw::FW_SERIALIZE_OK == status );
      // Expecting `value` to already be one's complement; so doing one's complement
      // here for correct hash updates
      this->hash_handle = ~this->hash_handle;
    }
}
