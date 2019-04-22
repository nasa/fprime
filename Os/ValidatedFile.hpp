// ======================================================================
// \title  ValidatedFile.hpp
// \author bocchino
// \brief  An fprime validated file
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef OS_ValidatedFile_HPP
#define OS_ValidatedFile_HPP

#include "Fw/Types/EightyCharString.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "Os/ValidateFile.hpp"

namespace Os {

  //! A validated file
  class ValidatedFile {

    public:

      //! Construct a validated file
      ValidatedFile(
          const char* const fileName //!< The file name
      );

    public:

      //! Validate the file
      //! \return Status
      Os::ValidateFile::Status validate(void);

      //! Create the hash file
      //! \return Status
      Os::ValidateFile::Status createHashFile(void);

    public:

      //! Get the file name
      //! \return The file name
      const Fw::EightyCharString& getFileName(void) const;

      //! Get the hash file name
      //! \return The hash file name
      const Fw::EightyCharString& getHashFileName(void) const;

      //! Get the hash file buffer
      //! \return The hash file buffer
      const Utils::HashBuffer& getHashBuffer(void) const;

    PRIVATE:

      //! The file name
      Fw::EightyCharString fileName;

      //! The hash file name
      Fw::EightyCharString hashFileName;

      //! The hash value after creating or loading a validation file
      Utils::HashBuffer hashBuffer;
  };

};

#endif
