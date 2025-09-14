// ======================================================================
// \title  DpContainerTester.hpp
// \author m-aleem
// \brief  hpp file for DpContainerTester
// ======================================================================

#ifndef Fw_DpContainerTester_HPP
#define Fw_DpContainerTester_HPP

#include "Fw/Dp/DpContainer.hpp"

namespace Fw {

class DpContainerTester {
  public:
    static bool verifyBufferSize(const Fw::DpContainer& container, FwSizeType expectedSize) {
        return container.m_buffer.getSize() == expectedSize;
    }

    static U8* getBufferPointers(const Fw::DpContainer& container) {
        U8* const buffPtr = container.m_buffer.getData();
        return buffPtr;
    }

    static bool verifyDataBufferAddress(const Fw::DpContainer& container, const U8* expectedAddr) {
        return container.m_dataBuffer.getBuffAddr() == expectedAddr;
    }

    static bool verifyDataBufferCapacity(const Fw::DpContainer& container, FwSizeType expectedCapacity) {
        return container.m_dataBuffer.getBuffCapacity() == expectedCapacity;
    }

    static bool isDataBufferEmpty(const Fw::DpContainer& container) {
        const Fw::SerializeBufferBase& buffer = container.m_dataBuffer;
        const FwSizeType buffLength = buffer.getBuffLength();
        const FwSizeType buffLeft = buffer.getBuffLeft();

        return buffLength == 0 && buffLeft == 0;
    }
};

}  // namespace Fw

#endif
