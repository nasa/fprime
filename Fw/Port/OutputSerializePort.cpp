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
    void OutputSerializePort::toString(char* buffer, NATIVE_INT_TYPE size) {
#if FW_OBJECT_NAMES == 1
        FW_ASSERT(size > 0);
        if (snprintf(buffer, static_cast<size_t>(size), "Output Serial Port: %s %s->(%s)", this->m_objName.toChar(), this->isConnected() ? "C" : "NC",
                     this->isConnected() ? this->m_connObj->getObjName() : "None") < 0) {
            buffer[0] = 0;
        }
#else
        (void)snprintf(buffer,size,"%s","OutputSerializePort");
#endif
    }
#endif


}

#endif // FW_PORT_SERIALIZATION
