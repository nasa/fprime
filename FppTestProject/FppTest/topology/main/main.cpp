// ======================================================================
// \title  main.cpp
// \author bocchino, tumbar
// \brief  Main program for FppTest deployment
// ======================================================================

#include "FppTest/component/types/FormalParamTypes.hpp"
#include "gtest/gtest.h"

#include "FppTest/topology/main/FppTestTopologyAc.hpp"
#include "FppTest/topology/ports/SenderIdEnumAc.hpp"
#include "FppTestTopologyDefs.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Os.hpp"

// For stack trace printing
#if defined(__linux__) || defined(__APPLE__)
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#endif

namespace FppTest {
static TopologyState state;

// Custom assertion hook that prints stack trace
class StackTraceAssertHook : public Fw::AssertHook {
  public:
    void reportAssert(FILE_NAME_ARG file,
                      FwSizeType lineNo,
                      FwSizeType numArgs,
                      FwAssertArgType arg1,
                      FwAssertArgType arg2,
                      FwAssertArgType arg3,
                      FwAssertArgType arg4,
                      FwAssertArgType arg5,
                      FwAssertArgType arg6) override {
        // Call base class to print the assertion message
        Fw::AssertHook::reportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);

        // Print stack trace
#if defined(__linux__) || defined(__APPLE__)
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        fprintf(stderr, "\nStack trace:\n");

        for (int i = 0; i < frames; i++) {
            Dl_info info;
            if (dladdr(callstack[i], &info)) {
                // Demangle C++ symbol name
                char* demangled = nullptr;
                int status = -1;
                if (info.dli_sname) {
                    demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
                }

                // Calculate offset from symbol
                ptrdiff_t offset = reinterpret_cast<char*>(callstack[i]) - reinterpret_cast<char*>(info.dli_saddr);

                fprintf(stderr, "  [%d] %p %s + %td", i, callstack[i],
                        (status == 0 && demangled) ? demangled : (info.dli_sname ? info.dli_sname : "???"), offset);

#ifdef __APPLE__
                // On macOS, use atos to get file:line information
                char cmd[1024];
                snprintf(cmd, sizeof(cmd), "atos -o %s -l %p %p 2>/dev/null", info.dli_fname, info.dli_fbase,
                         callstack[i]);
                FILE* pipe = popen(cmd, "r");
                if (pipe) {
                    char atos_output[512];
                    if (fgets(atos_output, sizeof(atos_output), pipe)) {
                        // Remove trailing newline
                        size_t len = strlen(atos_output);
                        if (len > 0 && atos_output[len - 1] == '\n') {
                            atos_output[len - 1] = '\0';
                        }
                        fprintf(stderr, " (%s)", atos_output);
                    }
                    pclose(pipe);
                }
#endif
                fprintf(stderr, "\n");

                if (demangled) {
                    free(demangled);
                }
            } else {
                fprintf(stderr, "  [%d] %p ???\n", i, callstack[i]);
            }
        }
#endif
    }
};

static StackTraceAssertHook assertHook;

class SenderTester : public testing::Test {
  public:
    static void SetUpTestSuite() {
        assertHook.registerHook();
        Os::init();
        setup(state);
    }

    static void TearDownTestSuite() { teardown(state); }

