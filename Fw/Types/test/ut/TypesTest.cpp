#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Os/IntervalTimer.hpp>
#include <Os/InterruptLock.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/InternalInterfaceString.hpp>
#include <Fw/Types/PolyType.hpp>

#include <stdio.h>
#include <string.h>
#include <iostream>

#include <iostream>

#include <iostream>

#define DEBUG_VERBOSE 1

#include <gtest/gtest.h>

class SerializeTestBuffer: public Fw::SerializeBufferBase {
    public:
        NATIVE_UINT_TYPE getBuffCapacity(void) const { // !< returns capacity, not current size, of buffer
            return sizeof(m_testBuff);
        }

        U8* getBuffAddr(void) { // !< gets buffer address for data filling
            return m_testBuff;
        }
        const U8* getBuffAddr(void) const { // !< gets buffer address for data reading
            return m_testBuff;
        }
    private:
        U8 m_testBuff[255];
};

TEST(SerializationTest,Serialization1) {

    printf("Testing Serialization code\n");

    SerializeTestBuffer buff;

#if DEBUG_VERBOSE
    printf("U8 Test\n");
#endif

    U8 u8t1 = 10;
    U8 u8t2 = 0;

    Fw::SerializeStatus stat1;
    Fw::SerializeStatus stat2;

// Test chars
    buff.resetSer();
    stat1 = buff.serialize(u8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(u8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);

    ASSERT_EQ(u8t2,u8t1);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", u8t1, u8t2, stat1,
            stat2);
    printf("I8 Test\n");
#endif

    I8 i8t1 = -10;
    I8 i8t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(i8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i8t1,i8t2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", i8t1, i8t2, stat1,
            stat2);
    printf("U16 Test\n");
#endif

    U16 u16t1 = 1000;
    U16 u16t2 = 0;

// Test shorts

    buff.resetSer();
    stat1 = buff.serialize(u16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(u16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u16t1,u16t2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", u16t1, u16t2, stat1,
            stat2);
    printf("I16 test\n");
#endif

    I16 i16t1 = -1000;
    I16 i16t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(i16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i16t1,i16t2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", i16t1, i16t2, stat1,
            stat2);

    printf("U32 Test\n");
#endif

    U32 u32t1 = 100000;
    U32 u32t2 = 0;

// Test ints

    buff.resetSer();
    stat1 = buff.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u32t1,u32t2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", u32t1, u32t2, stat1,
            stat2);
    printf("I32 Test\n");
#endif

    I32 i32t1 = -100000;
    I32 i32t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(i32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i32t1,i32t2);

#if DEBUG_VERBOSE
    printf("Val: in: %d out: %d stat1: %d stat2: %d\n", i32t1, i32t2, stat1,
            stat2);

    printf("U64 Test\n");
#endif

    U64 u64t1 = 10000000;
    U64 u64t2 = 0;

// Test ints

    buff.resetSer();
    stat1 = buff.serialize(u64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(u64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u64t1,u64t2);

#if DEBUG_VERBOSE
    printf("Val: in: %lld out: %lld stat1: %d stat2: %d\n", u64t1, u64t2, stat1,
            stat2);
    printf("I64 Test\n");
#endif

    I64 i64t1 = -10000000;
    I64 i64t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(i64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(i64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i64t1,i64t2);

#if DEBUG_VERBOSE
    printf("Val: in: %lld out: %lld stat1: %d stat2: %d\n", i64t1, i64t2, stat1,
            stat2);

    printf("F32 Test\n");
#endif

    F32 f32t1 = 100.2;
    F32 f32t2 = 0;

// Test ints

    buff.resetSer();
    stat1 = buff.serialize(f32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(f32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_FLOAT_EQ(f32t1,f32t2);

#if DEBUG_VERBOSE
    printf("Val: in: %f out: %f stat1: %d stat2: %d\n", f32t1, f32t2, stat1,
            stat2);
    printf("F64 Test\n");
#endif

    F64 f64t1 = -100.232145345346534;
    F64 f64t2 = 0;

    buff.resetSer();
    stat1 = buff.serialize(f64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(f64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_DOUBLE_EQ(f32t1,f32t2);

#if DEBUG_VERBOSE
    printf("Val: in: %lf out: %lf stat1: %d stat2: %d\n", f64t1, f64t2, stat1,
            stat2);
    printf("bool Test\n");
#endif

    bool boolt1 = true;
    bool boolt2 = false;

    buff.resetSer();
    stat1 = buff.serialize(boolt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserialize(boolt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(boolt1,boolt2);

#if DEBUG_VERBOSE
    printf("Val: in: %s out: %s stat1: %d stat2: %d\n",
            boolt1 ? "TRUE" : "FALSE", boolt2 ? "TRUE" : "FALSE", stat1, stat2);

    printf("Skip deserialization Tests\n");
#endif

// Test skipping:

    buff.resetSer();
    stat1 = buff.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.serialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);

    // should fail:
    stat1 = buff.deserializeSkip(10);
    ASSERT_EQ(Fw::FW_DESERIALIZE_SIZE_MISMATCH,stat1);

    // skip everything:
    stat1 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat2 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);

    // should fail:
    stat1 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY,stat1);

    // skip half/read half:
    buff.resetDeser();
    stat1 = buff.deserializeSkip(4);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    U32 u32val;
    stat2 = buff.deserialize(u32val);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u32t2,u32val);

#if DEBUG_VERBOSE
    printf("\nDeserialization Tests\n");
#endif

    SerializeTestBuffer buff2;

// Do a series of serializations
    u8t2 = 0;
    i8t2 = 0;
    u16t2 = 0;
    i16t2 = 0;
    u32t2 = 0;
    i32t2 = 0;
    u64t2 = 0;
    i64t2 = 0;
    f32t2 = 0.0;
    f64t2 = 0.0;
    boolt2 = false;

    buff.resetSer();
    stat1 = buff.serialize(u8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(i8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(u16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(i16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(i32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(u64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(i64t1);
    printf("i64t1 in stat: %d\n", stat1);
    stat1 = buff.serialize(f32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(f64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff.serialize(boolt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);

    std::cout << "Buffer contents: " << buff << std::endl;

    // Serialize second buffer and test for equality
    buff2.resetSer();
    stat1 = buff2.serialize(u8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(i8t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(u16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(i16t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(u32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(i32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(u64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(i64t1);
    printf("i64t1 in stat: %d\n", stat1);
    stat1 = buff2.serialize(f32t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(f64t1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);
    stat1 = buff2.serialize(boolt1);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat1);

    ASSERT_EQ(buff,buff2);

    // deserialize

    stat2 = buff.deserialize(u8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u8t1,u8t2);
    stat2 = buff.deserialize(i8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i8t1,i8t2);
    stat2 = buff.deserialize(u16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u16t1,u16t2);
    stat2 = buff.deserialize(i16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i16t1,i16t2);
    stat2 = buff.deserialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u32t1,u32t2);
    stat2 = buff.deserialize(i32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i32t1,i32t2);
    stat2 = buff.deserialize(u64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u64t1,u64t2);
    stat2 = buff.deserialize(i64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i64t1,i64t2);
    stat2 = buff.deserialize(f32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_FLOAT_EQ(f32t1,f32t2);
    stat2 = buff.deserialize(f64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_DOUBLE_EQ(f64t1,f64t2);
    stat2 = buff.deserialize(boolt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(boolt1,boolt2);

// reset and deserialize again
#if DEBUG_VERBOSE
    printf("\nReset and deserialize again.\n");
#endif

    buff.resetDeser();

    u8t2 = 0;
    i8t2 = 0;
    u16t2 = 0;
    i16t2 = 0;
    u32t2 = 0;
    i32t2 = 0;
    u64t2 = 0;
    i64t2 = 0;
    f32t2 = 0.0;
    f64t2 = 0.0;
    boolt2 = false;

    stat2 = buff.deserialize(u8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u8t1,u8t2);
    stat2 = buff.deserialize(i8t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i8t1,i8t2);
    stat2 = buff.deserialize(u16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u16t1,u16t2);
    stat2 = buff.deserialize(i16t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i16t1,i16t2);
    stat2 = buff.deserialize(u32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u32t1,u32t2);
    stat2 = buff.deserialize(i32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i32t1,i32t2);
    stat2 = buff.deserialize(u64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(u64t1,u64t2);
    stat2 = buff.deserialize(i64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(i64t1,i64t2);
    stat2 = buff.deserialize(f32t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_FLOAT_EQ(f32t1,f32t2);
    stat2 = buff.deserialize(f64t2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_DOUBLE_EQ(f64t1,f64t2);
    stat2 = buff.deserialize(boolt2);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,stat2);
    ASSERT_EQ(boolt1,boolt2);

    // serialize string
    Fw::EightyCharString str1;
    Fw::EightyCharString str2;

    str1 = "Foo";
    str2 = "BarBlat";
    buff.resetSer();
    str1.serialize(buff);
    str2.deserialize(buff);
    ASSERT_EQ(str1,str2);


}

struct TestStruct {
        U32 m_u32;
        U16 m_u16;
        U8 m_u8;
        F32 m_f32;
        U8 m_buff[25];
};

class MySerializable: public Fw::Serializable {
    public:
        Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const {
            buffer.serialize(m_testStruct.m_u32);
            buffer.serialize(m_testStruct.m_u16);
            buffer.serialize(m_testStruct.m_u8);
            buffer.serialize(m_testStruct.m_f32);
            buffer.serialize(m_testStruct.m_buff, sizeof(m_testStruct.m_buff));
            return Fw::FW_SERIALIZE_OK;
        }

        Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) {

            buffer.serialize(m_testStruct.m_buff, sizeof(m_testStruct.m_buff));
            buffer.serialize(m_testStruct.m_f32);
            buffer.serialize(m_testStruct.m_u8);
            buffer.serialize(m_testStruct.m_u16);
            buffer.serialize(m_testStruct.m_u32);
            return Fw::FW_SERIALIZE_OK;
        }
    private:
        TestStruct m_testStruct;
};

void serPerfTest(I32 iterations) {

    Os::IntervalTimer timer;

    MySerializable in;
    MySerializable out;
    SerializeTestBuffer buff;

    Os::InterruptLock intLock;

    intLock.lock();
    timer.start();

    for (I32 iter = 0; iter < iterations; iter++) {
        in.serialize(buff);
        out.deserialize(buff);
    }

    timer.stop();
    intLock.unLock();

    printf("%d iterations took %d us (%f each).\n", iterations,
            timer.getDiffUsec(),
            (F32) (timer.getDiffUsec()) / (F32) iterations);

}

void structCopyTest(I32 iterations) {
    char buff[sizeof(TestStruct)];
    TestStruct ts;

    Os::InterruptLock intLock;
    Os::IntervalTimer timer;

    intLock.lock();
    timer.start();

    for (I32 iter = 0; iter < iterations; iter++) {

// simulate the incoming MSL-style call by doing member assignments
        ts.m_u32 = 0;
        ts.m_u16 = 0;
        ts.m_u8 = 0;
        ts.m_f32 = 0.0;
        memcpy(ts.m_buff, "1234567890123456789012345", sizeof(ts.m_buff));

        memcpy(buff, &ts, sizeof(ts));
        memcpy(&ts, buff, sizeof(buff));
    }

    timer.stop();
    intLock.unLock();

    printf("%d iterations took %d us (%f each).\n", iterations,
            timer.getDiffUsec(),
            (F32) (timer.getDiffUsec()) / (F32) iterations);

}

void classCopyTest(I32 iterations) {

    char buff[sizeof(MySerializable)];
    MySerializable ms;

    Os::InterruptLock intLock;
    Os::IntervalTimer timer;

    intLock.lock();
    timer.start();

    for (I32 iter = 0; iter < iterations; iter++) {
        memcpy(buff, &ms, sizeof(ms));
        memcpy(&ms, buff, sizeof(buff));
    }

    timer.stop();
    intLock.unLock();

    printf("%d iterations took %d us (%f each).\n", iterations,
            timer.getDiffUsec(),
            (F32) (timer.getDiffUsec()) / (F32) iterations);

}

void printSizes(void) {
    printf("Sizeof TestStruct: %d\n", sizeof(TestStruct));
    printf("Sizeof MySerializable: %d\n", sizeof(MySerializable));
}

void AssertTest(void) {

    printf("Assert Tests\n");

    // Since native FW_ASSERT actually asserts,
    // manually test by setting arguments to
    // unequal values below one by one
//    FW_ASSERT(0 == 1);
//    FW_ASSERT(0 == 1, 1);
//    FW_ASSERT(0 == 1, 1, 2);
//    FW_ASSERT(0 == 1, 1, 2, 3);
//    FW_ASSERT(0 == 1, 1, 2, 3, 4);
//    FW_ASSERT(0 == 1, 1, 2, 3, 4, 5);
//    FW_ASSERT(0 == 1, 1, 2, 3, 4, 5, 6);

    // Define an Assert handler
    class TestAssertHook : public Fw::AssertHook {
        public:
            TestAssertHook() {}
            virtual ~TestAssertHook() {}
            void reportAssert(
                                FILE_NAME_ARG file,
                                NATIVE_UINT_TYPE lineNo,
                                NATIVE_UINT_TYPE numArgs,
                                AssertArg arg1,
                                AssertArg arg2,
                                AssertArg arg3,
                                AssertArg arg4,
                                AssertArg arg5,
                                AssertArg arg6
                                ) {
                this->m_file = file;
                this->m_lineNo = lineNo;
                this->m_numArgs = numArgs;
                this->m_arg1 = arg1;
                this->m_arg2 = arg2;
                this->m_arg3 = arg3;
                this->m_arg4 = arg4;
                this->m_arg5 = arg5;
                this->m_arg6 = arg6;

            };

            void doAssert(void) {
                this->m_asserted = true;
            }

            FILE_NAME_ARG getFile(void) {
                return this->m_file;
            }

            NATIVE_UINT_TYPE getLineNo(void) {
                return this->m_lineNo;
            }

            NATIVE_UINT_TYPE getNumArgs(void) {
                return this->m_numArgs;
            }

            AssertArg getArg1(void) {
                return this->m_arg1;
            }

            AssertArg getArg2(void) {
                return this->m_arg2;
            }

            AssertArg getArg3(void) {
                return this->m_arg3;
            }

            AssertArg getArg4(void) {
                return this->m_arg4;
            }

            AssertArg getArg5(void) {
                return this->m_arg5;
            }

            AssertArg getArg6(void) {
                return this->m_arg6;
            }

            bool asserted(void) {
                bool didAssert = this->m_asserted;
                this->m_asserted = false;
                return didAssert;
            }


        private:

            FILE_NAME_ARG m_file;
            NATIVE_UINT_TYPE m_lineNo;
            NATIVE_UINT_TYPE m_numArgs;
            AssertArg m_arg1;
            AssertArg m_arg2;
            AssertArg m_arg3;
            AssertArg m_arg4;
            AssertArg m_arg5;
            AssertArg m_arg6;
            bool m_asserted;

    };

    // register the class
    TestAssertHook hook;
    hook.registerHook();

    // issue an assert
    FW_ASSERT(0);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ((NATIVE_UINT_TYPE)0,hook.getNumArgs());

    // issue an assert
    FW_ASSERT(0,1);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ((NATIVE_UINT_TYPE)1,hook.getNumArgs());
    ASSERT_EQ((NATIVE_UINT_TYPE)1,hook.getArg1());

    // issue an assert
    FW_ASSERT(0,1,2);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ((NATIVE_UINT_TYPE)2,hook.getNumArgs());
    ASSERT_EQ((NATIVE_UINT_TYPE)1,hook.getArg1());
    ASSERT_EQ((NATIVE_UINT_TYPE)2,hook.getArg2());

    // issue an assert
    FW_ASSERT(0,1,2,3);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ((NATIVE_UINT_TYPE)3,hook.getNumArgs());
    ASSERT_EQ((NATIVE_UINT_TYPE)1,hook.getArg1());
    ASSERT_EQ((NATIVE_UINT_TYPE)2,hook.getArg2());
    ASSERT_EQ((NATIVE_UINT_TYPE)3,hook.getArg3());

    // issue an assert
    FW_ASSERT(0,1,2,3,4);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ((NATIVE_UINT_TYPE)4,hook.getNumArgs());
    ASSERT_EQ((NATIVE_UINT_TYPE)1,hook.getArg1());
    ASSERT_EQ((NATIVE_UINT_TYPE)2,hook.getArg2());
    ASSERT_EQ((NATIVE_UINT_TYPE)3,hook.getArg3());
    ASSERT_EQ((NATIVE_UINT_TYPE)4,hook.getArg4());

    // issue an assert
    FW_ASSERT(0,1,2,3,4,5);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ((NATIVE_UINT_TYPE)5,hook.getNumArgs());
    ASSERT_EQ((NATIVE_UINT_TYPE)1,hook.getArg1());
    ASSERT_EQ((NATIVE_UINT_TYPE)2,hook.getArg2());
    ASSERT_EQ((NATIVE_UINT_TYPE)3,hook.getArg3());
    ASSERT_EQ((NATIVE_UINT_TYPE)4,hook.getArg4());
    ASSERT_EQ((NATIVE_UINT_TYPE)5,hook.getArg5());

    // issue an assert
    FW_ASSERT(0,1,2,3,4,5,6);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ((NATIVE_UINT_TYPE)6,hook.getNumArgs());
    ASSERT_EQ((NATIVE_UINT_TYPE)1,hook.getArg1());
    ASSERT_EQ((NATIVE_UINT_TYPE)2,hook.getArg2());
    ASSERT_EQ((NATIVE_UINT_TYPE)3,hook.getArg3());
    ASSERT_EQ((NATIVE_UINT_TYPE)4,hook.getArg4());
    ASSERT_EQ((NATIVE_UINT_TYPE)5,hook.getArg5());
    ASSERT_EQ((NATIVE_UINT_TYPE)6,hook.getArg6());

}

TEST(TypesTest, CheckAssertTest) {
    AssertTest();
}

TEST(TypesTest,PolyTest) {

    U8 in8 = 13;
    U8 out8;

    Fw::PolyType pt(in8);

    printf("U8 PolyType Test\n");
    out8 = (U8) pt;
    if (out8 != in8) {
        printf("U8 in %d out %d mismatch!\n", in8, out8);
    }

// Should assert
// out16 = (U16)pt;
}

TEST(TypesTest,EightyCharTest) {

    Fw::EightyCharString str;
    str = "foo";
    Fw::EightyCharString str2;
    str2 = "foo";
    ASSERT_EQ(str,str2);
    ASSERT_EQ(str,"foo");
    str2 = "doodie";
    ASSERT_NE(str,str2);

    Fw::EightyCharString str3 = str;
    str3 += str2;
    ASSERT_EQ(str3,"foodoodie");

    str3 += "hoo";
    ASSERT_EQ(str3,"foodoodiehoo");


    Fw::EightyCharString copyStr("ASTRING");
    ASSERT_EQ(copyStr,"ASTRING");
    Fw::EightyCharString copyStr2 = "ASTRING";
    ASSERT_EQ(copyStr2,"ASTRING");
    Fw::EightyCharString copyStr3(copyStr2);
    ASSERT_EQ(copyStr3,"ASTRING");

    Fw::InternalInterfaceString ifstr("IfString");
    Fw::EightyCharString if2(ifstr);

    ASSERT_EQ(ifstr,if2);
    ASSERT_EQ(if2,"IfString");


    std::cout << "Stream: " << str2 << std::endl;

    // Make our own short string


}

TEST(TypesTest,StringFormatTest) {
    Fw::EightyCharString str;
    str.format("Int %d String %s",10,"foo");
    printf("Formatted: %s\n",str.toChar());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
