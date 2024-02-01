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
    m_allocator(allocator),
    m_recoverable(false), // for compiler; not used
    m_actualSize(maxSerializedSize),
    m_buffer(static_cast<U8*>(this->m_allocator->allocate(0, m_actualSize, m_recoverable)), m_actualSize)
  {
    // assert if allocator returns smaller size
    FW_ASSERT(maxSerializedSize == m_actualSize,maxSerializedSize,m_actualSize);
    FW_ASSERT(nullptr != m_buffer.getBuffAddr());
  }

  SerializableFile::~SerializableFile() {
    this->m_allocator->deallocate(0, this->m_buffer.getBuffAddr());
  }

  SerializableFile::Status SerializableFile::load(const char* fileName, Serializable& serializable) {
    Os::File file;
    Os::File::Status status;
    status = file.open(fileName, Os::File::OPEN_READ);
    if( Os::File::OP_OK != status ) {
      return FILE_OPEN_ERROR;
    }

    NATIVE_INT_TYPE capacity = this->m_buffer.getBuffCapacity();
    NATIVE_INT_TYPE length = capacity;
    status = file.read(this->m_buffer.getBuffAddr(), length, false);
    if( Os::File::OP_OK != status ) {
      file.close();
      return FILE_READ_ERROR;
    }
    file.close();

    this->reset();
    SerializeStatus serStatus;
    serStatus = this->m_buffer.setBuffLen(static_cast<NATIVE_UINT_TYPE>(length));
    FW_ASSERT(FW_SERIALIZE_OK == serStatus, serStatus);
    serStatus = serializable.deserialize(this->m_buffer);
    if(FW_SERIALIZE_OK != serStatus) {
      return DESERIALIZATION_ERROR;
    }

    return SerializableFile::OP_OK;
  }

  SerializableFile::Status SerializableFile::save(const char* fileName, Serializable& serializable) {
    this->reset();
    SerializeStatus serStatus = serializable.serialize(this->m_buffer);
    FW_ASSERT(FW_SERIALIZE_OK == serStatus, serStatus);

    Os::File file;
    Os::File::Status status;
    status = file.open(fileName, Os::File::OPEN_WRITE);
    if( Os::File::OP_OK != status ) {
      return FILE_OPEN_ERROR;
    }

    NATIVE_INT_TYPE length = this->m_buffer.getBuffLength();
    NATIVE_INT_TYPE size = length;
    status = file.write(this->m_buffer.getBuffAddr(), length);
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
    this->m_buffer.resetSer(); //!< reset to beginning of buffer to reuse for serialization
    this->m_buffer.resetDeser(); //!< reset deserialization to beginning
  }
}
