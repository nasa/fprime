// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/Basic.hpp"
#include "FppTest/state_machine/internal/state/BasicGuard.hpp"
#include "FppTest/state_machine/internal/state/BasicGuardString.hpp"
#include "FppTest/state_machine/internal/state/BasicGuardTestAbsType.hpp"
#include "FppTest/state_machine/internal/state/BasicGuardTestArray.hpp"
#include "FppTest/state_machine/internal/state/BasicGuardTestEnum.hpp"
#include "FppTest/state_machine/internal/state/BasicGuardTestStruct.hpp"
#include "FppTest/state_machine/internal/state/BasicGuardU32.hpp"
#include "FppTest/state_machine/internal/state/BasicInternal.hpp"
#include "FppTest/state_machine/internal/state/BasicSelf.hpp"
#include "FppTest/state_machine/internal/state/BasicString.hpp"
#include "FppTest/state_machine/internal/state/BasicTestAbsType.hpp"
#include "FppTest/state_machine/internal/state/BasicTestArray.hpp"
#include "FppTest/state_machine/internal/state/BasicTestEnum.hpp"
#include "FppTest/state_machine/internal/state/BasicTestStruct.hpp"
#include "FppTest/state_machine/internal/state/BasicU32.hpp"
#include "FppTest/state_machine/internal/state/Internal.hpp"
#include "FppTest/state_machine/internal/state/Polymorphism.hpp"
#include "FppTest/state_machine/internal/state/StateToChild.hpp"
#include "FppTest/state_machine/internal/state/StateToChoice.hpp"
#include "FppTest/state_machine/internal/state/StateToSelf.hpp"
#include "FppTest/state_machine/internal/state/StateToState.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, Test) {
    FppTest::SmState::Basic basic;
    basic.test();
}

TEST(BasicGuard, False) {
    FppTest::SmState::BasicGuard basicGuard;
    basicGuard.testFalse();
}

TEST(BasicGuard, True) {
    FppTest::SmState::BasicGuard basicGuard;
    basicGuard.testTrue();
}

TEST(BasicGuardString, False) {
    FppTest::SmState::BasicGuardString basicGuardString;
    basicGuardString.testFalse();
}

TEST(BasicGuardString, True) {
    FppTest::SmState::BasicGuardString basicGuardString;
    basicGuardString.testTrue();
}

TEST(BasicGuardTestAbsType, False) {
    FppTest::SmState::BasicGuardTestAbsType basicGuardTestAbsType;
    basicGuardTestAbsType.testFalse();
}

TEST(BasicGuardTestAbsType, True) {
    FppTest::SmState::BasicGuardTestAbsType basicGuardTestAbsType;
    basicGuardTestAbsType.testTrue();
}

TEST(BasicGuardTestArray, False) {
    FppTest::SmState::BasicGuardTestArray basicGuardTestArray;
    basicGuardTestArray.testFalse();
}

TEST(BasicGuardTestArray, True) {
    FppTest::SmState::BasicGuardTestArray basicGuardTestArray;
    basicGuardTestArray.testTrue();
}

TEST(BasicGuardTestEnum, False) {
    FppTest::SmState::BasicGuardTestEnum basicGuardTestEnum;
    basicGuardTestEnum.testFalse();
}

TEST(BasicGuardTestEnum, True) {
    FppTest::SmState::BasicGuardTestEnum basicGuardTestEnum;
    basicGuardTestEnum.testTrue();
}

TEST(BasicGuardTestStruct, False) {
    FppTest::SmState::BasicGuardTestStruct basicGuardTestStruct;
    basicGuardTestStruct.testFalse();
}

TEST(BasicGuardTestStruct, True) {
    FppTest::SmState::BasicGuardTestStruct basicGuardTestStruct;
    basicGuardTestStruct.testTrue();
}

TEST(BasicGuardU32, False) {
    FppTest::SmState::BasicGuardU32 basicGuardU32;
    basicGuardU32.testFalse();
}

TEST(BasicGuardU32, True) {
    FppTest::SmState::BasicGuardU32 basicGuardU32;
    basicGuardU32.testTrue();
}

TEST(BasicInternal, Test) {
    FppTest::SmState::BasicInternal basicInternal;
    basicInternal.test();
}

TEST(BasicSelf, Test) {
    FppTest::SmState::BasicSelf basicSelf;
    basicSelf.test();
}

TEST(BasicString, Test) {
    FppTest::SmState::BasicString basicString;
    basicString.test();
}

TEST(BasicTestAbsType, Test) {
    FppTest::SmState::BasicTestAbsType basicTestAbsType;
    basicTestAbsType.test();
}

