/**
 * \file
 * \author T. Canham
 * \brief Class declaration for a simple queue registry
 *
 * The simple object registry is meant to give a default implementation
 * and an example of an queue registry. When the registry is instantiated,
 * it registers itself with the setQueueRegistry() static method. When
 * queues in the system are instantiated, the will register themselves.
 * The registry can then query the instances about their names, sizes,
 * and high watermarks.
 *
 * \copyright
 * Copyright 2013-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged. Any commercial use must be negotiated with the Office
 * of Technology Transfer at the California Institute of Technology.
 *
 * This software may be subject to U.S. export control laws and
 * regulations.  By accepting this document, the user agrees to comply
 * with all U.S. export laws and regulations.  User has the
 * responsibility to obtain export licenses, or other export authority
 * as may be required before exporting such information to foreign
 * countries or providing access to foreign persons.
 */

#ifndef SIMPLEQUEUEREGISTRY_HPP_
#define SIMPLEQUEUEREGISTRY_HPP_

#if FW_QUEUE_REGISTRATION

#include <Os/Queue.hpp>

namespace Os {

    class SimpleQueueRegistry: public QueueRegistry {
        public:
            SimpleQueueRegistry(); //!< constructor
            virtual ~SimpleQueueRegistry(); //!< destructor
            void regQueue(Queue* obj); //!< method called by queue init() methods to register a new queue
            void dump(void); //!< dump list of queues and stats
    };

} /* namespace Os */

#endif // FW_QUEUE_REGISTRATION

#endif /* SIMPLEQUEUEREGISTRY_HPP_ */
