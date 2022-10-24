#ifndef FW_INPUT_PORT_BASE_HPP
#define FW_INPUT_PORT_BASE_HPP

#include <FpConfig.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Port/PortBase.hpp>

namespace Fw {

    class InputPortBase : public PortBase {
        public:
            void setPortNum(NATIVE_INT_TYPE portNum); // !< set the port number

#if FW_PORT_SERIALIZATION
            virtual SerializeStatus invokeSerial(SerializeBufferBase &buffer) = 0; // !< invoke the port with a serialized version of the call
#endif

        protected:

            InputPortBase(); // Constructor
            virtual ~InputPortBase(); // Destructor
            virtual void init();

            PassiveComponentBase* m_comp; // !< pointer to containing component
            NATIVE_INT_TYPE m_portNum; // !< port number in containing object
#if FW_OBJECT_TO_STRING == 1
            virtual void toString(char* str, NATIVE_INT_TYPE size);
#endif

        private:

            // Disable constructors since we don't want to instantiate directly
            InputPortBase(InputPortBase*);
            InputPortBase(InputPortBase&);
            InputPortBase& operator=(InputPortBase&);

    };

}

#endif
