// ======================================================================
// \title  AbstractState.hpp
// \author Rob Bocchino
// \brief  Header file for abstract state
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_AbstractState_HPP
#define Svc_AbstractState_HPP

#include <cstring>

#include "Fw/Types/Assert.hpp"
#include "Os/File.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/DpWriter/DpWriter.hpp"
#include "TestUtils/OnChangeChannel.hpp"
#include "TestUtils/Option.hpp"

namespace Svc {

class AbstractState {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! The minimum data size
    static constexpr FwSizeType MIN_DATA_SIZE = 0;

    //! The maximum data size
    static constexpr FwSizeType MAX_DATA_SIZE = 1024;

    //! The maximum buffer size
    static constexpr FwSizeType MAX_BUFFER_SIZE = Fw::DpContainer::getPacketSizeForDataSize(MAX_DATA_SIZE);

  public:
    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    //! Construct an AbstractState object
    AbstractState()
        : m_dataSizeOpt(),
          m_NumBuffersReceived(0),
          m_NumBytesWritten(0),
          m_NumFailedWrites(0),
          m_NumSuccessfulWrites(0),
          m_NumErrors(0),
          m_procTypes(0) {}

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the data size
    FwSizeType getDataSize() const {
        return this->m_dataSizeOpt.getOrElse(STest::Pick::lowerUpper(MIN_DATA_SIZE, MAX_DATA_SIZE));
    }

    //! Set the data size
    void setDataSize(FwSizeType dataSize) { this->m_dataSizeOpt.set(dataSize); }

    //! Get a data product buffer backed by bufferData
    //! \return The buffer
    Fw::Buffer getDpBuffer();

  private:
    // ----------------------------------------------------------------------
    // Private state variables
    // ----------------------------------------------------------------------

    //! The current buffer size
    TestUtils::Option<FwSizeType> m_dataSizeOpt;

  public:
    // ----------------------------------------------------------------------
    // Public state variables
    // ----------------------------------------------------------------------

    //! The number of buffers received
    TestUtils::OnChangeChannel<U32> m_NumBuffersReceived;

    //! The number of bytes written
    TestUtils::OnChangeChannel<U64> m_NumBytesWritten;

    //! The number of failed writes
    TestUtils::OnChangeChannel<U32> m_NumFailedWrites;

    //! The number of successful writes
    TestUtils::OnChangeChannel<U32> m_NumSuccessfulWrites;

    //! The number of errors
    TestUtils::OnChangeChannel<U32> m_NumErrors;

    //! The number of BufferTooSmallForData events since the last throttle clear
    FwSizeType m_bufferTooSmallForDataEventCount = 0;

    //! The number of BufferTooSmallForPacket events since the last throttle clear
    FwSizeType m_bufferTooSmallForPacketEventCount = 0;

    //! The number of buffer invalid events since the last throttle clear
    FwSizeType m_invalidBufferEventCount = 0;

    //! The number of file open error events since the last throttle clear
    FwSizeType m_fileOpenErrorEventCount = 0;

    //! The number of file write error events since the last throttle clear
    FwSizeType m_fileWriteErrorEventCount = 0;

    //! The number of invalid header hash events since the last throttle clear
    FwSizeType m_invalidHeaderHashEventCount = 0;

    //! The number of invalid header events since the last throttle clear
    FwSizeType m_invalidHeaderEventCount = 0;

    //! Data for buffers
    U8 m_bufferData[MAX_BUFFER_SIZE];

    //! Data for write results
    U8 m_writeResultData[MAX_BUFFER_SIZE];

    //! Bit mask for processing out port calls
    Fw::DpCfg::ProcType::SerialType m_procTypes;
};

}  // namespace Svc

#endif
