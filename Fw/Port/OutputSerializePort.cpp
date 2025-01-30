#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstdio>

#if FW_PORT_SERIALIZATION

namespace Fw {

    // SerializePort has no call interface. It is to pass through serialized data

    OutputSerializePort::OutputSerializePort() : OutputPortBase() {
    }

    OutputSerializePort::~OutputSerializePort() {
    }

    void OutputSerializePort::init() {
        OutputPortBase::init();
    }

#if FW_OBJECT_TO_STRING == 1
    const char* OutputSerializePort::getToStringFormatString() {
        return "Output Serial Port: %s %s->(%s)";
    }
#endif


}

#endif // FW_PORT_SERIALIZATION
