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
      m_configured(false),
      m_filesSent(this),
      m_packetsSent(this),
      m_warnings(this),
      m_sequenceIndex(0),
      m_curTimer(0),
      m_bufferSize(0),
      m_byteOffset(0),
      m_endOffset(0),
      m_lastCompletedType(Fw::FilePacket::T_NONE),
      m_lastBufferId(0),
      m_curEntry(),
      m_cntxId(0)
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
    this->m_timeout = timeout;
    this->m_cooldown = cooldown;
    this->m_cycleTime = cycleTime;
    this->m_configured = true;

    Os::Queue::QueueStatus stat = m_fileQueue.create(
      Os::QueueString("fileDownlinkQueue"),
      fileQueueDepth,
      sizeof(struct FileEntry)
    );
    FW_ASSERT(stat == Os::Queue::QUEUE_OK, stat);
  }

  void FileDownlink ::
    preamble()
  {
    FW_ASSERT(this->m_configured == true);
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
        U32 context
    )
  {
    switch(this->m_mode.get())
    {
      case Mode::IDLE: {
        NATIVE_INT_TYPE real_size = 0;
        NATIVE_INT_TYPE prio = 0;
        Os::Queue::QueueStatus stat = m_fileQueue.receive(
          reinterpret_cast<U8*>(&this->m_curEntry),
          sizeof(this->m_curEntry),
          real_size,
          prio,
          Os::Queue::QUEUE_NONBLOCKING
        );

        if(stat != Os::Queue::QUEUE_OK || sizeof(this->m_curEntry) != real_size) {
          return;
        }

        sendFile(
          this->m_curEntry.srcFilename,
          this->m_curEntry.destFilename,
          this->m_curEntry.offset,
          this->m_curEntry.length
        );
        break;
      }
      case Mode::COOLDOWN: {
        if (this->m_curTimer >= this->m_cooldown) {
          this->m_curTimer = 0;
          this->m_mode.set(Mode::IDLE);
        } else {
          this->m_curTimer += m_cycleTime;
        }
        break;
      }
      case Mode::WAIT: {
        //If current timeout is too-high and we are waiting for a packet, issue a timeout
        if (this->m_curTimer >= this->m_timeout) {
          this->m_curTimer = 0;
          this->log_WARNING_HI_DownlinkTimeout(this->m_file.getSourceName(), this->m_file.getDestName());
          this->enterCooldown();
          this->sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_ERROR);
        } else { //Otherwise update the current counter
          this->m_curTimer += m_cycleTime;
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
    entry.context = m_cntxId++;

    FW_ASSERT(sourceFilename.length() < sizeof(entry.srcFilename));
    FW_ASSERT(destFilename.length() < sizeof(entry.destFilename));
    (void) Fw::StringUtils::string_copy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    (void) Fw::StringUtils::string_copy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = m_fileQueue.send(reinterpret_cast<U8*>(&entry), sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

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
	  if (this->m_lastBufferId != fwBuffer.getContext() + 1 ||
	      this->m_mode.get() == Mode::IDLE) {
		  return;
	  }
	  //Non-ignored buffers cannot be returned in "DOWNLINK" and "IDLE" state.  Only in "WAIT", "CANCEL" state.
	  FW_ASSERT(this->m_mode.get() == Mode::WAIT || this->m_mode.get() == Mode::CANCEL, this->m_mode.get());
      //If the last packet has been sent (and is returning now) then finish the file
	  if (this->m_lastCompletedType == Fw::FilePacket::T_END ||
          this->m_lastCompletedType == Fw::FilePacket::T_CANCEL) {
          finishHelper(this->m_lastCompletedType == Fw::FilePacket::T_CANCEL);
          return;
      }
      //If waiting and a buffer is in-bound, then switch to downlink mode
      else if (this->m_mode.get() == Mode::WAIT) {
          this->m_mode.set(Mode::DOWNLINK);
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
    (void) Fw::StringUtils::string_copy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    (void) Fw::StringUtils::string_copy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = m_fileQueue.send(reinterpret_cast<U8*>(&entry), sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

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
    (void) Fw::StringUtils::string_copy(entry.srcFilename, sourceFilename.toChar(), sizeof(entry.srcFilename));
    (void) Fw::StringUtils::string_copy(entry.destFilename, destFilename.toChar(), sizeof(entry.destFilename));

    Os::Queue::QueueStatus status = m_fileQueue.send(reinterpret_cast<U8*>(&entry), sizeof(entry), 0, Os::Queue::QUEUE_NONBLOCKING);

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
      if (this->m_mode.get() == Mode::DOWNLINK || this->m_mode.get() == Mode::WAIT) {
          this->m_mode.set(Mode::CANCEL);
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
    if(this->m_curEntry.source == FileDownlink::COMMAND) {
      this->cmdResponse_out(this->m_curEntry.opCode, this->m_curEntry.cmdSeq, statusToCmdResp(resp));
    } else {
      for(NATIVE_INT_TYPE i = 0; i < this->getNum_FileComplete_OutputPorts(); i++) {
        if(this->isConnected_FileComplete_OutputPort(i)) {
          this->FileComplete_out(i, Svc::SendFileResponse(resp, this->m_curEntry.context));
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
    Os::File::Status status = this->m_file.open(
        sourceFilename,
        destFilename
    );

    // Reject command if error when opening file
    if (status != Os::File::OP_OK) {
      this->m_mode.set(Mode::IDLE);
      this->m_warnings.fileOpenError();
      sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_ERROR);
      return;
    }


    if (startOffset >= this->m_file.getSize()) {
        this->enterCooldown();
        this->log_WARNING_HI_DownlinkPartialFail(this->m_file.getSourceName(), this->m_file.getDestName(), startOffset, this->m_file.getSize());
        sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_INVALID);
        return;
    } else if (startOffset + length > this->m_file.getSize()) {
        // If the amount to downlink is greater than the file size, emit a Warning and then allow
        // the file to be downlinked anyway
        this->log_WARNING_LO_DownlinkPartialWarning(startOffset, length, this->m_file.getSize(), this->m_file.getSourceName(), this->m_file.getDestName());
        length = this->m_file.getSize() - startOffset;
    }

    // Send file and switch to WAIT mode
    this->getBuffer(this->m_buffer, FILE_PACKET);
    this->sendStartPacket();
    this->m_mode.set(Mode::WAIT);
    this->m_sequenceIndex = 1;
    this->m_curTimer = 0;
    this->m_byteOffset = startOffset;
    this->m_lastCompletedType = Fw::FilePacket::T_START;

    // zero length means read until end of file
    if (length > 0) {
        this->log_ACTIVITY_HI_SendStarted(length, this->m_file.getSourceName(), this->m_file.getDestName());
        this->m_endOffset = startOffset + length;
    }
    else {
        this->log_ACTIVITY_HI_SendStarted(this->m_file.getSize() - startOffset, this->m_file.getSourceName(), this->m_file.getDestName());
        this->m_endOffset = this->m_file.getSize();
    }
  }

  Os::File::Status FileDownlink ::
    sendDataPacket(U32 &byteOffset)
  {
    FW_ASSERT(byteOffset < this->m_endOffset);
    const U32 maxDataSize = FILEDOWNLINK_INTERNAL_BUFFER_SIZE - Fw::FilePacket::DataPacket::HEADERSIZE;
    const U32 dataSize = (byteOffset + maxDataSize > this->m_endOffset) ? (this->m_endOffset - byteOffset) : maxDataSize;
    U8 buffer[FILEDOWNLINK_INTERNAL_BUFFER_SIZE - Fw::FilePacket::DataPacket::HEADERSIZE];
    //This will be last data packet sent
    if (dataSize + byteOffset == this->m_endOffset) {
        this->m_lastCompletedType = Fw::FilePacket::T_DATA;
    }

    const Os::File::Status status =
      this->m_file.read(buffer, byteOffset, dataSize);
    if (status != Os::File::OP_OK) {
      this->m_warnings.fileRead(status);
      return status;
    }

    Fw::FilePacket::DataPacket dataPacket;
    dataPacket.initialize(
      this->m_sequenceIndex,
      byteOffset,
      static_cast<U16>(dataSize),
      buffer);
    ++this->m_sequenceIndex;
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
    Fw::FilePacket::CancelPacket cancelPacket;
    cancelPacket.initialize(this->m_sequenceIndex);

    Fw::FilePacket filePacket;
    filePacket.fromCancelPacket(cancelPacket);
    this->getBuffer(buffer, CANCEL_PACKET);

    const Fw::SerializeStatus status = filePacket.toBuffer(buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    this->bufferSendOut_out(0, buffer);
    this->m_packetsSent.packetSent();
  }

  void FileDownlink ::
    sendEndPacket()
  {
    CFDP::Checksum checksum;
    this->m_file.getChecksum(checksum);

    Fw::FilePacket::EndPacket endPacket;
    endPacket.initialize(this->m_sequenceIndex, checksum);

    Fw::FilePacket filePacket;
    filePacket.fromEndPacket(endPacket);
    this->sendFilePacket(filePacket);

  }

  void FileDownlink ::
    sendStartPacket()
  {
    Fw::FilePacket::StartPacket startPacket;
    startPacket.initialize(
        this->m_file.getSize(),
        this->m_file.getSourceName().toChar(),
        this->m_file.getDestName().toChar()
    );
    Fw::FilePacket filePacket;
    filePacket.fromStartPacket(startPacket);
    this->sendFilePacket(filePacket);
  }

  void FileDownlink ::
    sendFilePacket(const Fw::FilePacket& filePacket)
  {
    const U32 bufferSize = filePacket.bufferSize();
    FW_ASSERT(this->m_buffer.getData() != nullptr);
    FW_ASSERT(this->m_buffer.getSize() >= bufferSize, bufferSize, this->m_buffer.getSize());
    const Fw::SerializeStatus status = filePacket.toBuffer(this->m_buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    // set the buffer size to the packet size
    this->m_buffer.setSize(bufferSize);
    this->bufferSendOut_out(0, this->m_buffer);
    // restore buffer size to max
    this->m_buffer.setSize(FILEDOWNLINK_INTERNAL_BUFFER_SIZE);
    this->m_packetsSent.packetSent();
  }

  void FileDownlink ::
    enterCooldown()
  {
    this->m_file.getOsFile().close();
    this->m_mode.set(Mode::COOLDOWN);
    this->m_lastCompletedType = Fw::FilePacket::T_NONE;
    this->m_curTimer = 0;
  }

  void FileDownlink ::
    downlinkPacket()
  {
      FW_ASSERT(this->m_lastCompletedType != Fw::FilePacket::T_NONE, this->m_lastCompletedType);
      FW_ASSERT(this->m_mode.get() == Mode::CANCEL || this->m_mode.get() == Mode::DOWNLINK, this->m_mode.get());
      //If canceled mode and currently downlinking data then send a cancel packet
      if (this->m_mode.get() == Mode::CANCEL && this->m_lastCompletedType == Fw::FilePacket::T_START) {
          this->sendCancelPacket();
          this->m_lastCompletedType = Fw::FilePacket::T_CANCEL;
      }
      //If in downlink mode and currently downlinking data then continue with the next packer
      else if (this->m_mode.get() == Mode::DOWNLINK && this->m_lastCompletedType == Fw::FilePacket::T_START) {
          //Send the next packet, or fail doing so
          const Os::File::Status status = this->sendDataPacket(this->m_byteOffset);
          if (status != Os::File::OP_OK) {
              this->log_WARNING_HI_SendDataFail(this->m_file.getSourceName(), this->m_byteOffset);
              this->enterCooldown();
              this->sendResponse(FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? SendFileStatus::STATUS_OK : SendFileStatus::STATUS_ERROR);
              //Don't go to wait state
              return;
          }
      }
      //If in downlink mode or cancel and finished downlinking data then send the last packet
      else if (this->m_lastCompletedType == Fw::FilePacket::T_DATA) {
          this->sendEndPacket();
          this->m_lastCompletedType = Fw::FilePacket::T_END;
      }
      this->m_mode.set(Mode::WAIT);
      this->m_curTimer = 0;
  }

  void FileDownlink ::
    finishHelper(bool cancel)
  {
      //Complete command and switch to IDLE
      if (not cancel) {
          this->m_filesSent.fileSent();
          this->log_ACTIVITY_HI_FileSent(this->m_file.getSourceName(), this->m_file.getDestName());
      } else {
          this->log_ACTIVITY_HI_DownlinkCanceled(this->m_file.getSourceName(), this->m_file.getDestName());
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
      buffer.setData(this->m_memoryStore[type]);
      buffer.setSize(FILEDOWNLINK_INTERNAL_BUFFER_SIZE);
      //Set a known ID to look for later
      buffer.setContext(m_lastBufferId);
      m_lastBufferId++;
  }
} // end namespace Svc
