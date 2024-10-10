/*
 * ActiveComponentBase.hpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tcanham
 */

/*
 * Description:
 */
#ifndef FW_QUEUED_COMPONENT_BASE_HPP
#define FW_QUEUED_COMPONENT_BASE_HPP

#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Os/Queue.hpp>
#include <Os/Task.hpp>
#include <FpConfig.hpp>


namespace Fw {
    class QueuedComponentBase : public PassiveComponentBase {
        public:

    		// Note: Had to make MsgDispatchStatus public for LLVM.
    		typedef enum {
				MSG_DISPATCH_OK, //!< Dispatch was normal
				MSG_DISPATCH_EMPTY, //!< No more messages in the queue
				MSG_DISPATCH_ERROR, //!< Errors dispatching messages
				MSG_DISPATCH_EXIT //!< A message was sent requesting an exit of the loop
			} MsgDispatchStatus;

        PROTECTED:
            QueuedComponentBase(const char* name); //!< Constructor
            virtual ~QueuedComponentBase(); //!< Destructor
            void init(NATIVE_INT_TYPE instance); //!< initialization function
            Os::Queue m_queue; //!< queue object for active component
            Os::Queue::Status createQueue(FwSizeType depth, FwSizeType msgSize);
            virtual MsgDispatchStatus doDispatch()=0; //!< method to dispatch a single message in the queue.
#if FW_OBJECT_TO_STRING == 1
            virtual void toString(char* str, NATIVE_INT_TYPE size); //!< dump string representation of component
#endif
            NATIVE_INT_TYPE getNumMsgsDropped(); //!< return number of messages dropped
            void incNumMsgDropped(); //!< increment the number of messages dropped
        PRIVATE:
            NATIVE_INT_TYPE m_msgsDropped; //!< number of messages dropped from full queue
    };

}
#endif
