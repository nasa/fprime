// ====================================================================== 
// \title  SHA256.cpp
// \author dinkel
// \brief  cpp file for SHA implementation of Hash class
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
        U8 out[SHA256_DIGEST_LENGTH];
        U8* ret = SHA256((U8*) data, len, out); 
        FW_ASSERT(ret != NULL);
        HashBuffer bufferOut(out, sizeof(out));
        buffer = bufferOut;
    }
    
    void Hash :: 
        init(void)
    {
        int ret = SHA256_Init(&this->hash_handle);
        FW_ASSERT(ret == 1);
    }

    void Hash :: 
        update(const void *const data, NATIVE_INT_TYPE len)
    {
        int ret = SHA256_Update(&this->hash_handle, (U8*) data, len);
        FW_ASSERT(ret == 1);
    }
    
    void Hash :: 
        final(HashBuffer& buffer)
    {
        U8 out[SHA256_DIGEST_LENGTH];
        int ret = SHA256_Final(out, &this->hash_handle);
        FW_ASSERT(ret == 1);
        HashBuffer bufferOut(out, sizeof(out));
        buffer = bufferOut;
    }

}
