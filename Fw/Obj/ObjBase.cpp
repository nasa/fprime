#include <FpConfig.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <string.h>
#include <stdio.h>

namespace Fw {

#if FW_OBJECT_REGISTRATION == 1
    ObjRegistry* ObjBase::s_objRegistry = 0;
#endif

#if FW_OBJECT_NAMES == 1
    ObjBase::ObjBase(const char* objName) {
        if (0 == objName) {
            this->setObjName("NoName");
        } else {
            this->setObjName(objName);
        }
    }
#else
    ObjBase::ObjBase(const char* objName) {

    }
#endif

    void ObjBase::init() {
#if FW_OBJECT_REGISTRATION
        if (ObjBase::s_objRegistry) {
            ObjBase::s_objRegistry->regObject(this);
        }
#endif
    }

    ObjBase::~ObjBase() {

    }

#if FW_OBJECT_NAMES == 1
    const char* ObjBase::getObjName() {
        return this->m_objName;
    }

    void ObjBase::setObjName(const char* name) {
        strncpy(this->m_objName, name, sizeof(this->m_objName));
        this->m_objName[sizeof(this->m_objName)-1] = 0;
    }
#if FW_OBJECT_TO_STRING == 1
    void ObjBase::toString(char* str, NATIVE_INT_TYPE size) {
        (void)snprintf(str, size, "Obj: %s",this->m_objName);
        str[size-1] = 0;
    }
#endif
#endif

#if FW_OBJECT_REGISTRATION == 1
    void ObjBase::setObjRegistry(ObjRegistry* reg) {
        ObjBase::s_objRegistry = reg;
    }

    ObjRegistry::~ObjRegistry() {
    }

#endif
} // Fw
