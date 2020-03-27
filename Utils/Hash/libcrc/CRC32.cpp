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
            c = ((char*)data)[index];
            local_hash_handle = update_crc_32(local_hash_handle, c); 
        }
        HashBuffer bufferOut;
        // For CRC32 we need to return the one's compliment of the result:
        Fw::SerializeStatus status = bufferOut.serialize(~(local_hash_handle));
        FW_ASSERT( Fw::FW_SERIALIZE_OK == status );
        buffer = bufferOut;
    }
    
    void Hash :: 
        init(void)
    {
        this->hash_handle = 0xffffffffL;
    }

    void Hash :: 
        update(const void *const data, NATIVE_INT_TYPE len)
    {
        FW_ASSERT(data);
        char c;
        for(int index = 0; index < len; index++) {
            c = ((char*)data)[index];
            this->hash_handle = update_crc_32(this->hash_handle, c);
        }
    }
    
    void Hash :: 
        final(HashBuffer& buffer)
    {
        HashBuffer bufferOut;
        // For CRC32 we need to return the one's compliment of the result:
        Fw::SerializeStatus status = bufferOut.serialize(~(this->hash_handle));
        FW_ASSERT( Fw::FW_SERIALIZE_OK == status );
        buffer = bufferOut;
    }

}
