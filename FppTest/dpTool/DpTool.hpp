// ======================================================================
// \title  DpTool.hpp
// \brief  hpp file for DpTool component implementation class
// ======================================================================

#ifndef FppTest_DpTool_HPP
#define FppTest_DpTool_HPP

#include <array>

#include "FppTest/dpTool/DpToolComponentAc.hpp"

namespace FppTest {

  enum RecordTest {
    BoolRecord,
    I8Record,
    I16Record,
    I32Record,
    I64Record,
    U32Record,   
    F32Record,
    F64Record,
    EnumRecord,
    ComplexRecord,
    U8ArrayRecord,
    U32ArrayRecord,
    DataArrayRecord,
    FppArrayRecord,
};


class DpTool : public DpToolComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    static constexpr FwSizeType CONTAINER_1_DATA_SIZE = 2000;
    static constexpr FwSizeType CONTAINER_1_PACKET_SIZE = DpContainer::getPacketSizeForDataSize(CONTAINER_1_DATA_SIZE);


  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------
    static const U32 RECORD_ARRAY_SIZE = 300;

  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object DpTool
    DpTool(const char* const compName);

    //! Initialize object DpTool
    void init(const NATIVE_INT_TYPE queueDepth,   //!< The queue depth
              const NATIVE_INT_TYPE instance = 0  //!< The instance number
    );

    //! Destroy object DpTool
    ~DpTool();

  public:
    // ----------------------------------------------------------------------
    // Public interface methods
    // ----------------------------------------------------------------------

    //! Set the send time
    void setSendTime(Fw::Time time) { this->sendTime = time; }

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for schedIn
    void schedIn_handler(const NATIVE_INT_TYPE portNum,  //!< The port number
                         NATIVE_UINT_TYPE context        //!< The call order
                         ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Data product handler implementations
    // ----------------------------------------------------------------------

    void dpRecv_Container1_handler(DpContainer& container,  //!< The container
                                   Fw::Success::T           //!< The container status
                                   ) override;


  PRIVATE:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Check a container for validity
    void checkContainer(const DpContainer& container,  //!< The container
                        FwDpIdType localId,            //!< The expected local id
                        FwSizeType size                //!< The expected size
    ) const;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! Send time for testing
    Fw::Time sendTime;

    RecordTest recordTest;
};

}  // end namespace FppTest

#endif
