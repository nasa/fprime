#include <FpConfig.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/ExternalString.hpp>

namespace Fw {

#if FW_OBJECT_REGISTRATION == 1
    ObjRegistry* ObjBase::s_objRegistry = nullptr;
#endif

#if FW_OBJECT_NAMES == 1
    ObjBase::ObjBase(const char* objName) {
        if (nullptr == objName) {
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
        return this->m_objName.toChar();
    }

    void ObjBase::setObjName(const char* name) {
        this->m_objName = name;
    }
#if FW_OBJECT_TO_STRING == 1
    void ObjBase::toString(char* str, NATIVE_INT_TYPE size) {
        FW_ASSERT(size > 0);
        FW_ASSERT(str != nullptr);
        Fw::FormatStatus formatStatus = Fw::ExternalString(str, static_cast<Fw::ExternalString::SizeType>(size)).format("Obj: %s", this->m_objName.toChar());
        if (formatStatus != Fw::FormatStatus::SUCCESS) {
            str[0] = 0;
        }
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
