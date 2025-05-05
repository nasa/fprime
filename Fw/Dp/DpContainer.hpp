// ======================================================================
// \title  DpContainer.hpp
// \author bocchino
// \brief  hpp file for DpContainer
// ======================================================================

#ifndef Fw_DpContainer_HPP
#define Fw_DpContainer_HPP

#include "Fw/Buffer/Buffer.hpp"
#include "Fw/Dp/DpStateEnumAc.hpp"
#include "Fw/Time/Time.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Utils/Hash/Hash.hpp"
#include "config/FppConstantsAc.hpp"
#include "config/ProcTypeEnumAc.hpp"

namespace Fw {

//! A data product Container
class DpContainer {
  public:
    // ----------------------------------------------------------------------
    // Constants and Types
    // ----------------------------------------------------------------------

    //! A DpContainer packet header
    struct Header {
        //! The type of user data
        using UserData = U8[DpCfg::CONTAINER_USER_DATA_SIZE];
        //! The offset for the packet descriptor field
        static constexpr FwSizeType PACKET_DESCRIPTOR_OFFSET = 0;
        //! The offset for the id field
        static constexpr FwSizeType ID_OFFSET = PACKET_DESCRIPTOR_OFFSET + sizeof(FwPacketDescriptorType);
        //! The offset for the priority field
        static constexpr FwDpPriorityType PRIORITY_OFFSET = ID_OFFSET + sizeof(FwDpIdType);
        //! The offset for the time tag field
        static constexpr FwSizeType TIME_TAG_OFFSET = PRIORITY_OFFSET + sizeof(FwDpPriorityType);
        //! The offset for the processing types field
        static constexpr FwSizeType PROC_TYPES_OFFSET = TIME_TAG_OFFSET + Time::SERIALIZED_SIZE;
        //! The offset for the user data field
        static constexpr FwSizeType USER_DATA_OFFSET = PROC_TYPES_OFFSET + sizeof(DpCfg::ProcType::SerialType);
        //! The offset of the data product state field
        static constexpr FwSizeType DP_STATE_OFFSET = USER_DATA_OFFSET + DpCfg::CONTAINER_USER_DATA_SIZE;
        //! The offset for the data size field
        static constexpr FwSizeType DATA_SIZE_OFFSET = DP_STATE_OFFSET + DpState::SERIALIZED_SIZE;
        //! The header size
        static constexpr FwSizeType SIZE = DATA_SIZE_OFFSET + sizeof(FwSizeStoreType);
    };

    //! The header hash offset
    static constexpr FwSizeType HEADER_HASH_OFFSET = Header::SIZE;
    //! The data offset
    static constexpr FwSizeType DATA_OFFSET = HEADER_HASH_OFFSET + HASH_DIGEST_LENGTH;
    //! The minimum packet size
    //! Reserve space for the header, the header hash, and the data hash
    //! This is also the number of non-data bytes in the packet
    static constexpr FwSizeType MIN_PACKET_SIZE = Header::SIZE + 2 * HASH_DIGEST_LENGTH;

  public:
    // ----------------------------------------------------------------------
    // Constructors and destructors
    // ----------------------------------------------------------------------

    //! Constructor for initialized container
    DpContainer(FwDpIdType id,            //!< The container id
                const Fw::Buffer& buffer  //!< The buffer
    );

    //! Constructor for container with default initialization
    DpContainer();

    //! Destructor
    virtual ~DpContainer() {}

  protected:
    // ----------------------------------------------------------------------
    // Protected operators
    // ----------------------------------------------------------------------

    //! Copy assignment operator
    DpContainer& operator=(const DpContainer& src) = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the container id
    //! \return The id
    FwDpIdType getId() const { return this->m_id; }

    //! Get the data size
    //! \return The data size
    FwSizeType getDataSize() const { return this->m_dataSize; }

    //! Get the packet buffer
    //! \return The buffer
    Fw::Buffer getBuffer() const { return this->m_buffer; }

    //! Get the packet size corresponding to the data size
    FwSizeType getPacketSize() const { return getPacketSizeForDataSize(this->m_dataSize); }

    //! Get the priority
    //! \return The priority
    FwDpPriorityType getPriority() const { return this->m_priority; }

    //! Get the time tag
    //! \return The time tag
    Fw::Time getTimeTag() const { return this->m_timeTag; }

    //! Get the product state
    Fw::DpState getState() const { return this->m_dpState; }

    //! Get the processing types
    //! \return The processing types
    DpCfg::ProcType::SerialType getProcTypes() const { return this->m_procTypes; }

