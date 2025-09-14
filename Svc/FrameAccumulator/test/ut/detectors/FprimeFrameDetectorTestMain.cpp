// ======================================================================
// \title  FprimeFrameDetectorTestMain.cpp
// \author thomas-bc
// \brief  cpp file for FrameAccumulator component test main function
// ======================================================================

#include "STest/Random/Random.hpp"
#include "Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp"
#include "Utils/Hash/Hash.hpp"
#include "Utils/Types/test/ut/CircularBuffer/CircularBufferTester.hpp"
#include "gtest/gtest.h"

constexpr U32 CIRCULAR_BUFFER_TEST_SIZE = 2048;

//! \brief Create an F´ frame and serialize it into the supplied circular buffer
//! \param circular_buffer The circular buffer to serialize the frame into
//! \note The frame is generated with random data of random size
//! \return The size of the generated frame
FwSizeType generate_random_fprime_frame(Types::CircularBuffer& circular_buffer) {
    constexpr FwSizeType FRAME_HEADER_SIZE = 8;
    constexpr FwSizeType FRAME_FOOTER_SIZE = 4;
    // Generate random packet size (1-1024 bytes; because 0 would trigger undefined behavior warnings)
    // 1024 is max length as per FrameAccumulator/FrameDetector/FprimeFrameDetector @ LengthToken::MaximumLength
    U32 packet_size = STest::Random::lowerUpper(1, 1024);

    U8 packet_data[packet_size];
    // Generate random packet_data of random size
    for (FwSizeType i = 0; i < packet_size; i++) {
        packet_data[i] = static_cast<U8>(STest::Random::lowerUpper(0, 255));
    }
    // Frame header                      |  Start Word 4 bytes  |   Length (4 bytes)   |
    U8 frame_header[FRAME_HEADER_SIZE] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00, 0x00, 0x00};
    // Serialize actual packet size into header
    for (FwSizeType i = 0; i < 4; i++) {
        frame_header[i + 4] = static_cast<U8>(packet_size >> (8 * (3 - i)));
    }

    // Calculate CRC on header + packet_data
    Utils::Hash crc_calculator;
    Utils::HashBuffer crc_result;
    crc_calculator.update(frame_header, FRAME_HEADER_SIZE);
    crc_calculator.update(packet_data, packet_size);
    crc_calculator.final(crc_result);
    // printf("crc: %08X\n", crc);

    // Concatenate all packet_data to create the full frame (byte array)
    FwSizeType fprime_frame_size = FRAME_HEADER_SIZE + packet_size + FRAME_FOOTER_SIZE;
    U8 fprime_frame[fprime_frame_size];
    // Copy header, packet_data, and CRC into the full frame
    for (FwSizeType i = 0; i < static_cast<FwSizeType>(FRAME_HEADER_SIZE); i++) {
        fprime_frame[i] = frame_header[i];
    }
    for (FwSizeType i = 0; i < static_cast<FwSizeType>(packet_size); i++) {
        fprime_frame[i + FRAME_HEADER_SIZE] = packet_data[i];
    }
    for (FwSizeType i = 0; i < static_cast<FwSizeType>(FRAME_FOOTER_SIZE); i++) {
        // crc is a U32; unpack into 4 bytes (shift by 24->-16->8->0 bits, mask with 0xFF)
        fprime_frame[i + FRAME_HEADER_SIZE + static_cast<FwSizeType>(packet_size)] =
            static_cast<U8>((crc_result.asBigEndianU32() >> (8 * (3 - i))) & 0xFF);
    }
    // Serialize frame into circular buffer
    circular_buffer.serialize(fprime_frame, fprime_frame_size);

    // Uncomment for debugging
    // printf("Serialized %llu bytes:\n", fprime_frame_size);
    // for (FwSizeType i = 0; i < static_cast<FwSizeType>(fprime_frame_size); i++) {
    //     printf("%02X ", fprime_frame[i]);
    // }
    return fprime_frame_size;
}

