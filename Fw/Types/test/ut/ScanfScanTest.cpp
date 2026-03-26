#include <gtest/gtest.h>

#include "Fw/Types/scan.hpp"

#include <cstdarg>
#include <limits>

namespace {

Fw::ScanStatus stringScanWithVaList(FwSizeType& count,
                                    char* source,
                                    const FwSizeType maximumSize,
                                    const char* formatString,

                                    ...) {
    va_list args;
    va_start(args, formatString);
    Fw::ScanStatus status = Fw::stringScan(count, source, maximumSize, formatString, args);
    va_end(args);
    return status;
}

TEST(Nominal, scanIntegerWithVariadicOverload) {
    char source[] = "123";
    int parsed = 0;
    FwSizeType count = 0;

    const Fw::ScanStatus status = Fw::stringScan(count, source, sizeof(source), "%d", &parsed);

    EXPECT_EQ(status, Fw::ScanStatus::SUCCESS);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(parsed, 123);
}

TEST(Nominal, scanIntegerAndWordWithVaListOverload) {
    char source[] = "42 abc";
    int parsedNumber = 0;
    char parsedWord[4] = {};
    FwSizeType count = 0;

    const Fw::ScanStatus status =
        stringScanWithVaList(count, source, sizeof(source), "%d %3s", &parsedNumber, parsedWord);

    EXPECT_EQ(status, Fw::ScanStatus::SUCCESS);
    EXPECT_EQ(count, 2);
    EXPECT_EQ(parsedNumber, 42);
    EXPECT_STREQ(parsedWord, "abc");
}

TEST(Errors, nullFormatStringReturnsInvalidFormatString) {
    char source[] = "123";
    int parsed = 0;
    FwSizeType count = 0;

    const Fw::ScanStatus status = Fw::stringScan(count, source, sizeof(source), nullptr, &parsed);

    EXPECT_EQ(status, Fw::ScanStatus::INVALID_FORMAT_STRING);
    EXPECT_EQ(count, 0);
}

TEST(Errors, unterminatedSourceReturnsError) {
    char source[4] = {'1', '2', '3', '4'};
    int parsed = 0;
    FwSizeType count = 0;

    const Fw::ScanStatus status = Fw::stringScan(count, source, sizeof(source), "%d", &parsed);

    EXPECT_EQ(status, Fw::ScanStatus::UNTERMINATED_SOURCE_STRING);
    EXPECT_EQ(count, 0);
}

TEST(Nominal, zeroConversionsReturnsSuccessAndCountZero) {
    char source[] = "abc";
    int parsed = 77;
    FwSizeType count = 99;

    const Fw::ScanStatus status = Fw::stringScan(count, source, sizeof(source), "%d", &parsed);

    EXPECT_EQ(status, Fw::ScanStatus::SUCCESS);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(parsed, 77);
}

TEST(Nominal, noCaptureFormatReturnsSuccessAndCountZero) {
    char source[] = "literal";
    FwSizeType count = 99;

    const Fw::ScanStatus status = Fw::stringScan(count, source, sizeof(source), "literal");

    EXPECT_EQ(status, Fw::ScanStatus::SUCCESS);
    EXPECT_EQ(count, 0);
}

TEST(Errors, overflowMaximumSizeReturnsSizeOverflow) {
    if (std::numeric_limits<FwSizeType>::max() <= std::numeric_limits<size_t>::max()) {
        GTEST_SKIP() << "Cannot create maximumSize > size_t::max() when FwSizeType and size_t have equal max.";
    }

    char source[] = "123";
    int parsed = 0;
    FwSizeType count = 0;
    const FwSizeType overflowMaximumSize =
        static_cast<FwSizeType>(std::numeric_limits<size_t>::max()) + static_cast<FwSizeType>(1);

    const Fw::ScanStatus status = Fw::stringScan(count, source, overflowMaximumSize, "%d", &parsed);

    EXPECT_EQ(status, Fw::ScanStatus::SIZE_OVERFLOW);
    EXPECT_EQ(count, 0);
}

}  // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
