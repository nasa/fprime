// ======================================================================
// \title  ValidatedFile.cpp
// \author bocchino
// \brief  Os::ValidatedFile implementation
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
//
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ======================================================================

#include "Os/ValidatedFile.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Os {

  ValidatedFile ::
    ValidatedFile(const char *const fileName) :
      fileName(fileName),
      hashFileName(""),
      hashBuffer()      
  {
    Utils::Hash::addFileExtension(this->fileName, this->hashFileName);
  }

  Os::ValidateFile::Status ValidatedFile ::
    validate(void)
  {
    const Os::ValidateFile::Status status =
      Os::ValidateFile::validate(
          this->fileName.toChar(),
          this->hashFileName.toChar(),
          this->hashBuffer
      );
    return status;
  }

  Os::ValidateFile::Status ValidatedFile ::
    createHashFile(void)
  {
    const Os::ValidateFile::Status status =
      Os::ValidateFile::createValidation(
         this->fileName.toChar(),
         this->hashFileName.toChar(),
         this->hashBuffer
      );
    return status;
  }

  const Fw::EightyCharString& ValidatedFile ::
    getFileName(void) const
  {
    return this->fileName;
  }

  const Fw::EightyCharString& ValidatedFile ::
    getHashFileName(void) const
  {
    return this->hashFileName;
  }

  const Utils::HashBuffer& ValidatedFile ::
    getHashBuffer(void) const
  {
    return this->hashBuffer;
  }

}
