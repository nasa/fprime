#ifndef _Task_hpp_
#define _Task_hpp_

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Os/TaskString.hpp>

#include <Os/TaskId.hpp>
#include <Fw/Deprecate.hpp>
#include <limits>

namespace Os {

    class TaskRegistry; //!< forward declaration
    class Task {
        public:
            static const NATIVE_UINT_TYPE TASK_DEFAULT;
            typedef enum {
                TASK_OK, //!< message sent/received okay
                TASK_INVALID_PARAMS, //!< started task with invalid parameters
                TASK_INVALID_STACK, //!< started with invalid stack size
                TASK_UNKNOWN_ERROR, //!< unexpected error return value
                TASK_INVALID_AFFINITY, //!< unable to set the task affinity
                TASK_DELAY_ERROR, //!< error trying to delay the task
                TASK_JOIN_ERROR, //!< error trying to join the task
                TASK_ERROR_RESOURCES, //!< unable to allocate more tasks
                TASK_ERROR_PERMISSION, //!< permissions error setting-up tasks
            } TaskStatus ;

            typedef void (*taskRoutine)(void* ptr); //!< prototype for task routine started in task context

            struct TaskRoutineWrapper {
                taskRoutine routine; //!< contains the task entrypoint
                void* arg; //!< contains the task entrypoint pointer
            };

            Task(); //!< constructor
            virtual ~Task(); //!< destructor

            TaskStatus start(const Fw::StringBase &name, taskRoutine routine, void* arg, NATIVE_UINT_TYPE priority = TASK_DEFAULT, NATIVE_UINT_TYPE stackSize = TASK_DEFAULT,  NATIVE_UINT_TYPE cpuAffinity = TASK_DEFAULT, NATIVE_UINT_TYPE identifier = TASK_DEFAULT); //!< start the task

            // Deprecated: only the name, routine, and argument are **required** parameters.  This ordering of parameters is therefore inappropriate and will be removed in the future
            DEPRECATED(TaskStatus start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity = static_cast<NATIVE_INT_TYPE>(TASK_DEFAULT)),
                       "Please switch to start(Fw::StringBase &name, taskRoutine routine, void* arg, NATIVE_UINT_TYPE priority, NATIVE_UINT_TYPE stackSize, NATIVE_UINT_TYPE cpuAffinity, NATIVE_UINT_TYPE identifier)"); //!< start the task
            I32 getIdentifier(); //!< get the identifier for the task
            static TaskId getOsIdentifier(); //Gets the Os Task ID. Useful for passive components.

            static TaskStatus delay(NATIVE_UINT_TYPE msecs); //!< delay the task
            static NATIVE_INT_TYPE getNumTasks();

            TaskStatus join(void **value_ptr); //!< Wait for task to finish
            void suspend(bool onPurpose = false); //!< suspend task
            void resume(); //!< resume execution of task
            bool wasSuspended(); //!< returns whether or not task was suspended on purpose
            bool isSuspended(); //!< check with OS to see if it is suspended already
            bool isStarted(); //!< check to see if task is started
            void setStarted(bool started); //!< set task to started when thread is fully up. Avoids a VxWorks race condition.
            /**
             * Returns the task-handle owned by this task
             */
            POINTER_CAST getRawHandle();

            static void registerTaskRegistry(TaskRegistry* registry);

        private:

            POINTER_CAST m_handle; //!< handle for implementation specific task
            NATIVE_INT_TYPE m_identifier; //!< thread independent identifier
            TaskString m_name; //!< object name
            NATIVE_INT_TYPE m_affinity; //!< CPU affinity for SMP targets

            void toString(char* buf, NATIVE_INT_TYPE buffSize); //!< print a string of the state of the task
            bool m_started; //!< set when task has reached entry point
            bool m_suspendedOnPurpose; //!< set when task was suspended in purpose (i.e. simulation)
            TaskRoutineWrapper m_routineWrapper; //! Contains task entrypoint and argument for task wrapper

            static TaskRegistry* s_taskRegistry; //!< pointer to registered task
            static NATIVE_INT_TYPE s_numTasks; //!< stores the number of tasks created.

    };

    class TaskRegistry {
        public:
            TaskRegistry(); //!< constructor for task registry
            virtual ~TaskRegistry(); //!< destructor for task registry
            virtual void addTask(Task* task) = 0; //!< Add a task to the registry
            virtual void removeTask(Task* task) = 0; //!< remove a task from the registry

        private:
    };
}

#endif