    static void testIsConnected() {
#define CHECK_IS_CONNECTED_TYPE(kind, portName)                        \
    ASSERT_TRUE(sender1##kind.isConnected_##portName##_OutputPort(0)); \
    ASSERT_TRUE(sender1##kind.isConnected_##portName##_OutputPort(1)); \
    ASSERT_TRUE(sender2##kind.isConnected_##portName##_OutputPort(0)); \
    ASSERT_TRUE(sender2##kind.isConnected_##portName##_OutputPort(1));

#define CHECK_IS_CONNECTED_SG(portName)     \
    CHECK_IS_CONNECTED_TYPE(Sync, portName) \
    CHECK_IS_CONNECTED_TYPE(Guarded, portName)

#define CHECK_IS_CONNECTED_SGA(portName)       \
    CHECK_IS_CONNECTED_TYPE(Sync, portName)    \
    CHECK_IS_CONNECTED_TYPE(Guarded, portName) \
    CHECK_IS_CONNECTED_TYPE(Async, portName)

        CHECK_IS_CONNECTED_SGA(noArgsOut)
        CHECK_IS_CONNECTED_SGA(primitiveArgsOut)
        CHECK_IS_CONNECTED_SGA(stringArgsOut)
        CHECK_IS_CONNECTED_SGA(enumArgsOut)
        CHECK_IS_CONNECTED_SGA(arrayArgsOut)
        CHECK_IS_CONNECTED_SGA(structArgsOut)

        CHECK_IS_CONNECTED_SG(noArgsReturnOut)
        CHECK_IS_CONNECTED_SG(primitiveReturnOut)
        CHECK_IS_CONNECTED_SG(stringReturnOut)
        CHECK_IS_CONNECTED_SG(stringAliasReturnOut)
        CHECK_IS_CONNECTED_SG(enumReturnOut)
        CHECK_IS_CONNECTED_SG(arrayReturnOut)
        CHECK_IS_CONNECTED_SG(arrayStringAliasReturnOut)
        CHECK_IS_CONNECTED_SG(structReturnOut)
    }
};

class ReceiverTester {
  public:
    virtual ~ReceiverTester() = default;

    static void testIsConnected() {
        ASSERT_TRUE(receiver1.isConnected_replyOut_OutputPort(SenderId::SYNC));
        ASSERT_TRUE(receiver2.isConnected_replyOut_OutputPort(SenderId::SYNC));
        ASSERT_TRUE(receiver1.isConnected_replyOut_OutputPort(SenderId::GUARDED));
        ASSERT_TRUE(receiver2.isConnected_replyOut_OutputPort(SenderId::GUARDED));
        ASSERT_TRUE(receiver1.isConnected_replyOut_OutputPort(SenderId::ASYNC));
        ASSERT_TRUE(receiver2.isConnected_replyOut_OutputPort(SenderId::ASYNC));
    }
};

TEST_F(SenderTester, NoArgs) {
    sender1Sync.testNoArgs(TestDeploymentPort::NO_ARGS_SYNC);
    sender2Sync.testNoArgs(TestDeploymentPort::NO_ARGS_SYNC);
    sender1Guarded.testNoArgs(TestDeploymentPort::NO_ARGS_GUARDED);
    sender2Guarded.testNoArgs(TestDeploymentPort::NO_ARGS_GUARDED);
    sender1Async.testNoArgs(TestDeploymentPort::NO_ARGS_ASYNC);
    sender2Async.testNoArgs(TestDeploymentPort::NO_ARGS_ASYNC);
}

TEST_F(SenderTester, PrimitiveArgs) {
    sender1Sync.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
    sender2Sync.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
    sender1Guarded.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_GUARDED);
    sender2Guarded.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_GUARDED);
    sender1Async.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_ASYNC);
    sender2Async.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_ASYNC);
}

TEST_F(SenderTester, StringArgs) {
    sender1Sync.testStringArgs(TestDeploymentPort::STRING_ARGS_SYNC);
    sender2Sync.testStringArgs(TestDeploymentPort::STRING_ARGS_SYNC);
    sender1Guarded.testStringArgs(TestDeploymentPort::STRING_ARGS_GUARDED);
    sender2Guarded.testStringArgs(TestDeploymentPort::STRING_ARGS_GUARDED);
    sender1Async.testStringArgs(TestDeploymentPort::STRING_ARGS_ASYNC);
    sender2Async.testStringArgs(TestDeploymentPort::STRING_ARGS_ASYNC);
}

TEST_F(SenderTester, EnumArgs) {
    sender1Sync.testEnumArgs(TestDeploymentPort::ENUM_ARGS_SYNC);
    sender2Sync.testEnumArgs(TestDeploymentPort::ENUM_ARGS_SYNC);
    sender1Guarded.testEnumArgs(TestDeploymentPort::ENUM_ARGS_GUARDED);
    sender2Guarded.testEnumArgs(TestDeploymentPort::ENUM_ARGS_GUARDED);
    sender1Async.testEnumArgs(TestDeploymentPort::ENUM_ARGS_ASYNC);
    sender2Async.testEnumArgs(TestDeploymentPort::ENUM_ARGS_ASYNC);
}

