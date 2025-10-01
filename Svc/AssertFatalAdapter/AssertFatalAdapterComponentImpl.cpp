// ======================================================================
// \title  AssertFatalAdapterImpl.cpp
// \author tcanham
// \brief  cpp file for AssertFatalAdapter component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/AssertFatalAdapter/AssertFatalAdapterComponentImpl.hpp>
#include <cassert>
#include <cstdio>

namespace Fw {
void defaultReportAssert(FILE_NAME_ARG file,
                         FwSizeType lineNo,
                         FwSizeType numArgs,
                         FwAssertArgType arg1,
                         FwAssertArgType arg2,
                         FwAssertArgType arg3,
                         FwAssertArgType arg4,
                         FwAssertArgType arg5,
                         FwAssertArgType arg6,
                         CHAR* destBuffer,
                         FwSizeType buffSize);

}

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

AssertFatalAdapterComponentImpl ::AssertFatalAdapterComponentImpl(const char* const compName)
    : AssertFatalAdapterComponentBase(compName) {
    // register component with adapter
    this->m_adapter.regAssertReporter(this);
    // register adapter
    this->m_adapter.registerHook();
    // Initialize the assert counter
    this->m_assertCount = 0;
}

AssertFatalAdapterComponentImpl ::~AssertFatalAdapterComponentImpl() {}

void AssertFatalAdapterComponentImpl::AssertFatalAdapter::reportAssert(FILE_NAME_ARG file,
                                                                       FwSizeType lineNo,
                                                                       FwSizeType numArgs,
                                                                       FwAssertArgType arg1,
                                                                       FwAssertArgType arg2,
                                                                       FwAssertArgType arg3,
                                                                       FwAssertArgType arg4,
                                                                       FwAssertArgType arg5,
                                                                       FwAssertArgType arg6) {
    if (m_compPtr) {
        m_compPtr->reportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);
    } else {
        // Can't assert, what else can we do? Maybe somebody will see it.
        Fw::Logger::log("Svc::AssertFatalAdapter not registered!\n");
        assert(0);
    }
}

void AssertFatalAdapterComponentImpl::AssertFatalAdapter::regAssertReporter(AssertFatalAdapterComponentImpl* compPtr) {
    this->m_compPtr = compPtr;
}

AssertFatalAdapterComponentImpl::AssertFatalAdapter::AssertFatalAdapter() : m_compPtr(nullptr) {}

AssertFatalAdapterComponentImpl::AssertFatalAdapter::~AssertFatalAdapter() {}

void AssertFatalAdapterComponentImpl::AssertFatalAdapter::doAssert() {
    // do nothing since there will be a FATAL
}

void AssertFatalAdapterComponentImpl::reportAssert(FILE_NAME_ARG file,
                                                   FwSizeType lineNo,
                                                   FwSizeType numArgs,
                                                   FwAssertArgType arg1,
                                                   FwAssertArgType arg2,
                                                   FwAssertArgType arg3,
                                                   FwAssertArgType arg4,
                                                   FwAssertArgType arg5,
                                                   FwAssertArgType arg6) {
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    Fw::LogStringArg fileArg;
    fileArg.format("0x%08" PRIX32, file);
#else
    Fw::LogStringArg fileArg(file);
#endif

    CHAR msg[Fw::StringBase::BUFFER_SIZE(FW_ASSERT_TEXT_SIZE)] = {0};
    Fw::defaultReportAssert(file, static_cast<U32>(lineNo), numArgs, arg1, arg2, arg3, arg4, arg5, arg6, msg,
                            sizeof(msg));
    Fw::Logger::log("%s\n", msg);

    // Increment active assert counter
    this->m_assertCount++;

    // Handle the case where the ports aren't connected yet or we've surpassed the maximum cascading FW_ASSERT failures
    if (not this->isConnected_Log_OutputPort(0) || this->m_assertCount > FW_ASSERT_COUNT_MAX) {
        assert(0);
        return;
    }

    switch (numArgs) {
        case 0:
            this->log_FATAL_AF_ASSERT_0(fileArg, static_cast<U32>(lineNo));
            break;
        case 1:
            this->log_FATAL_AF_ASSERT_1(fileArg, static_cast<U32>(lineNo), static_cast<U32>(arg1));
            break;
        case 2:
            this->log_FATAL_AF_ASSERT_2(fileArg, static_cast<U32>(lineNo), static_cast<U32>(arg1),
                                        static_cast<U32>(arg2));
            break;
        case 3:
            this->log_FATAL_AF_ASSERT_3(fileArg, static_cast<U32>(lineNo), static_cast<U32>(arg1),
                                        static_cast<U32>(arg2), static_cast<U32>(arg3));
            break;
        case 4:
            this->log_FATAL_AF_ASSERT_4(fileArg, static_cast<U32>(lineNo), static_cast<U32>(arg1),
                                        static_cast<U32>(arg2), static_cast<U32>(arg3), static_cast<U32>(arg4));
            break;
        case 5:
            this->log_FATAL_AF_ASSERT_5(fileArg, static_cast<U32>(lineNo), static_cast<U32>(arg1),
                                        static_cast<U32>(arg2), static_cast<U32>(arg3), static_cast<U32>(arg4),
                                        static_cast<U32>(arg5));
            break;
        case 6:
            this->log_FATAL_AF_ASSERT_6(fileArg, static_cast<U32>(lineNo), static_cast<U32>(arg1),
                                        static_cast<U32>(arg2), static_cast<U32>(arg3), static_cast<U32>(arg4),
                                        static_cast<U32>(arg5), static_cast<U32>(arg6));
            break;
        default:
            this->log_FATAL_AF_UNEXPECTED_ASSERT(fileArg, static_cast<U32>(lineNo), static_cast<U32>(numArgs));
            break;
    }
    // Assert processing complete, decrement active assert counter
    this->m_assertCount--;
}
}  // end namespace Svc
