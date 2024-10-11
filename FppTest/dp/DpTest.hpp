// ======================================================================
// \title  DpTest.hpp
// \author bocchino
// \brief  hpp file for DpTest component implementation class
// ======================================================================

#ifndef FppTest_DpTest_HPP
#define FppTest_DpTest_HPP

#include <array>

#include "FppTest/dp/DpTestComponentAc.hpp"
#include "Fw/Types/String.hpp"

namespace FppTest {

class DpTest : public DpTestComponentBase {
    // Friend class for testing
    friend class Tester;

  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    static constexpr FwSizeType CONTAINER_1_DATA_SIZE = 100;
    static constexpr FwSizeType CONTAINER_1_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_1_DATA_SIZE);
    static constexpr FwSizeType CONTAINER_2_DATA_SIZE = 1000;
    static constexpr FwSizeType CONTAINER_2_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_2_DATA_SIZE);
    static constexpr FwSizeType CONTAINER_3_DATA_SIZE = 1000;
    static constexpr FwSizeType CONTAINER_3_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_3_DATA_SIZE);
    static constexpr FwSizeType CONTAINER_4_DATA_SIZE = 1000;
    static constexpr FwSizeType CONTAINER_4_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_4_DATA_SIZE);
    static constexpr FwSizeType CONTAINER_5_DATA_SIZE = 1000;
    static constexpr FwSizeType CONTAINER_5_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_5_DATA_SIZE);
    static constexpr FwSizeType CONTAINER_6_DATA_SIZE = 1000;
    static constexpr FwSizeType CONTAINER_6_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_6_DATA_SIZE);
    static constexpr FwSizeType CONTAINER_7_DATA_SIZE = 1000;
    static constexpr FwSizeType CONTAINER_7_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_7_DATA_SIZE);

    static constexpr FwSizeType STRING_ARRAY_RECORD_ARRAY_SIZE = 10;

  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    using U8ArrayRecordData = std::array<U8, 256>;
    using U32ArrayRecordData = std::array<U32, 100>;
    using DataArrayRecordData = std::array<DpTest_Data, 300>;
    using PtrToConstStringBase = const Fw::StringBase*;

  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object DpTest
    DpTest(const char* const compName,                      //!< The component name
           U32 u32RecordData,                               //!< The U32Record data
           U16 dataRecordData,                              //!< The DataRecord data
           const U8ArrayRecordData& u8ArrayRecordData,      //!< The U8ArrayRecord data
           const U32ArrayRecordData& u32ArrayRecordData,    //!< The U32ArrayRecord data
           const DataArrayRecordData& dataArrayRecordData,  //!< The DataArrayRecord data
           const Fw::StringBase& stringRecordData           //!< The StringRecord data
    );

    //! Destroy object DpTest
    ~DpTest();

  public:
    // ----------------------------------------------------------------------
    // Public interface methods
    // ----------------------------------------------------------------------

    //! Set the send time
    void setSendTime(Fw::Time time) { this->sendTime = time; }

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for schedIn
    void schedIn_handler(const NATIVE_INT_TYPE portNum,  //!< The port number
                         U32 context                     //!< The call order
                         ) final;

  private:
    // ----------------------------------------------------------------------
    // Data product handler implementations
    // ----------------------------------------------------------------------

    //! Receive a data product container of type Container1
    //! \return Serialize status
    void dpRecv_Container1_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) final;

    //! Receive a data product container of type Container2
    //! \return Serialize status
    void dpRecv_Container2_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) final;

    //! Receive a data product container of type Container3
    //! \return Serialize status
    void dpRecv_Container3_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) final;

    //! Receive a data product container of type Container4
    //! \return Serialize status
    void dpRecv_Container4_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) final;

    //! Receive a data product container of type Container5
    //! \return Serialize status
    void dpRecv_Container5_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) final;

    //! Receive a data product container of type Container6
    //! \return Serialize status
    void dpRecv_Container6_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) final;

    //! Receive a data product container of type Container7
    //! \return Serialize status
    void dpRecv_Container7_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) final;

  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Check that a container is empty
    void checkContainerEmpty(const DpContainer& container  //!< The container
    ) const;

    //! Check a container for validity
    void checkContainer(const DpContainer& container,  //!< The container
                        FwDpIdType localId,            //!< The expected local id
                        FwSizeType size,               //!< The expected size
                        FwDpPriorityType priority      //!< The expected priority
    ) const;

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! Stored container
    DpContainer m_container;

    //! U32Record data
    const U32 u32RecordData;

    //! DataRecord data
    const U16 dataRecordData;

    //! U8ArrayRecord data
    const U8ArrayRecordData& u8ArrayRecordData;

    //! U32ArrayRecord data
    const U32ArrayRecordData& u32ArrayRecordData;

    //! DataArrayRecord data
    const DataArrayRecordData& dataArrayRecordData;

    //! StringRecord data
    const Fw::StringBase& stringRecordData;

    //! StringArrayRecord data
    PtrToConstStringBase stringArrayRecordData[STRING_ARRAY_RECORD_ARRAY_SIZE];

    //! Send time for testing
    Fw::Time sendTime;
};

}  // end namespace FppTest

#endif
