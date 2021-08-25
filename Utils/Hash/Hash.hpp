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

#include "Fw/Types/StringType.hpp"
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
      //! \param data: pointer to start of data
      //! \param len: length of the data
      //! \param buffer: filled with resulting hash value
      static void hash(
          const void *data,
          const NATIVE_INT_TYPE len,
          HashBuffer& buffer
      );

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Initialize a Hash object for incremental hash computation
      //!
      void init(void);

      //! Set hash value to specified value
      //!
      void setHashValue(
          HashBuffer &value //! Hash value
      );

      //! Update an incremental computation with new data
      //! \param data: pointer to start of data to add to hash calculation
      //! \param len: length of data to add to hash calculation
      void update(
          const void *const data,
          const NATIVE_INT_TYPE len
      );

      //! Finalize an incremental computation and return the result
      //!
      void final(
          HashBuffer& buffer //! The result
      );

      //! Finalize an incremental computation and return the result
      //!
      void final(U32 &hashvalue);

      //! Get the file extension for the supported hash type
      //! E.g., could return "SHA256"
      //!
      static const char* getFileExtensionString(void);

      //! Add the extension for the supported hash type
      //!
      static void addFileExtension(
          const Fw::StringBase& baseName, //!< The base name
          Fw::StringBase& extendedName //!< The extended name
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
