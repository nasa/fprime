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
      m_fileName(fileName),
      m_hashFileName(""),
      m_hashBuffer()
  {
    Utils::Hash::addFileExtension(this->m_fileName, this->m_hashFileName);
  }

  Os::ValidateFile::Status ValidatedFile ::
    validate()
  {
    const Os::ValidateFile::Status status =
      Os::ValidateFile::validate(
          this->m_fileName.toChar(),
          this->m_hashFileName.toChar(),
          this->m_hashBuffer
      );
    return status;
  }

  Os::ValidateFile::Status ValidatedFile ::
    createHashFile()
  {
    const Os::ValidateFile::Status status =
      Os::ValidateFile::createValidation(
         this->m_fileName.toChar(),
         this->m_hashFileName.toChar(),
         this->m_hashBuffer
      );
    return status;
  }

  const Fw::StringBase& ValidatedFile ::
    getFileName() const
  {
    return this->m_fileName;
  }

  const Fw::StringBase& ValidatedFile ::
    getHashFileName() const
  {
    return this->m_hashFileName;
  }

  const Utils::HashBuffer& ValidatedFile ::
    getHashBuffer() const
  {
    return this->m_hashBuffer;
  }

}
