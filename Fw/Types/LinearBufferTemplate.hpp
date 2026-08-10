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

    LinearBufferTemplate(const LinearBufferTemplate& other) : LinearBufferBase(m_bufferData, MaxSize) {
        const SerializeStatus stat = this->setBuff(other.m_bufferData, other.m_serLoc);
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
    }

    ~LinearBufferTemplate() override = default;

    LinearBufferTemplate& operator=(const LinearBufferTemplate& other) {
        if (this == &other) {
            return *this;
        }
        const SerializeStatus stat = this->setBuff(other.m_bufferData, other.m_serLoc);
        FW_ASSERT(FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
        return *this;
    }

    DEPRECATED(FwSizeType getBuffCapacity() const, "Use getCapacity() instead") { return this->getCapacity(); }

    //! \brief Repair the internal buffer address to point at this object's own storage
    //!
    //! LinearBufferBase stores m_buffAddr as a self-pointer into m_bufferData. Raw byte
    //! copies of this object (e.g. queue enqueue/dequeue via memcpy) leave m_buffAddr
    //! pointing at the source object's storage. This restores the invariant that
    //! m_buffAddr references this instance's m_bufferData; it must be called after any
    //! such raw relocation before the buffer is used.
    void recomputeBuffAddr() { this->m_buffAddr = this->m_bufferData; }

  private:
    U8 m_bufferData[MaxSize];
};

}  // namespace Fw

#endif
