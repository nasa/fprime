#ifndef FW_OUTPUT_PORT_BASE_HPP
#define FW_OUTPUT_PORT_BASE_HPP

#include <FpConfig.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Port/InputPortBase.hpp>

namespace Fw {

    class OutputPortBase : public PortBase {
        public:
#if FW_PORT_SERIALIZATION == 1
            void registerSerialPort(InputPortBase* port); // !< register a port for serialized calls
            SerializeStatus invokeSerial(SerializeBufferBase &buffer); // !< invoke the port with a serialized version of the call
#endif

        protected:

            OutputPortBase(); // constructor
            virtual ~OutputPortBase(); // destructor
            virtual void init();

#if FW_OBJECT_TO_STRING == 1
            virtual void toString(char* str, NATIVE_INT_TYPE size);
#endif

#if FW_PORT_SERIALIZATION == 1
            Fw::InputPortBase* m_serPort; // !< pointer to port for serialized calls
#endif
        private:

            // Disable constructors
            OutputPortBase(OutputPortBase*);
            OutputPortBase(OutputPortBase&);
            OutputPortBase& operator=(OutputPortBase&);
    };

}

#endif