TEST_F(SenderTester, ArrayArgs) {
    sender1Sync.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_SYNC);
    sender2Sync.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_SYNC);
    sender1Guarded.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_GUARDED);
    sender2Guarded.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_GUARDED);
    sender1Async.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_ASYNC);
    sender2Async.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_ASYNC);
}

TEST_F(SenderTester, StructArgs) {
    sender1Sync.testStructArgs(TestDeploymentPort::STRUCT_ARGS_SYNC);
    sender2Sync.testStructArgs(TestDeploymentPort::STRUCT_ARGS_SYNC);
    sender1Guarded.testStructArgs(TestDeploymentPort::STRUCT_ARGS_GUARDED);
    sender2Guarded.testStructArgs(TestDeploymentPort::STRUCT_ARGS_GUARDED);
    sender1Async.testStructArgs(TestDeploymentPort::STRUCT_ARGS_ASYNC);
    sender2Async.testStructArgs(TestDeploymentPort::STRUCT_ARGS_ASYNC);
}

TEST_F(SenderTester, NoArgsReturn) {
    sender1Sync.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_SYNC);
    sender2Sync.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_SYNC);
    sender1Guarded.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_GUARDED);
    sender2Guarded.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_GUARDED);
}

TEST_F(SenderTester, PrimitiveReturn) {
    sender1Sync.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_SYNC);
    sender2Sync.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_SYNC);
    sender1Guarded.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_GUARDED);
    sender2Guarded.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_GUARDED);
}

TEST_F(SenderTester, EnumReturn) {
    sender1Sync.testEnumReturn(TestDeploymentPort::ENUM_RETURN_SYNC);
    sender2Sync.testEnumReturn(TestDeploymentPort::ENUM_RETURN_SYNC);
    sender1Guarded.testEnumReturn(TestDeploymentPort::ENUM_RETURN_GUARDED);
    sender2Guarded.testEnumReturn(TestDeploymentPort::ENUM_RETURN_GUARDED);
}

TEST_F(SenderTester, StringReturn) {
    sender1Sync.testStringReturn(TestDeploymentPort::STRING_RETURN_SYNC);
    sender2Sync.testStringReturn(TestDeploymentPort::STRING_RETURN_SYNC);
    sender1Guarded.testStringReturn(TestDeploymentPort::STRING_RETURN_GUARDED);
    sender2Guarded.testStringReturn(TestDeploymentPort::STRING_RETURN_GUARDED);
}

TEST_F(SenderTester, StringAliasReturn) {
    sender1Sync.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_SYNC);
    sender2Sync.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_SYNC);
    sender1Guarded.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_GUARDED);
    sender2Guarded.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_GUARDED);
}

TEST_F(SenderTester, ArrayStringAliasReturn) {
    sender1Sync.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_SYNC);
    sender2Sync.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_SYNC);
    sender1Guarded.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_GUARDED);
    sender2Guarded.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_GUARDED);
}

TEST_F(SenderTester, ArrayReturn) {
    sender1Sync.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_SYNC);
    sender2Sync.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_SYNC);
    sender1Guarded.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_GUARDED);
    sender2Guarded.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_GUARDED);
}

TEST_F(SenderTester, StructReturn) {
    sender1Sync.testStructReturn(TestDeploymentPort::STRUCT_RETURN_SYNC);
    sender2Sync.testStructReturn(TestDeploymentPort::STRUCT_RETURN_SYNC);
    sender1Guarded.testStructReturn(TestDeploymentPort::STRUCT_RETURN_GUARDED);
    sender2Guarded.testStructReturn(TestDeploymentPort::STRUCT_RETURN_GUARDED);
}

TEST_F(SenderTester, SenderTop) {
    sender1Top.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
    sender2Top.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
}

TEST_F(SenderTester, IsReceiverConnected) {
    ReceiverTester::testIsConnected();
}

TEST_F(SenderTester, IsSenderConnected) {
    testIsConnected();
}

}  // namespace FppTest
