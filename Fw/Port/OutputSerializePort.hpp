#ifndef FW_OUTPUT_SERIALIZE_PORT_HPP
#define FW_OUTPUT_SERIALIZE_PORT_HPP

#include <FpConfig.hpp>

#if FW_PORT_SERIALIZATION == 1

#include <Fw/Port/OutputPortBase.hpp>

namespace Fw {

    class OutputSerializePort : public OutputPortBase {
        public:
            OutputSerializePort();
            virtual ~OutputSerializePort();
            void init() override;

        protected:
#if FW_OBJECT_TO_STRING == 1
            const char* getToStringFormatString() override; //!< Get format string for toString call
#endif

        private:

            OutputSerializePort(OutputSerializePort*);
            OutputSerializePort(OutputSerializePort&);
            OutputSerializePort& operator=(OutputSerializePort&);
    };

}

#endif // FW_OUTPUT_SERIALIZE_PORT_HPP

#endif
