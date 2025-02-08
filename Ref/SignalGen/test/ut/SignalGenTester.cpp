// ======================================================================
// \title  SignalGen.hpp
// \author mstarch
// \brief  cpp file for SignalGen test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "SignalGenTester.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  SignalGenTester ::
    SignalGenTester() :
      SignalGenGTestBase("Tester", MAX_HISTORY_SIZE),
      component("SignalGen")
  {
    this->initComponents();
    this->connectPorts();
    this->m_reqDpBuff.set(this->m_dpBuff,sizeof(this->m_dpBuff));
  }

  SignalGenTester ::
    ~SignalGenTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void SignalGenTester ::
    test_start()
  {
       ASSERT_TLM_Output_SIZE(0);
       sendCmd_Toggle(0, 0);
       component.doDispatch();
       invoke_to_schedIn(0, 0);
       component.doDispatch();
       ASSERT_TLM_Output_SIZE(1);
       sendCmd_Dp(0,10,Ref::SignalGen_DpReqType::IMMEDIATE,1,1);
       component.doDispatch();
       // verify request for data product buffer
       ASSERT_PRODUCT_GET_SIZE(1);
       // run 2 cycles, should output data product on second
       invoke_to_schedIn(0, 0);
       ASSERT_PRODUCT_SEND_SIZE(1);

  }

    //! Handle a text event
    void SignalGenTester::textLogIn(
        FwEventIdType id, //!< The event ID
        const Fw::Time& timeTag, //!< The time
        const Fw::LogSeverity severity, //!< The severity
        const Fw::TextLogString& text //!< The event string
    ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);

    }

    Fw::Success::T SignalGenTester ::
    productGet_handler(
        FwDpIdType id,
        FwSizeType dataSize,
        Fw::Buffer& buffer
    )
  {
    printf ("Component requested %" PRI_FwSizeType " bytes.\n",dataSize);
    buffer.set(this->m_dpBuff,dataSize);
    this->pushProductGetEntry(id, dataSize);
    return Fw::Success::SUCCESS;
  }


} // end namespace Ref
