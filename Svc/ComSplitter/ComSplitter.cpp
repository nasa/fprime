// ----------------------------------------------------------------------
//
// ComSplitter.cpp
//
// ----------------------------------------------------------------------

#include <Svc/ComSplitter/ComSplitter.hpp>
#include <FpConfig.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ComSplitter ::
    ComSplitter(const char* compName) :
      ComSplitterComponentBase(compName)
  {

  }

  ComSplitter ::
    ~ComSplitter()
  {

  }

  void ComSplitter ::
    init(NATIVE_INT_TYPE instance)
  {
    ComSplitterComponentBase::init(instance);
  }

  // ----------------------------------------------------------------------
  // Handler implementations
  // ----------------------------------------------------------------------

  void ComSplitter ::
    comIn_handler(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FW_ASSERT(portNum == 0);

    NATIVE_INT_TYPE numPorts = getNum_comOut_OutputPorts();
    FW_ASSERT(numPorts > 0);

    for(NATIVE_INT_TYPE i = 0; i < numPorts; i++) {
      if( isConnected_comOut_OutputPort(i) ) {
        // Need to make a copy because we are passing by reference!:
        Fw::ComBuffer dataToSend = data;
        comOut_out(i, dataToSend, 0);
      }
    }
  }

};
