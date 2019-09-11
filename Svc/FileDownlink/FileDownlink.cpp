// ====================================================================== 
// \title  FileDownlink.hpp
// \author bocchino
// \brief  hpp file for FileDownlink component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include <Svc/FileDownlink/FileDownlink.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/BasicTypes.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  FileDownlink ::
    FileDownlink(
        const char *const name,
        const U16 downlinkPacketSize
    ) :
      FileDownlinkComponentBase(name),
      downlinkPacketSize(downlinkPacketSize),
      filesSent(this),
      packetsSent(this),
      warnings(this),
      sequenceIndex(0)
  {

  }

  void FileDownlink ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    FileDownlinkComponentBase::init(queueDepth, instance);
  }

  FileDownlink ::
    ~FileDownlink(void)
  {

  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void FileDownlink ::
    FileDownlink_SendFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& sourceFileName,
        const Fw::CmdStringArg& destFileName
    )
  {

    Os::File::Status status;

    if (this->mode.get() == Mode::CANCEL) {
      Fw::LogStringArg sourceLogStringArg(sourceFileName);
      Fw::LogStringArg destLogStringArg(destFileName);
      this->log_ACTIVITY_HI_FileDownlink_DownlinkCanceled(
          sourceLogStringArg,
          destLogStringArg
      );
      this->mode.set(Mode::IDLE);
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
      return;
    }

    this->mode.set(Mode::DOWNLINK);

    status = this->file.open(
        sourceFileName.toChar(),
        destFileName.toChar()
    );
    if (status != Os::File::OP_OK) { 
      this->warnings.fileOpenError();
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
      return; 
    }

    this->sendStartPacket();

    status = this->sendDataPackets();
    if (status != Os::File::OP_OK) { 
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
      return; 
    }

    this->file.osFile.close();

    if (this->mode.get() == Mode::CANCEL) {
      this->sendCancelPacket();
      this->log_ACTIVITY_HI_FileDownlink_DownlinkCanceled(
          this->file.sourceName,
          this->file.destName
      );
    }
    else {
      this->sendEndPacket();
      this->log_ACTIVITY_HI_FileDownlink_FileSent(
          this->file.sourceName,
          this->file.destName
      );
      this->filesSent.fileSent();
    }

    this->mode.set(Mode::IDLE);
    
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);

  }

  void FileDownlink ::
    FileDownlink_Cancel_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    if (this->mode.get() == Mode::DOWNLINK)
      this->mode.set(Mode::CANCEL);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  // ----------------------------------------------------------------------
  // Private helper methods 
  // ----------------------------------------------------------------------

  Os::File::Status FileDownlink ::
    sendDataPacket(const U32 byteOffset)
  {

    const U32 fileSize = this->file.size;
    FW_ASSERT(byteOffset < fileSize, byteOffset);
    const U16 maxDataSize = this->downlinkPacketSize;
    const U16 dataSize = (byteOffset + maxDataSize > fileSize) ?
      fileSize - byteOffset : maxDataSize;
    U8 buffer[dataSize];

    const Os::File::Status status = 
      this->file.read(buffer, byteOffset, dataSize);
    if (status != Os::File::OP_OK) {
      this->warnings.fileRead();
      return status;
    }

    const Fw::FilePacket::DataPacket dataPacket = {
      { Fw::FilePacket::T_DATA, this->sequenceIndex },
      byteOffset,
      dataSize,
      buffer
    };
    ++this->sequenceIndex;
    Fw::FilePacket filePacket;
    filePacket.fromDataPacket(dataPacket);
    this->sendFilePacket(filePacket);

    return Os::File::OP_OK;

  }

  Os::File::Status FileDownlink ::
    sendDataPackets(void)
  {
    this->sequenceIndex = 1;
    for (
        U32 byteOffset = 0; 
        byteOffset < this->file.size; 
        byteOffset += this->downlinkPacketSize
    ) {
      if (this->mode.get() == Mode::CANCEL)
        return Os::File::OP_OK;
      const Os::File::Status status = 
        this->sendDataPacket(byteOffset);
      if (status != Os::File::OP_OK)
        return status;
    }
    return Os::File::OP_OK;
  }

  void FileDownlink ::
    sendCancelPacket(void)
  {
    const Fw::FilePacket::CancelPacket cancelPacket = {
      { Fw::FilePacket::T_CANCEL, this->sequenceIndex }
    };
    Fw::FilePacket filePacket;
    filePacket.fromCancelPacket(cancelPacket);
    this->sendFilePacket(filePacket);
  }

  void FileDownlink ::
    sendEndPacket(void)
  {

    const Fw::FilePacket::Header header = {
      Fw::FilePacket::T_END,
      this->sequenceIndex
    };
    Fw::FilePacket::EndPacket endPacket;
    endPacket.header = header;

    CFDP::Checksum checksum;
    this->file.getChecksum(checksum);
    endPacket.setChecksum(checksum);

    Fw::FilePacket filePacket;
    filePacket.fromEndPacket(endPacket);
    this->sendFilePacket(filePacket);

  }

  void FileDownlink ::
    sendStartPacket(void)
  {
    Fw::FilePacket::StartPacket startPacket;
    startPacket.initialize(
        this->file.size,
        this->file.sourceName.toChar(),
        this->file.destName.toChar()
    );
    Fw::FilePacket filePacket;
    filePacket.fromStartPacket(startPacket);
    this->sendFilePacket(filePacket);
  }

  void FileDownlink ::
    sendFilePacket(const Fw::FilePacket& filePacket)
  {
    const U32 bufferSize = filePacket.bufferSize();
    Fw::Buffer buffer = this->bufferGetCaller_out(0, bufferSize);
    FW_ASSERT(buffer.getdata() != 0);
    FW_ASSERT(buffer.getsize() == bufferSize, bufferSize, buffer.getsize());
    const Fw::SerializeStatus status = filePacket.toBuffer(buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    this->bufferSendOut_out(0, buffer);
    this->packetsSent.packetSent();
  }

  void FileDownlink ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
      // return key
      this->pingOut_out(0,key);
  }

} // end namespace Svc
