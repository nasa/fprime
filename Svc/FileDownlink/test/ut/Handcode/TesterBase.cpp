// ======================================================================
// \title  FileDownlink/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for FileDownlink component test harness base class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <stdlib.h>
#include <string.h>
#include "TesterBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  FileDownlinkTesterBase ::
    FileDownlinkTesterBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) :
#if FW_OBJECT_NAMES == 1
      Fw::PassiveComponentBase(compName)
#else
      Fw::PassiveComponentBase()
#endif
  {
    // Initialize command history
    this->cmdResponseHistory = new History<CmdResponse>(maxHistorySize);
    // Initialize telemetry histories
    this->tlmHistory_FileDownlink_FilesSent = 
      new History<TlmEntry_FileDownlink_FilesSent>(maxHistorySize);
    this->tlmHistory_FileDownlink_PacketsSent = 
      new History<TlmEntry_FileDownlink_PacketsSent>(maxHistorySize);
    this->tlmHistory_FileDownlink_Warnings = 
      new History<TlmEntry_FileDownlink_Warnings>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_FileDownlink_FileOpenError =
      new History<EventEntry_FileDownlink_FileOpenError>(maxHistorySize);
    this->eventHistory_FileDownlink_FileReadError =
      new History<EventEntry_FileDownlink_FileReadError>(maxHistorySize);
    this->eventHistory_FileDownlink_FileSent =
      new History<EventEntry_FileDownlink_FileSent>(maxHistorySize);
    this->eventHistory_FileDownlink_DownlinkCanceled =
      new History<EventEntry_FileDownlink_DownlinkCanceled>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_bufferGetCaller =
      new History<FromPortEntry_bufferGetCaller>(maxHistorySize);
    this->fromPortHistory_bufferSendOut =
      new History<FromPortEntry_bufferSendOut>(maxHistorySize);
    this->fromPortHistory_pingOut =
      new History<FromPortEntry_pingOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  FileDownlinkTesterBase ::
    ~FileDownlinkTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy telemetry histories
    delete this->tlmHistory_FileDownlink_FilesSent;
    delete this->tlmHistory_FileDownlink_PacketsSent;
    delete this->tlmHistory_FileDownlink_Warnings;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_FileDownlink_FileOpenError;
    delete this->eventHistory_FileDownlink_FileReadError;
    delete this->eventHistory_FileDownlink_FileSent;
    delete this->eventHistory_FileDownlink_DownlinkCanceled;
  }

  void FileDownlinkTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port bufferGetCaller

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_bufferGetCaller();
        ++_port
    ) {

      this->m_from_bufferGetCaller[_port].init();
      this->m_from_bufferGetCaller[_port].addCallComp(
          this,
          from_bufferGetCaller_static
      );
      this->m_from_bufferGetCaller[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_bufferGetCaller[%d]",
          this->m_objName,
          _port
      );
      this->m_from_bufferGetCaller[_port].setObjName(_portName);
#endif

    }

    // Attach input port timeCaller

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_timeCaller();
        ++_port
    ) {

      this->m_from_timeCaller[_port].init();
      this->m_from_timeCaller[_port].addCallComp(
          this,
          from_timeCaller_static
      );
      this->m_from_timeCaller[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_timeCaller[%d]",
          this->m_objName,
          _port
      );
      this->m_from_timeCaller[_port].setObjName(_portName);
#endif

    }

    // Attach input port bufferSendOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_bufferSendOut();
        ++_port
    ) {

      this->m_from_bufferSendOut[_port].init();
      this->m_from_bufferSendOut[_port].addCallComp(
          this,
          from_bufferSendOut_static
      );
      this->m_from_bufferSendOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_bufferSendOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_bufferSendOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port tlmOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_tlmOut();
        ++_port
    ) {

      this->m_from_tlmOut[_port].init();
      this->m_from_tlmOut[_port].addCallComp(
          this,
          from_tlmOut_static
      );
      this->m_from_tlmOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_tlmOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_tlmOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port cmdResponseOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_cmdResponseOut();
        ++_port
    ) {

      this->m_from_cmdResponseOut[_port].init();
      this->m_from_cmdResponseOut[_port].addCallComp(
          this,
          from_cmdResponseOut_static
      );
      this->m_from_cmdResponseOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_cmdResponseOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_cmdResponseOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port cmdRegOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_cmdRegOut();
        ++_port
    ) {

      this->m_from_cmdRegOut[_port].init();
      this->m_from_cmdRegOut[_port].addCallComp(
          this,
          from_cmdRegOut_static
      );
      this->m_from_cmdRegOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_cmdRegOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_cmdRegOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port eventOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_eventOut();
        ++_port
    ) {

      this->m_from_eventOut[_port].init();
      this->m_from_eventOut[_port].addCallComp(
          this,
          from_eventOut_static
      );
      this->m_from_eventOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_eventOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_eventOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port pingOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_pingOut();
        ++_port
    ) {

      this->m_from_pingOut[_port].init();
      this->m_from_pingOut[_port].addCallComp(
          this,
          from_pingOut_static
      );
      this->m_from_pingOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_pingOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_pingOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port LogText

#if FW_ENABLE_TEXT_LOGGING == 1
    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_LogText();
        ++_port
    ) {

      this->m_from_LogText[_port].init();
      this->m_from_LogText[_port].addCallComp(
          this,
          from_LogText_static
      );
      this->m_from_LogText[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_LogText[%d]",
          this->m_objName,
          _port
      );
      this->m_from_LogText[_port].setObjName(_portName);
#endif

    }
