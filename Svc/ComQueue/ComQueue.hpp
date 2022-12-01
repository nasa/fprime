// ======================================================================
// \title  ComQueue.hpp
// \author vbai
// \brief  hpp file for ComQueue component implementation class
// ======================================================================

#ifndef Svc_ComQueue_HPP
#define Svc_ComQueue_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Svc/ComQueue/ComQueueComponentAc.hpp>
#include <Utils/Types/Queue.hpp>
#include "Fw/Types/MemAllocator.hpp"
#include "Os/Mutex.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------

class ComQueue : public ComQueueComponentBase {
  public:
    //!< Count of Fw::Com input ports and thus Fw::Com queues
    static const FwIndexType COM_PORT_COUNT = ComQueueComponentBase::NUM_COMQUEUEIN_INPUT_PORTS;

    //!< Count of Fw::Buffer input ports and thus Fw::Buffer queues
    static const FwIndexType BUFFER_PORT_COUNT = ComQueueComponentBase::NUM_BUFFQUEUEIN_INPUT_PORTS;

    //!< Total count of input buffer ports and thus total queues
    static const FwIndexType TOTAL_PORT_COUNT = COM_PORT_COUNT + BUFFER_PORT_COUNT;

    /**
     * \brief configuration data for each queue
     *
     * Each queue must be configured to specify the depth of the queue and the priority of the queue. Depth must be a
     * non-negative integer indicating the number of messages before overflow. A depth of 0 disables the given queue and
     * any message sent to it will overflow.
     *
     * Priority is an integer between 0 (inclusive) and TOTAL_PORT_COUNT (exclusive). Queues with lower priority values
     * will be serviced first. Priorities may be repeated and queues sharing priorities will be serviced in a balanced
     * manner.
     */
    struct QueueConfigurationEntry {
        FwSizeType depth;      //!< Depth of the queue [0, infinity)
        FwIndexType priority;  //!< Priority of the queue [0, TOTAL_PORT_COUNT)
    };

    /**
     * \brief configuration table for each queue
     *
     * This table should be filled-out and passed to the configure method of this component. It represents the
     * port-by-port configuration information for the associated queue. Each entry specifies the queue's depth and
     * priority.
     *
     * Entries are specified in-order first addressing Fw::Com ports then Fw::Buffer ports.
     */
    struct QueueConfigurationTable {
        QueueConfigurationEntry entries[TOTAL_PORT_COUNT];
        /**
         * \brief constructs a basic un-prioritized table with depth 0
         */
        QueueConfigurationTable();
    };

  private:
    // ----------------------------------------------------------------------
    // Internal data structures
    // ----------------------------------------------------------------------

    /**
     * Storage for internal queue metadata. This is stored in the prioritized list and contains indices to the the
     * un-prioritized queue objects. Depth and priority is copied from the configuration supplied by the configure
     * method. Index and message size are calculated by the configuration call.
     */
    struct QueueMetadata {
        FwSizeType depth;      //!< Depth of the queue in messages
        FwIndexType priority;  //!< Priority of the queue
        FwIndexType index;     //!< Index of this queue in the prioritized list
        FwSizeType msgSize;    //!< Message size of messages in this queue
    };

    /**
     * State of the component.
     */
    enum SendState {
        READY,   //!< Component is ready to send next priority message
        WAITING  //!< Component is waiting for status of the last sent message
    };

  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComQueue
    //!
    ComQueue(const char* const compName /*!< The component name */
    );

    //! Initialize object ComQueue
    //!
    void init(const NATIVE_INT_TYPE queueDepth,  /*!< The queue depth */
              const NATIVE_INT_TYPE instance = 0 /*!< The instance number */
    );

    //! Destroy object ComQueue
    //!
    ~ComQueue();

    //! Configure the queue depths, priorities, and memory allocation for the component
    //!
    //! Takes in the queue depth and priority per-port in order from Fw::Com through Fw::Buffer ports. Calculates the
    //! queue metadata stored `m_prioritizedList` and then sorts that list by priority.
    void configure(QueueConfigurationTable queueConfig,  //!< Table of the configuration properties for the component
                   NATIVE_UINT_TYPE allocationId,        //!< Identifier used  when dealing with the Fw::MemAllocator
                   Fw::MemAllocator& allocator           //!< Fw::MemAllocator used to acquire memory
    );

    //! Deallocate resources and cleanup ComQueue
    //!
    void cleanup();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Receive and queue a Fw::Buffer
    //!
    void buffQueueIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                             Fw::Buffer& fwBuffer /*!< Buffer containing packet data*/);

    //! Receive and queue a Fw::ComBuffer
    //!
    void comQueueIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                            Fw::ComBuffer& data,           /*!< Buffer containing packet data*/
                            U32 context                    /*!< Call context value; meaning chosen by user*/
    );

    //! Handle the status of the last sent message
    //!
    void comStatusIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                             Fw::Success& condition         /*!<Status of communication state*/
    );

    //! Schedules the transmission of telemetry
    //!
    void run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                     NATIVE_UINT_TYPE context       /*!<The call order*/
    );

    // ----------------------------------------------------------------------
    // Helper Functions
    // ----------------------------------------------------------------------

    //! Enqueues a message on the appropriate queue
    //!
    void enqueue(const FwIndexType queueNum,  //!< Index of the queue to enqueue the message
                 QueueType queueType,         //!< Type of the queue and message data
                 const U8* data,              //!< Pointer to the message data
                 const FwSizeType size        //!< Size of the message
    );

    //! Send a chosen Fw::ComBuffer
    //!
    void sendComBuffer(Fw::ComBuffer& comBuffer  //!< Reference to buffer to send
    );

    //! Send a chosen Fw::Buffer
    //!
    void sendBuffer(Fw::Buffer& buffer  //!< Reference to buffer to send
    );

    //! Process the queues to select the next priority message
    //!
    void processQueue();
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    Types::Queue m_queues[TOTAL_PORT_COUNT];  //!< Stores queued data waiting for transmission
    QueueMetadata m_prioritizedList[TOTAL_PORT_COUNT];  //!< Priority sorted list of queue metadata
    bool m_throttle[TOTAL_PORT_COUNT];  //!< Per-queue EVR throttles
    SendState m_state;  //!< State of the component

    // Storage for Fw::MemAllocator properties
    NATIVE_UINT_TYPE m_allocationId;  //!< Component's allocation ID
    Fw::MemAllocator* m_allocator;    //!< Pointer to Fw::MemAllocator instance for deallocation
    void* m_allocation;               //!< Pointer to allocated memory
};

}  // end namespace Svc

#endif