    //! Get the data product state
    DpState getDpState() const { return this->m_dpState; }

    //! Deserialize the header from the packet buffer
    //! Buffer must be valid, and its size must be at least MIN_PACKET_SIZE
    //! Before calling this function, you should call checkHeaderHash() to
    //! check the header hash
    //! \return The serialize status
    Fw::SerializeStatus deserializeHeader();

    //! Serialize the header into the packet buffer and update the header hash
    //! Buffer must be valid, and its size must be at least MIN_PACKET_SIZE
    void serializeHeader();

    //! Set the id
    void setId(FwDpIdType id  //!< The id
    ) {
        this->m_id = id;
    }

    //! Set the priority
    void setPriority(FwDpPriorityType priority  //!< The priority
    ) {
        this->m_priority = priority;
    }

    //! Set the time tag
    void setTimeTag(Fw::Time timeTag  //!< The time tag
    ) {
        this->m_timeTag = timeTag;
    }

    //! Set the processing types bit mask
    void setProcTypes(DpCfg::ProcType::SerialType procTypes  //!< The processing types
    ) {
        this->m_procTypes = procTypes;
    }

    //! Set the data product state
    void setDpState(DpState dpState  //!< The data product state
    ) {
        this->m_dpState = dpState;
    }

    //! Set the data size
    void setDataSize(FwSizeType dataSize  //!< The data size
    ) {
        this->m_dataSize = dataSize;
    }

    //! Set the packet buffer
    void setBuffer(const Buffer& buffer  //!< The packet buffer
    );

    //! Invalidate the packet buffer
    void invalidateBuffer() {
        this->m_buffer = Fw::Buffer();
        this->m_dataBuffer.clear();
        this->m_dataSize = 0;
    }

    //! Get the stored header hash
    //! \return The hash
    Utils::HashBuffer getHeaderHash() const;

    //! Compute the header hash from the header data
    //! \return The hash
    Utils::HashBuffer computeHeaderHash() const;

    //! Set the header hash
    void setHeaderHash(const Utils::HashBuffer& hash  //!< The hash
    );

    //! Compute and set the header hash
    void updateHeaderHash();

    //! Check the header hash
    Success::T checkHeaderHash(Utils::HashBuffer& storedHash,   //!< The stored hash (output)
                               Utils::HashBuffer& computedHash  //!< The computed hash (output)
    ) const;

    //! Get the data hash offset
    FwSizeType getDataHashOffset() const {
        // Data hash goes after the header, the header hash, and the data
        return Header::SIZE + HASH_DIGEST_LENGTH + this->m_dataSize;
    }

    //! Get the stored data hash
    //! \return The hash
    Utils::HashBuffer getDataHash() const;

    //! Compute the data hash from the data
    //! \return The hash
    Utils::HashBuffer computeDataHash() const;

    //! Set the data hash
    void setDataHash(Utils::HashBuffer hash  //!< The hash
    );

    //! Update the data hash
    void updateDataHash();

    //! Check the data hash
    Success::T checkDataHash(Utils::HashBuffer& storedHash,   //!< The stored hash (output)
                             Utils::HashBuffer& computedHash  //!< The computed hash (output)
    ) const;

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the packet size for a given data size
    static constexpr FwSizeType getPacketSizeForDataSize(FwSizeType dataSize  //!< The data size
    ) {
        return Header::SIZE + dataSize + 2 * HASH_DIGEST_LENGTH;
    }

  PRIVATE:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! Initialize the user data field
    void initUserDataField();

  public:
    // ----------------------------------------------------------------------
    // Public member variables
    // ----------------------------------------------------------------------

    //! The user data
    Header::UserData m_userData;

  PROTECTED:
    // ----------------------------------------------------------------------
    // Protected member variables
    // ----------------------------------------------------------------------

    //! The container id
    //! This is a system-global id (component-local id + component base id)
    FwDpIdType m_id;

    //! The priority
    FwDpPriorityType m_priority;

    //! The time tag
    Time m_timeTag;

    //! The processing types
    DpCfg::ProcType::SerialType m_procTypes;

    //! The data product state
    DpState m_dpState;

    //! The data size
    FwSizeType m_dataSize;

    //! The packet buffer
    Buffer m_buffer;

    //! The data buffer
    //! We use member copy semantics because m_dataBuffer points into m_buffer,
    //! which is owned by this object
    Fw::ExternalSerializeBufferWithMemberCopy m_dataBuffer;
};

}  // end namespace Fw

#endif
