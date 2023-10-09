//! ======================================================================
//! \title GenerateFrames.cpp
//! \brief Implementation file for GenerateFrames rule
//! \author mstarch, bocchino
//! ======================================================================

#include "GenerateFrames.hpp"
#include "Printing.hpp"
#include "STest/Pick/Pick.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

    GenerateFrames :: GenerateFrames() :
        STest::Rule<DeframerTester>("GenerateFrames")
    {

    }

    bool GenerateFrames :: precondition(const Svc::DeframerTester &state) {
        return state.m_framesToSend.size() == 0;
    }

    void GenerateFrames :: action(Svc::DeframerTester &state) {
        PRINT("----------------------------------------------------------------------");
        PRINT("GenerateFrames action");
        PRINT("----------------------------------------------------------------------");
        // Generate 1-100 frames
        const U32 numFrames = STest::Pick::lowerUpper(1, 100);
        PRINT_ARGS("Generating %d frames", numFrames)
        for (U32 i = 0; i < numFrames; i++) {
            // Generate a random frame
            DeframerTester::UplinkFrame frame = DeframerTester::UplinkFrame::random();
            // Push it on the sending list
            state.m_framesToSend.push_back(frame);
        }
        PRINT_ARGS("frameToSend.size()=%lu", state.m_framesToSend.size())
    }

}
