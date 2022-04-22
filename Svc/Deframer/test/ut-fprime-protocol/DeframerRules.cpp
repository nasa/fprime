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
        // Generate 1-100 frames
        for (U32 j = 0; j < STest::Pick::lowerUpper(1, 100); j++) {
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

        // Clear history
        state.clearHistory();

        // Set up the incoming buffer
        const U32 incomingBufferSize = STest::Pick::lowerUpper(
            1,
            sizeof state.m_incomingBufferBytes
        );
        ASSERT_LE(incomingBufferSize, sizeof state.m_incomingBufferBytes);
        state.m_incomingBuffer = Fw::Buffer(
            state.m_incomingBufferBytes,
            incomingBufferSize
        );

        // Set up a serial buffer for data transfer
        Fw::SerialBuffer serialBuffer(
            state.m_incomingBufferBytes,
            incomingBufferSize
        );

        // The expected number of com buffers emitted
        U32 expectedComCount = 0;
        // The expected number of file packet buffers emitted
        U32 expectedBuffCount = 0;
        // The number of bytes copied into the buffer
        U32 copiedSize = 0;
        // The size of available data in the buffer
        U32 buffAvailable = incomingBufferSize;

        // Fill the incoming buffer as much as possible with available frames
        for (U32 i = 0; i < incomingBufferSize; ++i) {

            // Check if there is anything to send
            if (state.m_framesToSend.size() == 0) {
                break;
            }

            // Get the frame from the head of the sending queue
            auto& frame = state.m_framesToSend.front();

            // Compute the amount to copy
            const U32 frameAvailable = frame.getRemainingCopySize();
            const U32 copyAmt = std::min(frameAvailable, buffAvailable);

            // Check if there is anything to copy
            if (copyAmt == 0) {
                break;
            }

            // Copy the frame data
            auto status = serialBuffer.pushBytes(
                &frame.data[frame.copyOffset],
                copyAmt
            );
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

            // Update the indices
            frame.copyOffset += copyAmt;
            ASSERT_GE(buffAvailable, copyAmt);
            buffAvailable -= copyAmt;
            copiedSize += copyAmt;

            // If we have received an entire frame, then remove it from
            // the send queue
            if (frame.copyOffset == frame.getSize()) {
                state.m_framesToSend.pop_front();
                // If the frame is valid, push it on the received queue
                if (frame.valid) {
                    state.m_framesReceived.push_back(frame);
                }
                // If the frame contains a command packet, then increment the expected
                // com count
                if (frame.packetType == Fw::ComPacket::FW_PACKET_COMMAND) {
                    ++expectedComCount;
                }
                // If the frame contains a file packet, then increment the expected
                // buffer count
                if (frame.packetType == Fw::ComPacket::FW_PACKET_FILE) {
                    ++expectedBuffCount;
                }
            }

        }

        // Update the buffer
        state.m_incomingBuffer.setSize(copiedSize);

        // Send the buffer
        state.invoke_to_framedIn(
            0,
            state.m_incomingBuffer,
            Drv::RecvStatus::RECV_OK
        );

        // Check the counts
        state.assert_from_comOut_size(__FILE__, __LINE__, expectedComCount);
        state.assert_from_bufferOut_size(__FILE__, __LINE__, expectedBuffCount);

    }

};
