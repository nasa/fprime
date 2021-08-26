// ====================================================================== 
// \title  EmptyFile.cpp
// \author Rob Bocchino
// \brief  EmptyFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/EmptyFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    EmptyFile ::
      EmptyFile(const Format::t format) :
        File("empty", format)
    {

    }

    void EmptyFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Do nothing
    }

    void EmptyFile ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      // CRC
      AMPCS::CRCs::createFile(buffer, this->getName().toChar());
    }

  }

}
