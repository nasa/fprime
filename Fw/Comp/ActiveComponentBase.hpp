/*
 * ActiveComponentBase.hpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tcanham
 */

/*
 * Description:
 */
#ifndef FW_ACTIVE_COMPONENT_BASE_HPP
#define FW_ACTIVE_COMPONENT_BASE_HPP

#include <Fw/Comp/QueuedComponentBase.hpp>
#include <Fw/Deprecate.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Os/Task.hpp>

namespace Fw {
class ActiveComponentBase : public QueuedComponentBase {
  public:
    void start(FwTaskPriorityType priority = Os::Task::TASK_PRIORITY_DEFAULT,
               FwSizeType stackSize = Os::Task::TASK_DEFAULT,
               FwSizeType cpuAffinity = Os::Task::TASK_DEFAULT,
               FwTaskIdType identifier = static_cast<FwTaskIdType>(
                   Os::Task::TASK_DEFAULT));  //!< called by instantiator when task is to be started
    void exit();                              //!< exit task in active component
    Os::Task::Status join();                  //!< Join the thread
    DEPRECATED(Os::Task::Status join(void** value_ptr),
               "Switch to .join()");  //!< Join to thread with discarded value_ptr

    enum {
        ACTIVE_COMPONENT_EXIT  //!< message to exit active component task
    };

  protected:
    //! Tracks the lifecycle of the component
    enum Lifecycle {
        CREATED,      //!< Initial stage, call preamble
        DISPATCHING,  //!< Component is dispatching messages
        FINALIZING,   //!< Penultimate stage, call finalizer
        DONE,         //!< Done, doing nothing
    };

    explicit ActiveComponentBase(const char* name);  //!< Constructor
    virtual ~ActiveComponentBase();                  //!< Destructor
    void init(FwEnumStoreType instance);             //!< initialization code
    virtual void preamble();       //!< A function that will be called before the event loop is entered
    MsgDispatchStatus dispatch();  //!< The function that will dispatching messages
    virtual void finalizer();      //!< A function that will be called after exiting the loop
    Os::Task m_task;               //!< task object for active component

#if FW_OBJECT_TO_STRING == 1
    virtual const char* getToStringFormatString();  //!< Format string for toString function
#endif
  private:
    Lifecycle m_stage;                      //!< Lifecycle stage of the component
    static void s_taskStateMachine(void*);  //!< Task lifecycle state machine
    static void s_taskLoop(void*);          //!< Standard multi-threading task loop
};

}  // namespace Fw
#endif
