/**
 * CircularRules.cpp:
 *
 * This file specifies Rule classes for testing of the Types::CircularRules. These rules can then be used by the main
 * testing program to test the code.
 *
 *
 * @author mstarch
 */
#include "CircularRules.hpp"

#include <cstdlib>
#include <cmath>

namespace Types {


    RandomizeRule::RandomizeRule(const char *const name)
        : STest::Rule<MockTypes::CircularState>(name) {}


    bool RandomizeRule::precondition(const MockTypes::CircularState& state) {
        return true;
    }


    void RandomizeRule::action(MockTypes::CircularState& truth) {
        (void) truth.generateRandomBuffer();
    }



    SerializeOkRule::SerializeOkRule(const char *const name)
        : STest::Rule<MockTypes::CircularState>(name) {}


    bool SerializeOkRule::precondition(const MockTypes::CircularState& state) {
        return state.getRemainingSize() >= state.getRandomSize();
    }


    void SerializeOkRule::action(MockTypes::CircularState& state) {
        state.checkSizes();
        Fw::SerializeStatus status = state.getTestBuffer().serialize(state.getBuffer(), state.getRandomSize());
        state.setRemainingSize(state.getRemainingSize() - state.getRandomSize());
        ASSERT_TRUE(state.addInfinite(state.getBuffer(), state.getRandomSize()));
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        state.checkSizes();
    }



    SerializeOverflowRule::SerializeOverflowRule(const char *const name)
            : STest::Rule<MockTypes::CircularState>(name) {}


    bool SerializeOverflowRule::precondition(const MockTypes::CircularState& state) {
        return state.getRemainingSize() < state.getRandomSize();
    }


    void SerializeOverflowRule::action(MockTypes::CircularState& state) {
        Fw::SerializeStatus status = state.getTestBuffer().serialize(state.getBuffer(), state.getRandomSize());
        ASSERT_EQ(status, Fw::FW_SERIALIZE_NO_ROOM_LEFT);
    }


    PeekOkRule::PeekOkRule(const char *const name)
            : STest::Rule<MockTypes::CircularState>(name) {}


    bool PeekOkRule::precondition(const MockTypes::CircularState& state) {
        NATIVE_UINT_TYPE peek_available = (MAX_BUFFER_SIZE - state.getRemainingSize());
        if (state.getPeekType() == 0 ) {
            return peek_available >= sizeof(I8) + state.getPeekOffset();
        }
        else if (state.getPeekType() == 1) {
            return peek_available >= sizeof(U8) + state.getPeekOffset();
        }
        else if (state.getPeekType() == 2) {
            return peek_available >= sizeof(U32) + state.getPeekOffset();
        }
        else if (state.getPeekType() == 3) {
            return peek_available >= state.getRandomSize() + state.getPeekOffset();
        }
        return false;
    }

