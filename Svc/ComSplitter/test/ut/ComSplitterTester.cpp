// ======================================================================
// \title  ComSplitter.hpp
// \author gcgandhi
// \brief  cpp file for ComSplitter test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "ComSplitterTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  ComSplitterTester ::
    ComSplitterTester() :
      ComSplitterGTestBase("Tester", MAX_HISTORY_SIZE),
      component("ComSplitter")
  {
    this->initComponents();
    this->connectPorts();
  }

  ComSplitterTester ::
    ~ComSplitterTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void ComSplitterTester ::
  test_nominal()
  {
      U8 d[4] = {0xde,0xad,0xbe,0xef};
      for(U8 i = 0; i < 3; i++){
        d[0] = i;
        Fw::ComBuffer buffer(d, sizeof(d));
        invoke_to_comIn(0, buffer, 0);
      }

      ASSERT_from_comOut_SIZE(9);

      for(int i = 0; i < 3; i++){
        d[0] = i;
        Fw::ComBuffer data(d, sizeof(d));
        for(int j = 0; j < 3; j++){
          assert_comOut(i*3 + j, data);
        }
      }
  }

  void ComSplitterTester ::
    assert_comOut(
        const U32 index,
        const Fw::ComBuffer &data
    )
    const
  {
    ASSERT_GT(fromPortHistory_comOut->size(), index);
    const FromPortEntry_comOut& e = fromPortHistory_comOut->at(index);
    ASSERT_EQ(data.getBuffLength(), e.data.getBuffLength());
    ASSERT_EQ(memcmp(data.getBuffAddr(), e.data.getBuffAddr(), data.getBuffLength()), 0);
    // for(int k=0; k < e.data.getBuffLength(); k++)
    //   printf("0x%02x ", e.data.getBuffAddr()[k]);
    // printf("\n");
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void ComSplitterTester ::
    from_comOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    this->pushFromPortEntry_comOut(data, context);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void ComSplitterTester ::
    connectPorts()
  {

    // comIn
    this->connect_to_comIn(
        0,
        this->component.get_comIn_InputPort(0)
    );

    // Just connect 3 of 5:
    // comOut
    for (NATIVE_INT_TYPE i = 0; i < 3; ++i) {
      this->component.set_comOut_OutputPort(
          i,
          this->get_from_comOut(i)
      );
    }

  }

  void ComSplitterTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
