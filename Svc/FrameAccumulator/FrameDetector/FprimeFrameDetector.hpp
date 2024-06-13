//
// Created by Michael Starch on 6/13/24.
//
#ifndef SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR
#define SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR
#include "Svc/FrameAccumulator/FrameDetector/StartLengthChecksumDetector.hpp"
namespace Svc {
    class FprimeFrameDetector : public StartLengthChecksumDetector {
        FprimeFrameDetector() : StartLengthChecksumDetector(
                {},
                {},
                {},
                );

    };
}
#endif //SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR