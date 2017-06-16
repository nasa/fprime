// ====================================================================== 
// \title  Hash.hpp
// \author dinkel
// \brief  hpp file for Hash class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef UTILS_HASH_BUFFER_HPP
#define UTILS_HASH_BUFFER_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/Assert.hpp>
#include <Utils/Hash/HashConfig.hpp>

namespace Utils {
  
    //! \class HashBuffer
    //! \brief An container class for holding a hash buffer
    //!
    class HashBuffer : public Fw::SerializeBufferBase {
        public:
            // ----------------------------------------------------------------------
            // Construction and destruction 
            // ----------------------------------------------------------------------

            //! Construct a HashBuffer object
            //!
            HashBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            HashBuffer(const HashBuffer& other);
            HashBuffer();

            //! Destroy a HashBuffer object
            //!
            virtual ~HashBuffer();

            // ---------------------------------------------------------------------- 
            // Public instance methods
            // ---------------------------------------------------------------------- 

            //! Assign a hash buffer from another hash buffer
            //!
            const HashBuffer& operator=(const HashBuffer& other);

            //! Compare two hash buffers for equality
            //!
            bool operator==(const HashBuffer& other) const;

            //! Compare two hash buffers for inequality
            //!
            bool operator!=(const HashBuffer& other) const;

            //! Get the total buffer length of a hash buffer
            //!
            NATIVE_UINT_TYPE getBuffCapacity(void) const; // !< returns capacity, not current size, of buffer

            //! Get a pointer to the buffer within the hash buffer
            //!
            U8* getBuffAddr(void);
            const U8* getBuffAddr(void) const;

        private:
            // ----------------------------------------------------------------------
            // Private member variables 
            // ----------------------------------------------------------------------
            
            //! The buffer which stores the hash digest
            //!
            U8 m_data[HASH_DIGEST_LENGTH]; // packet data buffer
    };
}

#endif
