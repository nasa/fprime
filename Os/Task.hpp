// ======================================================================
// \title Os/Task.hpp
// \brief common function definitions for Os::Task
// ======================================================================
#ifndef Os_Task_hpp_
#define Os_Task_hpp_

#include <FpConfig.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Os/Os.hpp>
#include <Os/TaskString.hpp>
#include <Os/Mutex.hpp>

#include <Fw/Deprecate.hpp>
#include <limits>

namespace Os {

    // Forward declarations
    class TaskRegistry;

    //! Task handle representation
    class TaskHandle {};

    class TaskInterface {
        public:
        static constexpr FwSizeType TASK_DEFAULT = std::numeric_limits<FwSizeType>::max();
            enum Status {
                OP_OK, //!< message sent/received okay
                INVALID_HANDLE, //!< Task handle invalid
                INVALID_PARAMS, //!< started task with invalid parameters
                INVALID_STACK, //!< started with invalid stack size
                UNKNOWN_ERROR, //!< unexpected error return value
                INVALID_AFFINITY, //!< unable to set the task affinity
                DELAY_ERROR, //!< error trying to delay the task
                JOIN_ERROR, //!< error trying to join the task
                ERROR_RESOURCES, //!< unable to allocate more tasks
                ERROR_PERMISSION, //!< permissions error setting-up tasks
                INVALID_STATE, //!< Task is in an invalid state for the operation
            };

            enum SuspensionType {
                INTENTIONAL,
                UNINTENTIONAL
            };

            enum State {
                NOT_STARTED,
                STARTING,
                RUNNING,
                SUSPENDED_INTENTIONALLY,
                SUSPENDED_UNINTENTIONALLY,
                EXITED,
                UNKNOWN
            };

            //! Prototype for task routine started in task context
            typedef void (*taskRoutine)(void* ptr);

            class Arguments {
              public:
                //! \brief construct a set of arguments to start a task
                //!
                //! Construct a set of arguments to start a task. It is illegal to supply a task routine that is
                //! set to a nullptr.
                //!
                //! \param name: name of the task
                //! \param routine: routine to run as part of this task
                //! \param routine_argument: (optional) argument to supply to the task routine
                //! \param priority: (optional) priority of this task
                //! \param stackSize: (optional) size of stack supplied to this task
                //! \param cpuAffinity: (optional) cpu affinity of this task. TODO: fix this into an array
                //! \param identifier: (optional) identifier for this task
                Arguments(const Fw::StringBase& name, const taskRoutine routine,
                          void* const routine_argument = nullptr,
                          const FwSizeType priority = TASK_DEFAULT,
                          const FwSizeType stackSize = TASK_DEFAULT,
                          const FwSizeType cpuAffinity = TASK_DEFAULT,
                          const PlatformUIntType identifier = static_cast<PlatformUIntType>(TASK_DEFAULT));

              public:
                const Os::TaskString m_name;
                taskRoutine m_routine;
                void* m_routine_argument;
                FwSizeType m_priority;
                FwSizeType m_stackSize;
                FwSizeType m_cpuAffinity;
                PlatformUIntType m_identifier;
            };

            //! \brief default constructor
            TaskInterface() = default;

            //! \brief default virtual destructor
            virtual ~TaskInterface() = default;

            //! \brief copy constructor is forbidden
            TaskInterface(const TaskInterface& other) = delete;

            //! \brief assignment operator is forbidden
            TaskInterface& operator=(const TaskInterface& other) = delete;

            // =================
            // Implementation functions (static) to be supplied by the linker
            // =================

            //! \brief delay the current task
            //!
            //! Delays, or sleeps, the current task by the supplied time interval. In non-preempting os implementations
            //! the task will resume no earlier than expected but an exact wake-up time is not guaranteed.
            //!
            //! \param interval: delay time
            //! \return status of the delay
            static Status delay(Fw::Time interval);

