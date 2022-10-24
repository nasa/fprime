#include <Fw/Port/PortBase.hpp>
#include <FpConfig.hpp>
#include <Fw/Logger/Logger.hpp>
#include <cstdio>
#include "Fw/Types/Assert.hpp"

#if FW_PORT_TRACING
void setConnTrace(bool trace) {
    Fw::PortBase::setTrace(trace);
}

namespace Fw {
    bool PortBase::s_trace = false;
}

#endif // FW_PORT_TRACING

namespace Fw {

    PortBase::PortBase()
                :
                Fw::ObjBase(nullptr),
                m_connObj(nullptr)
#if FW_PORT_TRACING == 1
                ,m_trace(false),
                m_ovr_trace(false)
#endif
    {

    }

    PortBase::~PortBase() {

    }

    void PortBase::init() {
        ObjBase::init();

    }

    bool PortBase::isConnected() {
        return m_connObj == nullptr?false:true;
    }

#if FW_PORT_TRACING == 1

    void PortBase::trace() {
        bool do_trace = false;

        if (this->m_ovr_trace) {
            if (this->m_trace) {
                do_trace = true;
            }
        } else if (PortBase::s_trace) {
            do_trace = true;
        }

        if (do_trace) {
#if FW_OBJECT_NAMES == 1
            Fw::Logger::logMsg("Trace: %s\n", reinterpret_cast<POINTER_CAST>(this->m_objName), 0, 0, 0, 0, 0);
#else
            Fw::Logger::logMsg("Trace: %p\n", reinterpret_cast<POINTER_CAST>(this), 0, 0, 0, 0, 0);
#endif
        }
    }

    void PortBase::setTrace(bool trace) {
        PortBase::s_trace = trace;
    }

    void PortBase::ovrTrace(bool ovr, bool trace) {
        this->m_ovr_trace = ovr;
        this->m_trace = trace;
    }

#endif // FW_PORT_TRACING

#if FW_OBJECT_NAMES == 1
#if FW_OBJECT_TO_STRING == 1
    void PortBase::toString(char* buffer, NATIVE_INT_TYPE size) {
        FW_ASSERT(size > 0);
        if (snprintf(buffer, size, "Port: %s %s->(%s)", this->m_objName, this->m_connObj ? "C" : "NC",
                     this->m_connObj ? this->m_connObj->getObjName() : "None") < 0) {
            buffer[0] = 0;
        }
    }
#endif // FW_OBJECT_TO_STRING
#endif // FW_OBJECT_NAMES


}

