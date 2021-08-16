#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>

#include <stdio.h>

namespace Fw {

    PassiveComponentBase::PassiveComponentBase(const char* name) : Fw::ObjBase(name), m_idBase(0), m_instance(0) {
    }

#if FW_OBJECT_TO_STRING == 1 && FW_OBJECT_NAMES == 1
    void PassiveComponentBase::toString(char* buffer, NATIVE_INT_TYPE size) {
        FW_ASSERT(buffer);
        (void)snprintf(buffer, size, "Comp: %s", this->m_objName);
        // null terminate
        buffer[size-1] = 0;
    }
#endif

    PassiveComponentBase::~PassiveComponentBase() {
    }

    void PassiveComponentBase::init(NATIVE_INT_TYPE instance) {
        ObjBase::init();
        this->m_instance = instance;
    }

    NATIVE_INT_TYPE PassiveComponentBase::getInstance() const {
        return this->m_instance;
    }

    void PassiveComponentBase ::
      setIdBase(const U32 idBase)
    {
      this->m_idBase = idBase;
    }

    U32 PassiveComponentBase ::
      getIdBase() const
    {
      return this->m_idBase;
    }

}
