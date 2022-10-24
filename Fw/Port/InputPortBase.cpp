#include <FpConfig.hpp>
#include <Fw/Port/InputPortBase.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstdio>

namespace Fw {

    InputPortBase::InputPortBase() :
                    PortBase(),
                    m_comp(nullptr),
                    m_portNum(-1) {
    }

    InputPortBase::~InputPortBase() {

    }

    void InputPortBase::init() {
        PortBase::init();

    }

    void InputPortBase::setPortNum(NATIVE_INT_TYPE portNum) {
        FW_ASSERT(portNum >= 0,portNum);
        this->m_portNum = portNum;
    }

#if FW_OBJECT_TO_STRING == 1
    void InputPortBase::toString(char* buffer, NATIVE_INT_TYPE size) {
#if FW_OBJECT_NAMES == 1 
        FW_ASSERT(size > 0);
        if (snprintf(buffer, size, "InputPort: %s->%s", this->m_objName,
                     this->isConnected() ? this->m_connObj->getObjName() : "None") < 0) {
            buffer[0] = 0;
        }
#else
        (void)snprintf(buffer,size,"%s","Unnamed Input port");
#endif
    }
#endif


}

