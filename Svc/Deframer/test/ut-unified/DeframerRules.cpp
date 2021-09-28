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

    void update_header_info(Tester::UplinkData& data, U32 garbage_index, U8 garbage_byte) {
        const U32 full_size = FP_FRAME_HEADER_SIZE + data.size + sizeof(U32);
        data.full_size = full_size;
        // Write token types
        for (U32 i = 0; i < sizeof(FP_FRAME_TOKEN_TYPE); i++) {
            data.data[i] = (FprimeFraming::START_WORD >> ((sizeof(FP_FRAME_TOKEN_TYPE) - 1 - i) * 8)) & 0xFF;
            data.data[i + sizeof(FP_FRAME_TOKEN_TYPE)] =
                    (data.size >> ((sizeof(FP_FRAME_TOKEN_TYPE) - 1 - i) * 8)) & 0xFF;
        }
        // Packet type
        data.data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 0] = (static_cast<U32>(data.type) >> 24) & 0xFF;
        data.data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 1] = (static_cast<U32>(data.type) >> 16) & 0xFF;
        data.data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 2] = (static_cast<U32>(data.type) >> 8) & 0xFF;
        data.data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 3] = (static_cast<U32>(data.type) >> 0) & 0xFF;

        Utils::Hash hash;
        Utils::HashBuffer hashBuffer;
        hash.update(data.data,  FP_FRAME_HEADER_SIZE + data.size);
        hash.final(hashBuffer);

        for (U32 i = 0; i < sizeof(U32); i++) {
            data.data[i + full_size - sizeof(U32)] = hashBuffer.getBuffAddr()[i] & 0xFF;
        }
        if (data.corrupted) {
            data.data[garbage_index] += garbage_byte;
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
        for (U32 j = 0; j < STest::Pick::lowerUpper(1, 100); j++) {
            Tester::UplinkData data;
            ::memset(&data, 0, sizeof(data));
            data.partial = 0;
            data.size = STest::Pick::lowerUpper(4, sizeof(data.data) - FP_FRAME_HEADER_SIZE - sizeof(U32) - 1);
            data.type = static_cast<Fw::ComPacket::ComPacketType>(
                    STest::Pick::lowerUpper(Fw::ComPacket::FW_PACKET_COMMAND, Fw::ComPacket::FW_PACKET_FILE));
            // Fill in randomized data
            U32 i = 0;
            for (i = 0; i < data.size; i++) {
                data.data[FP_FRAME_HEADER_SIZE + i] = static_cast<U8>(STest::Pick::lowerUpper(0, 255));
            }
            // Garbage may only be set if we refreshed the data
            data.corrupted = !STest::Pick::lowerUpper(0, 9); // One part failure in 10
            // Correct header info for items
            update_header_info(data, STest::Pick::lowerUpper(0, sizeof(U32) + FP_FRAME_HEADER_SIZE + data.size - 1),
                               STest::Pick::lowerUpper(1, 255));
            state.m_sending.push_back(data);
        }
    }

    // Constructor
    SendAvailableRule :: SendAvailableRule(const Fw::String& name) : STest::Rule<Tester>(name.toChar()) {}

    // Can always downlink
    bool SendAvailableRule :: precondition(const Svc::Tester &state) {
        return true;
    }

    // Pick a rule and downlink
    void SendAvailableRule :: action(Svc::Tester &state) {
        const U32 incoming_buffer_size = (state.m_polling) ? sizeof(state.component.m_poll_buffer) : STest::Pick::lowerUpper(1, 10240);
        U8* incoming_buffer = new U8[incoming_buffer_size];
        state.m_incoming_buffer.set(incoming_buffer, incoming_buffer_size);
        state.m_incoming_buffer.getSerializeRepr().resetSer();
        state.m_incoming_buffer.getSerializeRepr().resetDeser();
        U32 expected_com_count = 0;
        U32 expected_buf_count = 0;

        // Loop through all available packets
        U32 i = 0;
        while (state.m_sending.size() > 0 && i < incoming_buffer_size) {
            Tester::UplinkData data = state.m_sending.front();

            U32 available = std::min(data.full_size, (incoming_buffer_size - i));
            // Put in as much data as we can
            state.m_incoming_buffer.getSerializeRepr().serialize(data.data + data.partial, available, true);
            data.partial += available;
            i += available;

            // If we have sent the whole packet, consume it and add it to the receiving queue
            if (data.partial == data.full_size) {
                state.m_sending.pop_front();
                if (!data.corrupted) {
                    state.m_receiving.push_back(data);
                    expected_com_count += (data.type == Fw::ComPacket::FW_PACKET_COMMAND) ? 1 : 0;
                    expected_buf_count += (data.type == Fw::ComPacket::FW_PACKET_FILE) ? 1 : 0;
                }
            }
        }
        state.m_incoming_buffer.setSize(i);


        // Send based on polling or not
        if (!state.m_polling) {
            state.invoke_to_framedIn(0, state.m_incoming_buffer, Drv::RecvStatus::RECV_OK);
        } else {
            state.invoke_to_schedIn(0, 0);
        }

        // If corruption hits a size byte to a valid size, it must wait until more data.  This will flush the buffer.
        const U32 flush_buffer_size = (state.m_polling) ? sizeof(state.component.m_poll_buffer) : state.component.m_in_ring.get_capacity();
        U8* flush_buffer = new U8[flush_buffer_size];
        state.m_incoming_buffer.set(flush_buffer, incoming_buffer_size);
        state.m_incoming_buffer.getSerializeRepr().resetSer();
        state.m_incoming_buffer.getSerializeRepr().resetDeser();


        for (i = 0; i < flush_buffer_size; i++) {
            state.m_incoming_buffer.getData()[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 255));
        }
        state.m_incoming_buffer.setSize(flush_buffer_size);
        // Flush it
        if (!state.m_polling) {
            state.invoke_to_framedIn(0, state.m_incoming_buffer, Drv::RecvStatus::RECV_OK);
        } else {
            state.invoke_to_schedIn(0, 0);
        }

        state.assert_from_comOut_size(__FILE__, __LINE__, expected_com_count);
        state.assert_from_bufferOut_size(__FILE__, __LINE__, expected_buf_count);
        state.clearFromPortHistory();

    }
};
