#ifndef _Task_hpp_
#define _Task_hpp_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Os/TaskString.hpp>

namespace Os {
    
    class TaskRegistry; //!< forward declaration
    
    class Task {
        public:
        
            typedef enum {
                TASK_OK, //!< message sent/received okay
                TASK_INVALID_PARAMS, //!< started task with invalid parameters
                TASK_INVALID_STACK, //!< started with invalid stack size
                TASK_UNKNOWN_ERROR, //!< unexpected error return value
                TASK_INVALID_AFFINITY, //!< unable to set the task affinity
                TASK_DELAY_ERROR //!< error trying to delay the task
            } TaskStatus ;
            
            typedef void (*taskRoutine)(void* ptr); //!< prototype for task routine started in task context
        
            Task(); //!< constructor
            virtual ~Task(); //!< destructor
            // Priority is based on Posix priorities - 0 lowest, 255 highest
            TaskStatus start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity = -1); //!< start the task
            I32 getIdentifier(void); //!< get the identifier for the task
            
            static TaskStatus delay(NATIVE_UINT_TYPE msecs); //!< delay the task
            static NATIVE_INT_TYPE getNumTasks(void);
            
            void suspend(bool onPurpose = false); //!< suspend task
            void resume(void); //!< resume execution of task
            bool wasSuspended(void); //!< returns whether or not task was suspended on purpose
            bool isSuspended(void); //!< check with OS to see if it is suspended already
            bool isStarted(void); //!< check to see if task is started
            void setStarted(bool started); //!< set task to started when thread is fully up. Avoids a VxWorks race condition.
            /**
             * Returns the task-handle owned by this task
             */
            POINTER_CAST getRawHandle();

            static void registerTaskRegistry(TaskRegistry* registry);
            
        private:
            
            POINTER_CAST m_handle; //!< handle for implementation specific task
            NATIVE_INT_TYPE m_identifier; //!< thread independent identifer
            TaskString m_name; //!< object name
            NATIVE_INT_TYPE m_affinity; //!< CPU affinity for SMP targets

            void toString(char* buf, NATIVE_INT_TYPE buffSize); //!< print a string of the state of the task
            bool m_started; //!< set when task has reached entry point
            bool m_suspendedOnPurpose; //!< set when task was suspended in purpose (i.e. simulation)
            
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
