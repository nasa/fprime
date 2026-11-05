#include <gtest/gtest.h>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Log/LogPacket.hpp>
#include <Fw/Log/LogString.hpp>

TEST(FwLogTest, LogPacketSerialize) {
    // Serialize data

    Fw::LogPacket pktIn;
    Fw::LogBuffer buffIn;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffIn.serializeFrom(static_cast<U32>(12)));
    Fw::Time timeIn(TimeBase::TB_WORKSTATION_TIME, 10, 11);

    pktIn.setId(10);
    pktIn.setTimeTag(timeIn);
    pktIn.setLogBuffer(buffIn);

    Fw::ComBuffer comBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(pktIn));

    // Deserialize data
    Fw::LogPacket pktOut;
    Fw::LogBuffer buffOut;
    Fw::Time timeOut(TimeBase::TB_WORKSTATION_TIME, 10, 11);

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.deserializeTo(pktOut));
    ASSERT_EQ(pktOut.getId(), 10u);
    ASSERT_EQ(pktOut.getTimeTag(), timeOut);
    U32 valOut = 0;
    buffOut = pktOut.getLogBuffer();
    buffOut.resetDeser();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffOut.deserializeTo(valOut));
    ASSERT_EQ(valOut, 12u);

    // serialize string
    Fw::LogStringArg str1;
    Fw::LogStringArg str2;

    str1 = "Foo";
    buffOut.resetSer();
    str1.serializeTo(buffOut);
    str2.deserializeFrom(buffOut);
    ASSERT_EQ(str1, str2);
}

TEST(FwLogTest, LogPacketDeserializeOversizeRejected) {
    // A payload larger than the log buffer capacity must be rejected, not copied
    Fw::LogPacket pktIn;
    Fw::LogBuffer buffIn;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffIn.serializeFrom(static_cast<U32>(12)));
    pktIn.setId(10);
    pktIn.setTimeTag(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 11));
    pktIn.setLogBuffer(buffIn);

    U8 data[FW_COM_BUFFER_MAX_SIZE + 128];
    Fw::ExternalSerializeBuffer bigBuff(data, sizeof(data));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, bigBuff.serializeFrom(pktIn));
    // Append filler so the remaining payload exceeds the LogBuffer capacity
    U8 filler[FW_LOG_BUFFER_MAX_SIZE] = {};
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, bigBuff.serializeFrom(filler, sizeof(filler), Fw::Serialization::OMIT_LENGTH));

    Fw::LogPacket pktOut;
    ASSERT_EQ(Fw::FW_DESERIALIZE_SIZE_MISMATCH, bigBuff.deserializeTo(pktOut));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
