// ======================================================================
// \title  FileUplink/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for FileUplink component test harness base class
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

  FileUplinkTesterBase ::
    FileUplinkTesterBase(
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
    // Initialize telemetry histories
    this->tlmHistory_FileUplink_FilesReceived = 
      new History<TlmEntry_FileUplink_FilesReceived>(maxHistorySize);
    this->tlmHistory_FileUplink_PacketsReceived = 
      new History<TlmEntry_FileUplink_PacketsReceived>(maxHistorySize);
    this->tlmHistory_FileUplink_Warnings = 
      new History<TlmEntry_FileUplink_Warnings>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_FileUplink_BadChecksum =
      new History<EventEntry_FileUplink_BadChecksum>(maxHistorySize);
    this->eventHistory_FileUplink_FileOpenError =
      new History<EventEntry_FileUplink_FileOpenError>(maxHistorySize);
    this->eventHistory_FileUplink_FileReceived =
      new History<EventEntry_FileUplink_FileReceived>(maxHistorySize);
    this->eventHistory_FileUplink_FileWriteError =
      new History<EventEntry_FileUplink_FileWriteError>(maxHistorySize);
    this->eventHistory_FileUplink_InvalidReceiveMode =
      new History<EventEntry_FileUplink_InvalidReceiveMode>(maxHistorySize);
    this->eventHistory_FileUplink_PacketOutOfBounds =
      new History<EventEntry_FileUplink_PacketOutOfBounds>(maxHistorySize);
    this->eventHistory_FileUplink_PacketOutOfOrder =
      new History<EventEntry_FileUplink_PacketOutOfOrder>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_bufferSendOut =
      new History<FromPortEntry_bufferSendOut>(maxHistorySize);
    this->fromPortHistory_pingOut =
      new History<FromPortEntry_pingOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  FileUplinkTesterBase ::
    ~FileUplinkTesterBase(void) 
  {
    // Destroy telemetry histories
    delete this->tlmHistory_FileUplink_FilesReceived;
    delete this->tlmHistory_FileUplink_PacketsReceived;
    delete this->tlmHistory_FileUplink_Warnings;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_FileUplink_BadChecksum;
    delete this->eventHistory_FileUplink_FileOpenError;
    delete this->eventHistory_FileUplink_FileReceived;
    delete this->eventHistory_FileUplink_FileWriteError;
    delete this->eventHistory_FileUplink_InvalidReceiveMode;
    delete this->eventHistory_FileUplink_PacketOutOfBounds;
    delete this->eventHistory_FileUplink_PacketOutOfOrder;
  }

  void FileUplinkTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

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

    // Initialize output port bufferSendIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_bufferSendIn();
        ++_port
    ) {
      this->m_to_bufferSendIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_bufferSendIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_bufferSendIn[_port].setObjName(_portName);
#endif

    }

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

  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_to_bufferSendIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_bufferSendIn);
  }

  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_from_timeCaller(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_timeCaller);
  }

  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_from_bufferSendOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_bufferSendOut);
  }

  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_from_tlmOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_tlmOut);
  }

  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_from_eventOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_eventOut);
  }

  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_to_pingIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_pingIn);
  }

  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_from_pingOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_pingOut);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE FileUplinkTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    connect_to_bufferSendIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort *const bufferSendIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendIn[portNum].addCallPort(bufferSendIn);
  }

  void FileUplinkTesterBase ::
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

  void FileUplinkTesterBase ::
    invoke_to_bufferSendIn(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendIn[portNum].invoke(
        fwBuffer
    );
  }

  void FileUplinkTesterBase ::
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

  bool FileUplinkTesterBase ::
    isConnected_to_bufferSendIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(), static_cast<AssertArg>(portNum));
    return this->m_to_bufferSendIn[portNum].isConnected();
  }

  bool FileUplinkTesterBase ::
    isConnected_to_pingIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(), static_cast<AssertArg>(portNum));
    return this->m_to_pingIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputTimePort *FileUplinkTesterBase ::
    get_from_timeCaller(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_timeCaller(),static_cast<AssertArg>(portNum));
    return &this->m_from_timeCaller[portNum];
  }

  Fw::InputBufferSendPort *FileUplinkTesterBase ::
    get_from_bufferSendOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_bufferSendOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_bufferSendOut[portNum];
  }

  Fw::InputTlmPort *FileUplinkTesterBase ::
    get_from_tlmOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_tlmOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_tlmOut[portNum];
  }

  Fw::InputLogPort *FileUplinkTesterBase ::
    get_from_eventOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_eventOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_eventOut[portNum];
  }

  Svc::InputPingPort *FileUplinkTesterBase ::
    get_from_pingOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_pingOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_pingOut[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *FileUplinkTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    from_bufferSendOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(callComp);
    FileUplinkTesterBase* _testerBase = 
      static_cast<FileUplinkTesterBase*>(callComp);
    _testerBase->from_bufferSendOut_handlerBase(
        portNum,
        fwBuffer
    );
  }

  void FileUplinkTesterBase ::
    from_pingOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(callComp);
    FileUplinkTesterBase* _testerBase = 
      static_cast<FileUplinkTesterBase*>(callComp);
    _testerBase->from_pingOut_handlerBase(
        portNum,
        key
    );
  }

  void FileUplinkTesterBase ::
    from_tlmOut_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    FileUplinkTesterBase* _testerBase =
      static_cast<FileUplinkTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void FileUplinkTesterBase ::
    from_eventOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    FileUplinkTesterBase* _testerBase =
      static_cast<FileUplinkTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void FileUplinkTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    FileUplinkTesterBase* _testerBase =
      static_cast<FileUplinkTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void FileUplinkTesterBase ::
    from_timeCaller_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    FileUplinkTesterBase* _testerBase =
      static_cast<FileUplinkTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_bufferSendOut->clear();
    this->fromPortHistory_pingOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: bufferSendOut
  // ---------------------------------------------------------------------- 

  void FileUplinkTesterBase ::
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

  void FileUplinkTesterBase ::
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

  void FileUplinkTesterBase ::
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

  void FileUplinkTesterBase ::
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
  // History 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    clearHistory()
  {
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
    this->clearFromPortHistory();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
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

      case FileUplinkComponentBase::CHANNELID_FILEUPLINK_FILESRECEIVED:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing FileUplink_FilesReceived: %d\n", _status);
          return;
        }
        this->tlmInput_FileUplink_FilesReceived(timeTag, arg);
        break;
      }

      case FileUplinkComponentBase::CHANNELID_FILEUPLINK_PACKETSRECEIVED:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing FileUplink_PacketsReceived: %d\n", _status);
          return;
        }
        this->tlmInput_FileUplink_PacketsReceived(timeTag, arg);
        break;
      }

      case FileUplinkComponentBase::CHANNELID_FILEUPLINK_WARNINGS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing FileUplink_Warnings: %d\n", _status);
          return;
        }
        this->tlmInput_FileUplink_Warnings(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void FileUplinkTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_FileUplink_FilesReceived->clear();
    this->tlmHistory_FileUplink_PacketsReceived->clear();
    this->tlmHistory_FileUplink_Warnings->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: FileUplink_FilesReceived
  // ---------------------------------------------------------------------- 

  void FileUplinkTesterBase ::
    tlmInput_FileUplink_FilesReceived(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_FileUplink_FilesReceived e = { timeTag, val };
    this->tlmHistory_FileUplink_FilesReceived->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: FileUplink_PacketsReceived
  // ---------------------------------------------------------------------- 

  void FileUplinkTesterBase ::
    tlmInput_FileUplink_PacketsReceived(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_FileUplink_PacketsReceived e = { timeTag, val };
    this->tlmHistory_FileUplink_PacketsReceived->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: FileUplink_Warnings
  // ---------------------------------------------------------------------- 

  void FileUplinkTesterBase ::
    tlmInput_FileUplink_Warnings(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_FileUplink_Warnings e = { timeTag, val };
    this->tlmHistory_FileUplink_Warnings->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
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

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_BADCHECKSUM: 
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
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 computed;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(computed);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 read;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(read);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileUplink_BadChecksum(fileName, computed, read);

        break;

      }

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_FILEOPENERROR: 
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

        this->logIn_WARNING_HI_FileUplink_FileOpenError(fileName);

        break;

      }

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_FILERECEIVED: 
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

        this->logIn_ACTIVITY_HI_FileUplink_FileReceived(fileName);

        break;

      }

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_FILEWRITEERROR: 
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

        this->logIn_WARNING_HI_FileUplink_FileWriteError(fileName);

        break;

      }

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_INVALIDRECEIVEMODE: 
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
        U32 packetType;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(packetType);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 mode;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(mode);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileUplink_InvalidReceiveMode(packetType, mode);

        break;

      }

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_PACKETOUTOFBOUNDS: 
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
        U32 packetIndex;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(packetIndex);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileUplink_PacketOutOfBounds(packetIndex, fileName);

        break;

      }

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_PACKETOUTOFORDER: 
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
        U32 packetIndex;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(packetIndex);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 lastPacketIndex;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(lastPacketIndex);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileUplink_PacketOutOfOrder(packetIndex, lastPacketIndex);

        break;

      }

      case FileUplinkComponentBase::EVENTID_FILEUPLINK_UPLINKCANCELED: 
      {

#if FW_AMPCS_COMPATIBLE
        // For AMPCS, decode zero arguments
        Fw::SerializeStatus _zero_status = Fw::FW_SERIALIZE_OK;
        U8 _noArgs;
        _zero_status = args.deserialize(_noArgs);
        FW_ASSERT(
            _zero_status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_zero_status)
        );
