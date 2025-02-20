#include <Fw/Comp/PassiveComponentBase.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>

#include <Fw/Types/ExternalString.hpp>

namespace Fw {

    PassiveComponentBase::PassiveComponentBase(const char* name) : Fw::ObjBase(name), m_idBase(0), m_instance(0) {
    }

#if FW_OBJECT_TO_STRING == 1
    const char* PassiveComponentBase::getToStringFormatString() {
        return "Comp: %s";
    }

    void PassiveComponentBase::toString(char* buffer, NATIVE_INT_TYPE size) {
        FW_ASSERT(size > 0);
        FW_ASSERT(buffer != nullptr);
        Fw::FormatStatus status = Fw::ExternalString(buffer, static_cast<Fw::ExternalString::SizeType>(size)).format(
            this->getToStringFormatString(),
#if FW_OBJECT_NAMES == 1
            this->m_objName.toChar()
#else
            "UNKNOWN"
#endif
        );
        if (status != Fw::FormatStatus::SUCCESS) {
            buffer[0] = 0;
        }
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
