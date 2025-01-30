#ifndef FW_INPUT_SERIALIZE_PORT_HPP
#define FW_INPUT_SERIALIZE_PORT_HPP

#include <FpConfig.hpp>

#if FW_PORT_SERIALIZATION == 1

#include <Fw/Port/InputPortBase.hpp>

namespace Fw {

    class InputSerializePort : public InputPortBase {
        public:
            InputSerializePort();
            virtual ~InputSerializePort();

            void init() override;

            SerializeStatus invokeSerial(SerializeBufferBase &buffer) override; // !< invoke the port with a serialized version of the call

            typedef void (*CompFuncPtr)(Fw::PassiveComponentBase* callComp, NATIVE_INT_TYPE portNum, SerializeBufferBase &arg); //!< port callback definition
            void addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr); //!< call to register a component

        protected:

#if FW_OBJECT_TO_STRING == 1
            const char* getToStringFormatString() override; //!< Get format string for toString call
#endif

        private:
            CompFuncPtr m_func; //!< pointer to port callback function
            InputSerializePort(InputSerializePort*);
            InputSerializePort(InputSerializePort&);
            InputSerializePort& operator=(InputSerializePort&);
    };

}

#endif // FW_INPUT_SERIALIZE_PORT_HPP

#endif
