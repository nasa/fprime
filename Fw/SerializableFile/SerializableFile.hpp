// ====================================================================== 
// \title  SerializableFile.hpp
// \author dinkel
// \brief  hpp file for SerializableFile
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
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

#ifndef Fw_SerializableFile_HPP
#define Fw_SerializableFile_HPP

#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/MemAllocator.hpp>
#include <Fw/Types/SerialBuffer.hpp>

namespace Fw {

  //! The type of a packet header
  class SerializableFile {

    public:
    enum Status {
      OP_OK,
      FILE_OPEN_ERROR,
      FILE_WRITE_ERROR,
      FILE_READ_ERROR,
      DESERIALIZATION_ERROR
    };

    SerializableFile(MemAllocator* allocator, NATIVE_UINT_TYPE maxSerializedSize);
    ~SerializableFile();
    
    Status load(const char* fileName, Serializable& serializable);
    Status save(const char* fileName, Serializable& serializable);

    PRIVATE:
    void reset();
    MemAllocator* allocator;
    SerialBuffer buffer;
  };
}

#endif
