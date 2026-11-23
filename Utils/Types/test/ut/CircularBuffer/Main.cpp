/**
 * Main.cpp:
 *
 * Setup the GTests for rules-based testing runs these tests.
 *
 *  Created on: May 23, 2019
 *      Author: mstarch
 */
#include <STest/Scenario/BoundedScenario.hpp>
#include <STest/Scenario/RandomScenario.hpp>
#include <STest/Scenario/Scenario.hpp>

#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include <Utils/Types/test/ut/CircularBuffer/CircularRules.hpp>

#include <cmath>
#include <cstdio>
#include <limits>

#define STEP_COUNT 1000

/**
 * A random hopper for rules. Apply STEP_COUNT times.
 */
TEST(CircularBufferTests, RandomCircularTests) {
    F64 max_addr_mem = sizeof(FwSizeType) * 8.0;
    max_addr_mem = pow(2.0, max_addr_mem);
    // Ensure the maximum memory use is less that the max addressable memory
    F64 max_used_mem = static_cast<double>(STEP_COUNT) * static_cast<double>(MAX_BUFFER_SIZE);
    ASSERT_LT(max_used_mem, max_addr_mem);

    MockTypes::CircularState state;

    // Create rules, and assign them into the array
    Types::RandomizeRule randomize("Randomize");
    Types::SerializeOkRule serializeOk("SerializeOk");
    Types::SerializeOverflowRule serializeOverflow("serializeOverflow");
    Types::PeekOkRule peekOk("peekOk");
    Types::PeekBadRule peekBad("peekBad");
    Types::RotateOkRule rotateOk("rotateOk");
    Types::RotateBadRule rotateBad("rotateBad");
    Types::TrimOkRule trimOk("trimOk");
    Types::TrimBadRule trimBad("trimBad");

    // Setup a list of rules to choose from
    STest::Rule<MockTypes::CircularState>* rules[] = {
        &randomize, &serializeOk, &serializeOverflow, &peekOk, &peekBad, &rotateOk, &rotateBad, &trimOk, &trimBad};
    // Construct the random scenario and run it with the defined bounds
    STest::RandomScenario<MockTypes::CircularState> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Setup a bounded scenario to run rules a set number of times
    STest::BoundedScenario<MockTypes::CircularState> bounded("Bounded Random Rules Scenario", random, STEP_COUNT);
    // Run!
    const U32 numSteps = bounded.run(state);
    printf("Ran %u steps.\n", numSteps);
}

/**
 * Test that the most basic logging function works.
 */
TEST(CircularBufferTests, BasicSerializeTest) {
    // Setup and register state
    MockTypes::CircularState state;

    // Create rules, and assign them into the array
    Types::RandomizeRule randomGo("randomGo");
    Types::SerializeOkRule serializeOk("SerializeOk");
    randomGo.apply(state);
    serializeOk.apply(state);
}

/**
 * Test that the most basic circular overflow.
 */
TEST(CircularBufferTests, BasicOverflowTest) {
    // Setup state and fill it with garbage
    MockTypes::CircularState state;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, state.getTestBuffer().serialize(state.getBuffer(), state.getRandomSize()));
    state.setRemainingSize(0);

    // Create rules, and assign them into the array
    Types::RandomizeRule randomGo("randomGo", 1);
    Types::SerializeOverflowRule serializeOverflow("serializeOverflow");

    randomGo.apply(state);
    serializeOverflow.apply(state);
}

/**
 * Test that the most basic peeks work.
 */
TEST(CircularBufferTests, BasicPeekTest) {
    char peek_char = static_cast<char>(0x85);
    U8 peek_u8 = 0x95;
    U32 peek_u32 = 0xdeadc0de;
    U8 buffer[1024] = {};  // Clear out memory to appease valgrind
    // Setup all circular state
    MockTypes::CircularState state;
    state.addInfinite(reinterpret_cast<U8*>(&peek_char), sizeof(peek_char));
    state.getTestBuffer().serialize(reinterpret_cast<U8*>(&peek_char), sizeof(peek_char));
    state.addInfinite(&peek_u8, sizeof(peek_u8));
    state.getTestBuffer().serialize(&peek_u8, sizeof(peek_u8));
    for (FwSizeType i = sizeof(U32); i > 0; i--) {
        U8 byte = peek_u32 >> ((i - 1) * 8);
        state.addInfinite(&byte, sizeof(byte));
        state.getTestBuffer().serialize(&byte, sizeof(byte));
    }
    state.addInfinite(buffer, sizeof(buffer));
    state.getTestBuffer().serialize(buffer, sizeof(buffer));
    state.setRemainingSize(MAX_BUFFER_SIZE - 1030);
    // Run all peek variants
    Types::PeekOkRule peekOk("peekOk");
    state.setRandom(0, 0, 0);
    peekOk.apply(state);
    state.setRandom(0, 1, 1);
    peekOk.apply(state);
    state.setRandom(0, 2, 2);
    peekOk.apply(state);
    state.setRandom(sizeof(buffer), 3, 6);
    peekOk.apply(state);
}

