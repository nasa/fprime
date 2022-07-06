// ======================================================================
// \title  ComQueue.hpp
// \author vbai
// \brief  hpp file for ComQueue component implementation class
// ======================================================================

#ifndef ComQueue_HPP
#define ComQueue_HPP

#include "Svc/ComQueue/ComQueueComponentAc.hpp"

namespace Svc {

class ComQueue : public ComQueueComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComQueue
    //!
    ComQueue(const char* const compName /*!< The component name*/
    );

    //! Initialize object ComQueue
    //!
    void init(const NATIVE_INT_TYPE queueDepth,  /*!< The queue depth*/
              const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object ComQueue
    //!
    ~ComQueue();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufQueueIn
    //!
    void bufQueueIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                            Fw::Buffer& fwBuffer);

    //! Handler implementation for comQueueIn
    //!
    void comQueueIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                            Fw::ComBuffer& data, /*!<Buffer containing packet data*/
                            U32 context /*!<Call context value; meaning chosen by user*/
    );

    //! Handler implementation for comStatusIn
    //!
    void comStatusIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                             Svc::ComSendStatus& ComStatus  /*!<Status of communication state*/
    );

    //! Handler implementation for run
    //!
    void run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                     NATIVE_UINT_TYPE context       /*!<The call order*/
    );
};

}  // end namespace Svc

#endif
