// ====================================================================== 
// \title  MissingFile.cpp
// \author Rob Bocchino
// \brief  MissingFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/MissingFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    MissingFile ::
      MissingFile(const Format::t format) :
        File("missing", format)
    {

    }

    void MissingFile ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      // CRC
      AMPCS::CRCs::createFile(buffer, this->getName().toChar());
    }

  }


}

