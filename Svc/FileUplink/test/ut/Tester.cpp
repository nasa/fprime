// ====================================================================== 
// \title  FileUplink.hpp
// \author bocchino
// \brief  cpp file for FileUplink test harness implementation class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include <errno.h>
#include <string.h>

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) : 
      FileUplinkGTestBase("Tester", MAX_HISTORY_SIZE),
      component("FileUplink"),
      expectedPacketsReceived(0),
      sequenceIndex(0)
  {
    this->connectPorts();
    this->initComponents();
  }

  Tester ::
    ~Tester(void) 
  {
    this->component.file.osFile.close();
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------

  void Tester ::
    sendFile(void) 
  {

    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    const U32 numPackets = 2;
    U8 packetData[numPackets][5] = {
      { 0, 1, 2, 3, 4 }, 
      { 5, 6, 7, 8, 9 }
    };
    const U8 *const linearPacketData = reinterpret_cast<U8*>(packetData);
    const size_t fileSize = sizeof(packetData);

    // Send the start packet
    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    // Send the data packets
    for (size_t i = 0; i < numPackets; ++i) {
      const size_t byteOffset = i * PACKET_SIZE;
      this->sendDataPacket(byteOffset, packetData[i]);
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_FileUplink_PacketsReceived(
          0, 
          ++this->expectedPacketsReceived
      );
      ASSERT_EVENTS_SIZE(0);
    }

    // Send the end packet
    CFDP::Checksum checksum;
    checksum.update(linearPacketData, 0, fileSize);
    this->sendEndPacket(checksum);
    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_FilesReceived(0, 1);
    
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_FileReceived(0, destPath);

    // Assert we are back in START mode
    ASSERT_EQ(FileUplink::START, this->component.receiveMode);
   
    // Verify the file data
    this->verifyFileData(destPath, linearPacketData, fileSize);

    // Remove the file
    this->removeFile(destPath);

  }

  void Tester ::
    badChecksum(void) 
  {

    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    const U32 numPackets = 2;
    U8 packetData[numPackets][5] = {
      { 0, 1, 2, 3, 4 }, 
      { 5, 6, 7, 8, 9 }
    };
    const U8 *const linearPacketData = reinterpret_cast<U8*>(packetData);
    const size_t fileSize = sizeof(packetData);

    // Send the start packet
    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    // Send the data packets
    for (size_t i = 0; i < numPackets; ++i) {
      const size_t byteOffset = i * PACKET_SIZE;
      this->sendDataPacket(byteOffset, packetData[i]);
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_FileUplink_PacketsReceived(
          0, 
          ++this->expectedPacketsReceived
      );
      ASSERT_EVENTS_SIZE(0);
    }

    // Send the end packet
    CFDP::Checksum checksum;
    // Perturb the packet data to alter the checksum
    ++packetData[0][0];
    checksum.update(linearPacketData, 0, fileSize);
    this->sendEndPacket(checksum);
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_FilesReceived(0, 1);
    ASSERT_TLM_FileUplink_Warnings(0, 1);
    
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_FileUplink_FileReceived(0, destPath);
    ASSERT_EVENTS_FileUplink_BadChecksum(0, destPath, 202311690, 219088906);

    // Remove the file
    this->removeFile(destPath);

  }

  void Tester ::
    fileOpenError(void) 
  {

    const char *const sourcePath = "source.bin";
    const char *const destPath = "missing_directory/test.bin";
    this->sendStartPacket(sourcePath, destPath, 0);

    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_Warnings(0, 1);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_FileOpenError(0, destPath);

    ASSERT_EQ(FileUplink::START, this->component.receiveMode);

  }

  void Tester ::
    fileWriteError(void) 
  {
    
    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    U8 packetData[] = { 0, 1, 2, 3, 4 };
    const size_t fileSize = 2 * PACKET_SIZE;

    // Send the start packet (packet 0)
    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    // Close the file so writing will fail
    this->component.file.osFile.close();

    // Send the data packet (packet 1)
    const size_t byteOffset = PACKET_SIZE;
    this->sendDataPacket(byteOffset, packetData);
    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_Warnings(0, 1);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_FileWriteError(0, destPath);

  }

  void Tester ::
    startPacketInDataMode(void) 
  {
    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    const size_t fileSize = 0;

    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_Warnings(0, 1);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_InvalidReceiveMode(
        0,
        Fw::FilePacket::T_START, 
        FileUplink::DATA
    );

    ASSERT_EQ(FileUplink::DATA, this->component.receiveMode);

    this->removeFile(destPath);
  }

  void Tester ::
    dataPacketInStartMode(void) 
  {

    U8 packetData[PACKET_SIZE];
    const size_t byteOffset = 0;
    this->sendDataPacket(byteOffset, packetData);

    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_Warnings(0, 1);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_InvalidReceiveMode(
        0,
        Fw::FilePacket::T_DATA, 
        FileUplink::START
    );
  }

  void Tester ::
    endPacketInStartMode(void) 
  {
    CFDP::Checksum checksum;
    this->sendEndPacket(checksum);

    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_Warnings(0, 1);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_InvalidReceiveMode(
        0,
        Fw::FilePacket::T_END, 
        FileUplink::START
    );

    ASSERT_EQ(FileUplink::START, this->component.receiveMode);

  }

  void Tester ::
    packetOutOfBounds(void) 
  {

    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    const size_t fileSize = 0;

    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    const size_t byteOffset = 0;
    U8 packetData[PACKET_SIZE];
    this->sendDataPacket(byteOffset, packetData);

    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_Warnings(0, 1);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_PacketOutOfBounds(
        0, 1, destPath
    );

    this->removeFile(destPath);

  }

  void Tester ::
    packetOutOfOrder(void) 
  {
    
    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    U8 packetData[] = { 5, 6, 7, 8, 9 };
    const size_t fileSize = 2 * PACKET_SIZE;

    // Send the start packet (packet 0)
    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    // Simulate dropping of packet 1
    ++this->sequenceIndex;

    // Send the data packet (packet 2)
    const size_t byteOffset = PACKET_SIZE;
    this->sendDataPacket(byteOffset, packetData);
    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_TLM_FileUplink_Warnings(0, 1);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_PacketOutOfOrder(0, 2, 0);

    this->removeFile("test.bin");

  }

  void Tester ::
    cancelPacketInStartMode(void) 
  {
    
    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    const size_t fileSize = 0;

    // Send the start packet (packet 0)
    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    // Send a cancel packet
    this->sendCancelPacket();
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_UplinkCanceled_SIZE(1);

    // Check component state
    ASSERT_EQ(0U, this->component.lastSequenceIndex);
    ASSERT_EQ(FileUplink::START, this->component.receiveMode);

    // Remove the file
    this->removeFile("test.bin");

  }

  void Tester ::
    cancelPacketInDataMode(void) 
  {

    const char *const sourcePath = "source.bin";
    const char *const destPath = "dest.bin";
    U8 packetData[] = { 0, 1, 2, 3, 4 };
    const size_t fileSize = 2 * PACKET_SIZE;

    // Send the start packet (packet 0)
    this->sendStartPacket(sourcePath, destPath, fileSize);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    // Send the data packet (packet 1)
    const size_t byteOffset = PACKET_SIZE;
    this->sendDataPacket(byteOffset, packetData);
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(0);

    // Send a cancel packet
    this->sendCancelPacket();
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_FileUplink_PacketsReceived(
        0, 
        ++this->expectedPacketsReceived
    );
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileUplink_UplinkCanceled_SIZE(1);

    // Check component state
    ASSERT_EQ(0U, this->component.lastSequenceIndex);
    ASSERT_EQ(FileUplink::START, this->component.receiveMode);

    // Remove the file
    this->removeFile("test.bin");

  }
    
  // ----------------------------------------------------------------------
  // Handlers for from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_bufferSendOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& buffer
    )
  {
    this->pushFromPortEntry_bufferSendOut(buffer);
  }

  void Tester ::
    from_pingOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->pushFromPortEntry_pingOut(key);
  }

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
  {

    // bufferSendIn
    this->connect_to_bufferSendIn(
        0,
        this->component.get_bufferSendIn_InputPort(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(
        0, 
        this->get_from_timeCaller(0)
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

    // eventOut
    this->component.set_eventOut_OutputPort(
        0, 
        this->get_from_eventOut(0)
    );

    // LogText
    this->component.set_LogText_OutputPort(
        0, 
        this->get_from_LogText(0)
    );

    // pingIn
    this->connect_to_pingIn(
        0,
        this->component.get_pingIn_InputPort(0)
    );

    // pingOut
    this->component.set_pingOut_OutputPort(
        0,
        this->get_from_pingOut(0)
    );

  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->component.init(QUEUE_DEPTH, INSTANCE);
  }

  void Tester ::
    sendFilePacket(const Fw::FilePacket& filePacket)
  {

    this->clearHistory();

    const size_t bufferSize = filePacket.bufferSize();
    U8 bufferData[bufferSize];
    Fw::Buffer buffer(0, 0, reinterpret_cast<U64>(bufferData), bufferSize);

    const Fw::SerializeStatus status = filePacket.toBuffer(buffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);

    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();

    ASSERT_from_bufferSendOut_SIZE(1);
    ASSERT_from_bufferSendOut(0, buffer);

  }

  void Tester ::
    sendStartPacket(
        const char *const sourcePath,
        const char *const destPath,
        const size_t fileSize
    )
  {
    Fw::FilePacket::StartPacket startPacket;
    startPacket.initialize(fileSize, sourcePath, destPath);
    Fw::FilePacket filePacket;
    filePacket.fromStartPacket(startPacket);
    this->sendFilePacket(filePacket);
    this->sequenceIndex = 1;
  }

  void Tester ::
    sendDataPacket(
        const size_t byteOffset,
        U8 *const packetData
    )
  {
    const Fw::FilePacket::DataPacket dataPacket = {
      { Fw::FilePacket::T_DATA, this->sequenceIndex++ },
      static_cast<U32>(byteOffset),
      PACKET_SIZE,
      packetData
    };
    Fw::FilePacket filePacket;
    filePacket.fromDataPacket(dataPacket);
    this->sendFilePacket(filePacket);
  }

  void Tester ::
    sendEndPacket(const CFDP::Checksum& checksum)
  {
    const Fw::FilePacket::Header header = { 
      Fw::FilePacket::T_END, 
      this->sequenceIndex++ 
    };
    Fw::FilePacket::EndPacket endPacket;
    endPacket.header = header;
    endPacket.setChecksum(checksum);
    Fw::FilePacket filePacket;
    filePacket.fromEndPacket(endPacket);
    this->sendFilePacket(filePacket);
  }

  void Tester ::
    sendCancelPacket(void)
  {
    const Fw::FilePacket::Header header = { 
      Fw::FilePacket::T_CANCEL,
      this->sequenceIndex++
    };
    const Fw::FilePacket::CancelPacket cancelPacket = { header };
    Fw::FilePacket filePacket;
    filePacket.fromCancelPacket(cancelPacket);
    this->sendFilePacket(filePacket);
  }

  void Tester ::
    verifyFileData(
        const char *const path,
        const U8 *const sentData,
        const size_t dataSize
    )
  {

    Os::File file;
    U8 fileData[dataSize];

    file.open(path, Os::File::OPEN_READ);

    NATIVE_INT_TYPE intSize = static_cast<NATIVE_INT_TYPE>(dataSize);
    const Os::File::Status status = file.read(fileData, intSize);

    ASSERT_EQ(Os::File::OP_OK, status);
    ASSERT_EQ(static_cast<size_t>(intSize), dataSize);
    for (size_t i = 0; i < dataSize; ++i) {
      ASSERT_EQ(sentData[i], fileData[i]);
    }

  }

  void Tester ::
    removeFile(const char *const path)
  {
    const NATIVE_INT_TYPE status = ::unlink(path);
    if (status != 0) {
      ASSERT_EQ(ENOENT, errno);
    }
  }

}
