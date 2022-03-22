// ======================================================================
// \title  UartFramer.hpp
// \author tcanham
// \brief  hpp file for UartFramer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UartFramer_HPP
#define UartFramer_HPP

#include "Drv/UartFramer/UartFramerComponentAc.hpp"

namespace Drv {

  class UartFramer :
    public UartFramerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object UartFramer
      //!
      UartFramer(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object UartFramer
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object UartFramer
      //!
      ~UartFramer();


      //! Allocate pool of buffers for UART receive - BufferManager and UART
      //  instances must be connected and ready. BufferManager should have at least
      //  number+1 buffers allocated
      void allocate(NATIVE_UINT_TYPE number, NATIVE_UINT_TYPE size);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Framer
      //!
      Drv::SendStatus Framer_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &sendBuffer 
      );

      //! Handler implementation for serialRecv
      //!
      void serialRecv_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer, /*!< 
      Buffer containing data
      */
          Drv::SerialReadStatus &status /*!< 
      Status of read
      */
      );

      // Private data members

      NATIVE_UINT_TYPE m_size; //!< size of UART buffers


    };

} // end namespace Drv

#endif
