#include <Fw/Port/InputSerializePort.hpp>
#include <Fw/Types/Assert.hpp>
#include <stdio.h>

#if FW_PORT_SERIALIZATION == 1

namespace Fw {

    // SerializePort has no call interface. It is to pass through serialized data
    InputSerializePort::InputSerializePort() : InputPortBase(), m_func(0) {
    }
    InputSerializePort::~InputSerializePort() {
    }
    
    void InputSerializePort::init(void) {
        InputPortBase::init();
        
    }

    void InputSerializePort::invokeSerial(SerializeBufferBase &buffer) {
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp,this->m_portNum,buffer);
    }

    void InputSerializePort::addCallComp(Fw::PassiveComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);
        this->m_comp = callComp;
        this->m_func = funcPtr;
    }

#if FW_OBJECT_TO_STRING == 1
    void InputSerializePort::toString(char* buffer, NATIVE_INT_TYPE size) {
#if FW_OBJECT_NAMES == 1        
        (void)snprintf(buffer, size, "Input Serial Port: %s %s->(%s)", this->m_objName, this->isConnected() ? "C" : "NC",
                        this->isConnected() ? this->m_connObj->getObjName() : "None");
        buffer[size-1] = 0;
#else
        (void)snprintf(buffer,size,"%s","InputSerializePort");
#endif
    }
#endif

}
#endif
