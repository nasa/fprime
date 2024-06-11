// ======================================================================
// \title  StreamCrossover.hpp
// \author ethanchee
// \brief  hpp file for StreamCrossover component implementation class
// ======================================================================

#ifndef StreamCrossover_HPP
#define StreamCrossover_HPP

#include "Drv/StreamCrossover/StreamCrossoverComponentAc.hpp"

namespace Drv {

  class StreamCrossover :
    public StreamCrossoverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object StreamCrossover
      //!
      StreamCrossover(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object StreamCrossover
      //!
      ~StreamCrossover();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for streamIn
      //!
      void streamIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &recvBuffer, 
          const Drv::RecvStatus &recvStatus 
      );


    };

} // end namespace Drv

#endif
