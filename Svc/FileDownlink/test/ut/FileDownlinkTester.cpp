// ======================================================================
// \title  FileDownlinkTester.cpp
// \author bocchino
// \brief  cpp file for FileDownlink test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <cerrno>
#include <unistd.h>

#include "FileDownlinkTester.hpp"

#define INSTANCE 0
#define CMD_SEQ 0
#define QUEUE_DEPTH 10

#define TIMEOUT_MS 1000
#define COOLDOWN_MS 500
#define CYCLE_MS 100
#define MAX_ALLOCATED 100
namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  FileDownlinkTester ::
    FileDownlinkTester() :
      FileDownlinkGTestBase("Tester", MAX_HISTORY_SIZE),
      component("FileDownlink"),
      buffers_index(0)
  {
    this->component.configure(TIMEOUT_MS, COOLDOWN_MS, CYCLE_MS, 10);
    this->connectPorts();
    this->initComponents();
  }

  FileDownlinkTester ::
    ~FileDownlinkTester()
  {
      for (U32 i = 0; i < buffers_index; i++) {
          delete [] buffers[i];
      }
  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void FileDownlinkTester ::
    downlink()
  {
    // Assert idle mode
    ASSERT_EQ(FileDownlink::Mode::IDLE, this->component.mode.get());

    // Create a file
    const char *const sourceFileName = "source.bin";
    const char *const destFileName = "dest.bin";
    U8 data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    FileBuffer fileBufferOut(data, sizeof(data));
    fileBufferOut.write(sourceFileName);

    // Send the file and assert COMMAND_OK
    this->sendFile(sourceFileName, destFileName, Fw::CmdResponse::OK);

    // Assert telemetry
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_PacketsSent_SIZE(3);
    for (size_t i = 0; i < 3; ++i) {
        ASSERT_TLM_PacketsSent(i, i + 1);
    }
    ASSERT_TLM_FilesSent_SIZE(1);
    ASSERT_TLM_FilesSent(0, 1);

    // Assert events
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_FileSent_SIZE(1);
    ASSERT_EVENTS_SendStarted_SIZE(1);
//    printTextLogHistory(stdout);
    ASSERT_EVENTS_SendStarted(0, 10, sourceFileName, destFileName);
    ASSERT_EVENTS_FileSent(0, sourceFileName, destFileName);

    // Validate the packet history
    History<Fw::FilePacket::DataPacket> dataPackets(MAX_HISTORY_SIZE);
    CFDP::Checksum checksum;
    fileBufferOut.getChecksum(checksum);
    validatePacketHistory(
        *this->fromPortHistory_bufferSendOut,
        dataPackets,
        Fw::FilePacket::T_END,
        3,
        checksum,
        0
    );

    // Compare the outgoing and incoming files
    FileBuffer fileBufferIn(dataPackets);
    ASSERT_EQ(true, FileBuffer::compare(fileBufferIn, fileBufferOut));

    // Assert idle mode
    ASSERT_EQ(FileDownlink::Mode::IDLE, this->component.mode.get());

    // Remove the outgoing file
    this->removeFile(sourceFileName);
  }

  void FileDownlinkTester ::
    fileOpenError()
  {

    const char *const sourceFileName = "missing_directory/source.bin";
    const char *const destFileName = "dest.bin";

    // Send the file and assert CmdResponse::EXECUTION_ERROR
    this->sendFile(
        sourceFileName,
        destFileName,
        (FILEDOWNLINK_COMMAND_FAILURES_DISABLED) ? Fw::CmdResponse::OK : Fw::CmdResponse::EXECUTION_ERROR
    );

    // Assert telemetry
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_Warnings(0, 1);

    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileOpenError_SIZE(1);
//    ASSERT_EVENTS_FileDownlink_FileOpenError(0, sourceFileName);

  }

  void FileDownlinkTester ::
    cancelDownlink()
  {
    // Create a file
    const char *const sourceFileName = "source.bin";
    const char *const destFileName = "dest.bin";
    U8 data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    FileBuffer fileBufferOut(data, sizeof(data));
    fileBufferOut.write(sourceFileName);

    // Send a file
    Fw::CmdStringArg sourceCmdStringArg(sourceFileName);
    Fw::CmdStringArg destCmdStringArg(destFileName);
    this->sendCmd_SendFile(
        INSTANCE,
        CMD_SEQ,
        sourceCmdStringArg,
        destCmdStringArg
    );
    this->sendCmd_Cancel(INSTANCE, CMD_SEQ);
    this->component.doDispatch(); // Dispatch start command
    this->component.Run_handler(0,0); // Enqueue and start processing file
    this->component.doDispatch(); // Dispatch cancel command
    // Assert cancelation response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileDownlink::OPCODE_CANCEL, CMD_SEQ, Fw::CmdResponse::OK);
    this->cmdResponseHistory->clear();
    ASSERT_EQ(FileDownlink::Mode::CANCEL, this->component.mode.get());

    this->component.doDispatch(); // Process return of original buffer and send cancel packet
    this->component.doDispatch(); // Process return of cancel packet

    // Ensure initial send file command also receives a response.
    Fw::CmdResponse resp = (FILEDOWNLINK_COMMAND_FAILURES_DISABLED) ? Fw::CmdResponse::OK : Fw::CmdResponse::EXECUTION_ERROR;
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileDownlink::OPCODE_SENDFILE, CMD_SEQ, resp);

    // Assert telemetry
    ASSERT_TLM_SIZE(2);

    // Assert cancel event
    ASSERT_EVENTS_SIZE(2); // Started and cancel
    ASSERT_EVENTS_DownlinkCanceled_SIZE(1);

    ASSERT_EQ(FileDownlink::Mode::COOLDOWN, this->component.mode.get());

    this->removeFile(sourceFileName);
  }

  void FileDownlinkTester ::
    cancelInIdleMode()
  {
    // Assert idle mode
    ASSERT_EQ(FileDownlink::Mode::IDLE, this->component.mode.get());

    // Send a cancel command
    this->cancel(Fw::CmdResponse::OK);

    this->component.Run_handler(0,0);
    // Assert idle mode
    ASSERT_EQ(FileDownlink::Mode::IDLE, this->component.mode.get());

  }

  void FileDownlinkTester ::
    downlinkPartial()
  {
    // Assert idle mode
    ASSERT_EQ(FileDownlink::Mode::IDLE, this->component.mode.get());

    // Create a file
    const char *const sourceFileName = "source.bin";
    const char *const destFileName = "dest.bin";
    U8 data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    U8 dataSubset[] = { 1, 2, 3, 4 };
    U32 offset = 1;
    U32 length = 4;

    FileBuffer fileBufferOut(data, sizeof(data));
    fileBufferOut.write(sourceFileName);
    FileBuffer fileBufferOutSubset(dataSubset, sizeof(dataSubset));

    // Test send partial past end of file, should return COMMAND_OK but raise a warning event.
    Fw::CmdResponse expResp = FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? Fw::CmdResponse::OK : Fw::CmdResponse::VALIDATION_ERROR;
    this->sendFilePartial(sourceFileName, destFileName, expResp, sizeof(data), length);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DownlinkPartialFail(0, sourceFileName, destFileName, sizeof(data), sizeof(data));
    this->cmdResponseHistory->clear();
    this->clearEvents();

    // Send the file and assert COMMAND_OK
    this->sendFilePartial(sourceFileName, destFileName, Fw::CmdResponse::OK, offset, length);

    // Assert telemetry
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_PacketsSent_SIZE(3);
    for (size_t i = 0; i < 3; ++i) {
        ASSERT_TLM_PacketsSent(i, i + 1);
    }
    ASSERT_TLM_FilesSent_SIZE(1);
    ASSERT_TLM_FilesSent(0, 1);

    // Assert events
    ASSERT_EVENTS_SIZE(2); // Start and sent
    ASSERT_EVENTS_SendStarted_SIZE(1);
    ASSERT_EVENTS_FileSent_SIZE(1);
//    printTextLogHistory(stdout);
    ASSERT_EVENTS_FileSent(0, sourceFileName, destFileName);
    ASSERT_EVENTS_SendStarted(0, length, sourceFileName, destFileName);

    // Validate the packet history
    History<Fw::FilePacket::DataPacket> dataPackets(MAX_HISTORY_SIZE);

    CFDP::Checksum checksum;
    checksum.update(dataSubset, offset, length);

    validatePacketHistory(
        *this->fromPortHistory_bufferSendOut,
        dataPackets,
        Fw::FilePacket::T_END,
        3,
        checksum,
        1
    );

    // Compare the outgoing and incoming files
    FileBuffer fileBufferIn(dataPackets);
    ASSERT_EQ(true, FileBuffer::compare(fileBufferIn, fileBufferOutSubset));

    // Assert idle mode
    ASSERT_EQ(FileDownlink::Mode::IDLE, this->component.mode.get());

    // Remove the outgoing file
    this->removeFile(sourceFileName);

  }

    void FileDownlinkTester ::
      timeout()
    {
        // Assert idle mode
        ASSERT_EQ(FileDownlink::Mode::IDLE, this->component.mode.get());

        // Create a file
        const char *const sourceFileName = "source.bin";
        const char *const destFileName = "dest.bin";
        U8 data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        FileBuffer fileBufferOut(data, sizeof(data));
        fileBufferOut.write(sourceFileName);

        Fw::CmdStringArg sourceCmdStringArg(sourceFileName);
        Fw::CmdStringArg destCmdStringArg(destFileName);
        this->sendCmd_SendFile(
          INSTANCE,
          CMD_SEQ,
          sourceCmdStringArg,
          destCmdStringArg
        );
        this->component.doDispatch(); // Dispatch sendfile command
        this->component.Run_handler(0,0); // Pull file from queue

        // Continue running the component without dispatching the responses
        for (U32 i = 0; i < TIMEOUT_MS/CYCLE_MS; i++) {
            this->component.Run_handler(0,0);
            ASSERT_CMD_RESPONSE_SIZE(0);
        }

        this->component.Run_handler(0,0);
        ASSERT_CMD_RESPONSE_SIZE(1);
        Fw::CmdResponse expResp = FILEDOWNLINK_COMMAND_FAILURES_DISABLED ? Fw::CmdResponse::OK : Fw::CmdResponse::EXECUTION_ERROR;
        ASSERT_CMD_RESPONSE(0, FileDownlink::OPCODE_SENDFILE, CMD_SEQ, expResp);

        // Assert telemetry
        ASSERT_TLM_SIZE(1);
        ASSERT_TLM_PacketsSent_SIZE(1);

        // Assert events
        ASSERT_EVENTS_SIZE(2);
        ASSERT_EVENTS_DownlinkTimeout_SIZE(1);
        ASSERT_EVENTS_SendStarted_SIZE(1);
//    printTextLogHistory(stdout);
        ASSERT_EVENTS_SendStarted(0, 10, sourceFileName, destFileName);
        ASSERT_EVENTS_DownlinkTimeout(0, sourceFileName, destFileName);

        // Assert idle mode
        ASSERT_EQ(FileDownlink::Mode::COOLDOWN, this->component.mode.get());

        // Remove the outgoing file
        this->removeFile(sourceFileName);
    }

    void FileDownlinkTester ::
    sendFilePort()
  {
    // Create a file
    const char *const sourceFileName = "source.bin";
    const char *const destFileName = "dest.bin";
    U8 data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    FileBuffer fileBufferOut(data, sizeof(data));
    fileBufferOut.write(sourceFileName);

    sourceFileNameString srcFileArg(sourceFileName);
    destFileNameString  destFileArg(destFileName);
    Svc::SendFileResponse resp = this->invoke_to_SendFile(0, srcFileArg, destFileArg, 0, 0);

    ASSERT_EQ(resp.getstatus(), SendFileStatus::STATUS_OK);
    ASSERT_EQ(resp.getcontext(), 0);

    this->component.Run_handler(0,0); // Dequeue file downlink request
    while (this->component.mode.get() != FileDownlink::Mode::IDLE) {
      if(this->component.mode.get() != FileDownlink::Mode::COOLDOWN) {
        this->component.doDispatch();
      }
      this->component.Run_handler(0,0);
    }

    ASSERT_from_FileComplete_SIZE(1);
    ASSERT_from_FileComplete(0, Svc::SendFileResponse(SendFileStatus(SendFileStatus::STATUS_OK), 0));

    // Assert telemetry
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_PacketsSent_SIZE(3);
    for (size_t i = 0; i < 3; ++i) {
        ASSERT_TLM_PacketsSent(i, i + 1);
    }
    ASSERT_TLM_FilesSent_SIZE(1);
    ASSERT_TLM_FilesSent(0, 1);

    // Assert events
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_FileSent_SIZE(1);
    ASSERT_EVENTS_SendStarted_SIZE(1);
    ASSERT_EVENTS_SendStarted(0, 10, sourceFileName, destFileName);
    ASSERT_EVENTS_FileSent(0, sourceFileName, destFileName);

    // Validate the packet history
    History<Fw::FilePacket::DataPacket> dataPackets(MAX_HISTORY_SIZE);
    CFDP::Checksum checksum;
    fileBufferOut.getChecksum(checksum);
    validatePacketHistory(
        *this->fromPortHistory_bufferSendOut,
        dataPackets,
        Fw::FilePacket::T_END,
        3,
        checksum,
        0
    );

    // Compare the outgoing and incoming files
    FileBuffer fileBufferIn(dataPackets);
    ASSERT_EQ(true, FileBuffer::compare(fileBufferIn, fileBufferOut));

    // Remove the outgoing file
    this->removeFile(sourceFileName);
  }

  // ----------------------------------------------------------------------
  // Handlers for from ports
  // ----------------------------------------------------------------------

   void FileDownlinkTester ::
    from_bufferSendOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& buffer
    )
  {
    ASSERT_LT(buffers_index, FW_NUM_ARRAY_ELEMENTS(this->buffers));
    // Copy buffer before recycling
    U8* data = new U8[buffer.getSize()];
    this->buffers[buffers_index] = data;
    buffers_index++;
    ::memcpy(data, buffer.getData(), buffer.getSize());
    Fw::Buffer buffer_new = buffer;
    buffer_new.setData(data);
    pushFromPortEntry_bufferSendOut(buffer_new);
    invoke_to_bufferReturn(0, buffer);
  }

   void FileDownlinkTester ::
     from_pingOut_handler(
         const NATIVE_INT_TYPE portNum,
         U32 key
    )
    {
        pushFromPortEntry_pingOut(key);
    }

    void FileDownlinkTester ::
      from_FileComplete_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const Svc::SendFileResponse& response
      )
    {
      pushFromPortEntry_FileComplete(response);
    }

  // ----------------------------------------------------------------------
  // Private instance methods
  // ----------------------------------------------------------------------

  void FileDownlinkTester ::
    connectPorts()
  {
    // cmdIn
    this->connect_to_cmdIn(
        0,
        this->component.get_cmdIn_InputPort(0)
    );

    // Run
    this->connect_to_Run(
        0,
        this->component.get_Run_InputPort(0)
    );

    // bufferReturn
    this->connect_to_bufferReturn(
        0,
        this->component.get_bufferReturn_InputPort(0)
    );

    // Sendfile
    this->connect_to_SendFile(
      0,
      this->component.get_SendFile_InputPort(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(
        0,
        this->get_from_timeCaller(0)
    );

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(
        0,
        this->get_from_cmdRegOut(0)
    );

    // eventOut
    this->component.set_eventOut_OutputPort(
        0,
        this->get_from_eventOut(0)
    );

    // bufferSendOut
    this->component.set_bufferSendOut_OutputPort(
        0,
        this->get_from_bufferSendOut(0)
    );

    // tlmOut
    this->component.set_tlmOut_OutputPort(
        0,
        this->get_from_tlmOut(0)
    );

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(
        0,
        this->get_from_cmdResponseOut(0)
    );

    // LogText
    this->component.set_textEventOut_OutputPort(
        0,
        this->get_from_textEventOut(0)
    );

    // FileComplete
    this->component.set_FileComplete_OutputPort(
      0,
      this->get_from_FileComplete(0)
    );
  }

  void FileDownlinkTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH,
        INSTANCE
    );
  }

  void FileDownlinkTester ::
    sendFile(
        const char *const sourceFileName,
        const char *const destFileName,
        const Fw::CmdResponse response
    )
  {
    Fw::CmdStringArg sourceCmdStringArg(sourceFileName);
    Fw::CmdStringArg destCmdStringArg(destFileName);
    this->sendCmd_SendFile(
        INSTANCE,
        CMD_SEQ,
        sourceCmdStringArg,
        destCmdStringArg
    );

    this->component.doDispatch();
    this->component.Run_handler(0,0);

    while (this->component.mode.get() != FileDownlink::Mode::IDLE) {
      if(this->component.mode.get() != FileDownlink::Mode::COOLDOWN) {
        this->component.doDispatch();
      }
      this->component.Run_handler(0,0);
    }

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileDownlink::OPCODE_SENDFILE, CMD_SEQ, response);
  }

  void FileDownlinkTester ::
    sendFilePartial(
      const char *const sourceFileName, //!< The source file name
      const char *const destFileName, //!< The destination file name
      const Fw::CmdResponse response, //!< The expected command response
      U32 startIndex, //!< The starting index
      U32 length //!< The amount of bytes to downlink
    )
  {
    Fw::CmdStringArg sourceCmdStringArg(sourceFileName);
    Fw::CmdStringArg destCmdStringArg(destFileName);
    this->sendCmd_SendPartial(
        INSTANCE,
        CMD_SEQ,
        sourceCmdStringArg,
        destCmdStringArg,
        startIndex,
        length
    );

    this->component.doDispatch();
    this->component.Run_handler(0,0);

    while (this->component.mode.get() != FileDownlink::Mode::IDLE) {
      if(this->component.mode.get() != FileDownlink::Mode::COOLDOWN) {
        this->component.doDispatch();
      }
      this->component.Run_handler(0,0);
    }

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileDownlink::OPCODE_SENDPARTIAL, CMD_SEQ, response);
  }

  void FileDownlinkTester ::
    cancel(const Fw::CmdResponse response)
  {
    // Command the File Downlink component to cancel a file downlink
    this->sendCmd_Cancel(INSTANCE, CMD_SEQ);
    this->component.doDispatch(); // Cancel command processes here
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        FileDownlink::OPCODE_CANCEL,
        CMD_SEQ,
        response
    );
  }

  void FileDownlinkTester ::
    removeFile(const char *const name)
  {
    const NATIVE_INT_TYPE status = ::unlink(name);
    if (status != 0) {
      // OK if file is not there
      ASSERT_EQ(ENOENT, errno);
    }
  }

  // ----------------------------------------------------------------------
  // Private static methods
  // ----------------------------------------------------------------------

  void FileDownlinkTester ::
    validatePacketHistory(
        const History<FromPortEntry_bufferSendOut>& historyIn,
        History<Fw::FilePacket::DataPacket>& historyOut,
        const Fw::FilePacket::Type finalPacketType,
        const size_t numPackets,
        const CFDP::Checksum& checksum,
        U32 startOffset
    )
  {

    FW_ASSERT(numPackets > 0);

    const size_t size = historyIn.size();
    ASSERT_EQ(numPackets, size);

    {
      const Fw::Buffer& buffer = historyIn.at(0).fwBuffer;
      validateStartPacket(buffer);
    }

    for (size_t i = 1; i < size - 1; ++i) {
      const Fw::Buffer& buffer = historyIn.at(i).fwBuffer;
      Fw::FilePacket::DataPacket dataPacket;
      validateDataPacket(
          buffer,
          dataPacket,
          i,
          startOffset
      );
      historyOut.push_back(dataPacket);
    }

    const size_t index = size - 1;
    const Fw::Buffer& buffer = historyIn.at(index).fwBuffer;
    switch (finalPacketType) {
      case Fw::FilePacket::T_END: {
        validateEndPacket(buffer, index, checksum);
        break;
      }
      case Fw::FilePacket::T_CANCEL: {
        validateCancelPacket(buffer, index);
        break;
      }
      default:
        FW_ASSERT(0);
    }

  }

  void FileDownlinkTester ::
    validateFilePacket(
        const Fw::Buffer& buffer,
        Fw::FilePacket& filePacket
    )
  {
    const Fw::SerializeStatus status = filePacket.fromBuffer(buffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
  }

  void FileDownlinkTester ::
    validateStartPacket(const Fw::Buffer& buffer)
  {
    Fw::FilePacket filePacket;
    validateFilePacket(buffer, filePacket);
    const Fw::FilePacket::Header& header = filePacket.asHeader();
    ASSERT_EQ(0U, header.sequenceIndex);
    ASSERT_EQ(Fw::FilePacket::T_START, header.type);
  }

  void FileDownlinkTester ::
    validateDataPacket(
        const Fw::Buffer& buffer,
        Fw::FilePacket::DataPacket& dataPacket,
        const U32 sequenceIndex,
        U32& byteOffset
    )
  {
    Fw::FilePacket filePacket;
    validateFilePacket(buffer, filePacket);
    const Fw::FilePacket::Header& header = filePacket.asHeader();
    ASSERT_EQ(sequenceIndex, header.sequenceIndex);
    ASSERT_EQ(Fw::FilePacket::T_DATA, header.type);
    dataPacket = filePacket.asDataPacket();
    ASSERT_EQ(byteOffset, dataPacket.byteOffset);
    byteOffset += dataPacket.dataSize;
  }

  void FileDownlinkTester ::
    validateEndPacket(
        const Fw::Buffer& buffer,
        const U32 sequenceIndex,
        const CFDP::Checksum& checksum
    )
  {
    Fw::FilePacket filePacket;
    validateFilePacket(buffer, filePacket);
    const Fw::FilePacket::Header& header = filePacket.asHeader();
    ASSERT_EQ(sequenceIndex, header.sequenceIndex);
    ASSERT_EQ(Fw::FilePacket::T_END, header.type);
    const Fw::FilePacket::EndPacket endPacket = filePacket.asEndPacket();
    CFDP::Checksum computedChecksum;
    endPacket.getChecksum(computedChecksum);
    ASSERT_EQ(true, checksum == computedChecksum);
  }

  void FileDownlinkTester ::
    validateCancelPacket(
        const Fw::Buffer& buffer,
        const U32 sequenceIndex
    )
  {
    Fw::FilePacket filePacket;
    validateFilePacket(buffer, filePacket);
    const Fw::FilePacket::Header& header = filePacket.asHeader();
    ASSERT_EQ(sequenceIndex, header.sequenceIndex);
    ASSERT_EQ(Fw::FilePacket::T_CANCEL, header.type);
  }

}