            //! \brief provide a pointer to a task delegate object
            //!
            //! This function must return a pointer to a `TaskInterface` object that contains the real implementation of the file
            //! functions as defined by the implementor.  This function must do several things to be considered correctly
            //! implemented:
            //!
            //! 1. Assert that the supplied memory is non-null. e.g `FW_ASSERT(aligned_placement_new_memory != NULL);`
            //! 2. Assert that their implementation fits within FW_HANDLE_MAX_SIZE.
            //!    e.g. `static_assert(sizeof(PosixTaskImplementation) <= sizeof Os::Task::m_handle_storage,
            //!        "FW_HANDLE_MAX_SIZE to small");`
            //! 3. Assert that their implementation aligns within FW_HANDLE_ALIGNMENT.
            //!    e.g. `static_assert((FW_HANDLE_ALIGNMENT % alignof(PosixTaskImplementation)) == 0, "Bad handle alignment");`
            //! 4. Placement new their implementation into `aligned_placement_new_memory`
            //!    e.g. `TaskInterface* interface = new (aligned_placement_new_memory) PosixTaskImplementation;`
            //! 5. Return the result of the placement new
            //!    e.g. `return interface;`
            //!
            //! \return result of placement new, must be equivalent to `aligned_placement_new_memory`
            //!
            static TaskInterface* getDelegate(HandleStorage& aligned_placement_new_memory);

            // =================
            // Implementation functions (instance) to be supplied by the Os::TaskInterface children
            // =================

            //! \brief perform required task start actions
            virtual void onStart() = 0;

            //! \brief block until the task has ended
            //!
            //! Blocks the current (calling) task until this task execution has ended. Callers should ensure that any
            //! signals required to stop this task have already been emitted or will be emitted by another task.
            //!
            //! \return status of the block
            virtual Status join() = 0;

            //! \brief suspend the task given the suspension type
            //!
            //! Suspends the task. Some implementations track if the suspension of a task was intentional or
            //! unintentional. The supplied `suspensionType` parameter indicates that this was intentional or
            //! unintentional. The type of suspension is also returned when calling `isSuspended`.
            //!
            //! \param suspensionType intentionality of the suspension
            virtual void suspend(SuspensionType suspensionType) = 0;

            //! \brief resume a suspended task
            //!
            //! Resumes this task. Not started, running, and exited tasks take no action.
            //!
            virtual void resume() = 0;

            //! \brief determine if the task requires cooperative multitasking
            //!
            //! Some task implementations require cooperative multitasking where the task execution is run by a user
            //! defined task scheduler and not the operating system task scheduler. These tasks cooperatively on
            //! multitask by doing one unit of work and return from the function.
            //!
            //! This function indicates if the task requires cooperative support.
            //! The default implementation returns false.
            //!
            //! \return true when the task expects cooperation, false otherwise
            virtual bool isCooperative();

            //! \brief return the underlying task handle (implementation specific)
            //! \return internal task handle representation
            virtual TaskHandle* getHandle() = 0;

            //! \brief start the task
            //!
            //! Starts the task given the supplied arguments.
            //!
            //! \param arguments: arguments supplied to the task start call
            //! \return status of the task start
            virtual Status start(const Arguments& arguments) = 0;
    };

    //! Task class intended to be used by the rest of the fprime system. This is final as it is not intended to be a
    //! parent class. Instead it wraps a delegate provided by `TaskInterface::getDelegate()` to provide system specific
    //! behaviour.
    class Task final : public TaskInterface {
      public:
        //! Wrapper for task routine that ensures `onStart()` is called once the task actually begins
        class TaskRoutineWrapper {
          public:
            explicit TaskRoutineWrapper(Task& self);

            //! \brief run the task routine wrapper
            //!
            //! Sets the Os::Task to started via the setStarted method. Then runs the user function passing in the
            //! user argument.
            //! \param task_pointer: pointer to TaskRoutineWrapper being run
            static void run(void* task_pointer);

            //! \brief invoke the run method with "self" as argument
            void invoke();

            Task& m_task; //!< Reference to owning task
            taskRoutine m_user_function = nullptr; //!< User function to run once started
            void* m_user_argument = nullptr; //!<  Argument to user function
        };

        //! \brief backwards-compatible parameter type
        typedef FwSizeType ParamType;

        //! \brief default constructor
        Task();

        //! \brief default virtual destructor
        ~Task() final;

        //! \brief copy constructor is forbidden
        Task(const Task& other) = delete;

        //! \brief assignment operator is forbidden
        Task& operator=(const Task& other) = delete;

        //! \brief suspend the current task
        //!
        //! Suspend the current task unintentionally. If the user needs to indicate that the task was suspended
        //! intentionally then a call to `suspend(SuspensionType::INTENTIONAL)` should be used.
        void suspend();

        //! \brief get the task's state
        //!
        //! Returns the task state: not started, running, suspended (intentionally), suspended (unintentionally),
        //! and exited.
        //!
        //! \return task state
        State getState();

