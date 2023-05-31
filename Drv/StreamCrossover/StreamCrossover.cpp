// ======================================================================
// \title  StreamCrossover.cpp
// \author ethanchee
// \brief  cpp file for StreamCrossover component implementation class
// ======================================================================


#include <Drv/StreamCrossover/StreamCrossover.hpp>
#include <FpConfig.hpp>

#include <FprimeArduino.hpp>

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  StreamCrossover ::
    StreamCrossover(
        const char *const compName
    ) : StreamCrossoverComponentBase(compName)
  {

  }

  StreamCrossover ::
    ~StreamCrossover()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void StreamCrossover ::
    streamIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &recvBuffer,
        const Drv::RecvStatus &recvStatus
    )
  {
    this->streamOut_out(0, recvBuffer);
  }

} // end namespace Drv
