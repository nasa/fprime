// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/FilePacket/FilePacket.hpp>
#include <Fw/FilePacket/GTest/FilePackets.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

  // Serialize and deserialize a file packet header
  TEST(FilePacket, Header) {
    const FilePacket::Header expected = {
      FilePacket::T_DATA, // Packet type
      10 // Sequence number
    };
    const U32 size = expected.bufferSize();
    U8 bytes[size];
    SerialBuffer serialBuffer(bytes, size);
    {
      const SerializeStatus status = 
        expected.toSerialBuffer(serialBuffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    FilePacket::Header actual;
    {
      const SerializeStatus status = 
        actual.fromSerialBuffer(serialBuffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    GTest::FilePackets::Header::compare(
        expected, 
        actual
    );
  }


  // Serialize and deserialize a start packet
  TEST(FilePacket, StartPacket) {
    FilePacket::StartPacket expected;
    expected.initialize(
        10, // File size
        "source", // Source path
        "dest" // Destination path
    );
    const U32 size = expected.bufferSize();
    U8 bytes[size];
    Buffer buffer(bytes, size);
    SerialBuffer serialBuffer(bytes, size);
    {
      const SerializeStatus status = 
        expected.toBuffer(buffer);
      ASSERT_EQ(status, FW_SERIALIZE_OK);
    }
    FilePacket actual;
    {
      const SerializeStatus status = 
        actual.fromBuffer(buffer);
      ASSERT_EQ(status, FW_SERIALIZE_OK);
    }
    const FilePacket::StartPacket& actualStartPacket =
      actual.asStartPacket();
    GTest::FilePackets::StartPacket::compare(
        expected, 
        actualStartPacket
    );
  }

  // Serialize and deserialize a data packet
  TEST(FilePacket, DataPacket) {
    FilePacket::DataPacket expected;
    const U32 dataSize = 10;
    U8 data[dataSize] = {}; // Initialize to appease valgrind
    expected.initialize(
        3, // Sequence index
        42, // Byte offset
        dataSize, // Data size
        data // Data
    );
    const U32 size = expected.bufferSize();
    U8 bytes[size];
    Buffer buffer(bytes, size);
    SerialBuffer serialBuffer(bytes, size);
    {
      const SerializeStatus status = 
        expected.toBuffer(buffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    FilePacket actual;
    {
      const SerializeStatus status = 
        actual.fromBuffer(buffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    const FilePacket::DataPacket& actualDataPacket =
      actual.asDataPacket();
    GTest::FilePackets::DataPacket::compare(
        expected, 
        actualDataPacket
    );
  }

  // Serialize and deserialize an end packet
  TEST(FilePacket, EndPacket) {
    FilePacket::EndPacket expected;
    const CFDP::Checksum checksum(42);
    expected.initialize(
        15, // Sequence index
        checksum // Checksum
    );
    const U32 size = expected.bufferSize();
    U8 bytes[size];
    Buffer buffer(bytes, size);
    SerialBuffer serialBuffer(bytes, size);
    {
      const SerializeStatus status = 
        expected.toBuffer(buffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    FilePacket actual;
    {
      const SerializeStatus status = 
        actual.fromBuffer(buffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    const FilePacket::EndPacket& actualEndPacket =
      actual.asEndPacket();
    GTest::FilePackets::EndPacket::compare(
        expected, 
        actualEndPacket
    );
  }

  // Serialize and deserialize an end packet
  TEST(FilePacket, CancelPacket) {
    FilePacket::CancelPacket expected;
    const CFDP::Checksum checksum(42);
    expected.initialize(
        10 // Sequence index
    );
    const U32 size = expected.bufferSize();
    U8 bytes[size];
    Buffer buffer(bytes, size);
    SerialBuffer serialBuffer(bytes, size);
    {
      const SerializeStatus status = 
        expected.toBuffer(buffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    FilePacket actual;
    {
      const SerializeStatus status = 
        actual.fromBuffer(buffer);
      FW_ASSERT(status == FW_SERIALIZE_OK);
    }
    const FilePacket::CancelPacket& actualCancelPacket =
      actual.asCancelPacket();
    GTest::FilePackets::CancelPacket::compare(
        expected, 
        actualCancelPacket
    );
  }

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

