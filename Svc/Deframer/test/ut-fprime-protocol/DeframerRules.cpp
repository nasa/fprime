//! ======================================================================
//! DeframerRules.cpp
//! @author mstarch, bocchino
//! ======================================================================

#include <cstdio>

#include "DeframerRules.hpp"
#include "STest/Pick/Pick.hpp"
#include "Utils/Hash/Hash.hpp"

#define PRINTING

#ifdef PRINTING
#define PRINT(S) printf("[DeframerRules] " S "\n");
#define PRINT_ARGS(S, ...) printf("[DeframerRules] " S "\n", __VA_ARGS__);
#else
#define PRINT(S)
#define PRINT_ARGS(S, ...)
#endif

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
        PRINT("----------------------------------------------------------------------");
        PRINT("GenerateFrames action");
        PRINT("----------------------------------------------------------------------");
        // Generate 1-100 frames
        const U32 numFrames = STest::Pick::lowerUpper(1, 100);
        PRINT_ARGS("Generating %d frames", numFrames)
        for (U32 i = 0; i < numFrames; i++) {
            // Generate a random frame
            auto frame = Tester::UplinkFrame::random();
            // Push it on the sending list
            state.m_framesToSend.push_back(frame);
        }
        PRINT_ARGS("frameToSend.size()=%lu", state.m_framesToSend.size())
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

        PRINT("----------------------------------------------------------------------");
        PRINT("SendBufffer action");
        PRINT("----------------------------------------------------------------------");

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

            // If we have copied a complete frame F, then (1) remove F from
            // the send queue; and (2) if F is valid, then record F as
            // received
            if (frame.getRemainingCopySize() == 0) {
                state.m_framesToSend.pop_front();
                // If F is valid, then record it as received
                if (frame.isValid()) {
                    // Push F on the received queue
                    state.m_framesReceived.push_back(frame);
                    // Update the count of expected frames
                    switch (frame.packetType) {
                        case Fw::ComPacket::FW_PACKET_COMMAND:
                            PRINT("popped valid command frame")
                            // If F contains a command packet, then increment
                            // the expected com count
                            ++expectedComCount;
                            break;
                        case Fw::ComPacket::FW_PACKET_FILE:
                            PRINT("popped valid file frame")
                            // If F contains a file packet, then increment
                            // the expected buffer count
                            ++expectedBuffCount;
                            break;
                        default:
                            // This should not happen for a valid frame
                            FW_ASSERT(0);
                            break;
                    }
                }
                else {
                    PRINT("popped invalid frame")
                }
                PRINT_ARGS(
                    "frameToSend.size()=%lu",
                    state.m_framesToSend.size()
                )
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
        PRINT_ARGS("expectedComCount=%d", expectedComCount)
        state.assert_from_bufferOut_size(__FILE__, __LINE__, expectedBuffCount);
        PRINT_ARGS("expectedBuffCount=%d", expectedBuffCount)

    }

};
