//! ======================================================================
//! DeframerRules.cpp
//! @author mstarch, bocchino
//! ======================================================================

#include <cstdio>

#include "DeframerRules.hpp"
#include "STest/Pick/Pick.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

    // ----------------------------------------------------------------------
    // GenerateFrames 
    // ----------------------------------------------------------------------

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

    // ----------------------------------------------------------------------
    // SendBuffer 
    // ----------------------------------------------------------------------

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

            // Check if there is any room left in the buffer
            if (buffAvailable == 0) {
                break;
            }

            // Check if there are any frames to send
            if (state.m_framesToSend.size() == 0) {
                break;
            }

            // Get the frame from the head of the sending queue
            auto& frame = state.m_framesToSend.front();

            // Compute the amount to copy
            const U32 frameAvailable = frame.getRemainingCopySize();
            const U32 copyAmt = std::min(frameAvailable, buffAvailable);

            // Copy the frame data into the serial buffer
            frame.copyDataOut(serialBuffer, copyAmt);

            // Update buffAvailable
            ASSERT_GE(buffAvailable, copyAmt);
            buffAvailable -= copyAmt;

            // Update copiedSize
            copiedSize += copyAmt;
            ASSERT_LE(copiedSize, incomingBufferSize);

            // If we have copied a complete frame F, then remove it from
            // the send queue
            if (frame.getRemainingCopySize() == 0) {
                state.m_framesToSend.pop_front();
                // If F is valid, then record it as received
                if (frame.isValid()) {
                    // Push F on the received queue
                    state.m_framesReceived.push_back(frame);
                    // If F contains a command packet, then increment the expected
                    // com count
                    if (frame.packetType == Fw::ComPacket::FW_PACKET_COMMAND) {
                        ++expectedComCount;
                    }
                    // If F contains a file packet, then increment the expected
                    // buffer count
                    if (frame.packetType == Fw::ComPacket::FW_PACKET_FILE) {
                        ++expectedBuffCount;
                    }
                }
            }

        }

        // Update the buffer
        ASSERT_LE(copiedSize, incomingBufferSize);
        state.m_incomingBuffer.setSize(copiedSize);

        // Send the buffer
        // TODO: Handle polling mode
        state.invoke_to_framedIn(
            0,
            state.m_incomingBuffer,
            Drv::RecvStatus::RECV_OK
        );

        // Check the counts
        state.assert_from_comOut_size(__FILE__, __LINE__, expectedComCount);
        printf("expectedComCount=%d\n", expectedComCount);
        state.assert_from_bufferOut_size(__FILE__, __LINE__, expectedBuffCount);
        printf("expectedBuffCount=%d\n", expectedBuffCount);

    }

};
