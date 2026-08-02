#include <gtest/gtest.h>
#include <Fw/Cmd/CmdPacket.hpp>
#include <Fw/Com/ComBuffer.hpp>

// Helper: build a ComBuffer containing [descriptor][opcode][args...]
static Fw::ComBuffer buildPacketBuffer(FwPacketDescriptorType descriptor,
                                       FwOpcodeType opcode,
                                       const U8* args = nullptr,
                                       FwSizeType argsSize = 0) {
    Fw::ComBuffer buffer;
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(descriptor));
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(opcode));
    if (args != nullptr && argsSize > 0) {
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(args, argsSize, Fw::Serialization::OMIT_LENGTH));
    }
    return buffer;
}

TEST(FwCmdPacketTest, NominalDeserializeWithArgs) {
    const FwOpcodeType expectedOpcode = 0x123;
    const U8 expectedArgs[] = {0xDE, 0xAD, 0xBE, 0xEF};

    Fw::ComBuffer buffer = buildPacketBuffer(
        static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND), expectedOpcode, expectedArgs,
        sizeof(expectedArgs));

    Fw::CmdPacket packet;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, packet.deserializeFrom(buffer));
    ASSERT_EQ(expectedOpcode, packet.getOpCode());

    // Argument buffer should contain exactly the raw argument bytes
    Fw::CmdArgBuffer& argBuffer = packet.getArgBuffer();
    ASSERT_EQ(sizeof(expectedArgs), argBuffer.getSize());
    ASSERT_EQ(0, memcmp(expectedArgs, argBuffer.getBuffAddr(), sizeof(expectedArgs)));
}

TEST(FwCmdPacketTest, NominalDeserializeNoArgs) {
    const FwOpcodeType expectedOpcode = 0x42;

    Fw::ComBuffer buffer = buildPacketBuffer(
        static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND), expectedOpcode);

    Fw::CmdPacket packet;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, packet.deserializeFrom(buffer));
    ASSERT_EQ(expectedOpcode, packet.getOpCode());
    ASSERT_EQ(0u, packet.getArgBuffer().getSize());
}

TEST(FwCmdPacketTest, TypeMismatchReturnsError) {
    // Build a buffer with a telemetry descriptor instead of a command descriptor
    Fw::ComBuffer buffer = buildPacketBuffer(
        static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_TELEM), 0x123);

    Fw::CmdPacket packet;
    ASSERT_EQ(Fw::FW_DESERIALIZE_TYPE_MISMATCH, packet.deserializeFrom(buffer));
}

TEST(FwCmdPacketTest, EmptyBufferReturnsError) {
    Fw::ComBuffer buffer;

    Fw::CmdPacket packet;
    ASSERT_NE(Fw::FW_SERIALIZE_OK, packet.deserializeFrom(buffer));
}

TEST(FwCmdPacketTest, TruncatedBufferMissingOpcodeReturnsError) {
    // Only the descriptor is present; opcode is missing
    Fw::ComBuffer buffer;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              buffer.serializeFrom(
                  static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND)));

    Fw::CmdPacket packet;
    ASSERT_NE(Fw::FW_SERIALIZE_OK, packet.deserializeFrom(buffer));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
