// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "FppTest/dp/test/ut/Tester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

using namespace FppTest;

TEST(schedIn, OK) {
    COMMENT("schedIn OK");
    Tester tester;
    tester.schedIn_OK();
}

TEST(productRecvIn, Container1_SUCCESS) {
    COMMENT("Receive Container1 SUCCESS");
    Tester tester;
    tester.productRecvIn_Container1_SUCCESS();
}

TEST(productRecvIn, Container1_FAILURE) {
    COMMENT("Receive Container1 FAILURE");
    Tester tester;
    tester.productRecvIn_Container1_FAILURE();
}

TEST(productRecvIn, Container2_SUCCESS) {
    COMMENT("Receive Container2 SUCCESS");
    Tester tester;
    tester.productRecvIn_Container2_SUCCESS();
}

TEST(productRecvIn, Container2_FAILURE) {
    COMMENT("Receive Container2 FAILURE");
    Tester tester;
    tester.productRecvIn_Container2_FAILURE();
}

TEST(productRecvIn, Container3_SUCCESS) {
    COMMENT("Receive Container3 SUCCESS");
    Tester tester;
    tester.productRecvIn_Container3_SUCCESS();
}

TEST(productRecvIn, Container3_FAILURE) {
    COMMENT("Receive Container3 FAILURE");
    Tester tester;
    tester.productRecvIn_Container3_FAILURE();
}

TEST(productRecvIn, Container4_SUCCESS) {
    COMMENT("Receive Container4 SUCCESS");
    Tester tester;
    tester.productRecvIn_Container4_SUCCESS();
}

TEST(productRecvIn, Container4_FAILURE) {
    COMMENT("Receive Container4 FAILURE");
    Tester tester;
    tester.productRecvIn_Container4_FAILURE();
}

TEST(productRecvIn, Container5_SUCCESS) {
    COMMENT("Receive Container5 SUCCESS");
    Tester tester;
    tester.productRecvIn_Container5_SUCCESS();
}

TEST(productRecvIn, Container5_FAILURE) {
    COMMENT("Receive Container5 FAILURE");
    Tester tester;
    tester.productRecvIn_Container5_FAILURE();
}

TEST(productRecvIn, Container6_SUCCESS) {
    COMMENT("Receive Container6 SUCCESS");
    Tester tester;
    tester.productRecvIn_Container6_SUCCESS();
}

TEST(productRecvIn, Container6_FAILURE) {
    COMMENT("Receive Container6 FAILURE");
    Tester tester;
    tester.productRecvIn_Container6_FAILURE();
}

TEST(productRecvIn, Container7_SUCCESS) {
    COMMENT("Receive Container7 SUCCESS");
    Tester tester;
    tester.productRecvIn_Container7_SUCCESS();
}

TEST(productRecvIn, Container7_FAILURE) {
    COMMENT("Receive Container7 FAILURE");
    Tester tester;
    tester.productRecvIn_Container7_FAILURE();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
