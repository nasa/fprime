// ======================================================================
// \title  CcsdsTcFrameDetectorTestMain.cpp
// \author thomas-bc
// \brief  cpp file for FrameAccumulator component test main function
// ======================================================================

#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/Types/TCHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "Svc/FrameAccumulator/FrameDetector/CcsdsTcFrameDetector.hpp"
#include "Utils/Types/test/ut/CircularBuffer/CircularBufferTester.hpp"
#include "gtest/gtest.h"

using namespace Svc::Ccsds;

constexpr U32 CIRCULAR_BUFFER_TEST_SIZE = 2048;
constexpr U16 EXPECTED_START_TOKEN =
    0x1 << TCSubfields::BypassFlagOffset | (ComCfg::FppConstant_SpacecraftId::SpacecraftId);

// Test fixture to set up the detector under test and circular buffer
class CcsdsFrameDetectorTest : public ::testing::Test {
  protected:
    void SetUp() override {
        ::memset(this->m_buffer, 0, CIRCULAR_BUFFER_TEST_SIZE);
        this->circular_buffer = Types::CircularBuffer(this->m_buffer, CIRCULAR_BUFFER_TEST_SIZE);
    }

    U8 m_buffer[CIRCULAR_BUFFER_TEST_SIZE];
    Svc::FrameDetectors::CcsdsTcFrameDetector detector;
    Types::CircularBuffer circular_buffer;
};

//! \brief Create an F´ frame and serialize it into the supplied circular buffer
//! \param circular_buffer The circular buffer to serialize the frame into
//! \note The frame is generated with random data of random size
//! \return The size of the generated frame
FwSizeType generate_random_tc_frame(Types::CircularBuffer& circular_buffer) {
    // Generate random packet size (1-1024 bytes; because 0 would trigger undefined behavior warnings)
    // U16 packet_size = static_cast<U16>(STest::Random::lowerUpper(1, 1024));
    U16 packet_size = 10;

    FwSizeType total_frame_size = packet_size + TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE;

    U8 packet_data[packet_size];
    // Generate random packet_data of random size
    for (FwSizeType i = 0; i < packet_size; i++) {
        packet_data[i] = static_cast<U8>(STest::Random::lowerUpper(0, 255));
    }
    TCHeader tcHeader(EXPECTED_START_TOKEN,                               // Use a predefined token for flags and SC ID
                      static_cast<U16>(total_frame_size - 1),             // Length (and unused VcId)
                      static_cast<U8>(STest::Random::lowerUpper(0, 255))  // Random frame sequence number
    );

    U8 frame_header[TCHeader::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer header_ser_buffer(frame_header, TCHeader::SERIALIZED_SIZE);
    tcHeader.serialize(header_ser_buffer);

    // Serialize header and packet data into the circular buffer
    circular_buffer.serialize(frame_header, TCHeader::SERIALIZED_SIZE);
    circular_buffer.serialize(packet_data, packet_size);

    U8 frame_trailer[TCTrailer::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer trailer_ser_buffer(frame_trailer, TCTrailer::SERIALIZED_SIZE);
    TCTrailer tcTrailer;

    // Calculate CRC on header + packet_data
    Svc::Ccsds::Utils::CRC16 crc;
    for (FwSizeType i = 0; i < static_cast<FwSizeType>(packet_size + TCHeader::SERIALIZED_SIZE); ++i) {
        U8 byte = 0;
        circular_buffer.peek(byte, i);
        crc.update(byte);
    }
    tcTrailer.set_fecf(crc.finalize());
    tcTrailer.serialize(trailer_ser_buffer);
    // Serialize trailer into the circular buffer
    circular_buffer.serialize(frame_trailer, TCTrailer::SERIALIZED_SIZE);
    return total_frame_size;
}

TEST_F(CcsdsFrameDetectorTest, TestBufferTooSmall) {
    // Anything smaller than the size of header + trailer is invalid
    U32 minimum_valid_size = TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE;
    U32 invalid_size = STest::Random::lowerUpper(1, minimum_valid_size - 1);
    this->circular_buffer.serialize(this->m_buffer, invalid_size);

    Svc::FrameDetector::Status status;
    FwSizeType size_out = 0;
    status = this->detector.detect(circular_buffer, size_out);

    // Expect that the detector reports that more data is needed
    EXPECT_EQ(status, Svc::FrameDetector::Status::MORE_DATA_NEEDED);
    EXPECT_EQ(size_out, minimum_valid_size);
}

TEST_F(CcsdsFrameDetectorTest, TestFrameDetected) {
    FwSizeType frame_size = generate_random_tc_frame(this->circular_buffer);

    Svc::FrameDetector::Status status;
    FwSizeType size_out = 0;
    status = this->detector.detect(circular_buffer, size_out);

    EXPECT_EQ(status, Svc::FrameDetector::Status::FRAME_DETECTED);
    EXPECT_EQ(size_out, frame_size);
}

TEST_F(CcsdsFrameDetectorTest, TestManyFrameDetected) {
    U32 MAX_ITERS = 1000;
    for (U32 i = 0; i < MAX_ITERS; i++) {
        FwSizeType frame_size = generate_random_tc_frame(this->circular_buffer);
        Svc::FrameDetector::Status status;
        FwSizeType size_out = 0;
        status = this->detector.detect(this->circular_buffer, size_out);
        EXPECT_EQ(status, Svc::FrameDetector::Status::FRAME_DETECTED);
        EXPECT_EQ(size_out, frame_size);
        this->circular_buffer.rotate(size_out);  // clear up used data
    }
}

TEST_F(CcsdsFrameDetectorTest, TestNoFrameDetected) {
    (void)generate_random_tc_frame(this->circular_buffer);
    // Remove 1 byte from the beginning of the frame, making it invalid
    this->circular_buffer.rotate(1);
    Svc::FrameDetector::Status status;
    FwSizeType unused = 0;
    status = this->detector.detect(this->circular_buffer, unused);
    EXPECT_EQ(status, Svc::FrameDetector::Status::NO_FRAME_DETECTED);
}

TEST_F(CcsdsFrameDetectorTest, TestMoreDataNeeded) {
    (void)generate_random_tc_frame(this->circular_buffer);
    // Remove 1 byte from the end of the frame to trigger "more data needed"
    Types::CircularBufferTester::tester_m_allocated_size_decrement(this->circular_buffer);
    Svc::FrameDetector::Status status;
    FwSizeType unused = 0;
    status = this->detector.detect(this->circular_buffer, unused);
    EXPECT_EQ(status, Svc::FrameDetector::Status::MORE_DATA_NEEDED);
}

TEST_F(CcsdsFrameDetectorTest, TestCorruptedCrc) {
    FwSizeType frame_size = generate_random_tc_frame(this->circular_buffer);
    this->m_buffer[frame_size - 2] = 0xFF;  // Corrupt the last 2 bytes to fail CRC check
    this->m_buffer[frame_size - 1] = 0xFF;  // Corrupt the last 2 bytes to fail CRC check

    Svc::FrameDetector::Status status;
    FwSizeType unused = 0;
    status = this->detector.detect(this->circular_buffer, unused);
    EXPECT_EQ(status, Svc::FrameDetector::Status::NO_FRAME_DETECTED);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