TEST(BasicTestArray, Test) {
    FppTest::SmState::BasicTestArray basicTestArray;
    basicTestArray.test();
}

TEST(BasicTestEnum, Test) {
    FppTest::SmState::BasicTestEnum basicTestEnum;
    basicTestEnum.test();
}

TEST(BasicTestStruct, Test) {
    FppTest::SmState::BasicTestStruct basicTestStruct;
    basicTestStruct.test();
}

TEST(BasicU32, Test) {
    FppTest::SmState::BasicU32 basicU32;
    basicU32.test();
}

TEST(Internal, Init) {
    FppTest::SmState::Internal internal;
    internal.testInit();
}

TEST(Internal, S2_internal) {
    FppTest::SmState::Internal internal;
    internal.testS2_internal();
}

TEST(Internal, S2_to_S3) {
    FppTest::SmState::Internal internal;
    internal.testS2_to_S3();
}

TEST(Internal, S3_internal) {
    FppTest::SmState::Internal internal;
    internal.testS3_internal();
}

TEST(Polymorphism, Init) {
    FppTest::SmState::Polymorphism polymorphism;
    polymorphism.testInit();
}

TEST(Polymorphism, S2_poly) {
    FppTest::SmState::Polymorphism polymorphism;
    polymorphism.testS2_poly();
}

TEST(Polymorphism, S2_to_S3) {
    FppTest::SmState::Polymorphism polymorphism;
    polymorphism.testS2_to_S3();
}

TEST(Polymorphism, S3_poly) {
    FppTest::SmState::Polymorphism polymorphism;
    polymorphism.testS3_poly();
}

TEST(StateToChild, Init) {
    FppTest::SmState::StateToChild stateToChild;
    stateToChild.testInit();
}

TEST(StateToChild, S2_to_S2) {
    FppTest::SmState::StateToChild stateToChild;
    stateToChild.testS2_to_S2();
}

TEST(StateToChild, S2_to_S3) {
    FppTest::SmState::StateToChild stateToChild;
    stateToChild.testS2_to_S3();
}

TEST(StateToChild, S3_to_S2) {
    FppTest::SmState::StateToChild stateToChild;
    stateToChild.testS3_to_S2();
}

TEST(StateToChoice, Init) {
    FppTest::SmState::StateToChoice stateToChoice;
    stateToChoice.testInit();
}

TEST(StateToChoice, S2_to_C) {
    FppTest::SmState::StateToChoice stateToChoice;
    stateToChoice.testS2_to_C();
}

TEST(StateToChoice, S2_to_S4) {
    FppTest::SmState::StateToChoice stateToChoice;
    stateToChoice.testS2_to_S4();
}

TEST(StateToChoice, S2_to_S3) {
    FppTest::SmState::StateToChoice stateToChoice;
    stateToChoice.testS2_to_S3();
}

TEST(StateToChoice, S3_to_C) {
    FppTest::SmState::StateToChoice stateToChoice;
    stateToChoice.testS3_to_C();
}

TEST(StateToChoice, S3_to_S4) {
    FppTest::SmState::StateToChoice stateToChoice;
    stateToChoice.testS3_to_S4();
}

TEST(StateToSelf, Init) {
    FppTest::SmState::StateToSelf stateToSelf;
    stateToSelf.testInit();
}

TEST(StateToSelf, S2_to_S1) {
    FppTest::SmState::StateToSelf stateToSelf;
    stateToSelf.testS2_to_S1();
}

TEST(StateToSelf, S2_to_S3) {
    FppTest::SmState::StateToSelf stateToSelf;
    stateToSelf.testS2_to_S3();
}

TEST(StateToSelf, S3_to_S1) {
    FppTest::SmState::StateToSelf stateToSelf;
    stateToSelf.testS3_to_S1();
}

TEST(StateToState, Init) {
    FppTest::SmState::StateToState stateToState;
    stateToState.testInit();
}

TEST(StateToState, S2_to_S3) {
    FppTest::SmState::StateToState stateToState;
    stateToState.testS2_to_S3();
}

TEST(StateToState, S2_to_S4) {
    FppTest::SmState::StateToState stateToState;
    stateToState.testS2_to_S4();
}

TEST(StateToState, S2_to_S5) {
    FppTest::SmState::StateToState stateToState;
    stateToState.testS2_to_S5();
}

TEST(StateToState, S3_to_S4) {
    FppTest::SmState::StateToState stateToState;
    stateToState.testS3_to_S4();
}

TEST(StateToState, S3_to_S5) {
    FppTest::SmState::StateToState stateToState;
    stateToState.testS3_to_S5();
}

// ----------------------------------------------------------------------
// Main function
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
