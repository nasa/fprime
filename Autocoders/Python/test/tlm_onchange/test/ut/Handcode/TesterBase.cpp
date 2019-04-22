// ======================================================================
// \title  TestTlm/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TestTlm component test harness base class
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

namespace Tlm {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestTlmTesterBase ::
    TestTlmTesterBase(
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
    this->tlmHistory_somechan =
      new History<TlmEntry_somechan>(maxHistorySize);
    this->tlmHistory_enumchan =
      new History<TlmEntry_enumchan>(maxHistorySize);
    this->tlmHistory_quatchan =
      new History<TlmEntry_quatchan>(maxHistorySize);
    this->tlmHistory_stringchan =
      new History<TlmEntry_stringchan>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  TestTlmTesterBase ::
    ~TestTlmTesterBase(void)
  {
    // Destroy telemetry histories
    delete this->tlmHistory_somechan;
    delete this->tlmHistory_enumchan;
    delete this->tlmHistory_quatchan;
    delete this->tlmHistory_stringchan;
  }

  void TestTlmTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port Tlm

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_Tlm();
        ++_port
    ) {

      this->m_from_Tlm[_port].init();
      this->m_from_Tlm[_port].addCallComp(
          this,
          from_Tlm_static
      );
      this->m_from_Tlm[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_Tlm[%d]",
          this->m_objName,
          _port
      );
      this->m_from_Tlm[_port].setObjName(_portName);
#endif

    }

    // Attach input port Time

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_Time();
        ++_port
    ) {

      this->m_from_Time[_port].init();
      this->m_from_Time[_port].addCallComp(
          this,
          from_Time_static
      );
      this->m_from_Time[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_Time[%d]",
          this->m_objName,
          _port
      );
      this->m_from_Time[_port].setObjName(_portName);
#endif

    }

    // Initialize output port aport

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_aport();
        ++_port
    ) {
      this->m_to_aport[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_aport[%d]",
          this->m_objName,
          _port
      );
      this->m_to_aport[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TestTlmTesterBase ::
    getNum_to_aport(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_aport);
  }

  NATIVE_INT_TYPE TestTlmTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE TestTlmTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    connect_to_aport(
        const NATIVE_INT_TYPE portNum,
        Another::InputTestPort *const aport
    )
  {
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    this->m_to_aport[portNum].addCallPort(aport);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    invoke_to_aport(
        const NATIVE_INT_TYPE portNum,
        I32 arg4,
        F32 arg5,
        U8 arg6
    )
  {
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    this->m_to_aport[portNum].invoke(
        arg4, arg5, arg6
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool TestTlmTesterBase ::
    isConnected_to_aport(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_aport(), static_cast<AssertArg>(portNum));
    return this->m_to_aport[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  Fw::InputTlmPort *TestTlmTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputTimePort *TestTlmTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    TestTlmTesterBase* _testerBase =
      static_cast<TestTlmTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void TestTlmTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    TestTlmTesterBase* _testerBase =
      static_cast<TestTlmTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // History
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    clearHistory()
  {
    this->clearTlm();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
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

      case TestTlmComponentBase::CHANNELID_SOMECHAN:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing somechan: %d\n", _status);
          return;
        }
        this->tlmInput_somechan(timeTag, arg);
        break;
      }

      case TestTlmComponentBase::CHANNELID_ENUMCHAN:
      {
        FwEnumStoreType enumchanarg;
        const Fw::SerializeStatus _status = val.deserialize(enumchanarg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing enumchan: %d\n", _status);
          return;
        }
        TestTlmComponentBase::SomeEnum arg =
          static_cast<TestTlmComponentBase::SomeEnum>(enumchanarg);
        this->tlmInput_enumchan(timeTag, arg);
        break;
      }

      case TestTlmComponentBase::CHANNELID_QUATCHAN:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing quatchan: %d\n", _status);
          return;
        }
        this->tlmInput_quatchan(timeTag, arg);
        break;
      }

      case TestTlmComponentBase::CHANNELID_STRINGCHAN:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing stringchan: %d\n", _status);
          return;
        }
        this->tlmInput_stringchan(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void TestTlmTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_somechan->clear();
    this->tlmHistory_enumchan->clear();
    this->tlmHistory_quatchan->clear();
    this->tlmHistory_stringchan->clear();
  }

  // ----------------------------------------------------------------------
  // Channel: somechan
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    tlmInput_somechan(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_somechan e = { timeTag, val };
    this->tlmHistory_somechan->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Channel: enumchan
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    tlmInput_enumchan(
        const Fw::Time& timeTag,
        const TestTlmComponentBase::SomeEnum& val
    )
  {
    TlmEntry_enumchan e = { timeTag, val };
    this->tlmHistory_enumchan->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Channel: quatchan
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    tlmInput_quatchan(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_quatchan e = { timeTag, val };
    this->tlmHistory_quatchan->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Channel: stringchan
  // ----------------------------------------------------------------------

  void TestTlmTesterBase ::
    tlmInput_stringchan(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_stringchan e = { timeTag, val };
    this->tlmHistory_stringchan->push_back(e);
    ++this->tlmSize;
  }

} // end namespace Tlm
