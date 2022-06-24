#include <gtest/gtest.h>
#include <Fw/Tlm/TlmPacket.hpp>
#include <Fw/Com/ComBuffer.hpp>

TEST(FwTlmTest,TlmPacketSerializeSingle) {

    // Serialize data

    Fw::TlmPacket pktIn;
    Fw::TlmBuffer buffIn;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffIn.serialize(static_cast<U32>(12)));
    Fw::Time timeIn(TB_WORKSTATION_TIME,10,11);
    U32 id = 10;

    ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktIn.resetPktSer());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktIn.addValue(id,timeIn,buffIn));

    Fw::ComBuffer comBuff = pktIn.getBuffer();

    // Deserialize data
    Fw::TlmPacket pktOut;
    Fw::TlmBuffer buffOut;
    Fw::Time timeOut(TB_WORKSTATION_TIME,10,11);

    pktOut.setBuffer(comBuff);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktOut.resetPktDeser());
    // extract values
    id = 0;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktOut.extractValue(id,timeOut,buffOut,sizeof(U32)));

    ASSERT_EQ(10u,id);
    U32 valOut = 0;
    buffOut.resetDeser();
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffOut.deserialize(valOut));
    ASSERT_EQ(valOut,12u);

}

TEST(FwTlmTest,TlmPacketSerializeFill) {

    // compute a single entry size assuming for the test that the value of the telemetry channel
    // is a U32
    static const NATIVE_UINT_TYPE SIZE_OF_ENTRY = sizeof(FwChanIdType) + Fw::Time::SERIALIZED_SIZE + sizeof(U32);

    // compute the number of entries that should fit - will equal rounded down value of 
    // ComBuffer size - size of telemetry packet id / size of an entry
    static const NATIVE_UINT_TYPE NUM_ENTRIES = (FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType))/SIZE_OF_ENTRY;

    Fw::TlmPacket pktIn;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktIn.resetPktSer());

    // fill a telemetry packet

    for (NATIVE_UINT_TYPE entry = 0; entry < NUM_ENTRIES; entry++) {

        // Serialize data

        Fw::TlmBuffer buffIn;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffIn.serialize(static_cast<U32>(entry)));
        Fw::Time timeIn(TB_WORKSTATION_TIME,entry+1,entry+2);
        U32 id = NUM_ENTRIES-entry;

        ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktIn.addValue(id,timeIn,buffIn));
    }

    // Next one should fail because it's full
    {
        Fw::TlmBuffer buffIn;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffIn.serialize(static_cast<U32>(12)));
        Fw::Time timeIn(TB_WORKSTATION_TIME,10,11);
        U32 id = 10;

        ASSERT_EQ(Fw::FW_SERIALIZE_NO_ROOM_LEFT,pktIn.addValue(id,timeIn,buffIn));
    }

    // Create a new packet from the ComBuffer

    Fw::ComBuffer comBuff = pktIn.getBuffer();
    Fw::TlmPacket pktOut;
    pktOut.setBuffer(comBuff);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktOut.resetPktDeser());

    // empty the packet of entries
    for (NATIVE_UINT_TYPE entry = 0; entry < NUM_ENTRIES; entry++) {
        // Deserialize data
        Fw::TlmBuffer buffOut;
        Fw::Time timeOut;
        U32 id = 0;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,pktOut.extractValue(id,timeOut,buffOut,sizeof(U32)));
        ASSERT_EQ(NUM_ENTRIES-entry,id);
        Fw::Time expTime(TB_WORKSTATION_TIME,entry+1,entry+2);
        ASSERT_EQ(expTime,timeOut);
        U32 val = 0;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK,buffOut.deserialize(val));
        ASSERT_EQ(entry,val);
    }
    
    // try to extract one more, should fail
    {
        Fw::TlmBuffer buffOut;
        Fw::Time timeOut;
        U32 id = 0;
        ASSERT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY,pktOut.extractValue(id,timeOut,buffOut,sizeof(U32)));
    }

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
