//! ======================================================================
//! DeframerRules.cpp
//! @author mstarch, bocchino
//! ======================================================================

#include "DeframerRules.hpp"
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
            auto frame = Tester::UplinkFrame::random();
            state.m_sending.push_back(frame);
        }
    }

    // Constructor
    SendAvailableRule :: SendAvailableRule(const Fw::String& name) :
      STest::Rule<Tester>(name.toChar())
    {

    }

    // Can always uplink
    bool SendAvailableRule :: precondition(const Svc::Tester &state) {
        return state.m_sending.size() > 0;
    }

    // Pick a rule and uplink
    void SendAvailableRule :: action(Svc::Tester &state) {
        //const U32 incoming_buffer_size = STest::Pick::lowerUpper(10, 1000);
        const U32 incoming_buffer_size = 112 + 1;
        U8* incoming_buffer = new U8[incoming_buffer_size];
        state.m_incoming_buffer.set(incoming_buffer, incoming_buffer_size);
        state.m_incoming_buffer.getSerializeRepr().resetSer();
        state.m_incoming_buffer.getSerializeRepr().resetDeser();
        U32 expected_com_count = 0;
        //U32 expected_buf_count = 0;

        // Loop through all available frames
        U32 i = 0;
        while (state.m_sending.size() > 0 && i < incoming_buffer_size) {
            auto& frame = state.m_sending.front();

            // Compute the amount of frame data to copy
            const U32 buffAvailable = incoming_buffer_size - i;
            const U32 frameAvailable = frame.getSize() - frame.copyOffset;
            const U32 copyAmt = std::min(frameAvailable, buffAvailable);

            // Copy the frame data
            state.m_incoming_buffer.getSerializeRepr().serialize(
                frame.data + frame.copyOffset,
                copyAmt,
                true
            );
            frame.copyOffset += copyAmt;
            i += copyAmt;

            // If we have received the whole frame, consume it and
            // add it to the receiving queue
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
