#include <gtest/gtest.h>
#include <Fw/Tlm/TlmPacket.hpp>
#include <Fw/Com/ComBuffer.hpp>

TEST(FwTlmTest,TlmPacketSerialize) {

    // Serialize data

    Fw::TlmPacket pktIn;
    Fw::TlmBuffer buffIn;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffIn.serialize(static_cast<U32>(12)));
    Fw::Time timeIn(TB_WORKSTATION_TIME,10,11);

    pktIn.setId(10);
    pktIn.setTimeTag(timeIn);
    pktIn.setTlmBuffer(buffIn);

    Fw::ComBuffer comBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.serialize(pktIn));

    // Deserialize data
    Fw::TlmPacket pktOut;
    Fw::TlmBuffer buffOut;
    Fw::Time timeOut(TB_WORKSTATION_TIME,10,11);

    ASSERT_EQ(Fw::FW_SERIALIZE_OK,comBuff.deserialize(pktOut));
    ASSERT_EQ(pktOut.getId(),10u);
    ASSERT_EQ(pktOut.getTimeTag(),timeOut);
    U32 valOut = 0;
    buffOut = pktOut.getTlmBuffer();
    buffOut.resetDeser();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffOut.deserialize(valOut));
    ASSERT_EQ(valOut,12u);

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
