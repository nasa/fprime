// ======================================================================
// \title  FrameAccumulator.cpp
// \author mstarch
// \brief  cpp file for FrameAccumulator component implementation class
// ======================================================================

#include "Svc/FrameAccumulator/FrameAccumulator.hpp"
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FrameAccumulator ::FrameAccumulator(const char* const compName)
    : FrameAccumulatorComponentBase(compName),
      m_detector(nullptr),
      m_memoryAllocator(nullptr),
      m_memory(nullptr),
      m_allocatorId(0) {}

FrameAccumulator ::~FrameAccumulator() {}

void FrameAccumulator ::configure(const FrameDetector& detector,
                                  FwEnumStoreType allocationId,
                                  Fw::MemAllocator& allocator,
                                  FwSizeType store_size) {
    bool recoverable = false;
    U8* const data = static_cast<U8*>(allocator.allocate(allocationId, store_size, recoverable));
    FW_ASSERT(data != nullptr);
    m_inRing.setup(data, store_size);

    this->m_detector = &detector;
    this->m_allocatorId = allocationId;
    this->m_memoryAllocator = &allocator;
    this->m_memory = data;
}

void FrameAccumulator ::cleanup() {
    // If configuration happened, we must deallocate
    if (this->m_memoryAllocator != nullptr) {
        this->m_memoryAllocator->deallocate(this->m_allocatorId, this->m_memory);
        this->m_memory = nullptr;
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FrameAccumulator ::dataIn_handler(FwIndexType portNum, Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    // Check whether there is data to process
    if (buffer.isValid()) {
        // The buffer is not necessarily a full frame, so the attached context has no meaning and we ignore it
        this->processBuffer(buffer);
    }
    // Return ownership of the incoming data
    this->dataReturnOut_out(0, buffer, context);
}

void FrameAccumulator ::processBuffer(Fw::Buffer& buffer) {
    const FwSizeType bufferSize = buffer.getSize();
    U8* const bufferData = buffer.getData();
    // Current offset into buffer
    FwSizeType offset = 0;
    // Remaining data in buffer
    FwSizeType remaining = bufferSize;

    for (FwSizeType i = 0; i < bufferSize; ++i) {
        // If there is no data left or no space, exit the loop
        if (remaining == 0 || this->m_inRing.get_free_size() == 0) {
            break;
        }
        // Compute the size of data to serialize
        const FwSizeType ringFreeSize = this->m_inRing.get_free_size();
        const FwSizeType serSize = (ringFreeSize <= remaining) ? ringFreeSize : remaining;
        // Serialize data into the ring buffer
        const Fw::SerializeStatus status = this->m_inRing.serialize(&bufferData[offset], serSize);
        // If data does not fit, there is a coding error
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status),
                  static_cast<FwAssertArgType>(offset), static_cast<FwAssertArgType>(serSize));
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
    FwSizeType remaining = 0;
    // The protocol status
    FrameDetector::Status status = FrameDetector::Status::FRAME_DETECTED;
    // The ring buffer capacity
    const FwSizeType ringCapacity = this->m_inRing.get_capacity();

    // Process the ring buffer looking for at least the header
    for (FwSizeType i = 0; i < ringCapacity; i++) {
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
        FW_ASSERT(m_inRing.get_allocated_size() == remaining,
                  static_cast<FwAssertArgType>(m_inRing.get_allocated_size()), static_cast<FwAssertArgType>(remaining));
        // On successful detection, consume data from the ring buffer and place it into an allocated frame
        if (status == FrameDetector::FRAME_DETECTED) {
            // size_out must be set to the size of the buffer and must fit within the existing data
            FW_ASSERT(size_out != 0);
            FW_ASSERT(size_out <= remaining, static_cast<FwAssertArgType>(size_out),
                      static_cast<FwAssertArgType>(remaining));
            // check for overflow before casting down to U32
            FW_ASSERT(size_out <= std::numeric_limits<U32>::max());
            Fw::Buffer buffer = this->bufferAllocate_out(0, static_cast<U32>(size_out));
            if (buffer.isValid()) {
                // Copy data out of ring buffer into the allocated buffer
                Fw::SerializeStatus serialize_status = this->m_inRing.peek(buffer.getData(), size_out);
                buffer.setSize(static_cast<Fw::Buffer::SizeType>(size_out));
                FW_ASSERT(serialize_status == Fw::SerializeStatus::FW_SERIALIZE_OK);
                // Consume (rotate) the data from the ring buffer
                serialize_status = this->m_inRing.rotate(size_out);
                FW_ASSERT(serialize_status == Fw::SerializeStatus::FW_SERIALIZE_OK);
                FW_ASSERT(m_inRing.get_allocated_size() == remaining - size_out,
                          static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
                          static_cast<FwAssertArgType>(remaining), static_cast<FwAssertArgType>(size_out));
                ComCfg::FrameContext context;
                this->dataOut_out(0, buffer, context);
            } else {
                // No buffer is available, we need to exit and try again later
                this->log_WARNING_HI_NoBufferAvailable();
                break;
            }
        }
        // More data needed
        else if (status == FrameDetector::MORE_DATA_NEEDED) {
            // size_out can never be larger than the capacity of the ring. Otherwise all uplink will fail.
            FW_ASSERT(size_out < m_inRing.get_capacity(), static_cast<FwAssertArgType>(size_out));
            // Detection should report "more is needed" and set size_out to something larger than available data
            FW_ASSERT(size_out > remaining, static_cast<FwAssertArgType>(size_out),
                      static_cast<FwAssertArgType>(remaining));
            // Break out of loop: suspend detection until we receive another buffer
            break;
        }
        // No frame was detected or an unknown status was received
        else {
            // Discard a single byte of data and start again
            (void)this->m_inRing.rotate(1);
            FW_ASSERT(m_inRing.get_allocated_size() == remaining - 1,
                      static_cast<FwAssertArgType>(m_inRing.get_allocated_size()),
                      static_cast<FwAssertArgType>(remaining));
        }
    }
}

void FrameAccumulator ::dataReturnIn_handler(FwIndexType portNum,
                                             Fw::Buffer& fwBuffer,
                                             const ComCfg::FrameContext& context) {
    // Frame buffer ownership is returned to the component. Component had allocated with a buffer manager,
    // so we return it to the buffer manager for deallocation
    this->bufferDeallocate_out(0, fwBuffer);
}

}  // namespace Svc
