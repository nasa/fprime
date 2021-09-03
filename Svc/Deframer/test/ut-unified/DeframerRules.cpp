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
#include <stdio.h>
namespace Svc {
    // Constructor
    RandomizeRule :: RandomizeRule(const Fw::String& name) : STest::Rule<Tester>(name.toChar()) {}

    // Can always randomize
    bool RandomizeRule :: precondition(const Svc::Tester &state) {
        return true;
    }
    // Randomize
    void RandomizeRule :: action(Svc::Tester &state) {
        state.m_uplink_type = STest::Pick::lowerUpper(0, 1);
        state.m_uplink_size = STest::Pick::lowerUpper(0, sizeof(state.m_uplink_data) - 1);
        // Reset or not sent allows randomization
        if (state.m_uplink_type == 2 || state.m_uplink_point == 0) {
            state.m_uplink_type = STest::Pick::lowerUpper(0, 1);
            state.m_uplink_used = STest::Pick::lowerUpper(4, FW_COM_BUFFER_MAX_SIZE);
            state.m_uplink_com_type = static_cast<Fw::ComPacket::ComPacketType>(
                    STest::Pick::lowerUpper(Fw::ComPacket::FW_PACKET_COMMAND,Fw::ComPacket::FW_PACKET_FILE));
            U32 i = 0;
            for (i = 0; i < state.m_uplink_used; i++) {
                state.m_uplink_data[(sizeof(FP_FRAME_TOKEN_TYPE) * 2) + i] =
                        static_cast<U8>(STest::Pick::lowerUpper(0, 255));
            }
            for (; i < sizeof(state.m_uplink_data); i++) {
                state.m_uplink_data[i] = 0;
            }
            // Garbage may only be set if we refreshed the data
            state.m_garbage = !STest::Pick::lowerUpper(0, 9); // One part failure in 10
            // Correct header info for items
            state.update_header_info(STest::Pick::lowerUpper(0,sizeof(U32) + 2 * sizeof(FP_FRAME_TOKEN_TYPE) + state.m_uplink_used - 1),
                                     STest::Pick::lowerUpper(1,255));
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
        U32 total_used = state.m_uplink_used + FP_FRAME_HEADER_SIZE + sizeof(U32);
        U8* up_ptr = state.m_uplink_data + state.m_uplink_point;
        U32 size = ((state.m_uplink_point + state.m_uplink_size) >= total_used) ?
                    (total_used - state.m_uplink_point) : state.m_uplink_size;
        state.m_uplink_point = (state.m_uplink_point + size >= total_used) ? 0 : (state.m_uplink_point + size);
        Fw::Buffer buffer(up_ptr, size);
        // Uplink based on uplink type
        if (state.m_uplink_type) {
            state.invoke_to_framedIn(0, buffer, Drv::RecvStatus::RECV_OK);
        } else {
            state.m_incoming_buffer = buffer;
            state.invoke_to_schedIn(0, 0);
        }
        // Finished current uplink, check that the uplink was handled
        if (state.m_uplink_point == 0 && size > 0 && !state.m_garbage) {
            // Check uplink type
            if (state.m_uplink_com_type == Fw::ComPacket::FW_PACKET_COMMAND) {
                state.assert_from_comOut_size(__FILE__, __LINE__, 1);
            } else if (state.m_uplink_com_type == Fw::ComPacket::FW_PACKET_FILE) {
                state.assert_from_bufferOut_size(__FILE__, __LINE__, 1);
            }
            state.clearFromPortHistory();
        } else {
            state.assert_from_comOut_size(__FILE__, __LINE__, 0);
            state.assert_from_bufferOut_size(__FILE__, __LINE__, 0);
        }
    }
};
