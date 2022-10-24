// ======================================================================
// \title  FileUplink.cpp
// \author bocchino
// \brief  cpp file for FileUplink component implementation class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/FileUplink/FileUplink.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  FileUplink ::
    FileUplink(const char *const name) :
      FileUplinkComponentBase(name),
      receiveMode(START),
      lastSequenceIndex(0),
      filesReceived(this),
      packetsReceived(this),
      warnings(this)
  {

  }

  void FileUplink ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    FileUplinkComponentBase::init(queueDepth, instance);
  }

  FileUplink ::
    ~FileUplink()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void FileUplink ::
    bufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& buffer
    )
  {
    Fw::FilePacket filePacket;
    const Fw::SerializeStatus status = filePacket.fromBuffer(buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_DecodeError(status);
    } else {
        const Fw::FilePacket::Header& header = filePacket.asHeader();
        switch (header.type) {
          case Fw::FilePacket::T_START:
            this->handleStartPacket(filePacket.asStartPacket());
            break;
          case Fw::FilePacket::T_DATA:
            this->handleDataPacket(filePacket.asDataPacket());
            break;
          case Fw::FilePacket::T_END:
            this->handleEndPacket(filePacket.asEndPacket());
            break;
          case Fw::FilePacket::T_CANCEL:
            this->handleCancelPacket();
            break;
          default:
            FW_ASSERT(0);
            break;
        }
    }
    this->bufferSendOut_out(0, buffer);
  }

  void FileUplink ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
      // return key
      this->pingOut_out(0,key);
  }

  // ----------------------------------------------------------------------
  // Private helper functions
  // ----------------------------------------------------------------------

  void FileUplink ::
    handleStartPacket(const Fw::FilePacket::StartPacket& startPacket)
  {
    // Clear all event throttles in preparation for new start packet
    this->log_WARNING_HI_FileWriteError_ThrottleClear();
    this->log_WARNING_HI_InvalidReceiveMode_ThrottleClear();
    this->log_WARNING_HI_PacketOutOfBounds_ThrottleClear();
    this->log_WARNING_HI_PacketOutOfOrder_ThrottleClear();
    this->packetsReceived.packetReceived();
    if (this->receiveMode != START) {
      this->file.osFile.close();
      this->warnings.invalidReceiveMode(Fw::FilePacket::T_START);
    }
    const Os::File::Status status = this->file.open(startPacket);
    if (status == Os::File::OP_OK) {
      this->goToDataMode();
    }
    else {
      this->warnings.fileOpen(this->file.name);
      this->goToStartMode();
    }
  }

  void FileUplink ::
    handleDataPacket(const Fw::FilePacket::DataPacket& dataPacket)
  {
    this->packetsReceived.packetReceived();
    if (this->receiveMode != DATA) {
      this->warnings.invalidReceiveMode(Fw::FilePacket::T_DATA);
      return;
    }
    const U32 sequenceIndex = dataPacket.header.sequenceIndex;
    this->checkSequenceIndex(sequenceIndex);
    const U32 byteOffset = dataPacket.byteOffset;
    const U32 dataSize = dataPacket.dataSize;
    if (byteOffset + dataSize > this->file.size) {
      this->warnings.packetOutOfBounds(sequenceIndex, this->file.name);
      return;
    }
    const Os::File::Status status = this->file.write(
        dataPacket.data,
        byteOffset,
        dataSize
    );
    if (status != Os::File::OP_OK) {
      this->warnings.fileWrite(this->file.name);
    }
  }

  void FileUplink ::
    handleEndPacket(const Fw::FilePacket::EndPacket& endPacket)
  {
    this->packetsReceived.packetReceived();
    if (this->receiveMode == DATA) {
      this->filesReceived.fileReceived();
      this->checkSequenceIndex(endPacket.header.sequenceIndex);
      this->compareChecksums(endPacket);
      this->log_ACTIVITY_HI_FileReceived(this->file.name);
    }
    else {
      this->warnings.invalidReceiveMode(Fw::FilePacket::T_END);
    }
    this->goToStartMode();
  }

  void FileUplink ::
    handleCancelPacket()
  {
    this->packetsReceived.packetReceived();
    this->log_ACTIVITY_HI_UplinkCanceled();
    this->goToStartMode();
  }

  void FileUplink ::
    checkSequenceIndex(const U32 sequenceIndex)
  {
    if (sequenceIndex != this->lastSequenceIndex + 1) {
      this->warnings.packetOutOfOrder(
          sequenceIndex,
          this->lastSequenceIndex
      );
    }
    this->lastSequenceIndex = sequenceIndex;
  }

  void FileUplink ::
    compareChecksums(const Fw::FilePacket::EndPacket& endPacket)
  {
    CFDP::Checksum computed, stored;
    this->file.getChecksum(computed);
    endPacket.getChecksum(stored);
    if (computed != stored) {
      this->warnings.badChecksum(
          computed.getValue(),
          stored.getValue()
      );
    }
  }

  void FileUplink ::
    goToStartMode()
  {
    this->file.osFile.close();
    this->receiveMode = START;
    this->lastSequenceIndex = 0;
  }

  void FileUplink ::
    goToDataMode()
  {
    this->receiveMode = DATA;
    this->lastSequenceIndex = 0;
  }

}
