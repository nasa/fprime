// ======================================================================
// \title  UartFramer.cpp
// \author tcanham
// \brief  cpp file for UartFramer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/UartFramer/UartFramer.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  UartFramer ::
    UartFramer(
        const char *const compName
    ) : UartFramerComponentBase(compName), m_size(0)
  {

  }

  void UartFramer ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    UartFramerComponentBase::init(instance);
  }

  UartFramer ::
    ~UartFramer()
  {

  }

  void UartFramer::allocate(NATIVE_UINT_TYPE number, NATIVE_UINT_TYPE size) {

    FW_ASSERT(size > 0, size);
    this->m_size = size;

    Fw::Buffer buff;
    // request a buffer and pass it on to the UART for each requested
    for (NATIVE_UINT_TYPE buffNum = 0; buffNum < number; buffNum++) {
      buff = this->DeframerAllocate_out(0,this->m_size);
      FW_ASSERT(buff.getSize() == size,buff.getSize(),size);
      FW_ASSERT(buff.getData());
      this->readBufferSend_out(0,buff);
    }

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  Drv::SendStatus UartFramer ::
    Framer_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &sendBuffer
    )
  {
    this->serialSend_out(0,sendBuffer);
    this->FramerDeallocate_out(0,sendBuffer);
    // no status from send, so return OK
    return Drv::SendStatus::SEND_OK;
  }

  void UartFramer ::
    serialRecv_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer,
        Drv::SerialReadStatus &status
    )
  {

      Drv::RecvStatus outStat = Drv::RecvStatus::RECV_OK;
      // Check the UART status
      if (status != Drv::SerialReadStatus::SER_OK) {
        outStat = Drv::RecvStatus::RECV_ERROR;
      }

      // Forward buffer to deframer
      this->Deframer_out(0,serBuffer,outStat);

      // allocate a replacement buffer and send it to
      // the UART driver
      Fw::Buffer newBuff = this->DeframerAllocate_out(0,this->m_size);
      if (newBuff.getSize() != this->m_size) {
        this->log_WARNING_HI_BuffErr();
      } else {
        this->readBufferSend_out(0,newBuff);
      }
      
  }

} // end namespace Drv
