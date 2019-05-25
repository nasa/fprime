// ======================================================================
// \title  ValidatedFile.cpp
// \author bocchino
// \brief  Os::ValidatedFile implementation
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
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
