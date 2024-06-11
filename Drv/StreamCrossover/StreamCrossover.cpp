// ======================================================================
// \title  StreamCrossover.cpp
// \author ethanchee
// \brief  cpp file for StreamCrossover component implementation class
// ======================================================================


#include <Drv/StreamCrossover/StreamCrossover.hpp>
#include <FpConfig.hpp>

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
    if(recvStatus == Drv::RecvStatus::RECV_ERROR || recvBuffer.getSize() == 0)
    {
      this->log_WARNING_HI_StreamOutError(Drv::SendStatus::SEND_ERROR);
      this->errorDeallocate_out(0, recvBuffer);
      return;
    }

    Drv::SendStatus sendStatus = this->streamOut_out(0, recvBuffer);

    if(sendStatus != Drv::SendStatus::SEND_OK)
    {
      this->log_WARNING_HI_StreamOutError(sendStatus);
    }
  }

} // end namespace Drv
