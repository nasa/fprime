/**
 * CircularRules.hpp:
 *
 * This file specifies Rule classes for testing of the Types::CircularBuffer. These rules can then be used by the main
 * testing program to test the code. These rules support rule-based random testing.
 *
 * Circular rules:
 *
 * 1. Serialize into CircularBuffer with sufficient space should work.
 * 2. Serialize into CircularBuffer without sufficient space should error.
 * 3. Peeking into CircularBuffer with data should work (all variants).
 * 4. Peeking into CircularBuffer without data should error (all variants).
 * 5. Rotations should increase space when there is enough data.
 * 6. Rotations should error when there is not enough data.
 * 7. A rule exists to help randomize items.
 *
 * @author mstarch
 */
#ifndef FPRIME_GROUNDINTERFACERULES_HPP
#define FPRIME_GROUNDINTERFACERULES_HPP

#include <FpConfig.hpp>
#include <Fw/Types/String.hpp>
#include <Utils/Types/test/ut/CircularBuffer/CircularState.hpp>
#include <STest/STest/Rule/Rule.hpp>
#include <STest/STest/Pick/Pick.hpp>


namespace Types {

    /**
     * SetupRandomBufferRule:
     *
     * This rule sets up a random buffer, and other random state.
     */
    struct RandomizeRule : public STest::Rule<MockTypes::CircularState> {
        // Constructor
        RandomizeRule(const char *const name);

        // Always valid
        bool precondition(const MockTypes::CircularState& state);

        // Will randomize the test state
        void action(MockTypes::CircularState& truth);
    };

    /**
     * SerializeOkRule:
     *
     * This rule tests that the circular buffer can accept data when it is valid for the buffer to accept data.
     */
    struct SerializeOkRule : public STest::Rule<MockTypes::CircularState> {
        // Constructor
        SerializeOkRule(const char *const name);

        // Valid precondition for when the buffer should accept data
        bool precondition(const MockTypes::CircularState& state);

        // Action that tests the buffer accepting data
        void action(MockTypes::CircularState& state);
    };

    /**
     * SerializeOverflowRule:
     *
     * This rule tests that the circular buffer cannot accept data when it is full.
     */
    struct SerializeOverflowRule : public STest::Rule<MockTypes::CircularState> {
        // Constructor
        SerializeOverflowRule(const char *const name);

        // Valid precondition for when the buffer should reject data
        bool precondition(const MockTypes::CircularState& state);

        // Action that tests the buffer overflowing with an error
        void action(MockTypes::CircularState& state);
    };

    /**
     * PeekOkRule:
     *
     * This rule tests that the circular buffer can peek correctly.
     */
    struct PeekOkRule : public STest::Rule<MockTypes::CircularState> {
        // Constructor
        PeekOkRule(const char *const name);

        // Peek ok available for when buffer size - remaining size <= peek size
        bool precondition(const MockTypes::CircularState& state);

        // Action that tests the buffer's ability to peek
        void action(MockTypes::CircularState& state);
    };

    /**
     * PeekOkRule:
     *
     * This rule tests that the circular buffer cannot peek when it should not peek.
     */
    struct PeekBadRule : public STest::Rule<MockTypes::CircularState> {
        // Constructor
        PeekBadRule(const char *const name);

        // Peek bad available for when buffer size - remaining size > peek size
        bool precondition(const MockTypes::CircularState& state);

        // Action that tests the buffer's ability to peek with a fail
        void action(MockTypes::CircularState& state);
    };

    /**
     * RotateOkRule:
     *
     * This rule tests that the circular buffer can rotate correctly.
     */
    struct RotateOkRule : public STest::Rule<MockTypes::CircularState> {
        // Constructor
        RotateOkRule(const char *const name);

        // Rotate is ok when there is more data then rotational size
        bool precondition(const MockTypes::CircularState& state);

        // Action that tests the buffer's ability to rotate
        void action(MockTypes::CircularState& state);
    };

    /**
     * RotateOkRule:
     *
     * This rule tests that the circular buffer cannot rotate when it should not rotate.
     */
    struct RotateBadRule : public STest::Rule<MockTypes::CircularState> {
        // Constructor
        RotateBadRule(const char *const name);

        // Rotate is bad when there is less data then rotational size
        bool precondition(const MockTypes::CircularState& state);

        // Action that tests the buffer's ability to rotate
        void action(MockTypes::CircularState& state);
    };
};
#endif //FPRIME_GROUNDINTERFACERULES_HPP
