// ======================================================================
// \title  Warnings.cpp
// \author bocchino
// \brief  cpp file for FileUplink::Warnings
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/FileUplink/FileUplink.hpp>

namespace Svc {

  void FileUplink::Warnings ::
    invalidReceiveMode(const Fw::FilePacket::Type packetType)
  {
    this->m_fileUplink->log_WARNING_HI_InvalidReceiveMode(
        static_cast<U32>(packetType),
        static_cast<U32>(m_fileUplink->m_receiveMode)
    );
    this->warning();
  }

  void FileUplink::Warnings ::
    fileOpen(Fw::LogStringArg& fileName)
  {
    this->m_fileUplink->log_WARNING_HI_FileOpenError(fileName);
    this->warning();
  }

  void FileUplink::Warnings ::
    packetOutOfBounds(
        const U32 sequenceIndex,
        Fw::LogStringArg& fileName
    )
  {
    this->m_fileUplink->log_WARNING_HI_PacketOutOfBounds(
        sequenceIndex,
        fileName
    );
    this->warning();
  }

  void FileUplink::Warnings ::
    packetOutOfOrder(
        const U32 sequenceIndex,
        const U32 lastSequenceIndex
    )
  {
    this->m_fileUplink->log_WARNING_HI_PacketOutOfOrder(
        sequenceIndex,
        lastSequenceIndex
    );
    this->warning();
  }

  void FileUplink::Warnings ::
    fileWrite(Fw::LogStringArg& fileName)
  {
    this->m_fileUplink->log_WARNING_HI_FileWriteError(fileName);
    this->warning();
  }

  void FileUplink::Warnings ::
    badChecksum(
        const U32 computed,
        const U32 read
    )
  {
    this->m_fileUplink->log_WARNING_HI_BadChecksum(
        this->m_fileUplink->m_file.name,
        computed,
        read
    );
    this->warning();
  }

}
