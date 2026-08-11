// ======================================================================
// @file   LinearBufferTemplate.hpp
// @author F Prime
// @brief  A linear buffer template parameterized by size
// ======================================================================

#ifndef FW_LINEAR_BUFFER_TEMPLATE_HPP
#define FW_LINEAR_BUFFER_TEMPLATE_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

//! A linear buffer template parameterized by buffer size
//!
//! This class template provides a fixed-size linear serialization buffer
//! derived from LinearBufferBase. It replaces hand-coded concrete buffer
//! classes that share identical structure, eliminating boilerplate.
//!
//! \tparam MaxSize  Maximum buffer capacity in bytes
template <FwSizeType MaxSize>
class LinearBufferTemplate final : public LinearBufferBase {
  public:
    enum {
        SERIALIZED_SIZE = STATIC_SERIALIZED_SIZE(MaxSize),  //!< size when serialized: buffer + stored size
    };

    LinearBufferTemplate() : LinearBufferBase(m_bufferData, MaxSize) {}

    LinearBufferTemplate(const U8* args, FwSizeType size) : LinearBufferBase(m_bufferData, MaxSize) {
        const SerializeStatus stat = this->setBuff(args, size);
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    // m_bufferData contents are copied via setBuff below
    // cppcheck-suppress missingMemberCopy
    LinearBufferTemplate(const LinearBufferTemplate& other) : LinearBufferBase(m_bufferData, MaxSize) {
        const SerializeStatus stat = this->setBuff(other.m_bufferData, other.getSize());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    ~LinearBufferTemplate() override = default;

    LinearBufferTemplate& operator=(const LinearBufferTemplate& other) {
        if (this == &other) {
            return *this;
        }
        const SerializeStatus stat = this->setBuff(other.m_bufferData, other.getSize());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
        return *this;
    }

    DEPRECATED(FwSizeType getBuffCapacity() const, "Use getCapacity() instead") { return this->getCapacity(); }

  private:
    U8 m_bufferData[MaxSize];
};

}  // namespace Fw

#endif
