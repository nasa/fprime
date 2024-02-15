// ======================================================================
// \title  Interceptors.cpp
// \author Canham/Bocchino
// \brief  Implementation for CmdSequencerTester::Interceptors
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Os/Stub/test/File.hpp"
#include "CmdSequencerTester.hpp"
#include "gtest/gtest.h"

namespace Svc {

    CmdSequencerTester::Interceptor* CmdSequencerTester::Interceptor::PosixFileInterceptor::s_current_interceptor = nullptr;
    CmdSequencerTester::Interceptor::Interceptor() :
            enabled(EnableType::NONE),
            errorType(ErrorType::NONE),
            waitCount(0),
            size(0),
            fileStatus(Os::File::OP_OK) {
            CmdSequencerTester::Interceptor::PosixFileInterceptor::s_current_interceptor = this;
    }

    void CmdSequencerTester::Interceptor::enable(EnableType::t enableType) {
        this->enabled = enableType;
    }

    void CmdSequencerTester::Interceptor::disable() {
        this->enabled = EnableType::t::NONE;
    }

    Os::FileInterface::Status CmdSequencerTester::Interceptor::PosixFileInterceptor::open(const char *path, Mode mode, OverwriteType overwrite) {
        if ((s_current_interceptor != nullptr) && (s_current_interceptor ->enabled == EnableType::t::OPEN)) {
            return s_current_interceptor->fileStatus;
        }
        return this->Os::Posix::File::PosixFile::open(path, mode, overwrite);
    }

    Os::File::Status CmdSequencerTester::Interceptor::PosixFileInterceptor::read(
            U8 *buffer,
            FwSignedSizeType &requestSize,
            Os::File::WaitType waitType
    ) {
        (void) waitType;
        Os::File::Status status = this->Os::Posix::File::PosixFile::read(buffer, requestSize, waitType);
        if (s_current_interceptor == nullptr) {
            return status;
        } else if ((s_current_interceptor->enabled == EnableType::READ) && (s_current_interceptor->errorType != ErrorType::NONE)) {
            if (s_current_interceptor->waitCount > 0) {
                // Not time to inject an error yet: decrement wait count
                --s_current_interceptor->waitCount;
            } else {
                // Time to inject an error: check test scenario
                switch (s_current_interceptor->errorType) {
                    case ErrorType::READ:
                        status = s_current_interceptor->fileStatus;
                        break;
                    case ErrorType::SIZE:
                        requestSize = s_current_interceptor->size;
                        status = Os::File::OP_OK;
                        break;
                    case ErrorType::DATA:
                        memcpy(buffer, s_current_interceptor->data, s_current_interceptor->size);
                        status = Os::File::OP_OK;
                        break;
                    default:
                        EXPECT_TRUE(false);
                        break;
                }
            }
        }
        return status;
    }
}