    void PeekOkRule::action(MockTypes::CircularState& state) {
        U8* buffer = nullptr;
        char peek_char = 0;
        U8 peek_u8 = 0;
        U32 peek_u32 = 0;
        U8 peek_buffer[MAX_BUFFER_SIZE];
        // Handle all cases for deserialization
        if (state.getPeekType() == 0) {
            ASSERT_TRUE(state.peek(buffer, sizeof(I8), state.getPeekOffset()));
            peek_char = static_cast<char>(buffer[0]);
            ASSERT_EQ(state.getTestBuffer().peek(peek_char, state.getPeekOffset()), Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(static_cast<char>(buffer[0]), peek_char);
        }
        else if (state.getPeekType() == 1) {
            ASSERT_TRUE(state.peek(buffer, sizeof(U8), state.getPeekOffset()));
            peek_u8 = static_cast<U8>(buffer[0]);
            ASSERT_EQ(state.getTestBuffer().peek(peek_u8, state.getPeekOffset()), Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(buffer[0], peek_u8);
        }
        else if (state.getPeekType() == 2) {
            ASSERT_TRUE(state.peek(buffer, sizeof(U32), state.getPeekOffset()));
            ASSERT_EQ(state.getTestBuffer().peek(peek_u32, state.getPeekOffset()), Fw::FW_SERIALIZE_OK);
            // Big-endian U32
            U32 value = 0;
            value |= (buffer[0] << 24);
            value |= (buffer[1] << 16);
            value |= (buffer[2] << 8);
            value |= (buffer[3] << 0);
            ASSERT_EQ(value, peek_u32);
        }
        else if (state.getPeekType() == 3) {
            ASSERT_TRUE(state.peek(buffer, state.getRandomSize(), state.getPeekOffset()));
            ASSERT_EQ(state.getTestBuffer().peek(peek_buffer, state.getRandomSize(), state.getPeekOffset()),
                    Fw::FW_SERIALIZE_OK);
            for (NATIVE_UINT_TYPE i = 0; i < state.getRandomSize(); i++) {
                ASSERT_EQ(buffer[i], peek_buffer[i]);
            }
        }
        else {
            ASSERT_TRUE(false); // Fail the test, bad type
        }
    }


    PeekBadRule::PeekBadRule(const char *const name)
            : STest::Rule<MockTypes::CircularState>(name) {}


    bool PeekBadRule::precondition(const MockTypes::CircularState& state) {
        NATIVE_UINT_TYPE peek_available = (MAX_BUFFER_SIZE - state.getRemainingSize());
        if (state.getPeekType() == 0 ) {
            return peek_available < sizeof(I8) + state.getPeekOffset();
        }
        else if (state.getPeekType() == 1) {
            return peek_available < sizeof(U8) + state.getPeekOffset();
        }
        else if (state.getPeekType() == 2) {
            return peek_available < sizeof(U32) + state.getPeekOffset();
        }
        else if (state.getPeekType() == 3) {
            return peek_available < state.getRandomSize() + state.getPeekOffset();
        }
        return false;
    }

    void PeekBadRule::action(MockTypes::CircularState& state) {
        char peek_char = 0;
        U8 peek_u8 = 0;
        U32 peek_u32 = 0;
        U8 peek_buffer[MAX_BUFFER_SIZE];
        // Handle all cases for deserialization
        if (state.getPeekType() == 0) {
            ASSERT_EQ(state.getTestBuffer().peek(peek_char, state.getPeekOffset()), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
        }
        else if (state.getPeekType() == 1) {
            ASSERT_EQ(state.getTestBuffer().peek(peek_u8, state.getPeekOffset()), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
        }
        else if (state.getPeekType() == 2) {
            ASSERT_EQ(state.getTestBuffer().peek(peek_u32, state.getPeekOffset()), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
        }
        else if (state.getPeekType() == 3) {
            ASSERT_EQ(state.getTestBuffer().peek(peek_buffer, state.getRandomSize(), state.getPeekOffset()),
                      Fw::FW_DESERIALIZE_BUFFER_EMPTY);
        }
        else {
            ASSERT_TRUE(false); // Fail the test, bad type
        }
    }


    RotateOkRule::RotateOkRule(const char *const name)
            : STest::Rule<MockTypes::CircularState>(name) {}


    bool RotateOkRule::precondition(const MockTypes::CircularState& state) {
        NATIVE_UINT_TYPE rotate_available = (MAX_BUFFER_SIZE - state.getRemainingSize());
        return rotate_available >= state.getRandomSize();
    }

    void RotateOkRule::action(MockTypes::CircularState& state) {
        state.checkSizes();
        ASSERT_EQ(state.getTestBuffer().rotate(state.getRandomSize()), Fw::FW_SERIALIZE_OK);
        ASSERT_TRUE(state.rotate(state.getRandomSize()));
        state.setRemainingSize(state.getRemainingSize() + state.getRandomSize());
        state.checkSizes();
    }


    RotateBadRule::RotateBadRule(const char *const name)
            : STest::Rule<MockTypes::CircularState>(name) {}


    bool RotateBadRule::precondition(const MockTypes::CircularState& state) {
        NATIVE_UINT_TYPE rotate_available = (MAX_BUFFER_SIZE - state.getRemainingSize());
        return rotate_available < state.getRandomSize();
    }

    void RotateBadRule::action(MockTypes::CircularState& state) {
        ASSERT_EQ(state.getTestBuffer().rotate(state.getRandomSize()), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
    }
};
