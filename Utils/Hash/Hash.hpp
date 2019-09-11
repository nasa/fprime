// ======================================================================
// \title  Hash.hpp
// \author dinkel
// \brief  hpp file for Hash class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UTILS_HASH_HPP
#define UTILS_HASH_HPP

#include "Fw/Types/EightyCharString.hpp"
#include <Utils/Hash/HashBuffer.hpp>

namespace Utils {

  //! \class Hash
  //! \brief A generic interface for creating and comparing hash values
  //!
  class Hash {

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct a Hash object
      //!
      Hash();

      //! Destroy a Hash object
      //!
      ~Hash();

    public:

      // ----------------------------------------------------------------------
      // Public static methods
      // ----------------------------------------------------------------------

      //! Create a hash value all at once from raw data
      //!
      static void hash(
          const void *data, //! Pointer to start of data
          const NATIVE_INT_TYPE len, //! Length of the data
          HashBuffer& buffer //! Resulting hash value
      );

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Initialize a Hash object for incremental hash computation
      //!
      void init(void);

      //! Update an incremental computation with new data
      //!
      void update(
          const void *const data, //! Pointer to start of data
          const NATIVE_INT_TYPE len //! Length of the data
      );

      //! Finalize an incremental computation and return the result
      //!
      void final(
          HashBuffer& buffer //! The result
      );

      //! Get the file extension for the supported hash type
      //! E.g., could return "SHA256"
      //!
      static const char* getFileExtensionString(void);

      //! Add the extension for the supported hash type
      //!
      static void addFileExtension(
          const Fw::EightyCharString& baseName, //!< The base name
          Fw::EightyCharString& extendedName //!< The extended name
      );

      //! Get the length of the file extension string
      //!
      static NATIVE_UINT_TYPE getFileExtensionLength(void);

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The hash handle
      //!
      HASH_HANDLE_TYPE hash_handle;

  };

}

#endif
