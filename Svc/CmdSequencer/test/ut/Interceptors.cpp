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


    CmdSequencerTester::Interceptor::Interceptor() :
            enabled(EnableType::NONE),
            errorType(ErrorType::NONE),
            waitCount(0),
            size(0),
            fileStatus(Os::File::OP_OK) {

    }

    void CmdSequencerTester::Interceptor::enable(EnableType::t enableType) {
        this->enabled = enableType;
    }

    void CmdSequencerTester::Interceptor::disable() {
        this->enabled = EnableType::t::NONE;
    }

    Os::FileInterface::Status CmdSequencerTester::Interceptor::open(const char *path, Mode mode, OverwriteType overwrite) {
        if (this->enabled == EnableType::OPEN) {
            return this->fileStatus;
        }
        return this->Os::Test::SyntheticFile::open(path, mode, overwrite);
    }

    Os::File::Status CmdSequencerTester::Interceptor::read(
            U8 *buffer,
            FwSignedSizeType &requestSize,
            Os::File::WaitType waitType
    ) {
        (void) waitType;
        Os::File::Status status = this->Os::Test::SyntheticFile::read(buffer, requestSize, waitType);
        if ((this->enabled == EnableType::READ) && (this->errorType != ErrorType::NONE)) {
            if (this->waitCount > 0) {
                // Not time to inject an error yet: decrement wait count
                --this->waitCount;
            } else {
                // Time to inject an error: check test scenario
                switch (this->errorType) {
                    case ErrorType::READ:
                        status = this->fileStatus;
                        break;
                    case ErrorType::SIZE:
                        requestSize = this->size;
                        status = Os::File::OP_OK;
                        break;
                    case ErrorType::DATA:
                        memcpy(buffer, this->data, size);
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