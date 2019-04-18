// ======================================================================
// \title  BufferLogger.hpp
// \author bocchino, mereweth
// \brief  cpp file for BufferLogger test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Os/ValidatedFile.hpp"
#include "Os/FileSystem.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 30
#define QUEUE_DEPTH 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Instance variables
  // ----------------------------------------------------------------------

    const U8 Tester::data[COM_BUFFER_LENGTH] = { 0xDE, 0xAD, 0xBE, 0xEF };

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester(bool doInitLog) :
#if FW_OBJECT_NAMES == 1
      BufferLoggerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("BufferLogger")
#else
      BufferLoggerGTestBase(MAX_HISTORY_SIZE),
      component()
#endif
  {
    (void) system("rm -rf buf");
    (void) system("mkdir buf");
    this->initComponents();
    this->connectPorts();

    if (doInitLog) {
        this->component.initLog(
            "buf/log",
            ".buf",
            static_cast<U32>(MAX_BYTES_PER_FILE),
            sizeof(SIZE_TYPE)
        );
    }
  }

  Tester ::
    ~Tester(void)
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    LogNoInit(void)
  {
    this->component.m_file.baseName = Fw::EightyCharString("LogNoInit");
    // NOTE (mereweth) - make something sensible happen when no-one calls initLog()
    // Send data
    this->sendComBuffers(3);
    ASSERT_EVENTS_SIZE(3);
    ASSERT_EVENTS_BL_NoLogFileOpenInitError_SIZE(3);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_bufferSendOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    )
  {
    this->pushFromPortEntry_bufferSendOut(fwBuffer);
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

    // cmdIn
    this->connect_to_cmdIn(
        0,
        this->component.get_cmdIn_InputPort(0)
    );

    // comIn
    this->connect_to_comIn(
        0,
        this->component.get_comIn_InputPort(0)
    );

    // pingIn
    this->connect_to_pingIn(
        0,
        this->component.get_pingIn_InputPort(0)
    );

    // schedIn
    this->connect_to_schedIn(
        0,
        this->component.get_schedIn_InputPort(0)
    );

    // bufferSendOut
    this->component.set_bufferSendOut_OutputPort(
        0,
        this->get_from_bufferSendOut(0)
    );

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(
        0,
        this->get_from_cmdRegOut(0)
    );

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(
        0,
        this->get_from_cmdResponseOut(0)
    );

    // eventOut
    this->component.set_eventOut_OutputPort(
        0,
        this->get_from_eventOut(0)
    );

    // eventOutText
    this->component.set_eventOutText_OutputPort(
        0,
        this->get_from_eventOutText(0)
    );

    // pingOut
    this->component.set_pingOut_OutputPort(
        0,
        this->get_from_pingOut(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(
        0,
        this->get_from_timeCaller(0)
    );

    // tlmOut
    this->component.set_tlmOut_OutputPort(
        0,
        this->get_from_tlmOut(0)
    );

  }

  void Tester ::
    initComponents(void)
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

  void Tester ::
    dispatchOne(void)
  {
    this->component.doDispatch();
  }

  void Tester ::
    dispatchAll(void)
  {
    while(this->component.m_queue.getNumMsgs() > 0)
      this->dispatchOne();
  }

  Fw::Time Tester ::
    generateTestTime(const U32 seconds)
  {
    Fw::Time time(TB_DONT_CARE, FW_CONTEXT_DONT_CARE, 234567, seconds);
    return time;
  }

  void Tester ::
    setTestTimeSeconds(const U32 seconds)
  {
    Fw::Time time = this->generateTestTime(seconds);
    this->setTestTime(time);
  }

  void Tester ::
    sendComBuffers(const U32 n)
  {
    Fw::ComBuffer buffer(data, sizeof(data));
    for (U32 i = 0; i < n; ++i) {
      this->invoke_to_comIn(0, buffer, 0);
      this->dispatchOne();
    }
  }

  void Tester ::
    sendManagedBuffers(const U32 n)
  {
    Fw::Buffer buffer(
        0,
        0,
        reinterpret_cast<U64>(data),
        sizeof(data)
    );
    for (U32 i = 0; i < n; ++i) {
      this->invoke_to_bufferSendIn(0, buffer);
      this->dispatchOne();
    }
  }

  void Tester ::
    checkFileExists(const Fw::EightyCharString& fileName)
  {
    Fw::EightyCharString command;
    command.format("test -f %s", fileName.toChar());
    const int status = system(command.toChar());
    ASSERT_EQ(0, status);
  }

  void Tester ::
    checkHashFileExists(const Fw::EightyCharString& fileName)
  {
    Os::ValidatedFile validatedFile(fileName.toChar());
    const Fw::EightyCharString& hashFileName = validatedFile.getHashFileName();
    this->checkFileExists(hashFileName);
  }

  void Tester ::
    checkLogFileIntegrity(
        const char *const fileName,
        const U32 expectedSize,
        const U32 expectedNumBuffers
    )
  {

    {
      // Make sure the file size is within bounds
      U64 actualSize = 0;
      const Os::FileSystem::Status status =
        Os::FileSystem::getFileSize(fileName, actualSize);
      ASSERT_EQ(Os::FileSystem::OP_OK, status);
      ASSERT_LE(expectedSize, actualSize);
    }

    // Open the file
    Os::File file;
    {
      const Os::File::Status status =
        file.open(fileName, Os::File::OPEN_READ);
      ASSERT_EQ(Os::File::OP_OK, status);
    }

    // Check the data
    U8 buf[expectedSize];
    for (U32 i = 0; i < expectedNumBuffers; ++i) {
      // Get length of buffer to read
      NATIVE_INT_TYPE length = sizeof(SIZE_TYPE);
      Os::File::Status status = file.read(&buf, length);
      ASSERT_EQ(Os::File::OP_OK, status);
      ASSERT_EQ(sizeof(SIZE_TYPE), static_cast<U32>(length));
      Fw::SerialBuffer comBuffLength(buf, length);
      comBuffLength.fill();
      SIZE_TYPE bufferSize;
      const Fw::SerializeStatus serializeStatus =
        comBuffLength.deserialize(bufferSize);
      ASSERT_EQ(Fw::FW_SERIALIZE_OK, serializeStatus);
      ASSERT_EQ(sizeof(data), bufferSize);
      // Read and check the buffer
      length = bufferSize;
      status = file.read(&buf, length);
      ASSERT_EQ(Os::File::OP_OK, status);
      ASSERT_EQ(bufferSize, static_cast<U32>(length));
      ASSERT_EQ(memcmp(buf, data, sizeof(data)), 0);
    }

    // Make sure we reached the end of the file
    {
      NATIVE_INT_TYPE length = 10;
      const Os::File::Status status = file.read(&buf, length);
      ASSERT_EQ(Os::File::OP_OK, status);
      ASSERT_EQ(0, length);
    }

  }

  void Tester ::
    checkFileValidation(const char *const fileName)
  {
    Os::ValidatedFile validatedFile(fileName);
    const Os::ValidateFile::Status status = validatedFile.validate();
    ASSERT_EQ(Os::ValidateFile::VALIDATION_OK, status);
  }

} // end namespace Svc
