// ======================================================================
// \title  SerializableFile.cpp
// \author dinkel
// \brief  cpp file for SerializableFile
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/SerializableFile/SerializableFile.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/File.hpp"

namespace Fw {

  SerializableFile::SerializableFile(MemAllocator* allocator, NATIVE_UINT_TYPE maxSerializedSize) :
    allocator(allocator),
    recoverable(false), // for compiler; not used
    actualSize(maxSerializedSize),
    buffer( (U8 *) this->allocator->allocate(0, actualSize, recoverable), actualSize)
  {
    // assert if allocator returns smaller size
    FW_ASSERT(maxSerializedSize == actualSize,maxSerializedSize,actualSize);
    FW_ASSERT(NULL != buffer.getBuffAddr());
  }

  SerializableFile::~SerializableFile() {
    this->allocator->deallocate(0, this->buffer.getBuffAddr());
  }

  SerializableFile::Status SerializableFile::load(const char* fileName, Serializable& serializable) {
    Os::File file;
    Os::File::Status status;
    status = file.open(fileName, Os::File::OPEN_READ);
    if( Os::File::OP_OK != status ) {
      return FILE_OPEN_ERROR;
    }

    NATIVE_INT_TYPE capacity = this->buffer.getBuffCapacity();
    NATIVE_INT_TYPE length = capacity;
    status = file.read(this->buffer.getBuffAddr(), length, false);
    if( Os::File::OP_OK != status ) {
      file.close();
      return FILE_READ_ERROR;
    }
    file.close();

    this->reset();
    SerializeStatus serStatus;
    serStatus = this->buffer.setBuffLen(static_cast<NATIVE_UINT_TYPE>(length));
    FW_ASSERT(FW_SERIALIZE_OK == serStatus, serStatus);
    serStatus = serializable.deserialize(this->buffer);
    if(FW_SERIALIZE_OK != serStatus) {
      return DESERIALIZATION_ERROR;
    }

    return SerializableFile::OP_OK;
  }

  SerializableFile::Status SerializableFile::save(const char* fileName, Serializable& serializable) {
    this->reset();
    SerializeStatus serStatus = serializable.serialize(this->buffer);
    FW_ASSERT(FW_SERIALIZE_OK == serStatus, serStatus);

    Os::File file;
    Os::File::Status status;
    status = file.open(fileName, Os::File::OPEN_WRITE);
    if( Os::File::OP_OK != status ) {
      return FILE_OPEN_ERROR;
    }

    NATIVE_INT_TYPE length = this->buffer.getBuffLength();
    NATIVE_INT_TYPE size = length;
    status = file.write(this->buffer.getBuffAddr(), length);
    if( (Os::File::OP_OK != status) ||
        (length != size) )
    {
      file.close();
      return FILE_WRITE_ERROR;
    }

    file.close();

    return SerializableFile::OP_OK;
  }

  void SerializableFile::reset() {
    this->buffer.resetSer(); //!< reset to beginning of buffer to reuse for serialization
    this->buffer.resetDeser(); //!< reset deserialization to beginning
  }
}
