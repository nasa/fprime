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
#include <FpConfig.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/QueueString.hpp>
#include <limits>

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
      Os::QueueString("fileDownlinkQueue"),
      fileQueueDepth,
      sizeof(struct FileEntry)
    );
    FW_ASSERT(stat == Os::Queue::QUEUE_OK, stat);
  }

  void FileDownlink ::
    preamble()
  {
    FW_ASSERT(this->configured == true);
  }

  FileDownlink ::
    ~FileDownlink()
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
          reinterpret_cast<U8*>(&this->curEntry),
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
          this->sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_ERROR);
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
        const sourceFileNameString& sourceFilename, // lgtm[cpp/large-parameter] dictated by command architecture
        const destFileNameString& destFilename, // lgtm[cpp/large-parameter] dictated by command architecture
        U32 offset,
        U32 length
    )
  {
    struct FileEntry entry;
    entry.srcFilename[0] = 0;
    entry.destFilename[0] = 0;
    entry.offset = offset;
    entry.length = length;
    entry.source = FileDownlink::PORT;
    entry.opCode = 0;
    entry.cmdSeq = 0;
    entry.context = cntxId++;

    FW_ASSERT(sourceFilename.length() < sizeof(entry.srcFilename));
    FW_ASSERT(destFilename.length() < sizeof(entry.destFilename));
    Fw::StringUtils::string_copy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    Fw::StringUtils::string_copy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = fileQueue.send(reinterpret_cast<U8*>(&entry), sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

    if(status != Os::Queue::QUEUE_OK) {
      return SendFileResponse(SendFileStatus::STATUS_ERROR, std::numeric_limits<U32>::max());
    }
    return SendFileResponse(SendFileStatus::STATUS_OK, entry.context);
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
    struct FileEntry entry;
    entry.srcFilename[0] = 0;
    entry.destFilename[0] = 0;
    entry.offset = 0;
    entry.length = 0;
    entry.source = FileDownlink::COMMAND;
    entry.opCode = opCode;
    entry.cmdSeq = cmdSeq;
    entry.context = std::numeric_limits<U32>::max();


    FW_ASSERT(sourceFilename.length() < sizeof(entry.srcFilename));
    FW_ASSERT(destFilename.length() < sizeof(entry.destFilename));
    Fw::StringUtils::string_copy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    Fw::StringUtils::string_copy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = fileQueue.send(reinterpret_cast<U8*>(&entry), sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

    if(status != Os::Queue::QUEUE_OK) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
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
    struct FileEntry entry;
    entry.srcFilename[0] = 0;
    entry.destFilename[0] = 0;
    entry.offset = startOffset;
    entry.length = length;
    entry.source = FileDownlink::COMMAND;
    entry.opCode = opCode;
    entry.cmdSeq = cmdSeq;
    entry.context = std::numeric_limits<U32>::max();


    FW_ASSERT(sourceFilename.length() < sizeof(entry.srcFilename));
    FW_ASSERT(destFilename.length() < sizeof(entry.destFilename));
    Fw::StringUtils::string_copy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    Fw::StringUtils::string_copy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = fileQueue.send(reinterpret_cast<U8*>(&entry), sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

    if(status != Os::Queue::QUEUE_OK) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
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
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // ----------------------------------------------------------------------
  // Private helper methods
  // ----------------------------------------------------------------------

  Fw::CmdResponse FileDownlink ::
    statusToCmdResp(SendFileStatus status)
  {
    switch(status.e) {
    case SendFileStatus::STATUS_OK:
      return Fw::CmdResponse::OK;
    case SendFileStatus::STATUS_ERROR:
      return Fw::CmdResponse::EXECUTION_ERROR;
    case SendFileStatus::STATUS_INVALID:
      return Fw::CmdResponse::VALIDATION_ERROR;
    case SendFileStatus::STATUS_BUSY:
        return Fw::CmdResponse::BUSY;
    default:
        // Trigger assertion if given unknown status
        FW_ASSERT(false);
    }

    // It's impossible to reach this, but added to suppress gcc missing return warning
    return Fw::CmdResponse::EXECUTION_ERROR;
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
      sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_ERROR);
      return;
    }


    if (startOffset >= this->file.size) {
        this->enterCooldown();
        this->log_WARNING_HI_DownlinkPartialFail(this->file.sourceName, this->file.destName, startOffset, this->file.size);
        sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_INVALID);
        return;
    } else if (startOffset + length > this->file.size) {
        // If the amount to downlink is greater than the file size, emit a Warning and then allow
        // the file to be downlinked anyway
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
    U8 buffer[FILEDOWNLINK_INTERNAL_BUFFER_SIZE - Fw::FilePacket::DataPacket::HEADERSIZE];
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
      static_cast<U16>(dataSize),
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
    sendCancelPacket()
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
    sendEndPacket()
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
    sendStartPacket()
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
    FW_ASSERT(this->buffer.getData() != nullptr);
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
    enterCooldown()
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
              this->sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_ERROR);
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
      sendResponse(SendFileStatus::STATUS_OK);
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
