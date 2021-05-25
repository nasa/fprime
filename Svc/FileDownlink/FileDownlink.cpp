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
#include <Fw/Types/EightyCharString.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  FileDownlink ::
    FileDownlink(
        const char *const name
    ) :
      FileDownlinkComponentBase(name),
      configured(false),
      filesSent(this),
      packetsSent(this),
      warnings(this),
      sequenceIndex(0),
      curTimer(0),
      bufferSize(0),
      byteOffset(0),
      endOffset(0),
      lastCompletedType(Fw::FilePacket::T_NONE),
      lastBufferId(0),
      curEntry(),
      cntxId(0)
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

  void FileDownlink ::
    configure(
        U32 timeout,
        U32 cooldown,
        U32 cycleTime,
        U32 fileQueueDepth
    )
  {
    this->timeout = timeout;
    this->cooldown = cooldown;
    this->cycleTime = cycleTime;
    this->configured = true;

    Os::Queue::QueueStatus stat = fileQueue.create(
      Fw::EightyCharString("fileDownlinkQueue"),
      fileQueueDepth,
      sizeof(struct FileEntry)
    );
    FW_ASSERT(stat == Os::Queue::QUEUE_OK, stat);
  }

  void FileDownlink ::
    start(
      NATIVE_INT_TYPE identifier,
      NATIVE_INT_TYPE priority,
      NATIVE_INT_TYPE stackSize,
      NATIVE_INT_TYPE cpuAffinity
    )
  {
    FW_ASSERT(this->configured == true);
    FileDownlinkComponentBase::start(identifier, priority, stackSize, cpuAffinity);
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
    switch(this->mode.get())
    {
      case Mode::IDLE: {
        NATIVE_INT_TYPE real_size = 0;
        NATIVE_INT_TYPE prio = 0;
        Os::Queue::QueueStatus stat = fileQueue.receive(
          (U8 *) &this->curEntry,
          sizeof(this->curEntry),
          real_size,
          prio,
          Os::Queue::QUEUE_NONBLOCKING
        );

        if(stat != Os::Queue::QUEUE_OK || sizeof(this->curEntry) != real_size) {
          return;
        }

        sendFile(
          this->curEntry.srcFilename,
          this->curEntry.destFilename,
          this->curEntry.offset,
          this->curEntry.length
        );
        break;
      }
      case Mode::COOLDOWN: {
        if (this->curTimer >= this->cooldown) {
          this->curTimer = 0;
          this->mode.set(Mode::IDLE);
        } else {
          this->curTimer += cycleTime;
        }
        break;
      }
      case Mode::WAIT: {
        //If current timeout is too-high and we are waiting for a packet, issue a timeout
        if (this->curTimer >= this->timeout) {
          this->curTimer = 0;
          this->log_WARNING_HI_DownlinkTimeout(this->file.sourceName, this->file.destName);
          this->enterCooldown();
          this->sendResponse(SendFileStatus::ERROR);
        } else { //Otherwise update the current counter
          this->curTimer += cycleTime;
        }
        break;
      }
      default:
        break;
    }
  }

  Svc::SendFileResponse FileDownlink ::
    SendFile_handler(
        const NATIVE_INT_TYPE portNum,
        sourceFileNameString sourceFilename,
        destFileNameString destFilename,
        U32 offset,
        U32 length
    )
  {
    struct FileEntry entry = {
      .srcFilename = {0},
      .destFilename = {0},
      .offset = offset,
      .length = length,
      .source = FileDownlink::PORT,
      .opCode = 0,
      .cmdSeq = 0,
      .context = cntxId++
    };

    FW_ASSERT(sourceFilename.length() < sizeof(entry.srcFilename));
    FW_ASSERT(destFilename.length() < sizeof(entry.destFilename));
    memcpy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    memcpy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = fileQueue.send((U8 *) &entry, sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

    if(status != Os::Queue::QUEUE_OK) {
      return SendFileResponse(SendFileStatus::ERROR, __UINT32_MAX__);
    }
    return SendFileResponse(SendFileStatus::OK, entry.context);
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
        const Fw::CmdStringArg& sourceFilename,
        const Fw::CmdStringArg& destFilename
    )
  {
    struct FileEntry entry = {
      .srcFilename = {0},
      .destFilename = {0},
      .offset = 0,
      .length = 0,
      .source = FileDownlink::COMMAND,
      .opCode = opCode,
      .cmdSeq = cmdSeq,
      .context =__UINT32_MAX__
    };

    FW_ASSERT(sourceFilename.length() < sizeof(entry.srcFilename));
    FW_ASSERT(destFilename.length() < sizeof(entry.destFilename));
    memcpy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    memcpy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = fileQueue.send((U8 *) &entry, sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

    if(status != Os::Queue::QUEUE_OK) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }
  }

  void FileDownlink ::
    SendPartial_cmdHandler(
      FwOpcodeType opCode,
      U32 cmdSeq,
      const Fw::CmdStringArg& sourceFilename,
      const Fw::CmdStringArg& destFilename,
      U32 startOffset,
      U32 length
   )
  {
    struct FileEntry entry = {
      .srcFilename = {0},
      .destFilename = {0},
      .offset = startOffset,
      .length = length,
      .source = FileDownlink::COMMAND,
      .opCode = opCode,
      .cmdSeq = cmdSeq,
      .context = __UINT32_MAX__
    };

    FW_ASSERT(sourceFilename.length() < sizeof(entry.srcFilename));
    FW_ASSERT(destFilename.length() < sizeof(entry.destFilename));
    memcpy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    memcpy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = fileQueue.send((U8 *) &entry, sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

    if(status != Os::Queue::QUEUE_OK) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
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

  Fw::CommandResponse FileDownlink ::
    statusToCmdResp(SendFileStatus status)
  {
    switch(status.e) {
    case SendFileStatus::OK:
      return Fw::COMMAND_OK;
    case SendFileStatus::ERROR:
      return Fw::COMMAND_EXECUTION_ERROR;
    case SendFileStatus::INVALID:
      return Fw::COMMAND_VALIDATION_ERROR;
    case SendFileStatus::BUSY:
        return Fw::COMMAND_BUSY;
    default:
        // Trigger assertion if given unknown status
        FW_ASSERT(false);
    }

    // It's impossible to reach this, but added to suppress gcc missing return warning
    return Fw::COMMAND_EXECUTION_ERROR;
  }

  void FileDownlink ::
    sendResponse(SendFileStatus resp)
  {
    if(this->curEntry.source == FileDownlink::COMMAND) {
      this->cmdResponse_out(this->curEntry.opCode, this->curEntry.cmdSeq, statusToCmdResp(resp));
    } else {
      for(NATIVE_INT_TYPE i = 0; i < this->getNum_FileComplete_OutputPorts(); i++) {
        if(this->isConnected_FileComplete_OutputPort(i)) {
          this->FileComplete_out(i, Svc::SendFileResponse(resp, this->curEntry.context));
        }
      }
    }
  }

  void FileDownlink ::
    sendFile(
          const char* sourceFilename,
          const char* destFilename,
          U32 startOffset,
          U32 length
      )
  {
    // Open file for downlink
    Os::File::Status status = this->file.open(
        sourceFilename,
        destFilename
    );

    // Reject command if error when opening file
    if (status != Os::File::OP_OK) {
      this->mode.set(Mode::IDLE);
      this->warnings.fileOpenError();
      if (FILEDOWNLINK_COMMAND_FAIL_ON_MISSING_FILE) {
          sendResponse(SendFileStatus::ERROR);
      } else {
          sendResponse(SendFileStatus::OK);
      }
      return;
    }

    // If the amount to downlink is greater than the file size, emit a Warning and then allow
    // the file to be downlinked anyway
    if (startOffset >= this->file.size) {
        this->enterCooldown();
        this->log_WARNING_HI_DownlinkPartialFail(this->file.sourceName, this->file.destName, startOffset, this->file.size);
        sendResponse(SendFileStatus::INVALID);
        return;
    } else if (startOffset + length > this->file.size) {
        this->log_WARNING_LO_DownlinkPartialWarning(startOffset, length, this->file.size, this->file.sourceName, this->file.destName);
        length = this->file.size - startOffset;
    }

    // Send file and switch to WAIT mode
    this->getBuffer(this->buffer, FILE_PACKET);
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
    enterCooldown(void)
  {
    this->file.osFile.close();
    this->mode.set(Mode::COOLDOWN);
    this->lastCompletedType = Fw::FilePacket::T_NONE;
    this->curTimer = 0;
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
              this->enterCooldown();
              sendResponse(SendFileStatus::ERROR);
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
      this->enterCooldown();
      sendResponse(SendFileStatus::OK);
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