/**
 * Test that the most basic bad-peeks work.
 */
TEST(CircularBufferTests, BasicPeekBadTest) {
    // Setup all circular state
    MockTypes::CircularState state;
    // Run all peek variants
    Types::PeekBadRule peekBad("peekBad");
    state.setRandom(0, 0, 0);
    peekBad.apply(state);
    state.setRandom(0, 1, 1);
    peekBad.apply(state);
    state.setRandom(0, 2, 2);
    peekBad.apply(state);
    state.setRandom(1024, 3, 6);
    peekBad.apply(state);
}

/**
 * Test that the most basic rotate work.
 */
TEST(CircularBufferTests, BasicRotateTest) {
    // Setup and register state
    MockTypes::CircularState state;

    // Create rules, and assign them into the array
    Types::RandomizeRule randomGo("randomGo");
    Types::SerializeOkRule serializeOk("SerializeOk");
    Types::RotateOkRule rotateOk("rotateOk");
    randomGo.apply(state);
    serializeOk.apply(state);
    rotateOk.apply(state);
}

/**
 * Test that the most basic bad-rotate work.
 */
TEST(CircularBufferTests, BasicRotateBadTest) {
    // Setup all circular state
    MockTypes::CircularState state;
    // Run all peek variants
    Types::RotateBadRule rotateBad("rotateBad");
    rotateBad.apply(state);
}

/**
 * Test that the most basic trim work.
 */
TEST(CircularBufferTests, BasicTrimTest) {
    // Setup and register state
    MockTypes::CircularState state;

    // Create rules, and assign them into the array
    Types::RandomizeRule randomGo("randomGo");
    Types::SerializeOkRule serializeOk("SerializeOk");
    Types::TrimOkRule trimOk("trimOk");
    randomGo.apply(state);
    serializeOk.apply(state);
    trimOk.apply(state);
}

/**
 * Test that the most basic bad-trim work.
 */
TEST(CircularBufferTests, BasicTrimBadTest) {
    // Setup all circular state
    MockTypes::CircularState state;
    // Run trim bad rule
    Types::TrimBadRule trimBad("trimBad");
    trimBad.apply(state);
}

/**
 * Test boundary cases
 */
TEST(CircularBufferTests, BoundaryCases) {
    MockTypes::CircularState state;
    // Serialize an empty buffer
    state.setRandom(0, 0, 0);
    Types::SerializeOkRule serializeOk("serializeOk");
    serializeOk.apply(state);
    // Serialize a max size buffer
    state.setRandom(MAX_BUFFER_SIZE, 0, 0);
    serializeOk.apply(state);
}

/**
 * Test that peeks with huge offsets are rejected, even when the
 * (size + offset) range check would overflow. Regression test for #6412.
 */
TEST(CircularBufferTests, PeekOverflowOffsetTest) {
    // Serialize 4 bytes so the buffer is non-empty
    MockTypes::CircularState state;
    U8 data[4] = {0x11, 0x22, 0x33, 0x44};
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, state.getTestBuffer().serialize(data, sizeof(data)));
    state.setRemainingSize(MAX_BUFFER_SIZE - sizeof(data));

    // Offsets chosen so (size + offset) wraps to a small value on the old check
    const FwSizeType max_offset = std::numeric_limits<FwSizeType>::max();
    U8 peek_u8 = 0;
    ASSERT_EQ(state.getTestBuffer().peek(peek_u8, max_offset), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
    U32 peek_u32 = 0;
    ASSERT_EQ(state.getTestBuffer().peek(peek_u32, max_offset), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
    U8 peek_buffer[2] = {};
    ASSERT_EQ(state.getTestBuffer().peek(peek_buffer, sizeof(peek_buffer), max_offset - 1),
              Fw::FW_DESERIALIZE_BUFFER_EMPTY);
    // The reference model must reject the same request
    U8* model_buffer = nullptr;
    ASSERT_FALSE(state.peek(model_buffer, sizeof(peek_buffer), max_offset - 1));
    // Sanity: an in-range peek still works
    ASSERT_EQ(state.getTestBuffer().peek(peek_u8, 0), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(peek_u8, 0x11);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
