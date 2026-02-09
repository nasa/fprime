// ======================================================================
// \title  Tlv.cpp
// \author campuzan
// \brief  cpp file for CFDP TLV (Type-Length-Value) classes
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/Tlv.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstring>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ======================================================================
// TlvData
// ======================================================================

TlvData::TlvData() : m_dataLength(0) {
    // Initialize union to zero
    memset(this->m_rawData, 0, sizeof(this->m_rawData));
}

void TlvData::setEntityId(EntityId eid) {
    this->m_eid = eid;
    // Entity ID length depends on the value
    // For now, use sizeof(EntityId) as the length
    this->m_dataLength = sizeof(EntityId);
}

void TlvData::setData(const U8* data, U8 length) {
    FW_ASSERT(length <= 255, length);
    FW_ASSERT(data != nullptr);

    memcpy(this->m_rawData, data, length);
    this->m_dataLength = length;
}

EntityId TlvData::getEntityId() const {
    return this->m_eid;
}

const U8* TlvData::getData() const {
    return this->m_rawData;
}

U8 TlvData::getLength() const {
    return this->m_dataLength;
}

// ======================================================================
// Tlv
// ======================================================================

Tlv::Tlv() : m_type(TLV_TYPE_ENTITY_ID) {
    // Default constructor
}

void Tlv::initialize(EntityId eid) {
    this->m_type = TLV_TYPE_ENTITY_ID;
    this->m_data.setEntityId(eid);
}

void Tlv::initialize(TlvType type, const U8* data, U8 length) {
    this->m_type = type;
    this->m_data.setData(data, length);
}

TlvType Tlv::getType() const {
    return this->m_type;
}

const TlvData& Tlv::getData() const {
    return this->m_data;
}

U32 Tlv::getEncodedSize() const {
    // Type (1 byte) + Length (1 byte) + Data (variable)
    return 2 + this->m_data.getLength();
}

Fw::SerializeStatus Tlv::toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const {
    Fw::SerializeStatus status;

    // Serialize type byte
    status = serialBuffer.serializeFrom(static_cast<U8>(this->m_type));
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Serialize length byte
    U8 length = this->m_data.getLength();
    status = serialBuffer.serializeFrom(length);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Serialize data
    if (this->m_type == TLV_TYPE_ENTITY_ID) {
        // For Entity ID, serialize as EntityId
        EntityId eid = this->m_data.getEntityId();
        status = serialBuffer.serializeFrom(eid);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    } else {
        // For other types, serialize raw data
        const U8* data = this->m_data.getData();
        for (U8 i = 0; i < length; i++) {
            status = serialBuffer.serializeFrom(data[i]);
            if (status != Fw::FW_SERIALIZE_OK) {
                return status;
            }
        }
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Tlv::fromSerialBuffer(Fw::SerialBufferBase& serialBuffer) {
    Fw::SerializeStatus status;

    // Deserialize type byte
    U8 typeVal;
    status = serialBuffer.deserializeTo(typeVal);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    this->m_type = static_cast<TlvType>(typeVal);

    // Deserialize length byte
    U8 length;
    status = serialBuffer.deserializeTo(length);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Deserialize data
    if (this->m_type == TLV_TYPE_ENTITY_ID) {
        // For Entity ID, deserialize as EntityId
        EntityId eid;
        status = serialBuffer.deserializeTo(eid);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
        this->m_data.setEntityId(eid);
    } else {
        // For other types, deserialize raw data
        U8 rawData[256];
        for (U8 i = 0; i < length; i++) {
            status = serialBuffer.deserializeTo(rawData[i]);
            if (status != Fw::FW_SERIALIZE_OK) {
                return status;
            }
        }
        this->m_data.setData(rawData, length);
    }

    return Fw::FW_SERIALIZE_OK;
}

// ======================================================================
// TlvList
// ======================================================================

TlvList::TlvList() : m_numTlv(0) {
    // Default constructor
}

bool TlvList::appendTlv(const Tlv& tlv) {
    if (this->m_numTlv >= CFDP_MAX_TLV) {
        return false;  // List is full
    }

    this->m_tlvs[this->m_numTlv] = tlv;
    this->m_numTlv++;
    return true;
}

void TlvList::clear() {
    this->m_numTlv = 0;
}

U8 TlvList::getNumTlv() const {
    return this->m_numTlv;
}

const Tlv& TlvList::getTlv(U8 index) const {
    FW_ASSERT(index < this->m_numTlv, index, this->m_numTlv);
    return this->m_tlvs[index];
}

U32 TlvList::getEncodedSize() const {
    U32 size = 0;
    for (U8 i = 0; i < this->m_numTlv; i++) {
        size += this->m_tlvs[i].getEncodedSize();
    }
    return size;
}

Fw::SerializeStatus TlvList::toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const {
    Fw::SerializeStatus status;

    // Encode all TLVs
    for (U8 i = 0; i < this->m_numTlv; i++) {
        status = this->m_tlvs[i].toSerialBuffer(serialBuffer);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus TlvList::fromSerialBuffer(Fw::SerialBufferBase& serialBuffer) {
    Fw::SerializeStatus status;

    // Clear existing TLVs
    this->m_numTlv = 0;

    // Decode TLVs until buffer is exhausted or max count reached
    while (serialBuffer.getDeserializeSizeLeft() > 0 && this->m_numTlv < CFDP_MAX_TLV) {
        status = this->m_tlvs[this->m_numTlv].fromSerialBuffer(serialBuffer);
        if (status != Fw::FW_SERIALIZE_OK) {
            // If we fail to decode a TLV, stop (could be end of buffer or invalid data)
            // Only return error if we haven't successfully decoded any TLVs yet
            if (this->m_numTlv == 0) {
                return status;
            } else {
                // We've decoded some TLVs successfully, so consider this a success
                break;
            }
        }
        this->m_numTlv++;
    }

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
