/**
 * DeframerRules.cpp:
 *
 * This file specifies Rule classes for testing of the Svc::Deframer. These rules can then be used by the main
 * testing program to test the code.
 *
 *
 * @author mstarch
 */
#include "DeframerRules.hpp"
#include <STest/Pick/Pick.hpp>
#include <Utils/Hash/Hash.hpp>

namespace Svc {

    void update_header_info(Tester::UplinkFrame& frame) {
        const U32 full_size = FpFrameHeader::SIZE + frame.size + sizeof(U32);
        frame.full_size = full_size;
        // Write token types
        for (U32 i = 0; i < sizeof(FpFrameHeader::TokenType); i++) {
            frame.data[i] = (FpFrameHeader::START_WORD >> ((sizeof(FpFrameHeader::TokenType) - 1 - i) * 8)) & 0xFF;
            frame.data[i + sizeof(FpFrameHeader::TokenType)] =
                    (frame.size >> ((sizeof(FpFrameHeader::TokenType) - 1 - i) * 8)) & 0xFF;
        }
        // Packet type
        frame.data[2 * sizeof(FpFrameHeader::TokenType) + 0] = (static_cast<U32>(frame.packetType) >> 24) & 0xFF;
        frame.data[2 * sizeof(FpFrameHeader::TokenType) + 1] = (static_cast<U32>(frame.packetType) >> 16) & 0xFF;
        frame.data[2 * sizeof(FpFrameHeader::TokenType) + 2] = (static_cast<U32>(frame.packetType) >> 8) & 0xFF;
        frame.data[2 * sizeof(FpFrameHeader::TokenType) + 3] = (static_cast<U32>(frame.packetType) >> 0) & 0xFF;

        Utils::Hash hash;
        Utils::HashBuffer hashBuffer;
        hash.update(frame.data,  FpFrameHeader::SIZE + frame.size);
        hash.final(hashBuffer);

        for (U32 i = 0; i < sizeof(U32); i++) {
            frame.data[i + full_size - sizeof(U32)] = hashBuffer.getBuffAddr()[i] & 0xFF;
        }
    }


    // Constructor
    RandomizeRule :: RandomizeRule(const Fw::String& name) : STest::Rule<Tester>(name.toChar()) {}

    // Can always randomize
    bool RandomizeRule :: precondition(const Svc::Tester &state) {
        return true;
    }
    // Randomize
    void RandomizeRule :: action(Svc::Tester &state) {
        for (U32 j = 0; j < STest::Pick::lowerUpper(1, 10); j++) {
            Tester::UplinkFrame data;
            ::memset(&data, 0xFF, sizeof(data));
            data.partial = 0;
            //data.size = STest::Pick::lowerUpper(4, sizeof(data.data) - FpFrameHeader::SIZE - sizeof(U32) - 1);
            data.size = 100;
            data.packetType = Fw::ComPacket::FW_PACKET_COMMAND;
            // Correct header info for items
            update_header_info(data);
            state.m_sending.push_back(data);
        }
    }

    // Constructor
    SendAvailableRule :: SendAvailableRule(const Fw::String& name) :
      STest::Rule<Tester>(name.toChar())
    {

    }

    // Can always uplink
    bool SendAvailableRule :: precondition(const Svc::Tester &state) {
        return true;
    }

    // Pick a rule and uplink
    void SendAvailableRule :: action(Svc::Tester &state) {
        //const U32 incoming_buffer_size = STest::Pick::lowerUpper(10, 1000);
        const U32 incoming_buffer_size = 112;
        U8* incoming_buffer = new U8[incoming_buffer_size];
        state.m_incoming_buffer.set(incoming_buffer, incoming_buffer_size);
        state.m_incoming_buffer.getSerializeRepr().resetSer();
        state.m_incoming_buffer.getSerializeRepr().resetDeser();
        U32 expected_com_count = 0;
        //U32 expected_buf_count = 0;

        // Loop through all available packets
        U32 i = 0;
        while (state.m_sending.size() > 0 && i < incoming_buffer_size) {
            Tester::UplinkFrame data = state.m_sending.front();

            U32 available = std::min(data.full_size - data.partial, (incoming_buffer_size - i));
            // Put in as much data as we can
            state.m_incoming_buffer.getSerializeRepr().serialize(
                data.data + data.partial,
                available,
                true
            );
            data.partial += available;
            i += available;

            // If we have received the whole packet, consume it and add it to the receiving queue
            if (data.partial == data.full_size) {
                state.m_sending.pop_front();
                state.m_receiving.push_back(data);
                //++expected_buf_count;
                ++expected_com_count;
            }
        }
        state.m_incoming_buffer.setSize(i);

        state.invoke_to_framedIn(0, state.m_incoming_buffer, Drv::RecvStatus::RECV_OK);

        //state.assert_from_comOut_size(__FILE__, __LINE__, expected_com_count);
        //state.assert_from_bufferOut_size(__FILE__, __LINE__, expected_buf_count);

        state.clearHistory();

    }
};
