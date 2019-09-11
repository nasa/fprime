#include <Fw/Port/PortBase.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <stdio.h>

#if FW_PORT_TRACING
void setConnTrace(bool trace) {
    Fw::PortBase::setTrace(trace);
}

namespace Fw {
    bool PortBase::s_trace = false;
}

#endif // FW_PORT_TRACING

namespace Fw {

#if FW_OBJECT_NAMES == 1    
    PortBase::PortBase()
                :
                Fw::ObjBase(0),
                m_connObj(0)
#if FW_PORT_TRACING == 1     
                ,m_trace(false),
                m_override_trace(false)
#endif                
    {
        
    }
#else // no object names
    PortBase::PortBase()
                :
                Fw::ObjBase(),
                m_connObj(0)
#if FW_PORT_TRACING == 1     
                ,m_trace(false),
                m_override_trace(false)
#endif                
    {
        
    }
#endif
    
    PortBase::~PortBase(void) {
        
    }
    
    void PortBase::init(void) {
        ObjBase::init();
        
    }
    
    bool PortBase::isConnected(void) {
        return m_connObj == 0?false:true;
    }

#if FW_PORT_TRACING == 1    
    
    void PortBase::trace(void) {
        bool do_trace = false;

        if (this->m_override_trace) {
            if (this->m_trace) {
                do_trace = true;
            }
        } else if (PortBase::s_trace) {
            do_trace = true;
        }

        if (do_trace) {
#if FW_OBJECT_NAMES == 1
            Fw::Logger::logMsg("Trace: %s\n", (POINTER_CAST)this->m_objName, 0, 0, 0, 0, 0);
#else
            Fw::Logger::logMsg("Trace: %p\n", (POINTER_CAST)this, 0, 0, 0, 0, 0);
#endif
        }
    }

    void PortBase::setTrace(bool trace) {
        PortBase::s_trace = trace;
    }

    void PortBase::overrideTrace(bool override, bool trace) {
        this->m_override_trace = override;
        this->m_trace = trace;
    }

#endif // FW_PORT_TRACING
    
#if FW_OBJECT_NAMES == 1
#if FW_OBJECT_TO_STRING == 1
    void PortBase::toString(char* buffer, NATIVE_INT_TYPE size) {
        (void)snprintf(buffer, size, "Port: %s %s->(%s)", this->m_objName, this->m_connObj ? "C" : "NC",
                        this->m_connObj ? this->m_connObj->getObjName() : "None");
        // NULL terminate
        buffer[size-1] = 0;
    }
#endif // FW_OBJECT_TO_STRING
#endif // FW_OBJECT_NAMES
    
    
}

