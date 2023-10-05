/**
 * GroundInterfaceRules.cpp:
 *
 * This file specifies Rule classes for testing of the Svc::GroundInterface. These rules can then be used by the main
 * testing program to test the code.
 *
 *
 * @author mstarch
 */
#include "GroundInterfaceRules.hpp"
#include <STest/Pick/Pick.hpp>

namespace Svc {
    // Constructor
    RandomizeRule :: RandomizeRule(const Fw::String& name) : STest::Rule<GroundInterfaceTester>(name.toChar()) {}

    // Can always randomize
    bool RandomizeRule :: precondition(const Svc::GroundInterfaceTester &state) {
        return true;
    }
    // Randomize
    void RandomizeRule :: action(Svc::GroundInterfaceTester &state) {
        state.m_uplink_type = STest::Pick::lowerUpper(0, 1);
        state.m_uplink_size = STest::Pick::lowerUpper(0, sizeof(state.m_uplink_data) - 1);
        // Reset or not sent allows randomization
        if (state.m_uplink_type == 2 || state.m_uplink_point == 0) {
            state.m_uplink_type = STest::Pick::lowerUpper(0, 1);
            state.m_uplink_used = STest::Pick::lowerUpper(4, FW_COM_BUFFER_MAX_SIZE);
            state.m_uplink_com_type = static_cast<Fw::ComPacket::ComPacketType>(
                    STest::Pick::lowerUpper(Fw::ComPacket::FW_PACKET_COMMAND,Fw::ComPacket::FW_PACKET_FILE));
            for (U32 i = 0; i < state.m_uplink_used; i++) {
                state.m_uplink_data[(sizeof(TOKEN_TYPE) * 2) + i] =
                        static_cast<U8>(STest::Pick::lowerUpper(0, 255));
            }
        }
        // Correct header info for items
        state.update_header_info();
    }


    // Constructor
    DownlinkRule :: DownlinkRule(const Fw::String& name) : STest::Rule<GroundInterfaceTester>(name.toChar()) {}

    // Can always downlink
    bool DownlinkRule :: precondition(const Svc::GroundInterfaceTester &state) {
        return true;
    }
    // Pick a rule and downlink
    void DownlinkRule :: action(Svc::GroundInterfaceTester &state) {
        Fw::ComBuffer buffer;
        // Force a U32 to know the size
        buffer.serialize(static_cast<U32>(0xdeadbeef));
        state.setInputParams(sizeof(U32), buffer.getBuffAddr());
        state.invoke_to_downlinkPort(0, buffer, 0);
        state.assert_from_write_size(__FILE__, __LINE__, 1);
        state.clearFromPortHistory();
    }

    // Constructor
    FileDownlinkRule :: FileDownlinkRule(const Fw::String& name) : STest::Rule<GroundInterfaceTester>(name.toChar()) {}

    // Can always downlink
    bool FileDownlinkRule :: precondition(const Svc::GroundInterfaceTester &state) {
        return true;
    }
    // Pick a rule and downlink
    void FileDownlinkRule :: action(Svc::GroundInterfaceTester &state) {
        U8 data_holder[2048];
        Fw::Buffer buffer(data_holder, sizeof(data_holder));
        Fw::ExternalSerializeBuffer buffer_wrapper(buffer.getData(), buffer.getSize());
        // Force a U32 to know the size
        buffer_wrapper.serialize(static_cast<U32>(0xdeadbeef));
        buffer.setSize(buffer_wrapper.getBuffLength());
        state.setInputParams(sizeof(U32) + sizeof(TOKEN_TYPE), buffer.getData(),
                             Fw::ComPacket::FW_PACKET_FILE);
        state.invoke_to_fileDownlinkBufferSendIn(0, buffer);
        state.assert_from_write_size(__FILE__, __LINE__, 1);
        state.assert_from_fileDownlinkBufferSendOut_size(__FILE__, __LINE__, 1);
        state.clearFromPortHistory();
    }

    // Constructor
    SendAvailableRule :: SendAvailableRule(const Fw::String& name) : STest::Rule<GroundInterfaceTester>(name.toChar()) {}

    // Can always downlink
    bool SendAvailableRule :: precondition(const Svc::GroundInterfaceTester &state) {
        return true;
    }

    // Pick a rule and downlink
    void SendAvailableRule :: action(Svc::GroundInterfaceTester &state) {
        U32 total_used = state.m_uplink_used + HEADER_SIZE + sizeof(U32);
        U8* up_ptr = state.m_uplink_data + state.m_uplink_point;
        U32 size = ((state.m_uplink_point + state.m_uplink_size) >= total_used) ?
                    (total_used - state.m_uplink_point) : state.m_uplink_size;
        state.m_uplink_point = (state.m_uplink_point + size >= total_used) ? 0 : (state.m_uplink_point + size);
        Fw::Buffer buffer(up_ptr, size);
        // Uplink based on uplink type
        if (state.m_uplink_type) {
            state.invoke_to_readCallback(0,buffer);
        } else {
            state.m_incoming_buffer = buffer;
            state.invoke_to_schedIn(0, 0);
        }
        // Finished current uplink, check that the uplink was handled
        if (state.m_uplink_point == 0 && size > 0) {
            // Check uplink type
            if (state.m_uplink_com_type == Fw::ComPacket::FW_PACKET_COMMAND) {
                state.assert_from_uplinkPort_size(__FILE__, __LINE__, 1);
            } else if (state.m_uplink_com_type == Fw::ComPacket::FW_PACKET_FILE) {
                state.assert_from_fileUplinkBufferSendOut_size(__FILE__, __LINE__, 1);
            }
            state.clearFromPortHistory();
        } else {
            state.assert_from_uplinkPort_size(__FILE__, __LINE__, 0);
        }
    }
};
