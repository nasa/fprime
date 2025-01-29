// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef SVCLOGFILE_HPP_
#define SVCLOGFILE_HPP_

#include <Fw/Types/FileNameString.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>


namespace Svc {

    //! \class LogFile
    //! \brief LogFile struct
    //!
    //! The object is used for writing to a log file.  Making it a struct so all
    //! members are public, for ease of use in object composition.

    struct LogFile {

        //!  \brief Constructor
        //!
        LogFile();

        //!  \brief Destructor
        //!
        ~LogFile();

        // ----------------------------------------------------------------------
        // Member Functions
        // ----------------------------------------------------------------------

        //!  \brief Set log file and max size
        //!
        //!  \param fileName The name of the file to create.  Must be less than 80 characters.
        //!  \param maxSize The max size of the file
        //!  \param maxBackups The max backups for the file. Default: 10
        //!
        //!  \return true if creating the file was successful, false otherwise
        bool set_log_file(const char* fileName, const U32 maxSize, const U32 maxBackups = 10);

        //!  \brief Write the passed buf to the log if possible
        //!
        //!  \param buf The buffer of data to write
        //!  \param size The size of buf
        //!
        //!  \return true if writing to the file was successful, false otherwise
        bool write_to_log(const char *const buf, const U32 size);

        // ----------------------------------------------------------------------
        // Member Variables
        // ----------------------------------------------------------------------

        // The name of the file to text logs to:
        Fw::FileNameString m_fileName;

        // The file to write text logs to:
        Os::File m_file;

        // The max size of the text log file:
        U32 m_maxFileSize;

        // True if there is currently an open file to write text logs to:
        bool m_openFile;

        // Current size of the file:
        U32 m_currentFileSize;
    };

}
#endif /* SVCLOGFILEL_HPP_ */
