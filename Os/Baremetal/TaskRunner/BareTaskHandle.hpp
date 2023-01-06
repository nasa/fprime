/*
 * BareTaskHandle.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: lestarch
 */
#ifndef OS_BAREMETAL_TASKRUNNER_BARETASKHANDLE_HPP_
#define OS_BAREMETAL_TASKRUNNER_BARETASKHANDLE_HPP_
#include <Os/Task.hpp>
namespace Os {
/**
 * Helper handle used to pretend to be a task.
 */
class BareTaskHandle {
    public:
        //!< Constructor sets enabled to false
        BareTaskHandle() : m_enabled(false), m_priority(0), m_routine(nullptr) {}
        //!< Is this task enabled or not
        bool m_enabled;
        //!< Save the priority
        NATIVE_INT_TYPE m_priority;
        //!< Function passed into the task
        Task::taskRoutine m_routine;
        //!< Argument input pointer
        void* m_argument;
};
}
#endif /* OS_BAREMETAL_TASKRUNNER_BARETASKHANDLE_HPP_ */
