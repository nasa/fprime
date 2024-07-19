// ======================================================================
// \title  FrameAccumulator.cpp
// \author mstarch
// \brief  cpp file for FrameAccumulator component implementation class
// ======================================================================

#include "Svc/FrameAccumulator/FrameAccumulator.hpp"
#include "Fw/Types/Assert.hpp"
#include "FpConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FrameAccumulator ::FrameAccumulator(const char* const compName) : FrameAccumulatorComponentBase(compName),
    m_detector(nullptr), m_memoryAllocator(nullptr), m_memory(nullptr), m_allocatorId(-1) {}

FrameAccumulator ::~FrameAccumulator() {
    // If configuration happened, we must deallocate
    if (this->m_memoryAllocator != nullptr) {
        //TODO: after this line we have ownership of a deallocated buffer
        this->m_memoryAllocator->deallocate(this->m_allocatorId, this->m_memory);
    }
}

void FrameAccumulator ::configure(const FrameDetector& detector, NATIVE_UINT_TYPE allocationId,
               Fw::MemAllocator& allocator,
               FwSizeType store_size
) {
    bool recoverable = false;
    FW_ASSERT(std::numeric_limits<NATIVE_INT_TYPE>::max() >= store_size, static_cast<FwAssertArgType>(store_size));
    NATIVE_UINT_TYPE store_size_int = static_cast<NATIVE_UINT_TYPE>(store_size);
    U8* data = reinterpret_cast<U8*>(allocator.allocate(allocationId, store_size_int, recoverable));
    FW_ASSERT(data != nullptr);
    FW_ASSERT(store_size_int >= store_size);
    m_inRing.setup(data, store_size_int);

    this->m_detector = &detector;
    this->m_allocatorId = allocationId;
    this->m_memoryAllocator = &allocator;
    this->m_memory = data;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FrameAccumulator ::dataIn_handler(FwIndexType portNum, Fw::Buffer& buffer, const Drv::RecvStatus& status) {
    // Check whether there is data to process
    if (status.e == Drv::RecvStatus::RECV_OK) {
        // There is: process the data
        this->processBuffer(buffer);
    }
    // Deallocate the buffer
    this->dataDeallocate_out(0, buffer);
}

void FrameAccumulator ::processBuffer(Fw::Buffer& buffer) {
    const U32 bufferSize = buffer.getSize();
    U8 *const bufferData = buffer.getData();
    // Current offset into buffer
    U32 offset = 0;
    // Remaining data in buffer
    U32 remaining = bufferSize;

    for (U32 i = 0; i < bufferSize; ++i) {
        // If there is no data left or no space, exit the loop
        if (remaining == 0 || this->m_inRing.get_free_size() == 0) {
            break;
        }
        // Compute the size of data to serialize
        const NATIVE_UINT_TYPE ringFreeSize = this->m_inRing.get_free_size();
        const NATIVE_UINT_TYPE serSize = (ringFreeSize <= remaining) ?
                                         ringFreeSize : static_cast<NATIVE_UINT_TYPE>(remaining);
        // Serialize data into the ring buffer
        const Fw::SerializeStatus status =
                this->m_inRing.serialize(&bufferData[offset], serSize);
        // If data does not fit, there is a coding error
        FW_ASSERT(
                status == Fw::FW_SERIALIZE_OK,
                static_cast<FwAssertArgType>(status),
                static_cast<FwAssertArgType>(offset),
                static_cast<FwAssertArgType>(serSize));
        // Process the data
        this->processRing();
        // Update buffer offset and remaining
        offset += serSize;
        remaining -= serSize;
    }
    // Either all the bytes from the data buffer must be processed, or the ring must be full
    FW_ASSERT(remaining == 0 || this->m_inRing.get_free_size() == 0, static_cast<FwAssertArgType>(remaining));
}

    void FrameAccumulator ::processRing() {
        FW_ASSERT(this->m_detector != nullptr);

        // The number of remaining bytes in the ring buffer
        U32 remaining = 0;
        // The protocol status
        FrameDetector::Status status = FrameDetector::Status::FRAME_DETECTED;
        // The ring buffer capacity
        const NATIVE_UINT_TYPE ringCapacity = this->m_inRing.get_capacity();

        // Process the ring buffer looking for at least the header
        for (U32 i = 0; i < ringCapacity; i++) {
            // Get the number of bytes remaining in the ring buffer
            remaining = this->m_inRing.get_allocated_size();
            // If there are none, we are done
            if (remaining == 0) {
                break;
            }
            // size_out is a return variable we initialize to zero, but it should be overwritten
            FwSizeType size_out = 0;
            // Attempt to detect the frame without changing the circular buffer
            status = this->m_detector->detect(this->m_inRing, size_out);
            // Detect must not consume data in the ring buffer
            FW_ASSERT(
                    m_inRing.get_allocated_size() == remaining,
                    static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
                    static_cast<FwAssertArgType>(remaining)
            );
            // On successful detection, consume data from the ring buffer and place it into an allocated frame
            if (status == FrameDetector::FRAME_DETECTED) {
                // size_out must be set to the size of the buffer and must fit within the existing data
                FW_ASSERT(size_out != 0);
                FW_ASSERT(
                        size_out <= remaining,
                        static_cast<FwAssertArgType>(size_out),
                        static_cast<FwAssertArgType>(remaining));
                Fw::Buffer buffer = this->frameAllocate_out(0, size_out);
                if (buffer.isValid()) {
                    // Copy out data and rotate
                    FW_ASSERT(this->m_inRing.peek(buffer.getData(), size_out) == Fw::SerializeStatus::FW_SERIALIZE_OK);
                    buffer.setSize(size_out);
                    m_inRing.rotate(static_cast<U32>(size_out));
                    FW_ASSERT(
                            m_inRing.get_allocated_size() == static_cast<U32>(remaining - size_out),
                            static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
                            static_cast<FwAssertArgType>(remaining),
                            static_cast<FwAssertArgType>(size_out)
                    );
                    this->frameOut_out(0, buffer);
                }
                else {
                    // No buffer is available, we need to exit and try again later
                    break;
                }
            }
            // More data needed
            else if (status == FrameDetector::MORE_DATA_NEEDED) {
                // size_out can never be larger than the capacity of the ring. Otherwise all uplink will fail.
                FW_ASSERT(size_out < m_inRing.get_capacity(), static_cast<FwAssertArgType>(size_out));
                // Detection should report "more is needed" and set size_out to something larger than available data
                FW_ASSERT(
                        size_out > remaining,
                        static_cast<FwAssertArgType>(size_out),
                        static_cast<FwAssertArgType>(remaining));
                // Break out of loop: suspend detection until we receive another buffer
                break;
            }
            // No frame was detected or an unknown status was received
            else {
                // Discard a single byte of data and start again
                m_inRing.rotate(1);
                FW_ASSERT(
                        m_inRing.get_allocated_size() == remaining - 1,
                        static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
                        static_cast<FwAssertArgType>(remaining)
                );
            }
        }
    }
}  // namespace Svc
