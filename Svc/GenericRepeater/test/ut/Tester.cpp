// ======================================================================
// \title  GenericRepeater.hpp
// \author joshuaa
// \brief  cpp file for GenericRepeater test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"


#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      GenericRepeaterGTestBase("Tester", MAX_HISTORY_SIZE),
      component("GenericRepeater"),
      m_num_msg_per_port()
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    testRepeater()
  {
    m_msg.resetSer();
    for(U8 i = 0; i < 32; i++) {
      m_msg.serialize(i);
    }

    invoke_to_portIn(0, m_msg);

    ASSERT_EQ(m_num_msg_per_port[0], 1);
    ASSERT_EQ(m_num_msg_per_port[1], 1);
  }

  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_portOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    ASSERT_LT(portNum, 2);
    m_num_msg_per_port[portNum]++;

    ASSERT_TRUE(Buffer == m_msg);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester::connectPorts()
  {
    // Only connecting 2/4 ports to verify that repeater doesn't send to disconnected ports
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->component.set_portOut_OutputPort(
          i,
          this->get_from_portOut(i)
      );
  }


  // ----------------------------------------------------------------------
  // Connect serial input ports
  // ----------------------------------------------------------------------
    // portIn
    this->connect_to_portIn(
        0,
        this->component.get_portIn_InputPort(0)
    );


  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
