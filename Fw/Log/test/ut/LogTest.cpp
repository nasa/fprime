#include <gtest/gtest.h>
#include <Fw/Log/LogPacket.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Log/LogString.hpp>

TEST(FwLogTest,LogPacketSerialize) {

    // Serialize data

    Fw::LogPacket pktIn;
    Fw::LogBuffer buffIn;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffIn.serialize(static_cast<U32>(12)));
    Fw::Time timeIn(TB_WORKSTATION_TIME,10,11);

    pktIn.setId(10);
    pktIn.setTimeTag(timeIn);
    pktIn.setLogBuffer(buffIn);

    Fw::ComBuffer comBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize(pktIn));

    // Deserialize data
    Fw::LogPacket pktOut;
    Fw::LogBuffer buffOut;
    Fw::Time timeOut(TB_WORKSTATION_TIME,10,11);

    ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.deserialize(pktOut));
    ASSERT_EQ(pktOut.getId(),10u);
    ASSERT_EQ(pktOut.getTimeTag(),timeOut);
    U32 valOut = 0;
    buffOut = pktOut.getLogBuffer();
    buffOut.resetDeser();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffOut.deserialize(valOut));
    ASSERT_EQ(valOut,12u);

    // serialize string
    Fw::LogStringArg str1;
    Fw::LogStringArg str2;

    str1 = "Foo";
    buffOut.resetSer();
    str1.serialize(buffOut);
    str2.deserialize(buffOut);
    ASSERT_EQ(str1,str2);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
