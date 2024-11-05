// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/Basic.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuard.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardString.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestAbsType.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestArray.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestEnum.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestStruct.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardU32.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicInternal.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicSelf.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicString.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestAbsType.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestArray.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestEnum.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestStruct.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicU32.hpp"
#include "FppTest/state_machine/internal_instance/state/Internal.hpp"
#include "FppTest/state_machine/internal_instance/state/Polymorphism.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToChild.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToChoice.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToSelf.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToState.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, Test) {
    FppTest::SmInstanceState::Basic basic("basic");
    basic.test();
}

TEST(BasicGuard, False) {
    FppTest::SmInstanceState::BasicGuard basicGuard("basicGuard");
    basicGuard.testFalse();
}

TEST(BasicGuard, True) {
    FppTest::SmInstanceState::BasicGuard basicGuard("basicGuard");
    basicGuard.testTrue();
}

TEST(BasicGuardString, False) {
    FppTest::SmInstanceState::BasicGuardString basicGuardString("basicGuardString");
    basicGuardString.testFalse();
}

TEST(BasicGuardString, True) {
    FppTest::SmInstanceState::BasicGuardString basicGuardString("basicGuardString");
    basicGuardString.testTrue();
}

TEST(BasicGuardTestAbsType, False) {
    FppTest::SmInstanceState::BasicGuardTestAbsType basicGuardTestAbsType("basicGuardTestAbsType");
    basicGuardTestAbsType.testFalse();
}

TEST(BasicGuardTestAbsType, Overflow) {
    FppTest::SmInstanceState::BasicGuardTestAbsType basicGuardTestAbsType("basicGuardTestAbsType");
    basicGuardTestAbsType.testOverflow();
}

TEST(BasicGuardTestAbsType, True) {
    FppTest::SmInstanceState::BasicGuardTestAbsType basicGuardTestAbsType("basicGuardTestAbsType");
    basicGuardTestAbsType.testTrue();
}

TEST(BasicGuardTestArray, False) {
    FppTest::SmInstanceState::BasicGuardTestArray basicGuardTestArray("basicGuardTestArray");
    basicGuardTestArray.testFalse();
}

TEST(BasicGuardTestArray, True) {
    FppTest::SmInstanceState::BasicGuardTestArray basicGuardTestArray("basicGuardTestArray");
    basicGuardTestArray.testTrue();
}

TEST(BasicGuardTestEnum, False) {
    FppTest::SmInstanceState::BasicGuardTestEnum basicGuardTestEnum("basicGuardTestEnum");
    basicGuardTestEnum.testFalse();
}

TEST(BasicGuardTestEnum, True) {
    FppTest::SmInstanceState::BasicGuardTestEnum basicGuardTestEnum("basicGuardTestEnum");
    basicGuardTestEnum.testTrue();
}

TEST(BasicGuardTestStruct, False) {
    FppTest::SmInstanceState::BasicGuardTestStruct basicGuardTestStruct("basicGuardTestStruct");
    basicGuardTestStruct.testFalse();
}

TEST(BasicGuardTestStruct, True) {
    FppTest::SmInstanceState::BasicGuardTestStruct basicGuardTestStruct("basicGuardTestStruct");
    basicGuardTestStruct.testTrue();
}

TEST(BasicGuardU32, False) {
    FppTest::SmInstanceState::BasicGuardU32 basicGuardU32("basicGuardU32");
    basicGuardU32.testFalse();
}

TEST(BasicGuardU32, True) {
    FppTest::SmInstanceState::BasicGuardU32 basicGuardU32("basicGuardU32");
    basicGuardU32.testTrue();
}

TEST(BasicInternal, Test) {
    FppTest::SmInstanceState::BasicInternal basicInternal("basicInternal");
    basicInternal.test();
}

TEST(BasicSelf, Test) {
    FppTest::SmInstanceState::BasicSelf basicSelf("basicSelf");
    basicSelf.test();
}

TEST(BasicString, Test) {
    FppTest::SmInstanceState::BasicString basicString("basicString");
    basicString.test();
}

TEST(BasicTestAbsType, Test) {
    FppTest::SmInstanceState::BasicTestAbsType basicTestAbsType("basicTestAbsType");
    basicTestAbsType.test();
}

TEST(BasicTestArray, Test) {
    FppTest::SmInstanceState::BasicTestArray basicTestArray("basicTestArray");
    basicTestArray.test();
}

TEST(BasicTestEnum, Test) {
    FppTest::SmInstanceState::BasicTestEnum basicTestEnum("basicTestEnum");
    basicTestEnum.test();
}

TEST(BasicTestStruct, Test) {
    FppTest::SmInstanceState::BasicTestStruct basicTestStruct("basicTestStruct");
    basicTestStruct.test();
}

TEST(BasicU32, Test) {
    FppTest::SmInstanceState::BasicU32 basicU32("basicU32");
    basicU32.test();
}

TEST(Internal, Init) {
    FppTest::SmInstanceState::Internal internal("internal");
    internal.testInit();
}

TEST(Internal, S2_internal) {
    FppTest::SmInstanceState::Internal internal("internal");
    internal.testS2_internal();
}

TEST(Internal, S2_to_S3) {
    FppTest::SmInstanceState::Internal internal("internal");
    internal.testS2_to_S3();
}

TEST(Internal, S3_internal) {
    FppTest::SmInstanceState::Internal internal("internal");
    internal.testS3_internal();
}

TEST(Polymorphism, Init) {
    FppTest::SmInstanceState::Polymorphism polymorphism("polymorphism");
    polymorphism.testInit();
}

TEST(Polymorphism, S2_poly) {
    FppTest::SmInstanceState::Polymorphism polymorphism("polymorphism");
    polymorphism.testS2_poly();
}

TEST(Polymorphism, S2_to_S3) {
    FppTest::SmInstanceState::Polymorphism polymorphism("polymorphism");
    polymorphism.testS2_to_S3();
}

TEST(Polymorphism, S3_poly) {
    FppTest::SmInstanceState::Polymorphism polymorphism("polymorphism");
    polymorphism.testS3_poly();
}

TEST(StateToChild, Init) {
    FppTest::SmInstanceState::StateToChild stateToChild("stateToChild");
    stateToChild.testInit();
}

TEST(StateToChild, S2_to_S2) {
    FppTest::SmInstanceState::StateToChild stateToChild("stateToChild");
    stateToChild.testS2_to_S2();
}

TEST(StateToChild, S2_to_S3) {
    FppTest::SmInstanceState::StateToChild stateToChild("stateToChild");
    stateToChild.testS2_to_S3();
}

TEST(StateToChild, S3_to_S2) {
    FppTest::SmInstanceState::StateToChild stateToChild("stateToChild");
    stateToChild.testS3_to_S2();
}

TEST(StateToChoice, Init) {
    FppTest::SmInstanceState::StateToChoice stateToChoice("stateToChoice");
    stateToChoice.testInit();
}

TEST(StateToChoice, S2_to_C) {
    FppTest::SmInstanceState::StateToChoice stateToChoice("stateToChoice");
    stateToChoice.testS2_to_C();
}

TEST(StateToChoice, S2_to_S4) {
    FppTest::SmInstanceState::StateToChoice stateToChoice("stateToChoice");
    stateToChoice.testS2_to_S4();
}

TEST(StateToChoice, S2_to_S3) {
    FppTest::SmInstanceState::StateToChoice stateToChoice("stateToChoice");
    stateToChoice.testS2_to_S3();
}

TEST(StateToChoice, S3_to_C) {
    FppTest::SmInstanceState::StateToChoice stateToChoice("stateToChoice");
    stateToChoice.testS3_to_C();
}

TEST(StateToChoice, S3_to_S4) {
    FppTest::SmInstanceState::StateToChoice stateToChoice("stateToChoice");
    stateToChoice.testS3_to_S4();
}

TEST(StateToSelf, Init) {
    FppTest::SmInstanceState::StateToSelf stateToSelf("stateToSelf");
    stateToSelf.testInit();
}

TEST(StateToSelf, S2_to_S1) {
    FppTest::SmInstanceState::StateToSelf stateToSelf("stateToSelf");
    stateToSelf.testS2_to_S1();
}

TEST(StateToSelf, S2_to_S3) {
    FppTest::SmInstanceState::StateToSelf stateToSelf("stateToSelf");
    stateToSelf.testS2_to_S3();
}

TEST(StateToSelf, S3_to_S1) {
    FppTest::SmInstanceState::StateToSelf stateToSelf("stateToSelf");
    stateToSelf.testS3_to_S1();
}

TEST(StateToState, Init) {
    FppTest::SmInstanceState::StateToState stateToState("stateToState");
    stateToState.testInit();
}

TEST(StateToState, S2_to_S3) {
    FppTest::SmInstanceState::StateToState stateToState("stateToState");
    stateToState.testS2_to_S3();
}

TEST(StateToState, S2_to_S4) {
    FppTest::SmInstanceState::StateToState stateToState("stateToState");
    stateToState.testS2_to_S4();
}

TEST(StateToState, S2_to_S5) {
    FppTest::SmInstanceState::StateToState stateToState("stateToState");
    stateToState.testS2_to_S5();
}

TEST(StateToState, S3_to_S4) {
    FppTest::SmInstanceState::StateToState stateToState("stateToState");
    stateToState.testS3_to_S4();
}

TEST(StateToState, S3_to_S5) {
    FppTest::SmInstanceState::StateToState stateToState("stateToState");
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
