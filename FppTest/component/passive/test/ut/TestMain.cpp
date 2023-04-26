// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include "FppTest/component/macros.hpp"
#include "FppTest/types/FormalParamTypes.hpp"
#include "FppTest/typed_tests/ComponentTest.hpp"

TLM_TEST_INST

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
