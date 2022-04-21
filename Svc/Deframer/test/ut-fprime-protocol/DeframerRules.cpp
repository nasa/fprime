//! ======================================================================
//! DeframerRules.cpp
//! @author mstarch, bocchino
//! ======================================================================

#include "DeframerRules.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "STest/Pick/Pick.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

    // Constructor
    RandomizeRule :: RandomizeRule(const Fw::String& name) : STest::Rule<Tester>(name.toChar()) {}

    // Can always randomize
    bool RandomizeRule :: precondition(const Svc::Tester &state) {
        return true;
    }
    // Randomize
    void RandomizeRule :: action(Svc::Tester &state) {
        for (U32 j = 0; j < STest::Pick::lowerUpper(1, 10); j++) {
            // Generate a random frame
            auto frame = Tester::UplinkFrame::random();
            // Push it on the sending list
            state.m_sending.push_back(frame);
        }
    }

    // Constructor
    SendAvailableRule :: SendAvailableRule(const Fw::String& name) :
        STest::Rule<Tester>(name.toChar())
    {

    }

    // Uplink if there is something to send
    bool SendAvailableRule :: precondition(const Svc::Tester &state) {
        return state.m_sending.size() > 0;
    }

    // Uplink available frames that will fit in incoming buffer
    void SendAvailableRule :: action(Svc::Tester &state) {
        const U32 incoming_buffer_size = STest::Pick::lowerUpper(
            1,
            DeframerCfg::POLL_BUFFER_SIZE
        );
        U8* incoming_buffer = new U8[incoming_buffer_size];
        state.m_incoming_buffer = Fw::Buffer(incoming_buffer, incoming_buffer_size);
        Fw::SerialBuffer serialBuffer(incoming_buffer, incoming_buffer_size);

        U32 expected_com_count = 0;
        //U32 expected_buf_count = 0;

        // Loop through available frames
        U32 i = 0;
        while (state.m_sending.size() > 0 && i < incoming_buffer_size) {
            auto& frame = state.m_sending.front();

            // Compute the amount of frame data to copy
            const U32 buffAvailable = incoming_buffer_size - i;
            const U32 frameAvailable = frame.getSize() - frame.copyOffset;
            const U32 copyAmt = std::min(frameAvailable, buffAvailable);

            // Copy the frame data
            auto status = serialBuffer.pushBytes(
                &frame.data[frame.copyOffset],
                copyAmt
            );
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

            // Update the offsets
            frame.copyOffset += copyAmt;
            i += copyAmt;

            // If we have received an entire frame, move it from
            // the sending queue to the receiving queue
            if (frame.copyOffset == frame.getSize()) {
                state.m_sending.pop_front();
                state.m_receiving.push_back(frame);
                //++expected_buf_count;
                ++expected_com_count;
            }
        }
        state.m_incoming_buffer.setSize(i);

        state.invoke_to_framedIn(0, state.m_incoming_buffer, Drv::RecvStatus::RECV_OK);

        state.assert_from_comOut_size(__FILE__, __LINE__, expected_com_count);
        //state.assert_from_bufferOut_size(__FILE__, __LINE__, expected_buf_count);

        state.clearHistory();

    }
};
