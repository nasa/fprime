/**
 * CircularState.hpp:
 *
 * Setup a fake logger for use with the testing. This allows for the capture of messages from the system and ensure that
 * the proper log messages are coming through as expected.
 *
 * @author mstarch
 */

#include <Fw/FPrimeBasicTypes.hpp>
#include <Utils/Types/CircularBuffer.hpp>

#ifndef FPRIME_CIRCULARSTATE_HPP
#define FPRIME_CIRCULARSTATE_HPP

#define MAX_BUFFER_SIZE 10240

namespace MockTypes {

    class CircularState {
        public:
            // Constructor
            CircularState();
            // Destructor
            ~CircularState();
            /**
             * Generates a random buffer for input to various calls to the CircularBuffer.
             * @return size of this buffer
             */
            FwSizeType generateRandomBuffer();
            /**
             * Sets the random settings
             * @param random: random size
             * @param peek_type: peek type (0-3)
             * @param peek_offset: offset size
             */
            void setRandom(FwSizeType random, FwSizeType peek_type, FwSizeType peek_offset);
            /**
             * Add to the infinite pool of data.
             * @return true if successful, false otherwise
             */
            bool addInfinite(const U8* buffer, FwSizeType size);
            /**
             * Grab a peek buffer for given size and offset.
             * @return true if successful, false if cannot.
             */
            bool peek(U8*& buffer, FwSizeType size, FwSizeType offset = 0);
            /**
             * Rotate the circular buffer.
             * @param size: size to rotate
             * @return true if successful, false otherwise
             */
            bool rotate(FwSizeType size);
            /**
             * Get the size of the random buffer data.
             * @return size of the buffer
             */
            FwSizeType getRandomSize() const;
            /**
             * Get the size of the random buffer data.
             * @return size of the buffer
             */
            FwSizeType getPeekOffset() const;
            /**
             * Get the size of the random buffer data.
             * @return size of the buffer
             */
            FwSizeType getPeekType() const;
            /**
             * Gets a pointer to the random buffer.
             * @return random buffer storing data
             */
            const U8 *getBuffer() const;
            /**
             * Get the remaining size of the circular buffer. This is a shadow field.
             * @return shadow field for circular buffer.
             */
            FwSizeType getRemainingSize() const;
            /**
             * Set the remaining size shadow field input.
             * @param mRemainingSize: remaining size shadow field
             */
            void setRemainingSize(FwSizeType mRemainingSize);
            /**
             * Get the in-test circular buffer.
             * @return in-test circular buffer
             */
            Types::CircularBuffer& getTestBuffer();

            /**
             * Check allocated and free sizes
             */
            void checkSizes() const;

        private:
            FwSizeType m_remaining_size;
            FwSizeType m_random_size;
            FwSizeType m_peek_offset;
            FwSizeType m_peek_type;

            U8 m_buffer[MAX_BUFFER_SIZE];
            // May use just under 100MB of space
            U8* m_infinite_store;
            FwSizeType m_infinite_read;
            FwSizeType m_infinite_write;
            FwSizeType m_infinite_size;

            Types::CircularBuffer m_test_buffer;
    };

}
#endif //FPRIME_CIRCULARSTATE_HPP
