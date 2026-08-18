// ======================================================================
// \title  DataBuffer.hpp
// \author tumbar
// \brief  hpp file for DataBuffer implementation class
// ======================================================================

#ifndef FPPTEST_DATA_BUFFER_HPP
#define FPPTEST_DATA_BUFFER_HPP

#include "Fw/Types/Serializable.hpp"

namespace FppTest {
namespace FrameworkPortData {
class DataBuffer final : public Fw::Serializable {
  public:
    enum {
        SERIALIZED_SIZE = sizeof(U32) + sizeof(U8*),  //!< Size of Fw::Buffer when serialized
    };

    DataBuffer();
    explicit DataBuffer(const DataBuffer& buffer);
    DataBuffer(const Fw::LinearBufferBase& buffer);
    DataBuffer(const U8* data, FwSizeType size);

    bool operator==(const DataBuffer& src) const;
    DataBuffer& operator=(const DataBuffer& src);

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
    //! Supports writing this buffer to a string representation
    void toString(Fw::StringBase& text) const override;
#endif

#ifdef BUILD_UT
    //! Supports GTest framework for outputting this type to a stream
    //!
    friend std::ostream& operator<<(std::ostream& os, const DataBuffer& obj);
#endif

  private:
    U8 m_data[1024];
    FwSizeType m_size;
};
}  // namespace FrameworkPortData
}  // namespace FppTest

#endif  // FPPTEST_DATA_BUFFER_HPP
