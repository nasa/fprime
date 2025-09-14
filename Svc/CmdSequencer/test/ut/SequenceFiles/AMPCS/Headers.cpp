// ======================================================================
// \title  Headers.cpp
// \author Rob Bocchino
// \brief  AMPCS sequence file headers
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/Headers.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

namespace AMPCS {

namespace Headers {

void serialize(Fw::SerializeBufferBase& buffer) {
    serialize(0x11223344, buffer);
}

void serialize(const U32 value, Fw::SerializeBufferBase& buffer) {
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serialize(value));
}

}  // namespace Headers

}  // namespace AMPCS

}  // namespace SequenceFiles

}  // namespace Svc
