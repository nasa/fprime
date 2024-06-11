// ======================================================================
// \title  File.hpp
// \author Rob Bocchino
// \brief  File interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_SequenceFiles_File_HPP
#define Svc_SequenceFiles_File_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! A sequence file
    class File {

      public:

        // ----------------------------------------------------------------------
        // Types
        // ----------------------------------------------------------------------

        //! Binary file formats
        struct Format {

          typedef enum {
            //! F Prime format
            F_PRIME,
            //! AMPCS format
            AMPCS,
          } t;

        };

        //! Information for error reporting
        struct ErrorInfo {

          //! The type of open errors
          struct Open {

            //! The name of the file for error reporting
            Fw::String fileName;

          };

          //! The type of header read errors
          struct HeaderRead {

            //! The wait count
            U32 waitCount;

            //! The name of the file for error reporting
            Fw::String fileName;

          };

          //! The type of data read errors
          struct DataRead {

            //! The wait count
            U32 waitCount;

            //! The name of the file for error reporting
            Fw::String fileName;

          };

          //! Open errors
          Open open;

          //! Header read errors
          HeaderRead headerRead;

          //! Data read errors
          DataRead dataRead;

        };

      public:

        // ----------------------------------------------------------------------
        // Constructors and destructors
        // ----------------------------------------------------------------------

        //! Construct a File with default initialization
        File(
            const Format::t format = Format::F_PRIME //!< The file format
        );

        //! Construct a File with the given base name
        File(
            const char* const baseName, //!< The base name
            const Format::t format = Format::F_PRIME //!< The file format
        );

        //! Destroy a file
        virtual ~File();

      public:

        // ----------------------------------------------------------------------
        // Public instance methods
        // ----------------------------------------------------------------------

        // Set the name from the given base name
        void setName(
            const char* const baseName //!< The base name
        );

        //! Write the file to the disk
        void write();

        //! Remove the file from the disk
        void remove();

        //! Get the file name
        const Fw::StringBase& getName() const;

        //! Get error info for the file
        void getErrorInfo(
            ErrorInfo& errorInfo //!< The error info
        );

      public:

        // ----------------------------------------------------------------------
        // Virtual interface
        // ----------------------------------------------------------------------

        //! Serialize the file in F Prime format
        virtual void serializeFPrime(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

        //! Serialize the file in AMPCS format
        virtual void serializeAMPCS(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

      private:

        // ----------------------------------------------------------------------
        // Private member variables
        // ----------------------------------------------------------------------

        //! The file name
        Fw::String name;

      public:

        // ----------------------------------------------------------------------
        // Public member variables
        // ----------------------------------------------------------------------

        //! The file format
        const Format::t format;

    };

  }

}

#endif
