// ======================================================================
// \title  CircularBufferTester.hpp
// \author m-aleem
// \brief  hpp file for CircularBufferTester
// ======================================================================

#ifndef Svc_CircularBufferTester_HPP
#define Svc_CircularBufferTester_HPP

#include "Utils/Types/CircularBuffer.hpp"

namespace Types {

    class CircularBufferTester{

        public:

            static void tester_m_allocated_size_decrement(Types::CircularBuffer &circular_buffer){
                circular_buffer.m_allocated_size--;
            }

            static FwSizeType tester_get_m_head_idx(Types::CircularBuffer &circular_buffer){
                return circular_buffer.m_head_idx;
            }

    };

}

#endif
