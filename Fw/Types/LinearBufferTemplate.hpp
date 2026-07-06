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

    LinearBufferTemplate() = default;

    LinearBufferTemplate(const U8* args, FwSizeType size) {
        const SerializeStatus stat = this->setBuff(args, size);
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    LinearBufferTemplate(const LinearBufferTemplate& other) : LinearBufferBase() {
        const SerializeStatus stat = this->setBuff(other.getBuffAddr(), other.getSize());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    ~LinearBufferTemplate() override = default;

    LinearBufferTemplate& operator=(const LinearBufferTemplate& other) {
        if (this == &other) {
            return *this;
        }
        const SerializeStatus stat = this->setBuff(other.getBuffAddr(), other.getSize());
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
        return *this;
    }

    DEPRECATED(FwSizeType getBuffCapacity() const, "Use getCapacity() instead") { return this->getCapacity(); }

    FwSizeType getCapacity() const override { return sizeof this->m_bufferData; }

    U8* getBuffAddr() override { return this->m_bufferData; }

    const U8* getBuffAddr() const override { return this->m_bufferData; }

  private:
    U8 m_bufferData[MaxSize] = {};
};

}  // namespace Fw

#endif
