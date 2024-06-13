// ======================================================================
// \title  FrameDetector.hpp
// \author mstarch
// \brief  hpp interface specification for FrameDetector
// ======================================================================
#ifndef FPRIME_FRAMEDETECTOR_HPP
#define FPRIME_FRAMEDETECTOR_HPP
#include <FpConfig.hpp>
#include <Utils/Types/CircularBuffer.hpp>

namespace Svc {

    //! \brief interface class used to codify what must be supported to allow frame detection
    class FrameDetector {
      public:
        //! \brief status returned from the detection step
        enum Status {
            FRAME_DETECTED,    //!< Frame detected. Extract frame and return with new data.
            NO_FRAME_DETECTED, //!< No frame detected. Discard data and return with new data.
            MORE_DATA_NEEDED   //!< More data is needed to detect a frame. Keep current data and return with more.
        };

        //! \brief detect if a frame is available within the circular buffer
        //!
        //! Function implemented by sub classes used to determine if a frame is available at the current position of the
        //! circular buffer. Implementors should detect if a frame is available, set size_out, and return a status while
        //! following these expectations:
        //!
        //!  1. FRAME_DETECTED status implies a frame is available at the current offset of the circular buffer.
        //!     size_out must be set to the size of the frame from that location.
        //!
        //!  2. NO_FRAME_DETECTED status implies no frame is possible at the current offset of the circular buffer.
        //!     e.g. no start word is found at the current offset. size_out is ignored.
        //!
        //!  3. MORE_DATA_NEEDED status implies that a frame might be possible but more data is needed before a
        //!     determination is possible. size_out must be set to the total amount of data needed.
        //!
        //!     For example, if a frame start word is 4 bytes, and 3 bytes are available in the circular buffer then the
        //!     return status would be NO_FRAME_DETECTED and size_out must be set to 4 to ensure that at least the start
        //!     word is available.
        //!
        //! \param data: circular buffer with read-only access
        //! \param size_out: set as output to caller indicating size when appropriate
        //! \return status of the detection to be pared with size_out
        virtual Status detect(const Types::CircularBuffer& data, const FwSizeType& size_out) const = 0;
    };

} // Svc

#endif //FPRIME_FRAMEDETECTOR_HPP