#endif    
        this->logIn_ACTIVITY_HI_FileUplink_UplinkCanceled();

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void FileUplinkTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_FileUplink_BadChecksum->clear();
    this->eventHistory_FileUplink_FileOpenError->clear();
    this->eventHistory_FileUplink_FileReceived->clear();
    this->eventHistory_FileUplink_FileWriteError->clear();
    this->eventHistory_FileUplink_InvalidReceiveMode->clear();
    this->eventHistory_FileUplink_PacketOutOfBounds->clear();
    this->eventHistory_FileUplink_PacketOutOfOrder->clear();
    this->eventsSize_FileUplink_UplinkCanceled = 0;
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
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

  void FileUplinkTesterBase ::
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

  void FileUplinkTesterBase ::
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
  // Event: FileUplink_BadChecksum 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_WARNING_HI_FileUplink_BadChecksum(
        Fw::LogStringArg& fileName,
        U32 computed,
        U32 read
    )
  {
    EventEntry_FileUplink_BadChecksum e = {
      fileName, computed, read
    };
    eventHistory_FileUplink_BadChecksum->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_FileOpenError 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_WARNING_HI_FileUplink_FileOpenError(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_FileUplink_FileOpenError e = {
      fileName
    };
    eventHistory_FileUplink_FileOpenError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_FileReceived 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_ACTIVITY_HI_FileUplink_FileReceived(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_FileUplink_FileReceived e = {
      fileName
    };
    eventHistory_FileUplink_FileReceived->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_FileWriteError 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_WARNING_HI_FileUplink_FileWriteError(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_FileUplink_FileWriteError e = {
      fileName
    };
    eventHistory_FileUplink_FileWriteError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_InvalidReceiveMode 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_WARNING_HI_FileUplink_InvalidReceiveMode(
        U32 packetType,
        U32 mode
    )
  {
    EventEntry_FileUplink_InvalidReceiveMode e = {
      packetType, mode
    };
    eventHistory_FileUplink_InvalidReceiveMode->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_PacketOutOfBounds 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_WARNING_HI_FileUplink_PacketOutOfBounds(
        U32 packetIndex,
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_FileUplink_PacketOutOfBounds e = {
      packetIndex, fileName
    };
    eventHistory_FileUplink_PacketOutOfBounds->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_PacketOutOfOrder 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_WARNING_HI_FileUplink_PacketOutOfOrder(
        U32 packetIndex,
        U32 lastPacketIndex
    )
  {
    EventEntry_FileUplink_PacketOutOfOrder e = {
      packetIndex, lastPacketIndex
    };
    eventHistory_FileUplink_PacketOutOfOrder->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_UplinkCanceled 
  // ----------------------------------------------------------------------

  void FileUplinkTesterBase ::
    logIn_ACTIVITY_HI_FileUplink_UplinkCanceled(
        void
    )
  {
    ++this->eventsSize_FileUplink_UplinkCanceled;
    ++this->eventsSize;
  }

} // end namespace Svc