TEST(FprimeFrameDetector, TestBufferTooSmall) {
    Svc::FrameDetectors::FprimeFrameDetector fprime_detector;
    U8 buffer[CIRCULAR_BUFFER_TEST_SIZE];
    ::memset(buffer, 0, CIRCULAR_BUFFER_TEST_SIZE);
    Types::CircularBuffer circular_buffer(buffer, CIRCULAR_BUFFER_TEST_SIZE);

    // Anything smaller than the size of header + trailer is invalid
    U32 minimum_valid_size =
        Svc::FprimeProtocol::FrameHeader::SERIALIZED_SIZE + Svc::FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    U32 invalid_size = STest::Random::lowerUpper(1, minimum_valid_size - 1);
    // Set the circular buffer to hold data of invalid size
    circular_buffer.serialize(buffer, invalid_size);

    Svc::FrameDetector::Status status;
    FwSizeType size_out = 0;
    status = fprime_detector.detect(circular_buffer, size_out);

    // Expect that the detector reports that more data is needed
    EXPECT_EQ(status, Svc::FrameDetector::Status::MORE_DATA_NEEDED);
    EXPECT_EQ(size_out, minimum_valid_size);
}

TEST(FprimeFrameDetector, TestFrameDetected) {
    Svc::FrameDetectors::FprimeFrameDetector fprime_detector;
    U8 buffer[CIRCULAR_BUFFER_TEST_SIZE];
    ::memset(buffer, 0, CIRCULAR_BUFFER_TEST_SIZE);
    Types::CircularBuffer circular_buffer(buffer, CIRCULAR_BUFFER_TEST_SIZE);

    FwSizeType frame_size = generate_random_fprime_frame(circular_buffer);

    Svc::FrameDetector::Status status;
    FwSizeType size_out = 0;
    status = fprime_detector.detect(circular_buffer, size_out);

    EXPECT_EQ(status, Svc::FrameDetector::Status::FRAME_DETECTED);
    EXPECT_EQ(size_out, frame_size);
}

TEST(FprimeFrameDetector, TestManyFrameDetected) {
    U32 MAX_ITERS = 1000;
    Svc::FrameDetectors::FprimeFrameDetector fprime_detector;
    U8 buffer[CIRCULAR_BUFFER_TEST_SIZE];
    ::memset(buffer, 0, CIRCULAR_BUFFER_TEST_SIZE);
    Types::CircularBuffer circular_buffer(buffer, CIRCULAR_BUFFER_TEST_SIZE);

    for (U32 i = 0; i < MAX_ITERS; i++) {
        FwSizeType frame_size = generate_random_fprime_frame(circular_buffer);
        Svc::FrameDetector::Status status;
        FwSizeType size_out = 0;
        status = fprime_detector.detect(circular_buffer, size_out);

        EXPECT_EQ(status, Svc::FrameDetector::Status::FRAME_DETECTED);
        EXPECT_EQ(size_out, frame_size);
        circular_buffer.rotate(size_out);  // clear up used data
    }
}

TEST(FprimeFrameDetector, TestNoFrameDetected) {
    Svc::FrameDetectors::FprimeFrameDetector fprime_detector;
    U8 buffer[CIRCULAR_BUFFER_TEST_SIZE];
    ::memset(buffer, 0, CIRCULAR_BUFFER_TEST_SIZE);
    Types::CircularBuffer circular_buffer(buffer, CIRCULAR_BUFFER_TEST_SIZE);

    (void)generate_random_fprime_frame(circular_buffer);
    // Remove 1 byte from the beginning of the frame, making it invalid
    circular_buffer.rotate(1);

    Svc::FrameDetector::Status status;
    FwSizeType unused = 0;
    status = fprime_detector.detect(circular_buffer, unused);

    EXPECT_EQ(status, Svc::FrameDetector::Status::NO_FRAME_DETECTED);
}

TEST(FprimeFrameDetector, TestMoreDataNeeded) {
    Svc::FrameDetectors::FprimeFrameDetector fprime_detector;
    U8 buffer[CIRCULAR_BUFFER_TEST_SIZE];
    ::memset(buffer, 0, CIRCULAR_BUFFER_TEST_SIZE);
    Types::CircularBuffer circular_buffer(buffer, CIRCULAR_BUFFER_TEST_SIZE);

    (void)generate_random_fprime_frame(circular_buffer);
    // Remove 1 byte from the end of the frame to trigger "more data needed"
    Types::CircularBufferTester::tester_m_allocated_size_decrement(circular_buffer);

    Svc::FrameDetector::Status status;
    FwSizeType unused = 0;
    status = fprime_detector.detect(circular_buffer, unused);

    EXPECT_EQ(status, Svc::FrameDetector::Status::MORE_DATA_NEEDED);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
