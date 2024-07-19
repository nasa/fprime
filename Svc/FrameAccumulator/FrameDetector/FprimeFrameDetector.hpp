// ======================================================================
// \title  FprimeFrameDetector.hpp
// \author mstarch
// \brief  hpp file for fprime frame detector definitions
// ======================================================================
#ifndef SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR
#define SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR
#include "FpConfig.h"
#include "Svc/FrameAccumulator/FrameDetector/StartLengthCrcDetector.hpp"
namespace Svc {
namespace FrameDetectors {

//! fprime framing start word is a configurable type and matched against 0xdeadbeef as cast into the appropriate type
using FprimeStartWord = StartToken<FwFramingTokenType, static_cast<FwFramingTokenType>(0xdeadbeef)>;
//! fprime framing length is a configurable type
using FprimeLength = LengthToken<FwFramingTokenType, sizeof(FwFramingTokenType)>;
//! fprime uses a CRC32 checksum anchored at the end of the data
using FprimeChecksum = CRC<U32, 2 * sizeof(FwFramingTokenType), 0, CRC32>;

//! fprime frame detector is a start/length/crc detector using the configured fprime tokens
using FprimeFrameDetector = StartLengthCrcDetector<FprimeStartWord, FprimeLength, FprimeChecksum>;
}  // namespace FrameDetectors
}  // namespace Svc
#endif  // SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR
