// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicGuardStringTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestAbsTypeTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestArrayTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestEnumTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestStructTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardU32Tester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicInternalTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicSelfTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicStringTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestAbsTypeTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestArrayTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestEnumTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestStructTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTester.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicU32Tester.hpp"
#include "FppTest/state_machine/internal_instance/state/InternalTester.hpp"
#include "FppTest/state_machine/internal_instance/state/PolymorphismTester.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToChildTester.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToChoiceTester.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToSelfTester.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToStateTester.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, Test) {
    FppTest::SmInstanceState::BasicTester basic("basic");
    basic.test();
}

TEST(BasicGuard, False) {
    FppTest::SmInstanceState::BasicGuardTester basicGuard("basicGuard");
    basicGuard.testFalse();
}

TEST(BasicGuard, True) {
    FppTest::SmInstanceState::BasicGuardTester basicGuard("basicGuard");
    basicGuard.testTrue();
}

TEST(BasicGuardString, False) {
    FppTest::SmInstanceState::BasicGuardStringTester basicGuardString("basicGuardString");
    basicGuardString.testFalse();
}

TEST(BasicGuardString, True) {
    FppTest::SmInstanceState::BasicGuardStringTester basicGuardString("basicGuardString");
    basicGuardString.testTrue();
}

TEST(BasicGuardTestAbsType, False) {
    FppTest::SmInstanceState::BasicGuardTestAbsTypeTester basicGuardTestAbsType("basicGuardTestAbsType");
    basicGuardTestAbsType.testFalse();
}

TEST(BasicGuardTestAbsType, Overflow) {
    FppTest::SmInstanceState::BasicGuardTestAbsTypeTester basicGuardTestAbsType("basicGuardTestAbsType");
    basicGuardTestAbsType.testOverflow();
}

TEST(BasicGuardTestAbsType, True) {
    FppTest::SmInstanceState::BasicGuardTestAbsTypeTester basicGuardTestAbsType("basicGuardTestAbsType");
    basicGuardTestAbsType.testTrue();
}

TEST(BasicGuardTestArray, False) {
    FppTest::SmInstanceState::BasicGuardTestArrayTester basicGuardTestArray("basicGuardTestArray");
    basicGuardTestArray.testFalse();
}

TEST(BasicGuardTestArray, True) {
    FppTest::SmInstanceState::BasicGuardTestArrayTester basicGuardTestArray("basicGuardTestArray");
    basicGuardTestArray.testTrue();
}

TEST(BasicGuardTestEnum, False) {
    FppTest::SmInstanceState::BasicGuardTestEnumTester basicGuardTestEnum("basicGuardTestEnum");
    basicGuardTestEnum.testFalse();
}

TEST(BasicGuardTestEnum, True) {
    FppTest::SmInstanceState::BasicGuardTestEnumTester basicGuardTestEnum("basicGuardTestEnum");
    basicGuardTestEnum.testTrue();
}

TEST(BasicGuardTestStruct, False) {
    FppTest::SmInstanceState::BasicGuardTestStructTester basicGuardTestStruct("basicGuardTestStruct");
    basicGuardTestStruct.testFalse();
}

TEST(BasicGuardTestStruct, True) {
    FppTest::SmInstanceState::BasicGuardTestStructTester basicGuardTestStruct("basicGuardTestStruct");
    basicGuardTestStruct.testTrue();
}

TEST(BasicGuardU32, False) {
    FppTest::SmInstanceState::BasicGuardU32Tester basicGuardU32("basicGuardU32");
    basicGuardU32.testFalse();
}

TEST(BasicGuardU32, True) {
    FppTest::SmInstanceState::BasicGuardU32Tester basicGuardU32("basicGuardU32");
    basicGuardU32.testTrue();
}

TEST(BasicInternal, Test) {
    FppTest::SmInstanceState::BasicInternalTester basicInternal("basicInternal");
    basicInternal.test();
}

TEST(BasicSelf, Test) {
    FppTest::SmInstanceState::BasicSelfTester basicSelf("basicSelf");
    basicSelf.test();
}

TEST(BasicString, Test) {
    FppTest::SmInstanceState::BasicStringTester basicString("basicString");
    basicString.test();
}

TEST(BasicTestAbsType, Test) {
    FppTest::SmInstanceState::BasicTestAbsTypeTester basicTestAbsType("basicTestAbsType");
    basicTestAbsType.test();
}

TEST(BasicTestArray, Test) {
    FppTest::SmInstanceState::BasicTestArrayTester basicTestArray("basicTestArray");
    basicTestArray.test();
}

TEST(BasicTestEnum, Test) {
    FppTest::SmInstanceState::BasicTestEnumTester basicTestEnum("basicTestEnum");
    basicTestEnum.test();
}

TEST(BasicTestStruct, Test) {
    FppTest::SmInstanceState::BasicTestStructTester basicTestStruct("basicTestStruct");
    basicTestStruct.test();
}

TEST(BasicU32, Test) {
    FppTest::SmInstanceState::BasicU32Tester basicU32("basicU32");
    basicU32.test();
}

TEST(Internal, Init) {
    FppTest::SmInstanceState::InternalTester internal("internal");
    internal.testInit();
}

TEST(Internal, S2_internal) {
    FppTest::SmInstanceState::InternalTester internal("internal");
    internal.testS2_internal();
}

TEST(Internal, S2_to_S3) {
    FppTest::SmInstanceState::InternalTester internal("internal");
    internal.testS2_to_S3();
}

TEST(Internal, S3_internal) {
    FppTest::SmInstanceState::InternalTester internal("internal");
    internal.testS3_internal();
}

TEST(Polymorphism, Init) {
    FppTest::SmInstanceState::PolymorphismTester polymorphism("polymorphism");
    polymorphism.testInit();
}

TEST(Polymorphism, S2_poly) {
    FppTest::SmInstanceState::PolymorphismTester polymorphism("polymorphism");
    polymorphism.testS2_poly();
}

TEST(Polymorphism, S2_to_S3) {
    FppTest::SmInstanceState::PolymorphismTester polymorphism("polymorphism");
    polymorphism.testS2_to_S3();
}

TEST(Polymorphism, S3_poly) {
    FppTest::SmInstanceState::PolymorphismTester polymorphism("polymorphism");
    polymorphism.testS3_poly();
}

TEST(StateToChild, Init) {
    FppTest::SmInstanceState::StateToChildTester stateToChild("stateToChild");
    stateToChild.testInit();
}

TEST(StateToChild, S2_to_S2) {
    FppTest::SmInstanceState::StateToChildTester stateToChild("stateToChild");
    stateToChild.testS2_to_S2();
}

TEST(StateToChild, S2_to_S3) {
    FppTest::SmInstanceState::StateToChildTester stateToChild("stateToChild");
    stateToChild.testS2_to_S3();
}

TEST(StateToChild, S3_to_S2) {
    FppTest::SmInstanceState::StateToChildTester stateToChild("stateToChild");
    stateToChild.testS3_to_S2();
}

TEST(StateToChoice, Init) {
    FppTest::SmInstanceState::StateToChoiceTester stateToChoice("stateToChoice");
    stateToChoice.testInit();
}

TEST(StateToChoice, S2_to_C) {
    FppTest::SmInstanceState::StateToChoiceTester stateToChoice("stateToChoice");
    stateToChoice.testS2_to_C();
}

TEST(StateToChoice, S2_to_S4) {
    FppTest::SmInstanceState::StateToChoiceTester stateToChoice("stateToChoice");
    stateToChoice.testS2_to_S4();
}

TEST(StateToChoice, S2_to_S3) {
    FppTest::SmInstanceState::StateToChoiceTester stateToChoice("stateToChoice");
    stateToChoice.testS2_to_S3();
}

TEST(StateToChoice, S3_to_C) {
    FppTest::SmInstanceState::StateToChoiceTester stateToChoice("stateToChoice");
    stateToChoice.testS3_to_C();
}

TEST(StateToChoice, S3_to_S4) {
    FppTest::SmInstanceState::StateToChoiceTester stateToChoice("stateToChoice");
    stateToChoice.testS3_to_S4();
}

TEST(StateToSelf, Init) {
    FppTest::SmInstanceState::StateToSelfTester stateToSelf("stateToSelf");
    stateToSelf.testInit();
}

TEST(StateToSelf, S2_to_S1) {
    FppTest::SmInstanceState::StateToSelfTester stateToSelf("stateToSelf");
    stateToSelf.testS2_to_S1();
}

TEST(StateToSelf, S2_to_S3) {
    FppTest::SmInstanceState::StateToSelfTester stateToSelf("stateToSelf");
    stateToSelf.testS2_to_S3();
}

TEST(StateToSelf, S3_to_S1) {
    FppTest::SmInstanceState::StateToSelfTester stateToSelf("stateToSelf");
    stateToSelf.testS3_to_S1();
}

TEST(StateToState, Init) {
    FppTest::SmInstanceState::StateToStateTester stateToState("stateToState");
    stateToState.testInit();
}

TEST(StateToState, S2_to_S3) {
    FppTest::SmInstanceState::StateToStateTester stateToState("stateToState");
    stateToState.testS2_to_S3();
}

TEST(StateToState, S2_to_S4) {
    FppTest::SmInstanceState::StateToStateTester stateToState("stateToState");
    stateToState.testS2_to_S4();
}

TEST(StateToState, S2_to_S5) {
    FppTest::SmInstanceState::StateToStateTester stateToState("stateToState");
    stateToState.testS2_to_S5();
}

TEST(StateToState, S3_to_S4) {
    FppTest::SmInstanceState::StateToStateTester stateToState("stateToState");
    stateToState.testS3_to_S4();
}

TEST(StateToState, S3_to_S5) {
    FppTest::SmInstanceState::StateToStateTester stateToState("stateToState");
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
