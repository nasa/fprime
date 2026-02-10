// ======================================================================
// \title  Tlv.hpp
// \author Brian Campuzano
// \brief  hpp file for CFDP TLV types
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_Tlv_HPP
#define Svc_Ccsds_Cfdp_Tlv_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <config/EntityIdAliasAc.hpp>
#include <config/CfdpCfg.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// CFDP TLV Types
// Blue Book section 5.4, table 5-3
enum TlvType : U8 {
    TLV_TYPE_FILESTORE_REQUEST = 0,       // Filestore request
    TLV_TYPE_FILESTORE_RESPONSE = 1,      // Filestore response
    TLV_TYPE_MESSAGE_TO_USER = 2,         // Message to user
    TLV_TYPE_FAULT_HANDLER_OVERRIDE = 4,  // Fault handler override
    TLV_TYPE_FLOW_LABEL = 5,              // Flow label
    TLV_TYPE_ENTITY_ID = 6                // Entity ID
};

//! TLV data storage
class TlvData {
  private:
    union {
        EntityId m_eid;      // Valid when type=ENTITY_ID
        U8 m_rawData[256];       // Valid for other types (max 255 bytes + null term)
    };
    U8 m_dataLength;             // Actual length of data

  public:
    TlvData();

    // Set entity ID (for TLV type 6)
    void setEntityId(EntityId eid);

    // Set raw data (for other TLV types)
    void setData(const U8* data, U8 length);

    // Get entity ID
    EntityId getEntityId() const;

    // Get raw data pointer
    const U8* getData() const;

    // Get data length
    U8 getLength() const;
};

//! Single TLV entry
class Tlv {
  private:
    TlvType m_type;
    TlvData m_data;

  public:
    Tlv();

    // Initialize with entity ID
    void initialize(EntityId eid);

    // Initialize with raw data
    void initialize(TlvType type, const U8* data, U8 length);

    // Getters
    TlvType getType() const;
    const TlvData& getData() const;

    // Compute encoded size
    U32 getEncodedSize() const;

    // Encode to SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;

    // Decode from SerialBuffer
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);
};

//! List of TLVs
class TlvList {
  private:
    U8 m_numTlv;
    Tlv m_tlvs[CFDP_MAX_TLV];

  public:
    TlvList();

    // Add a TLV (returns false if list is full)
    bool appendTlv(const Tlv& tlv);

    // Clear all TLVs
    void clear();

    // Get number of TLVs
    U8 getNumTlv() const;

    // Get TLV at index
    const Tlv& getTlv(U8 index) const;

    // Compute total encoded size of all TLVs
    U32 getEncodedSize() const;

    // Encode all TLVs to SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;

    // Decode all TLVs from SerialBuffer (reads until buffer exhausted)
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_Tlv_HPP
