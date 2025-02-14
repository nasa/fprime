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

    void InputPortBase::setPortNum(FwIndexType portNum) {
        FW_ASSERT(portNum >= 0,portNum);
        this->m_portNum = portNum;
    }

#if FW_OBJECT_TO_STRING == 1
    const char* InputPortBase::getToStringFormatString() {
        return "Input Port: %s %s->(%s)";
    }
#endif


}