        //! \brief start this task
        //!
        //! Start this task with supplied name, task routine (run function), priority, stack, affinity, and task
        //! identifier. These arguments are supplied into an Arguments class and that version of the function is called.
        //! It is illegal to supply a nullptr as routine.
        //!
        //! \param name: name of the task to start
        //! \param routine: user routine to run
        //! \param arg: (optional) user argument to supply to task routine
        //! \param priority: (optional) priority of this task
        //! \param stackSize: (optional) stack size of this task
        //! \param cpuAffinity: (optional) affinity of this task. Use `Task::start(Arguments&)` to supply affinity set.
        //! \param identifier: (optional) identifier of this task
        //! \return: status of the start call
        DEPRECATED(Status start(const Fw::StringBase &name, const taskRoutine routine, void* const arg = nullptr,
                                const ParamType priority = TASK_DEFAULT,
                                const ParamType stackSize = TASK_DEFAULT,
                                const ParamType cpuAffinity = TASK_DEFAULT,
                                const ParamType identifier = TASK_DEFAULT), "Switch to Task::start(Arguments&)");

        //! \brief start the task
        //!
        //! Starts the task given the supplied arguments. This is done via the a task routine wrapper intermediary that
        //! ensures that `setStarted` is called once the task has actually started to run. The task then runs the user
        //! routine. This function may return before the new task begins to run.
        //
        //! It is illegal for arguments.m_routine to be null.
        //!
        //! \param arguments: arguments supplied to the task start call
        //! \return status of the task start
        Status start(const Arguments& arguments) override;

        //! \brief perform delegate's required task start actions
        void onStart() override;

        //! \brief invoke the task's routine
        //~
        //! This will invoke the task's routine passing this as the argument to that call. This is used as a helper when
        //! running this task (e.g. repetitive cooperative calls).
        void invokeRoutine();

        //! \brief join calling thread to this thread
        //!
        //! Note: this function is deprecated as the value_ptr object is not used anyway and should always be set
        //! to nullptr.
        //!
        //! \param value_ptr must be set to nullptr
        //! \return status of the join
        DEPRECATED(Status join(void** value_ptr), "Please switch to argument free join.");

        //! \brief block until the task has ended
        //!
        //! Blocks the current (calling) task until this task execution has ended. Callers should ensure that any
        //! signals required to stop this task have already been emitted or will be emitted by another task.
        //!
        //! \return status of the block
        Status join() override; //!< Wait for task to finish

        //! \brief suspend the task given the suspension type
        //!
        //! Suspends the task. Some implementations track if the suspension of a task was intentional or
        //! unintentional. The supplied `suspensionType` parameter indicates that this was intentional or
        //! unintentional. The type of suspension is also returned when calling `isSuspended`.
        //!
        //! \param suspensionType intentionality of the suspension
        void suspend(SuspensionType suspensionType) override;

        //! \brief resume a suspended task
        //!
        //! Resumes this task. Not started, running, and exited tasks take no action.
        //!
        void resume() override;

        //! \brief determine if the task is cooperative multitasking (implementation specific)
        //! \return true if cooperative, false otherwise
        bool isCooperative() override;

        //! \brief return the underlying task handle (implementation specific)
        //! \return internal task handle representation
        TaskHandle* getHandle() override;

        //! \brief get the current number of tasks
        //! \return current number of tasks
        static FwSizeType getNumTasks();

        //! \brief register a task registry to track Threads
        //!
        static void registerTaskRegistry(TaskRegistry* registry);

      PRIVATE:
        static TaskRegistry* s_taskRegistry; //!< Pointer to registered task registry
        static FwSizeType s_numTasks; //!< Stores the number of tasks created.
        static Mutex s_taskMutex; //!< Guards s_numTasks

        TaskString m_name; //!< Task object name
        TaskInterface::State m_state = Task::NOT_STARTED;
        Mutex m_lock; //!< Guards state transitions
        TaskRoutineWrapper m_wrapper; //!< Concrete storage for task routine wrapper

        bool m_registered = false; //!< Was this task registered

        // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
        // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
        // the byte-array here and set `handle` to that address for storage.
        //
        alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage; //!< Storage for aligned FileHandle data
        TaskInterface& m_delegate; //!< Delegate for the real implementation
    };

    class TaskRegistry {
        public:
            //! \brief default task registry constructor
            TaskRegistry() = default;
            //! \brief default task registry constructor
            virtual ~TaskRegistry() = default;
            //! \brief add supplied task to the registry
            //!
            //! \param task: pointer to task to register
            virtual void addTask(Task* task) = 0; //!< Add a task to the registry

            //! \brief remove supplied task to the registry
            //!
            //! \param task: pointer to task to deregister
            virtual void removeTask(Task* task) = 0;
    };
}

#endif
