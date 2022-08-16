#include "FppTest/array/StringArrayAc.hpp"
#include "FppTest/array/String100ArrayAc.hpp"
#include "FppTest/string/StringTest.hpp"

#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

// Instantiate string tests for arrays
using StringTestImplementations = ::testing::Types<
    String::StringSize80,
    String100::StringSize100
>;
INSTANTIATE_TYPED_TEST_SUITE_P(Array, StringTest, StringTestImplementations);

template<>
U32 getSize<String100::StringSize100>() {
    return 100;
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
