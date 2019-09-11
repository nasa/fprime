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
#include <Os/Task.hpp>
#include <Fw/Cfg/Config.hpp>


namespace Fw {
    class ActiveComponentBase : public QueuedComponentBase {
        public:
            void start(NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, NATIVE_INT_TYPE cpuAffinity = -1); //!< called by instantiator when task is to be started
            void exit(void); //!< exit task in active component
            Os::Task::TaskStatus join(void **value_ptr); //!< provide return value of thread if value_ptr is not NULL

            enum {
                ACTIVE_COMPONENT_EXIT //!< message to exit active component task
            };

        PROTECTED:
#if FW_OBJECT_NAMES == 1
            ActiveComponentBase(const char* name); //!< Constructor
#else
            ActiveComponentBase(); //!< Constructor
#endif
            virtual ~ActiveComponentBase(); //!< Destructor
            void init(NATIVE_INT_TYPE instance); //!< initialization code
            virtual void preamble(void); //!< A function that will be called before the event loop is entered
            virtual void loop(void); //!< The function that will loop dispatching messages
            virtual void finalizer(void); //!< A function that will be called after exiting the loop
            Os::Task m_task; //!< task object for active component
#if FW_OBJECT_TO_STRING == 1
            virtual void toString(char* str, NATIVE_INT_TYPE size); //!< create string description of component
#endif
        PRIVATE:
            static void s_baseTask(void*); //!< function provided to task class for new thread.
            static void s_baseBareTask(void*); //!< function provided to task class for new thread.
    };

}
#endif
