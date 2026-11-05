#include <gtest/gtest.h>
#include <Fw/Types/Optional.hpp>

// Test with a simple integer type
TEST(OptionalTest, DefaultConstructIsEmpty) {
    Fw::Optional<I32> opt;
    ASSERT_FALSE(opt.has_value());
    ASSERT_EQ(opt, Fw::NONE);
}

TEST(OptionalTest, NoneConstructIsEmpty) {
    Fw::Optional<I32> opt(Fw::NONE);
    ASSERT_FALSE(opt.has_value());
}

TEST(OptionalTest, ValueConstructHasValue) {
    Fw::Optional<I32> opt(42);
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), 42);
}

TEST(OptionalTest, CopyConstructEmpty) {
    Fw::Optional<I32> opt1;
    Fw::Optional<I32> opt2(opt1);
    ASSERT_FALSE(opt2.has_value());
}

TEST(OptionalTest, CopyConstructWithValue) {
    Fw::Optional<I32> opt1(99);
    Fw::Optional<I32> opt2(opt1);
    ASSERT_TRUE(opt2.has_value());
    ASSERT_EQ(opt2.value(), 99);
}

TEST(OptionalTest, AssignValue) {
    Fw::Optional<I32> opt;
    opt = 7;
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), 7);
}

TEST(OptionalTest, AssignNone) {
    Fw::Optional<I32> opt(10);
    opt = Fw::NONE;
    ASSERT_FALSE(opt.has_value());
}

TEST(OptionalTest, Reset) {
    Fw::Optional<I32> opt(5);
    ASSERT_TRUE(opt.has_value());
    opt.reset();
    ASSERT_FALSE(opt.has_value());
}

TEST(OptionalTest, ValueOrWithValue) {
    Fw::Optional<I32> opt(42);
    ASSERT_EQ(opt.value_or(0), 42);
}

TEST(OptionalTest, ValueOrWithoutValue) {
    Fw::Optional<I32> opt;
    ASSERT_EQ(opt.value_or(99), 99);
}

TEST(OptionalTest, MutableValueAccess) {
    Fw::Optional<I32> opt(10);
    opt.value() = 20;
    ASSERT_EQ(opt.value(), 20);
}

TEST(OptionalTest, CopyAssignmentWithValue) {
    Fw::Optional<I32> opt1(42);
    Fw::Optional<I32> opt2;
    opt2 = opt1;
    ASSERT_TRUE(opt2.has_value());
    ASSERT_EQ(opt2.value(), 42);
}

TEST(OptionalTest, CopyAssignmentWithEmpty) {
    Fw::Optional<I32> opt1;
    Fw::Optional<I32> opt2(42);
    opt2 = opt1;
    ASSERT_FALSE(opt2.has_value());
}

TEST(OptionalTest, EqualityBothEmpty) {
    Fw::Optional<I32> opt1;
    Fw::Optional<I32> opt2;
    ASSERT_EQ(opt1, opt2);
}

TEST(OptionalTest, EqualitySameValue) {
    Fw::Optional<I32> opt1(42);
    Fw::Optional<I32> opt2(42);
    ASSERT_EQ(opt1, opt2);
}

TEST(OptionalTest, InequalityDifferentValues) {
    Fw::Optional<I32> opt1(1);
    Fw::Optional<I32> opt2(2);
    ASSERT_NE(opt1, opt2);
}

TEST(OptionalTest, InequalityOneEmpty) {
    Fw::Optional<I32> opt1(1);
    Fw::Optional<I32> opt2;
    ASSERT_NE(opt1, opt2);
}

TEST(OptionalTest, NoneComparisonOperators) {
    Fw::Optional<I32> empty;
    Fw::Optional<I32> full(42);
    ASSERT_TRUE(empty == Fw::NONE);
    ASSERT_FALSE(empty != Fw::NONE);
    ASSERT_FALSE(full == Fw::NONE);
    ASSERT_TRUE(full != Fw::NONE);
}

TEST(OptionalTest, ReassignValue) {
    Fw::Optional<I32> opt(1);
    opt = 2;
    ASSERT_EQ(opt.value(), 2);
    opt = 3;
    ASSERT_EQ(opt.value(), 3);
}

// Test with a struct type
struct TestStruct {
    I32 x;
    I32 y;
    bool operator==(const TestStruct& other) const { return x == other.x && y == other.y; }
};

TEST(OptionalTest, StructValue) {
    TestStruct s{10, 20};
    Fw::Optional<TestStruct> opt(s);
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value().x, 10);
    ASSERT_EQ(opt.value().y, 20);
}

TEST(OptionalTest, StructAssign) {
    Fw::Optional<TestStruct> opt;
    TestStruct s{5, 6};
    opt = s;
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value().x, 5);
    ASSERT_EQ(opt.value().y, 6);
}

// Test with a floating point type
TEST(OptionalTest, FloatingPointValue) {
    Fw::Optional<F64> opt(3.14);
    ASSERT_TRUE(opt.has_value());
    ASSERT_DOUBLE_EQ(opt.value(), 3.14);
}

// Test constexpr construction
TEST(OptionalTest, ConstexprEmpty) {
    constexpr Fw::Optional<I32> opt;
    ASSERT_FALSE(opt.has_value());
}

TEST(OptionalTest, ConstexprWithValue) {
    constexpr Fw::Optional<I32> opt(42);
    ASSERT_TRUE(opt.has_value());
    ASSERT_EQ(opt.value(), 42);
}
