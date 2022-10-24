#ifndef FW_PORT_BASE_HPP
#define FW_PORT_BASE_HPP

#include <Fw/Obj/ObjBase.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>

#if FW_PORT_TRACING == 1
extern "C" {
    void setConnTrace(bool trace);
}
#endif

namespace Fw {

    class PortBase : public Fw::ObjBase {
        public:
#if FW_PORT_TRACING == 1
            static void setTrace(bool trace); // !< turn tracing on or off
            void ovrTrace(bool ovr, bool trace); // !< override tracing for a particular port
#endif

            bool isConnected();
        protected:
            // Should only be accessed by derived classes
            PortBase(); // Constructor
            virtual ~PortBase(); // Destructor
            virtual void init(); // !< initialization function

#if FW_PORT_TRACING == 1
            void trace(); // !<  trace port calls if active
#endif
            Fw::ObjBase* m_connObj; // !< object port is connected to

#if FW_OBJECT_TO_STRING
            virtual void toString(char* str, NATIVE_INT_TYPE size);
#endif


        private:
#if FW_PORT_TRACING == 1
            static bool s_trace; // !< global tracing is active
            bool m_trace; // !< local trace flag
            bool m_ovr_trace; // !< flag to override global trace
#endif
            // Disable constructors
            PortBase(PortBase*);
            PortBase(PortBase&);
            PortBase& operator=(PortBase&);

    };

}

#endif
