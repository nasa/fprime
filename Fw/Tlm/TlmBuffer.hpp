/*
 * TlmBuffer.hpp
 *
 *      Author: tcanham
 */

/*
 * Description:
 * This object contains the TlmBuffer type, used for storing telemetry
 */
#ifndef FW_TLM_BUFFER_HPP
#define FW_TLM_BUFFER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/SerIds.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

class TlmBuffer final : public SerializeBufferBase {
  public:
    enum { SERIALIZED_TYPE_ID = FW_TYPEID_TLM_BUFF, SERIALIZED_SIZE = FW_TLM_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType) };

    TlmBuffer(const U8* args, FwSizeType size);
    TlmBuffer();
    TlmBuffer(const TlmBuffer& other);
    virtual ~TlmBuffer();
    TlmBuffer& operator=(const TlmBuffer& other);

    FwSizeType getBuffCapacity() const;  // !< returns capacity, not current size, of buffer
    U8* getBuffAddr();
    const U8* getBuffAddr() const;

  private:
    U8 m_bufferData[FW_TLM_BUFFER_MAX_SIZE];  // command argument buffer
};

}  // namespace Fw

#endif
