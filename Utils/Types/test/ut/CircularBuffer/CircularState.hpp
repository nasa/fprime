/**
 * CircularState.hpp:
 *
 * Setup a fake logger for use with the testing. This allows for the capture of messages from the system and ensure that
 * the proper log messages are coming through as expected.
 *
 * @author mstarch
 */

#include <FpConfig.hpp>
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
            NATIVE_UINT_TYPE generateRandomBuffer();
            /**
             * Sets the random settings
             * @param random: random size
             * @param peek_type: peek type (0-3)
             * @param peek_offset: offset size
             */
            void setRandom(NATIVE_UINT_TYPE random, NATIVE_UINT_TYPE peek_type, NATIVE_UINT_TYPE peek_offset);
            /**
             * Add to the infinite pool of data.
             * @return true if successful, false otherwise
             */
            bool addInfinite(const U8* buffer, NATIVE_UINT_TYPE size);
            /**
             * Grab a peek buffer for given size and offset.
             * @return true if successful, false if cannot.
             */
            bool peek(U8*& buffer, NATIVE_UINT_TYPE size, NATIVE_UINT_TYPE offset = 0);
            /**
             * Rotate the circular buffer.
             * @param size: size to rotate
             * @return true if successful, false otherwise
             */
            bool rotate(NATIVE_UINT_TYPE size);
            /**
             * Get the size of the random buffer data.
             * @return size of the buffer
             */
            NATIVE_UINT_TYPE getRandomSize() const;
            /**
             * Get the size of the random buffer data.
             * @return size of the buffer
             */
            NATIVE_UINT_TYPE getPeekOffset() const;
            /**
             * Get the size of the random buffer data.
             * @return size of the buffer
             */
            NATIVE_UINT_TYPE getPeekType() const;
            /**
             * Gets a pointer to the random buffer.
             * @return random buffer storing data
             */
            const U8 *getBuffer() const;
            /**
             * Get the remaining size of the circular buffer. This is a shadow field.
             * @return shadow field for circular buffer.
             */
            NATIVE_UINT_TYPE getRemainingSize() const;
            /**
             * Set the remaining size shadow field input.
             * @param mRemainingSize: remaining size shadow field
             */
            void setRemainingSize(NATIVE_UINT_TYPE mRemainingSize);
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
            NATIVE_UINT_TYPE m_remaining_size;
            NATIVE_UINT_TYPE m_random_size;
            NATIVE_UINT_TYPE m_peek_offset;
            NATIVE_UINT_TYPE m_peek_type;

            U8 m_buffer[MAX_BUFFER_SIZE];
            // May use just under 100MB of space
            U8* m_infinite_store;
            NATIVE_UINT_TYPE m_infinite_read;
            NATIVE_UINT_TYPE m_infinite_write;
            NATIVE_UINT_TYPE m_infinite_size;

            Types::CircularBuffer m_test_buffer;
    };

};
#endif //FPRIME_CIRCULARSTATE_HPP
