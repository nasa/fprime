//! ======================================================================
//! DeframerRules.cpp
//! @author mstarch, bocchino
//! ======================================================================

#include "DeframerRules.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "STest/Pick/Pick.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

    GenerateFrames :: GenerateFrames() :
        STest::Rule<Tester>("GenerateFrames")
    {
    
    }

    bool GenerateFrames :: precondition(const Svc::Tester &state) {
        return state.m_framesToSend.size() == 0;
    }

    void GenerateFrames :: action(Svc::Tester &state) {
        for (U32 j = 0; j < STest::Pick::lowerUpper(1, 10); j++) {
            // Generate a random frame
            auto frame = Tester::UplinkFrame::random();
            // Push it on the sending list
            state.m_framesToSend.push_back(frame);
        }
    }

    SendBuffer :: SendBuffer() :
        STest::Rule<Tester>("SendBuffer")
    {

    }

    bool SendBuffer :: precondition(const Svc::Tester &state) {
        return state.m_framesToSend.size() > 0;
    }

    void SendBuffer :: action(Svc::Tester &state) {
        const U32 incoming_buffer_size = STest::Pick::lowerUpper(
            1,
            DeframerCfg::POLL_BUFFER_SIZE
        );
        U8* incoming_buffer = new U8[incoming_buffer_size];
        state.m_incoming_buffer = Fw::Buffer(incoming_buffer, incoming_buffer_size);
        Fw::SerialBuffer serialBuffer(incoming_buffer, incoming_buffer_size);

        U32 expectedComCount = 0;
        U32 expectedBufCount = 0;

        // The number of bytes copied into the buffer
        U32 copiedSize = 0;
        // The size of available data in the buffer
        U32 buffAvailable = incoming_buffer_size;
        // Loop through available frames
        while (state.m_framesToSend.size() > 0 && copiedSize < incoming_buffer_size) {
            auto& frame = state.m_framesToSend.front();

            // Compute the amount of frame data to copy
            const U32 frameSize = frame.getSize();
            ASSERT_GE(frameSize, frame.copyOffset);
            const U32 frameAvailable = frameSize - frame.copyOffset;
            const U32 copyAmt = std::min(frameAvailable, buffAvailable);

            // Copy the frame data
            auto status = serialBuffer.pushBytes(
                &frame.data[frame.copyOffset],
                copyAmt
            );
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

            // Update the indices
            frame.copyOffset += copyAmt;
            buffAvailable -= copyAmt;
            copiedSize += copyAmt;

            // If we have received an entire frame, remove it from
            // the sending queue
            if (frame.copyOffset == frameSize) {
                state.m_framesToSend.pop_front();
                // If the frame is valid, push it on the receiving queue
                if (frame.valid) {
                    state.m_receiving.push_back(frame);
                }
                // If the frame contains a command packet, increment the expected
                // com count
                if (frame.packetType == Fw::ComPacket::FW_PACKET_COMMAND) {
                    ++expectedComCount;
                }
                // If the frame contains a file packet, increment the expected
                // buffer count
                if (frame.packetType == Fw::ComPacket::FW_PACKET_FILE) {
                    ++expectedBufCount;
                }
            }
        }

        // Update the buffer
        state.m_incoming_buffer.setSize(copiedSize);

        // Send the buffer
        state.invoke_to_framedIn(
            0,
            state.m_incoming_buffer,
            Drv::RecvStatus::RECV_OK
        );

        state.assert_from_comOut_size(__FILE__, __LINE__, expectedComCount);
        state.assert_from_bufferOut_size(__FILE__, __LINE__, expectedBufCount);

        state.clearHistory();

    }

};
