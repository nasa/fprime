// ======================================================================
// \title  ComQueue.hpp
// \author vbai
// \brief  hpp file for ComQueue component implementation class
// ======================================================================

#ifndef ComQueue_HPP
#define ComQueue_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Svc/ComQueue/ComQueueComponentAc.hpp>
#include <Utils/Types/Queue.hpp>
#include "Fw/Types/MemAllocator.hpp"
#include "Svc/ComQueue/FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------
static const NATIVE_INT_TYPE totalSize = ComQueueComSize + ComQueueBuffSize;

struct QueueData {
    NATIVE_UINT_TYPE depth;
    NATIVE_UINT_TYPE priority;
    NATIVE_UINT_TYPE index;
    NATIVE_UINT_TYPE msgSize;
};

struct QueueConfiguration {
    NATIVE_UINT_TYPE depth;
    NATIVE_UINT_TYPE priority;
};

enum SendState { READY, RETRY, WAITING };
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

    void configure(QueueConfiguration queueConfig[], NATIVE_UINT_TYPE configSize, Fw::MemAllocator& allocator);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufQueueIn
    //!
    void buffQueueIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                             Fw::Buffer& fwBuffer);

    //! Handler implementation for comQueueIn
    //!
    void comQueueIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                            Fw::ComBuffer& data,           /*!<Buffer containing packet data*/
                            U32 context                    /*!<Call context value; meaning chosen by user*/
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

    // ----------------------------------------------------------------------
    // Helper Functions
    // ----------------------------------------------------------------------
    void sendComBuffer(Fw::ComBuffer& comBuffer);
    void sendBuffer(Fw::Buffer& buffer);
    void retryQueue();
    void processQueue();
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    // List of queues matched to ports
    Types::Queue m_queues[totalSize];

    // Sorted list on order of priority, will have the indexes of queues
    QueueData m_prioritizedList[totalSize];
    QueueData m_lastEntry;

    // Store combuffer and buffer
    Fw::ComBuffer m_comBufferMessage;
    Fw::Buffer m_bufferMessage;

    // Flag that indicates whether messages to clear messages or not
    bool m_throttle[totalSize];

    // Keeps track of the send state on whether or not to send a message
    SendState m_state;
};

}  // end namespace Svc

#endif
