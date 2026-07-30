// ======================================================================
// @file   CmdPacketTest.cpp
// @brief  Unit tests for Fw::CmdPacket::deserializeFrom
//
// CmdPacket has no serializeTo path in FSW (it asserts), so each test builds
// the wire bytes by hand -- [packet descriptor][opcode][args] -- and exercises
// deserializeFrom. All cases are reachable without hardware.
// ======================================================================

#include <gtest/gtest.h>
#include <cstring>

#include <Fw/Cmd/CmdPacket.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Types/Serializable.hpp>

namespace {

// Serialize a packet descriptor into an already-reset buffer.
Fw::SerializeStatus putDescriptor(Fw::SerialBufferBase& buff, Fw::ComPacketType type) {
    return buff.serializeFrom(static_cast<FwPacketDescriptorType>(type));
}

}  // namespace

// 1. A well-formed command packet round-trips to the expected opcode and args.
TEST(FwCmdPacketTest, DeserializeWellFormed) {
    const FwOpcodeType opcode = 0x1234;
    const U8 args[] = {0xDE, 0xAD, 0xBE, 0xEF};

    Fw::ComBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, putDescriptor(buff, Fw::ComPacketType::FW_PACKET_COMMAND));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(opcode));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(args, static_cast<FwSizeType>(sizeof(args)), Fw::Serialization::OMIT_LENGTH));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pkt.deserializeFrom(buff));
    ASSERT_EQ(opcode, pkt.getOpCode());
    ASSERT_EQ(static_cast<FwSizeType>(sizeof(args)), pkt.getArgBuffer().getSize());
    ASSERT_EQ(0, memcmp(pkt.getArgBuffer().getBuffAddr(), args, sizeof(args)));
}

// 2. A packet whose descriptor is not FW_PACKET_COMMAND is rejected.
TEST(FwCmdPacketTest, DeserializeWrongDescriptor) {
    Fw::ComBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, putDescriptor(buff, Fw::ComPacketType::FW_PACKET_TELEM));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<FwOpcodeType>(0x1234)));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_DESERIALIZE_TYPE_MISMATCH, pkt.deserializeFrom(buff));
}

// 3. A buffer too short for even the descriptor propagates the failure from
//    ComPacket::deserializeBase.
TEST(FwCmdPacketTest, DeserializeEmptyBuffer) {
    Fw::ComBuffer buff;  // nothing serialized

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY, pkt.deserializeFrom(buff));
}

// 4. A buffer that ends after the descriptor, before the opcode, propagates the
//    opcode deserialization failure.
TEST(FwCmdPacketTest, DeserializeTruncatedBeforeOpcode) {
    Fw::ComBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, putDescriptor(buff, Fw::ComPacketType::FW_PACKET_COMMAND));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY, pkt.deserializeFrom(buff));
}

// 4a. A field that is present but incomplete is a different failure from a field
//     that is absent: LinearBufferBase::deserializeTo returns BUFFER_EMPTY only
//     when nothing is left, and SIZE_MISMATCH when some bytes are left but too
//     few. Cases 3 and 4 above cover the absent side of that split; these two
//     cover the incomplete side, which is what a truncated uplink looks like.
TEST(FwCmdPacketTest, DeserializePartialDescriptor) {
    static_assert(sizeof(FwPacketDescriptorType) > 1,
                  "a partially present descriptor requires a multi-byte FwPacketDescriptorType");
    U8 partial[sizeof(FwPacketDescriptorType) - 1] = {};

    Fw::ComBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              buff.serializeFrom(partial, static_cast<FwSizeType>(sizeof(partial)), Fw::Serialization::OMIT_LENGTH));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_DESERIALIZE_SIZE_MISMATCH, pkt.deserializeFrom(buff));
}

// 4b. Same split, one field later: a complete descriptor followed by an opcode
//     that is short by one byte.
TEST(FwCmdPacketTest, DeserializePartialOpcode) {
    static_assert(sizeof(FwOpcodeType) > 1, "a partially present opcode requires a multi-byte FwOpcodeType");
    U8 partial[sizeof(FwOpcodeType) - 1] = {};

    Fw::ComBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, putDescriptor(buff, Fw::ComPacketType::FW_PACKET_COMMAND));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,
              buff.serializeFrom(partial, static_cast<FwSizeType>(sizeof(partial)), Fw::Serialization::OMIT_LENGTH));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_DESERIALIZE_SIZE_MISMATCH, pkt.deserializeFrom(buff));
}

// 5. A command with no arguments succeeds and yields an empty argument buffer,
//    exercising the getDeserializeSizeLeft() == 0 branch that skips copyRaw.
TEST(FwCmdPacketTest, DeserializeNoArgs) {
    const FwOpcodeType opcode = 0x42;

    Fw::ComBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, putDescriptor(buff, Fw::ComPacketType::FW_PACKET_COMMAND));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(opcode));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pkt.deserializeFrom(buff));
    ASSERT_EQ(opcode, pkt.getOpCode());
    ASSERT_EQ(static_cast<FwSizeType>(0), pkt.getArgBuffer().getSize());
}

// 6a. Arguments that exactly fill FW_CMD_ARG_BUFFER_MAX_SIZE succeed (a ComBuffer
//     holds exactly descriptor + opcode + FW_CMD_ARG_BUFFER_MAX_SIZE bytes).
TEST(FwCmdPacketTest, DeserializeArgsExactlyFill) {
    U8 args[FW_CMD_ARG_BUFFER_MAX_SIZE];
    memset(args, 0xAB, sizeof(args));

    Fw::ComBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, putDescriptor(buff, Fw::ComPacketType::FW_PACKET_COMMAND));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<FwOpcodeType>(0x7)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(args, static_cast<FwSizeType>(sizeof(args)), Fw::Serialization::OMIT_LENGTH));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pkt.deserializeFrom(buff));
    ASSERT_EQ(static_cast<FwSizeType>(FW_CMD_ARG_BUFFER_MAX_SIZE), pkt.getArgBuffer().getSize());
}

// 6b. Arguments one byte past FW_CMD_ARG_BUFFER_MAX_SIZE are rejected at the
//     copyRaw boundary. A ComBuffer cannot hold this many bytes, so an
//     oversized external buffer is used as the source.
TEST(FwCmdPacketTest, DeserializeArgsExceedMax) {
    U8 store[FW_COM_BUFFER_MAX_SIZE + 16];
    Fw::ExternalSerializeBuffer buff(store, static_cast<Fw::Serializable::SizeType>(sizeof(store)));

    U8 args[FW_CMD_ARG_BUFFER_MAX_SIZE + 1];
    memset(args, 0xCD, sizeof(args));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, putDescriptor(buff, Fw::ComPacketType::FW_PACKET_COMMAND));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(static_cast<FwOpcodeType>(0x7)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buff.serializeFrom(args, static_cast<FwSizeType>(sizeof(args)), Fw::Serialization::OMIT_LENGTH));

    Fw::CmdPacket pkt;
    ASSERT_EQ(Fw::FW_SERIALIZE_NO_ROOM_LEFT, pkt.deserializeFrom(buff));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
