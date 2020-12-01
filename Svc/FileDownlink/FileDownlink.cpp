// ====================================================================== 
// \title  FileDownlink.hpp
// \author bocchino, mstarch
// \brief  hpp file for FileDownlink component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
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
        const U32 timeout,
        const U32 cycleTime
    ) :
      FileDownlinkComponentBase(name),
      filesSent(this),
      packetsSent(this),
      warnings(this),
      sequenceIndex(0),
      timeout(timeout),
      curTimer(0),
      cycleTime(cycleTime),
      curOpCode(0),
      curCmdSeq(0),
      bufferSize(0),
      byteOffset(0),
      endOffset(0),
      lastCompletedType(Fw::FilePacket::T_NONE),
      lastBufferId(0)
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
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void FileDownlink ::
    Run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      //If current timeout is too-high and we are waiting for a packet, issue a timeout
      if (this->curTimer >= this->timeout && this->mode.get() == Mode::WAIT) {
          this->curTimer = 0;
          this->log_WARNING_HI_DownlinkTimeout(this->file.sourceName, this->file.destName);
          this->enterIdle();
          this->cmdResponse_out(this->curOpCode, this->curCmdSeq, Fw::COMMAND_EXECUTION_ERROR);
      }
      //Otherwise update the current counter
      else if (this->mode.get() == Mode::WAIT) {
          this->curTimer += cycleTime;
      }
  }

  void FileDownlink ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
      this->pingOut_out(0,key);
  }

  void FileDownlink ::
    bufferReturn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
	  //If this is a stale buffer (old, timed-out, or both), then ignore its return.
	  //File downlink actions only respond to the return of the most-recently-sent buffer.
	  if (this->lastBufferId != fwBuffer.getContext() + 1 ||
	      this->mode.get() == Mode::IDLE) {
		  return;
	  }
	  //Non-ignored buffers cannot be returned in "DOWNLINK" and "IDLE" state.  Only in "WAIT", "CANCEL" state.
	  FW_ASSERT(this->mode.get() == Mode::WAIT || this->mode.get() == Mode::CANCEL, this->mode.get());
      //If the last packet has been sent (and is returning now) then finish the file
	  if (this->lastCompletedType == Fw::FilePacket::T_END ||
          this->lastCompletedType == Fw::FilePacket::T_CANCEL) {
          finishHelper(this->lastCompletedType == Fw::FilePacket::T_CANCEL);
          return;
      }
      //If waiting and a buffer is in-bound, then switch to downlink mode
      else if (this->mode.get() == Mode::WAIT) {
          this->mode.set(Mode::DOWNLINK);
      }

      this->downlinkPacket();
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void FileDownlink ::
    SendFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& sourceFileName,
        const Fw::CmdStringArg& destFileName
    )
  {
    Os::File::Status status;

    // We are processing a file currently, reject command
    if (this->mode.get() != Mode::IDLE) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
        return;
    }

    // Store the current command info for completion later
    this->curOpCode = opCode;
    this->curCmdSeq = cmdSeq;

    // Open file for downlink
    status = this->file.open(
        sourceFileName.toChar(),
        destFileName.toChar()
    );

    // Reject command if error when opening file
    if (status != Os::File::OP_OK) {
      this->mode.set(Mode::IDLE);
      this->warnings.fileOpenError();
      if (FILEDOWNLINK_COMMAND_FAIL_ON_MISSING_FILE) {
          this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
      } else {
          this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
      }
      return;
    }
    this->getBuffer(this->buffer, FILE_PACKET);

    // Send file and switch to WAIT mode
    this->sendStartPacket();
    this->mode.set(Mode::WAIT);
    this->sequenceIndex = 1;
    this->curTimer = 0;
    this->byteOffset = 0;
    this->endOffset = this->file.size;
    this->lastCompletedType = Fw::FilePacket::T_START;
    this->log_ACTIVITY_HI_SendStarted(this->file.size, this->file.sourceName, this->file.destName);
  }

  void FileDownlink ::
    SendPartial_cmdHandler(
      FwOpcodeType opCode,
      U32 cmdSeq,
      const Fw::CmdStringArg& sourceFileName,
      const Fw::CmdStringArg& destFileName,
      U32 startOffset,
      U32 length
   )
  {
    Os::File::Status status;

    // We are processing a file currently, reject command
    if (this->mode.get() != Mode::IDLE) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
        return;
    }

    // Open file for downlink
    status = this->file.open(
        sourceFileName.toChar(),
        destFileName.toChar()
    );

    // Reject command if error when opening file
    if (status != Os::File::OP_OK) {
      this->mode.set(Mode::IDLE);
      this->warnings.fileOpenError();
      if (FILEDOWNLINK_COMMAND_FAIL_ON_MISSING_FILE) {
          this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
      } else {
          this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
      }
      return;
    }

    // If the amount to downlink is greater than the file size, emit a Warning and then allow
    // the file to be downlinked anyway
    if (startOffset >= this->file.size) {
        this->enterIdle();
        this->log_WARNING_HI_DownlinkPartialFail(this->file.sourceName, this->file.destName, startOffset, this->file.size);
        this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_VALIDATION_ERROR);
        return;
    } else if (startOffset + length > this->file.size) {
        this->log_WARNING_LO_DownlinkPartialWarning(startOffset, length, this->file.size, this->file.sourceName, this->file.destName);
        length = this->file.size - startOffset;
    }

    // Store the current command info for completion later
    this->curOpCode = opCode;
    this->curCmdSeq = cmdSeq;
    this->getBuffer(this->buffer, FILE_PACKET);

    // Send file and switch to WAIT mode
    this->sendStartPacket();
    this->mode.set(Mode::WAIT);
    this->sequenceIndex = 1;
    this->curTimer = 0;
    this->byteOffset = startOffset;
    this->lastCompletedType = Fw::FilePacket::T_START;

    // zero length means read until end of file
    if (length > 0) {
        this->log_ACTIVITY_HI_SendStarted(length, this->file.sourceName, this->file.destName);
        this->endOffset = startOffset + length;
    }
    else {
        this->log_ACTIVITY_HI_SendStarted(this->file.size - startOffset, this->file.sourceName, this->file.destName);
        this->endOffset = this->file.size;
    }
  }

  void FileDownlink ::
    Cancel_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
      //Must be able to cancel in both downlink and waiting states
      if (this->mode.get() == Mode::DOWNLINK || this->mode.get() == Mode::WAIT) {
          this->mode.set(Mode::CANCEL);
      }
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  // ----------------------------------------------------------------------
  // Private helper methods 
  // ----------------------------------------------------------------------

  Os::File::Status FileDownlink ::
    sendDataPacket(U32 &byteOffset)
  {
    FW_ASSERT(byteOffset < this->endOffset);
    const U32 maxDataSize = FILEDOWNLINK_INTERNAL_BUFFER_SIZE - Fw::FilePacket::DataPacket::HEADERSIZE;
    const U32 dataSize = (byteOffset + maxDataSize > this->endOffset) ? (this->endOffset - byteOffset) : maxDataSize;
    U8 buffer[dataSize];
    //This will be last data packet sent
    if (dataSize + byteOffset == this->endOffset) {
        this->lastCompletedType = Fw::FilePacket::T_DATA;
    }

    const Os::File::Status status = 
      this->file.read(buffer, byteOffset, dataSize);
    if (status != Os::File::OP_OK) {
      this->warnings.fileRead(status);
      return status;
    }
   
    const Fw::FilePacket::DataPacket dataPacket = {
      { Fw::FilePacket::T_DATA, this->sequenceIndex },
      byteOffset,
      (U16)dataSize,
      buffer
    };
    ++this->sequenceIndex;
    Fw::FilePacket filePacket;
    filePacket.fromDataPacket(dataPacket);
    this->sendFilePacket(filePacket);

    byteOffset += dataSize;

    return Os::File::OP_OK;

  }

  void FileDownlink ::
    sendCancelPacket(void)
  {
    Fw::Buffer buffer;
    const Fw::FilePacket::CancelPacket cancelPacket = {
      { Fw::FilePacket::T_CANCEL, this->sequenceIndex }
    };

    Fw::FilePacket filePacket;
    filePacket.fromCancelPacket(cancelPacket);
    this->getBuffer(buffer, CANCEL_PACKET);

    const Fw::SerializeStatus status = filePacket.toBuffer(buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    this->bufferSendOut_out(0, buffer);
    this->packetsSent.packetSent();
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
    FW_ASSERT(this->buffer.getData() != 0);
    FW_ASSERT(this->buffer.getSize() >= bufferSize, bufferSize, this->buffer.getSize());
    const Fw::SerializeStatus status = filePacket.toBuffer(this->buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    // set the buffer size to the packet size
    this->buffer.setSize(bufferSize);
    this->bufferSendOut_out(0, this->buffer);
    // restore buffer size to max
    this->buffer.setSize(FILEDOWNLINK_INTERNAL_BUFFER_SIZE);
    this->packetsSent.packetSent();
  }

  void FileDownlink ::
    enterIdle(void)
  {
    this->file.osFile.close();
    this->mode.set(Mode::IDLE);
    this->lastCompletedType = Fw::FilePacket::T_NONE;
  }

  void FileDownlink ::
    downlinkPacket()
  {
      FW_ASSERT(this->lastCompletedType != Fw::FilePacket::T_NONE, this->lastCompletedType);
      FW_ASSERT(this->mode.get() == Mode::CANCEL || this->mode.get() == Mode::DOWNLINK, this->mode.get());
      //If canceled mode and currently downlinking data then send a cancel packet
      if (this->mode.get() == Mode::CANCEL && this->lastCompletedType == Fw::FilePacket::T_START) {
          this->sendCancelPacket();
          this->lastCompletedType = Fw::FilePacket::T_CANCEL;
      }
      //If in downlink mode and currently downlinking data then continue with the next packer
      else if (this->mode.get() == Mode::DOWNLINK && this->lastCompletedType == Fw::FilePacket::T_START) {
          //Send the next packet, or fail doing so
          const Os::File::Status status = this->sendDataPacket(this->byteOffset);
          if (status != Os::File::OP_OK) {
              this->log_WARNING_HI_SendDataFail(this->file.sourceName, this->byteOffset);
              this->enterIdle();
              this->cmdResponse_out(this->curOpCode, this->curCmdSeq, Fw::COMMAND_EXECUTION_ERROR);
              //Don't go to wait state
              return;
          }
      }
      //If in downlink mode or cancel and finished downlinking data then send the last packet
      else if (this->lastCompletedType == Fw::FilePacket::T_DATA) {
          this->sendEndPacket();
          this->lastCompletedType = Fw::FilePacket::T_END;
      }
      this->mode.set(Mode::WAIT);
      this->curTimer = 0;
  }

  void FileDownlink ::
    finishHelper(bool cancel)
  {
      //Complete command and switch to IDLE
      if (not cancel) {
          this->filesSent.fileSent();
          this->log_ACTIVITY_HI_FileSent(this->file.sourceName, this->file.destName);
      } else {
          this->log_ACTIVITY_HI_DownlinkCanceled(this->file.sourceName, this->file.destName);
      }
      this->enterIdle();
      this->cmdResponse_out(this->curOpCode, this->curCmdSeq, Fw::COMMAND_OK);
  }

  void FileDownlink ::
    getBuffer(Fw::Buffer& buffer, PacketType type)
  {
      //Check type is correct
      FW_ASSERT(type < COUNT_PACKET_TYPE && type >= 0, type);
      // Wrap the buffer around our indexed memory.
      buffer.setData(this->memoryStore[type]);
      buffer.setSize(FILEDOWNLINK_INTERNAL_BUFFER_SIZE);
      //Set a known ID to look for later
      buffer.setContext(lastBufferId);
      lastBufferId++;
  }
} // end namespace Svc
