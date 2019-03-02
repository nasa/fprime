/*
 * TaskRunner.hpp
 *
 *  Created on: Feb 28, 2019
 *      Author: lestarch
 */

#include <Os/Task.hpp>

#ifndef OS_BAREMETAL_TASKRUNNER_TASKRUNNER_HPP_
#define OS_BAREMETAL_TASKRUNNER_TASKRUNNER_HPP_

#define TASK_REGISTRY_CAP 100
namespace Os {
/**
 * Combination TaskRegistry and task runner. This does the "heavy lifting" for
 * baremetal running of tasks.
 */
class TaskRunner : TaskRegistry {
    public:
        //!< Nothing constructor
        TaskRunner(Task::taskRoutine callback);
        //!< Nothing destructor
        ~TaskRunner();
        /**
         * Add a task to the registry. These tasks will be run on a bare-metal
         * loop. The function used in this task may be overridden.
         * \param Task* task: task to be added
         */
        void addTask(Task* task);
        /**
         * Remove a task to the registry. These tasks will no-longer be run.
         * \param Task* task: task to be removed
         */
        void removeTask(Task* task);
        /**
         * Stop this task registry
         */
        void stop();
        /**
         * Program loop, used to run all tasks forever
         */
        void run();
    private:
        U32 m_index;
        Task* m_task_table[TASK_REGISTRY_CAP];
        Task::taskRoutine m_callback;
        bool m_cont;
};
} //End Namespace Os
#endif /* OS_BAREMETAL_TASKRUNNER_TASKRUNNER_HPP_ */
