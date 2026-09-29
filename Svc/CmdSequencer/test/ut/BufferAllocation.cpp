// Regression tests for Sequence::allocateBuffer and allocator-granted capacity.
#include <cstddef>
#include "Svc/CmdSequencer/test/ut/CmdSequencerTester.hpp"

namespace Svc {
namespace {

// Fixed backing storage keeps the requested capacity distinct from the grant.
class LimitedAllocator final : public Fw::MemAllocator {
  public:
    explicit LimitedAllocator(FwSizeType grantedSize, bool failAllocation = false)
        : Fw::MemAllocator(), granted(grantedSize), fail(failAllocation) {}

    void* allocate(FwEnumStoreType identifier, FwSizeType& size, bool& recoverable, FwSizeType alignment) override {
        EXPECT_LE(alignment, alignof(std::max_align_t));
        EXPECT_LE(this->granted, sizeof this->storage);
        this->allocationId = identifier;
        this->requested = size;
        ++this->allocations;
        size = this->granted;
        recoverable = false;
        return this->fail ? nullptr : this->storage;
    }

    void deallocate(FwEnumStoreType identifier, void* buffer) override {
        EXPECT_EQ(identifier, this->allocationId);
        EXPECT_EQ(buffer, this->storage);
        ++this->deallocations;
    }

    FwSizeType granted;
    bool fail;
    FwEnumStoreType allocationId = 0;
    FwSizeType requested = 0;
    U32 allocations = 0;
    U32 deallocations = 0;
    alignas(std::max_align_t) U8 storage[128] = {};
};

class InspectableSequence final : public CmdSequencerComponentImpl::FPrimeSequence {
  public:
    explicit InspectableSequence(CmdSequencerComponentImpl& component) : FPrimeSequence(component) {}

    Fw::ExternalSerializeBuffer& buffer() { return this->m_buffer; }
};

void checkGrantedCapacity(FwSizeType granted) {
    CmdSequencerComponentImpl component("AllocationTest");
    LimitedAllocator allocator(granted);
    InspectableSequence sequence(component);
    const FwSizeType requested = sizeof allocator.storage;
    const FwEnumStoreType identifier = 42;

    sequence.allocateBuffer(identifier, allocator, requested);
    EXPECT_EQ(allocator.allocations, 1U);
    EXPECT_EQ(allocator.requested, requested);
    EXPECT_EQ(allocator.allocationId, identifier);
    EXPECT_EQ(sequence.buffer().getBuffAddr(), allocator.storage);
    EXPECT_EQ(sequence.buffer().getCapacity(), granted);

    // The granted bytes are usable, but serialization must not cross the grant.
    EXPECT_EQ(sequence.buffer().setBuffLen(granted), Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(sequence.buffer().serializeFrom(static_cast<U8>(0xA5)), Fw::FW_SERIALIZE_NO_ROOM_LEFT);
    if (granted < sizeof allocator.storage) {
        EXPECT_EQ(allocator.storage[granted], 0U);
    }

    sequence.deallocateBuffer(allocator);
    EXPECT_EQ(allocator.deallocations, 1U);
    EXPECT_EQ(sequence.buffer().getCapacity(), 0U);
    EXPECT_EQ(sequence.buffer().getBuffAddr(), nullptr);
}

class AllocationFileTester final : public CmdSequencerTester {
  public:
    void fileLargerThanGrant() {
        LimitedAllocator allocator(64);
        InspectableSequence sequence(this->component);
        sequence.allocateBuffer(42, allocator, BUFFER_SIZE);
        SequenceFiles::TooLargeFile file(static_cast<U32>(allocator.granted), SequenceFiles::File::Format::F_PRIME);
        file.write();
        this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 0));
        this->clearHistory();

        const bool loaded = sequence.loadFile(file.getName());
        sequence.deallocateBuffer(allocator);

        EXPECT_FALSE(loaded);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_CS_FileSizeError_SIZE(1);
        ASSERT_EVENTS_CS_FileSizeError(0, file.getName().toChar(), file.getDataSize());
    }
};

TEST(BufferAllocation, ShrinkingAllocator) {
    TEST_CASE(5900.1, "Use the granted capacity when the allocator reduces the requested size.");
    checkGrantedCapacity(64);
}

TEST(BufferAllocation, FullAllocation) {
    TEST_CASE(5900.2, "Preserve full-size allocations and release the original pointer and identifier.");
    checkGrantedCapacity(128);
}

TEST(BufferAllocation, MinimumHeaderAllocation) {
    TEST_CASE(5900.3, "A reduced allocation that holds exactly one header remains usable.");
    checkGrantedCapacity(CmdSequencerComponentImpl::Sequence::Header::SERIALIZED_SIZE);
}

TEST(BufferAllocation, NullAllocation) {
    TEST_CASE(5900.4, "Reject a failed allocation during initialization, including a zero-sized grant.");
    CmdSequencerComponentImpl component("AllocationTest");
    LimitedAllocator allocator(0, true);
    InspectableSequence sequence(component);
    ASSERT_DEATH(sequence.allocateBuffer(42, allocator, 128), ".*");
}

TEST(BufferAllocation, TooSmallAllocation) {
    TEST_CASE(5900.5, "Reject a granted allocation too small for the sequence header during initialization.");
    CmdSequencerComponentImpl component("AllocationTest");
    LimitedAllocator allocator(CmdSequencerComponentImpl::Sequence::Header::SERIALIZED_SIZE - 1);
    InspectableSequence sequence(component);
    ASSERT_DEATH(sequence.allocateBuffer(42, allocator, 128), ".*");
}

TEST(BufferAllocation, RejectsFileLargerThanGrant) {
    REQUIREMENT("ISF-CMDS-002");
    TEST_CASE(5900.6, "Reject a sequence whose record size fits the request but exceeds the allocation grant.");
    AllocationFileTester tester;
    tester.fileLargerThanGrant();
}

}  // namespace
}  // namespace Svc
