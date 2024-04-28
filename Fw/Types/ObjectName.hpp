// ======================================================================
// @file   ObjectName.hpp
// @author F Prime
// @brief  A string sized to store an object name
// ======================================================================

#ifndef FW_OBJECT_NAME_HPP
#define FW_OBJECT_NAME_HPP

#include <FpConfig.hpp>

#include "Fw/Cfg/SerIds.hpp"
#include "Fw/Types/StringBase.hpp"

namespace Fw {

class ObjectName final : public StringBase {
  public:
    enum {
        SERIALIZED_TYPE_ID = FW_TYPEID_OBJECT_NAME,
        STRING_SIZE = FW_OBJ_NAME_MAX_SIZE,
        SERIALIZED_SIZE = STRING_SIZE + sizeof(FwSizeStoreType)
    };

    ObjectName() : StringBase() { *this = ""; }

    ObjectName(const ObjectName& src) : StringBase() { *this = src; }

    ObjectName(const StringBase& src) : StringBase() { *this = src; }

    ObjectName(const char* src) : StringBase() { *this = src; }

    ~ObjectName() {}

    ObjectName& operator=(const ObjectName& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    ObjectName& operator=(const StringBase& src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    ObjectName& operator=(const char* src) {
        (void)StringBase::operator=(src);
        return *this;
    }

    const char* toChar() const { return this->m_buf; }

    StringBase::SizeType getCapacity() const { return sizeof this->m_buf; }

  private:
    char m_buf[ObjectName::STRING_SIZE];
};
}  // namespace Fw

#endif