#endif

    // Initialize output port pingIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_pingIn();
        ++_port
    ) {
      this->m_to_pingIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_pingIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_pingIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_bufferGetCaller(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_bufferGetCaller);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_timeCaller(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_timeCaller);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_to_cmdIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_cmdIn);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_bufferSendOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_bufferSendOut);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_tlmOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_tlmOut);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_cmdResponseOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdResponseOut);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_cmdRegOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdRegOut);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_eventOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_eventOut);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_to_pingIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_pingIn);
  }

  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_pingOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_pingOut);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE FileDownlinkTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    connect_to_cmdIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdPort *const cmdIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(),static_cast<AssertArg>(portNum));
    this->m_to_cmdIn[portNum].addCallPort(cmdIn);
  }

  void FileDownlinkTesterBase ::
    connect_to_pingIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputPingPort *const pingIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    this->m_to_pingIn[portNum].addCallPort(pingIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    invoke_to_pingIn(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    this->m_to_pingIn[portNum].invoke(
        key
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool FileDownlinkTesterBase ::
    isConnected_to_cmdIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(), static_cast<AssertArg>(portNum));
    return this->m_to_cmdIn[portNum].isConnected();
  }

  bool FileDownlinkTesterBase ::
    isConnected_to_pingIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(), static_cast<AssertArg>(portNum));
    return this->m_to_pingIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputBufferGetPort *FileDownlinkTesterBase ::
    get_from_bufferGetCaller(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_bufferGetCaller(),static_cast<AssertArg>(portNum));
    return &this->m_from_bufferGetCaller[portNum];
  }

  Fw::InputTimePort *FileDownlinkTesterBase ::
    get_from_timeCaller(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_timeCaller(),static_cast<AssertArg>(portNum));
    return &this->m_from_timeCaller[portNum];
  }

  Fw::InputBufferSendPort *FileDownlinkTesterBase ::
    get_from_bufferSendOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_bufferSendOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_bufferSendOut[portNum];
  }

  Fw::InputTlmPort *FileDownlinkTesterBase ::
    get_from_tlmOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_tlmOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_tlmOut[portNum];
  }

  Fw::InputCmdResponsePort *FileDownlinkTesterBase ::
    get_from_cmdResponseOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdResponseOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdResponseOut[portNum];
  }

  Fw::InputCmdRegPort *FileDownlinkTesterBase ::
    get_from_cmdRegOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdRegOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdRegOut[portNum];
  }

  Fw::InputLogPort *FileDownlinkTesterBase ::
    get_from_eventOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_eventOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_eventOut[portNum];
  }

  Svc::InputPingPort *FileDownlinkTesterBase ::
    get_from_pingOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_pingOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_pingOut[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *FileDownlinkTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  Fw::Buffer FileDownlinkTesterBase ::
    from_bufferGetCaller_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    FW_ASSERT(callComp);
    FileDownlinkTesterBase* _testerBase = 
      static_cast<FileDownlinkTesterBase*>(callComp);
    return _testerBase->from_bufferGetCaller_handlerBase(
        portNum,
        size
    );
  }

  void FileDownlinkTesterBase ::
    from_bufferSendOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(callComp);
    FileDownlinkTesterBase* _testerBase = 
      static_cast<FileDownlinkTesterBase*>(callComp);
    _testerBase->from_bufferSendOut_handlerBase(
        portNum,
        fwBuffer
    );
  }

  void FileDownlinkTesterBase ::
    from_pingOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(callComp);
    FileDownlinkTesterBase* _testerBase = 
      static_cast<FileDownlinkTesterBase*>(callComp);
    _testerBase->from_pingOut_handlerBase(
        portNum,
        key
    );
  }

  void FileDownlinkTesterBase ::
    from_cmdResponseOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    FileDownlinkTesterBase* _testerBase =
      static_cast<FileDownlinkTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void FileDownlinkTesterBase ::
    from_cmdRegOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void FileDownlinkTesterBase ::
    from_tlmOut_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    FileDownlinkTesterBase* _testerBase =
      static_cast<FileDownlinkTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void FileDownlinkTesterBase ::
    from_eventOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    FileDownlinkTesterBase* _testerBase =
      static_cast<FileDownlinkTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void FileDownlinkTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    FileDownlinkTesterBase* _testerBase =
      static_cast<FileDownlinkTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void FileDownlinkTesterBase ::
    from_timeCaller_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    FileDownlinkTesterBase* _testerBase =
      static_cast<FileDownlinkTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_bufferGetCaller->clear();
    this->fromPortHistory_bufferSendOut->clear();
    this->fromPortHistory_pingOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: bufferGetCaller
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    pushFromPortEntry_bufferGetCaller(
        U32 size
    )
  {
    FromPortEntry_bufferGetCaller _e = {
      size
    };
    this->fromPortHistory_bufferGetCaller->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: bufferSendOut
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    pushFromPortEntry_bufferSendOut(
        Fw::Buffer &fwBuffer
    )
  {
    FromPortEntry_bufferSendOut _e = {
      fwBuffer
    };
    this->fromPortHistory_bufferSendOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: pingOut
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    pushFromPortEntry_pingOut(
        U32 key
    )
  {
    FromPortEntry_pingOut _e = {
      key
    };
    this->fromPortHistory_pingOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  Fw::Buffer FileDownlinkTesterBase ::
    from_bufferGetCaller_handlerBase(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    FW_ASSERT(portNum < this->getNum_from_bufferGetCaller(),static_cast<AssertArg>(portNum));
    return this->from_bufferGetCaller_handler(
        portNum,
        size
    );
  }

  void FileDownlinkTesterBase ::
    from_bufferSendOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_from_bufferSendOut(),static_cast<AssertArg>(portNum));
    this->from_bufferSendOut_handler(
        portNum,
        fwBuffer
    );
  }

  void FileDownlinkTesterBase ::
    from_pingOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_from_pingOut(),static_cast<AssertArg>(portNum));
    this->from_pingOut_handler(
        portNum,
        key
    );
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    cmdResponseIn(
        const FwOpcodeType opCode,
        const U32 seq,
        const Fw::CommandResponse response
    )
  {
    CmdResponse e = { opCode, seq, response };
    this->cmdResponseHistory->push_back(e);
  }

  // ---------------------------------------------------------------------- 
  // Command: FileDownlink_SendFile
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    sendCmd_FileDownlink_SendFile(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        const Fw::CmdStringArg& sourceFileName,
        const Fw::CmdStringArg& destFileName
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(sourceFileName);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(destFileName);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = FileDownlinkComponentBase::OPCODE_FILEDOWNLINK_SENDFILE + idBase;

    if (this->m_to_cmdIn[0].isConnected()) {
      this->m_to_cmdIn[0].invoke(
          _opcode,
          cmdSeq,
          buff
      );
    }
    else {
      printf("Test Command Output port not connected!\n");
    }

  }

  // ---------------------------------------------------------------------- 
  // Command: FileDownlink_Cancel
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    sendCmd_FileDownlink_Cancel(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = FileDownlinkComponentBase::OPCODE_FILEDOWNLINK_CANCEL + idBase;

    if (this->m_to_cmdIn[0].isConnected()) {
      this->m_to_cmdIn[0].invoke(
          _opcode,
          cmdSeq,
          buff
      );
    }
    else {
      printf("Test Command Output port not connected!\n");
    }

  }

  
  void FileDownlinkTesterBase ::
    sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args) {
       
    const U32 idBase = this->getIdBase();   
    FwOpcodeType _opcode = opcode + idBase;
    if (this->m_to_cmdIn[0].isConnected()) {
      this->m_to_cmdIn[0].invoke(
          _opcode,
          cmdSeq,
          args
      );
    }
    else {
      printf("Test Command Output port not connected!\n");
    }
        
  }
  
  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
    this->clearFromPortHistory();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    dispatchTlm(
        const FwChanIdType id,
        const Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {

    val.resetDeser();

    const U32 idBase = this->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);

    switch (id - idBase) {

      case FileDownlinkComponentBase::CHANNELID_FILEDOWNLINK_FILESSENT:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing FileDownlink_FilesSent: %d\n", _status);
          return;
        }
        this->tlmInput_FileDownlink_FilesSent(timeTag, arg);
        break;
      }

      case FileDownlinkComponentBase::CHANNELID_FILEDOWNLINK_PACKETSSENT:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing FileDownlink_PacketsSent: %d\n", _status);
          return;
        }
        this->tlmInput_FileDownlink_PacketsSent(timeTag, arg);
        break;
      }

      case FileDownlinkComponentBase::CHANNELID_FILEDOWNLINK_WARNINGS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing FileDownlink_Warnings: %d\n", _status);
          return;
        }
        this->tlmInput_FileDownlink_Warnings(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void FileDownlinkTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_FileDownlink_FilesSent->clear();
    this->tlmHistory_FileDownlink_PacketsSent->clear();
    this->tlmHistory_FileDownlink_Warnings->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: FileDownlink_FilesSent
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    tlmInput_FileDownlink_FilesSent(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_FileDownlink_FilesSent e = { timeTag, val };
    this->tlmHistory_FileDownlink_FilesSent->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: FileDownlink_PacketsSent
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    tlmInput_FileDownlink_PacketsSent(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_FileDownlink_PacketsSent e = { timeTag, val };
    this->tlmHistory_FileDownlink_PacketsSent->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: FileDownlink_Warnings
  // ---------------------------------------------------------------------- 

  void FileDownlinkTesterBase ::
    tlmInput_FileDownlink_Warnings(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_FileDownlink_Warnings e = { timeTag, val };
    this->tlmHistory_FileDownlink_Warnings->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    dispatchEvents(
        const FwEventIdType id,
        Fw::Time &timeTag,
        const Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {

    args.resetDeser();

    const U32 idBase = this->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);
    switch (id - idBase) {

      case FileDownlinkComponentBase::EVENTID_FILEDOWNLINK_FILEOPENERROR: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileDownlink_FileOpenError(fileName);

        break;

      }

      case FileDownlinkComponentBase::EVENTID_FILEDOWNLINK_FILEREADERROR: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileDownlink_FileReadError(fileName);

        break;

      }

      case FileDownlinkComponentBase::EVENTID_FILEDOWNLINK_FILESENT: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 2,_numArgs,2);
        
#endif    
        Fw::LogStringArg sourceFileName;
        _status = args.deserialize(sourceFileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        Fw::LogStringArg destFileName;
        _status = args.deserialize(destFileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_FileDownlink_FileSent(sourceFileName, destFileName);

        break;

      }

      case FileDownlinkComponentBase::EVENTID_FILEDOWNLINK_DOWNLINKCANCELED: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 2,_numArgs,2);
        
#endif    
        Fw::LogStringArg sourceFileName;
        _status = args.deserialize(sourceFileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        Fw::LogStringArg destFileName;
        _status = args.deserialize(destFileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_FileDownlink_DownlinkCanceled(sourceFileName, destFileName);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void FileDownlinkTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_FileDownlink_FileOpenError->clear();
    this->eventHistory_FileDownlink_FileReadError->clear();
    this->eventHistory_FileDownlink_FileSent->clear();
    this->eventHistory_FileDownlink_DownlinkCanceled->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    textLogIn(
        const U32 id,
        Fw::Time &timeTag,
        const Fw::TextLogSeverity severity,
        const Fw::TextLogString &text
    )
  {
    TextLogEntry e = { id, timeTag, severity, text };
    textLogHistory->push_back(e);
  }

  void FileDownlinkTesterBase ::
    printTextLogHistoryEntry(
        const TextLogEntry& e,
        FILE* file
    )
  {
    const char *severityString = "UNKNOWN";
    switch (e.severity) {
      case Fw::LOG_FATAL:
        severityString = "FATAL";
        break;
      case Fw::LOG_WARNING_HI:
        severityString = "WARNING_HI";
        break;
      case Fw::LOG_WARNING_LO:
        severityString = "WARNING_LO";
        break;
      case Fw::LOG_COMMAND:
        severityString = "COMMAND";
        break;
      case Fw::LOG_ACTIVITY_HI:
        severityString = "ACTIVITY_HI";
        break;
      case Fw::LOG_ACTIVITY_LO:
        severityString = "ACTIVITY_LO";
        break;
      case Fw::LOG_DIAGNOSTIC:
       severityString = "DIAGNOSTIC";
        break;
      default:
        severityString = "SEVERITY ERROR";
        break;
    }

    fprintf(
        file,
        "EVENT: (%d) (%d:%d,%d) %s: %s\n",
        e.id,
        const_cast<TextLogEntry&>(e).timeTag.getTimeBase(),
        const_cast<TextLogEntry&>(e).timeTag.getSeconds(),
        const_cast<TextLogEntry&>(e).timeTag.getUSeconds(),
        severityString,
        e.text.toChar()
    );

  }

  void FileDownlinkTesterBase ::
    printTextLogHistory(FILE *file) 
  {
    for (U32 i = 0; i < this->textLogHistory->size(); ++i) {
      this->printTextLogHistoryEntry(
          this->textLogHistory->at(i), 
          file
      );
    }
  }

#endif

  // ----------------------------------------------------------------------
  // Event: FileDownlink_FileOpenError 
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    logIn_WARNING_HI_FileDownlink_FileOpenError(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_FileDownlink_FileOpenError e = {
      fileName
    };
    eventHistory_FileDownlink_FileOpenError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileDownlink_FileReadError 
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    logIn_WARNING_HI_FileDownlink_FileReadError(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_FileDownlink_FileReadError e = {
      fileName
    };
    eventHistory_FileDownlink_FileReadError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileDownlink_FileSent 
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    logIn_ACTIVITY_HI_FileDownlink_FileSent(
        Fw::LogStringArg& sourceFileName,
        Fw::LogStringArg& destFileName
    )
  {
    EventEntry_FileDownlink_FileSent e = {
      sourceFileName, destFileName
    };
    eventHistory_FileDownlink_FileSent->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileDownlink_DownlinkCanceled 
  // ----------------------------------------------------------------------

  void FileDownlinkTesterBase ::
    logIn_ACTIVITY_HI_FileDownlink_DownlinkCanceled(
        Fw::LogStringArg& sourceFileName,
        Fw::LogStringArg& destFileName
    )
  {
    EventEntry_FileDownlink_DownlinkCanceled e = {
      sourceFileName, destFileName
    };
    eventHistory_FileDownlink_DownlinkCanceled->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
