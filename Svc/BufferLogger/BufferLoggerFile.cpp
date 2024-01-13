// ======================================================================
// \title  BufferLoggerFile.cpp
// \author bocchino, dinkel, mereweth
// \brief  Implementation for Svc::BufferLogger::BufferLoggerFile
//
// \copyright
// Copyright (C) 2015-2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/BufferLogger/BufferLogger.hpp"
#include "Os/ValidateFile.hpp"
#include "Os/ValidatedFile.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Constructors and destructors
  // ----------------------------------------------------------------------

  BufferLogger::File ::
    File(
        BufferLogger& bufferLogger
    ) :
      m_bufferLogger(bufferLogger),
      m_prefix(""),
      m_suffix(""),
      m_baseName(""),
      m_fileCounter(0),
      m_maxSize(0),
      m_sizeOfSize(0),
      m_mode(Mode::CLOSED),
      m_bytesWritten(0)
  {
  }

  BufferLogger::File ::
    ~File()
  {
    this->close();
  }

  // ----------------------------------------------------------------------
  // Public functions
  // ----------------------------------------------------------------------

  void BufferLogger::File ::
    init(
        const char *const logFilePrefix,
        const char *const logFileSuffix,
        const U32 maxFileSize,
        const U8 sizeOfSize
    )
  {
      //NOTE(mereweth) - only call this before opening the file
      FW_ASSERT(this->m_mode == File::Mode::CLOSED);

      this->m_prefix = logFilePrefix;
      this->m_suffix = logFileSuffix;
      this->m_maxSize = maxFileSize;
      this->m_sizeOfSize = sizeOfSize;

      FW_ASSERT(sizeOfSize <= sizeof(U32), sizeOfSize);
      FW_ASSERT(m_maxSize > sizeOfSize, m_maxSize);
  }

  void BufferLogger::File ::
    setBaseName(
        const Fw::StringBase& baseName
    )
  {
      if (this->m_mode == File::Mode::OPEN) {
          this->closeAndEmitEvent();
      }
      this->m_baseName = baseName;
      this->m_fileCounter = 0;
      this->open();
  }

  void BufferLogger::File ::
    logBuffer(
        const U8 *const data,
        const U32 size
    )
  {
    // Close the file if it will be too big
    if (this->m_mode == File::Mode::OPEN) {
      const U32 projectedByteCount =
        this->m_bytesWritten + this->m_sizeOfSize + size;
      if (projectedByteCount > this->m_maxSize) {
        this->closeAndEmitEvent();
      }
    }
    // Open a file if necessary
    if (this->m_mode == File::Mode::CLOSED) {
      this->open();
    }
    // Write to the file if it is open
    if (this->m_mode == File::Mode::OPEN) {
      (void) this->writeBuffer(data, size);
    }
  }

  void BufferLogger::File ::
    closeAndEmitEvent()
  {
    if (this->m_mode == File::Mode::OPEN) {
      this->close();
      Fw::LogStringArg logStringArg(this->m_name.toChar());
      this->m_bufferLogger.log_DIAGNOSTIC_BL_LogFileClosed(logStringArg);
    }
  }

  // ----------------------------------------------------------------------
  // Private functions
  // ----------------------------------------------------------------------

  void BufferLogger::File ::
    open()
  {
    FW_ASSERT(this->m_mode == File::Mode::CLOSED);

    // NOTE(mereweth) - check that file path has been set and that initLog has been called
    if ((this->m_baseName.toChar()[0] == '\0') ||
        (this->m_sizeOfSize > sizeof(U32)) ||
        (this->m_maxSize <= this->m_sizeOfSize)) {
        this->m_bufferLogger.log_WARNING_HI_BL_NoLogFileOpenInitError();
        return;
    }

    if (this->m_fileCounter == 0) {
        this->m_name.format(
            "%s%s%s",
            this->m_prefix.toChar(),
            this->m_baseName.toChar(),
            this->m_suffix.toChar()
        );
    }
    else {
        this->m_name.format(
            "%s%s%d%s",
            this->m_prefix.toChar(),
            this->m_baseName.toChar(),
            this->m_fileCounter,
            this->m_suffix.toChar()
        );
    }

    const Os::File::Status status = this->m_osFile.open(
        this->m_name.toChar(),
        Os::File::OPEN_WRITE
    );
    if (status == Os::File::OP_OK) {
      this->m_fileCounter++;
      // Reset bytes written
      this->m_bytesWritten = 0;
      // Set mode
      this->m_mode = File::Mode::OPEN;
    }
    else {
      Fw::LogStringArg string(this->m_name.toChar());
      this->m_bufferLogger.log_WARNING_HI_BL_LogFileOpenError(status, string);
    }
  }

  bool BufferLogger::File ::
    writeBuffer(
        const U8 *const data,
        const U32 size
    )
  {
    bool status = this->writeSize(size);
    if (status) {
      status = this->writeBytes(data, size);
    }
    return status;
  }

  bool BufferLogger::File ::
    writeSize(const U32 size)
  {
    FW_ASSERT(this->m_sizeOfSize <= sizeof(U32));
    U8 sizeBuffer[sizeof(U32)];
    U32 sizeRegister = size;
    for (U8 i = 0; i < this->m_sizeOfSize; ++i) {
      sizeBuffer[this->m_sizeOfSize - i - 1] = sizeRegister & 0xFF;
      sizeRegister >>= 8;
    }
    const bool status = this->writeBytes(
        sizeBuffer,
        this->m_sizeOfSize
    );
    return status;
  }

  bool BufferLogger::File ::
    writeBytes(
        const void *const data,
        const U32 length
    )
  {
    FW_ASSERT(length > 0, length);
    NATIVE_INT_TYPE size = length;
    const Os::File::Status fileStatus = this->m_osFile.write(data, size);
    bool status;
    if (fileStatus == Os::File::OP_OK && size == static_cast<NATIVE_INT_TYPE>(length)) {
      this->m_bytesWritten += length;
      status = true;
    }
    else {
      Fw::LogStringArg string(this->m_name.toChar());

      this->m_bufferLogger.log_WARNING_HI_BL_LogFileWriteError(fileStatus, size, length, string);
      status = false;
    }
    return status;
  }

  void BufferLogger::File ::
    writeHashFile()
  {
    Os::ValidatedFile validatedFile(this->m_name.toChar());
    const Os::ValidateFile::Status status =
      validatedFile.createHashFile();
    if (status !=  Os::ValidateFile::VALIDATION_OK) {
      const Fw::String &hashFileName = validatedFile.getHashFileName();
      Fw::LogStringArg logStringArg(hashFileName.toChar());
      this->m_bufferLogger.log_WARNING_HI_BL_LogFileValidationError(
          logStringArg,
          status
      );
    }
  }

  bool BufferLogger::File ::
  flush()
  {
    return true;
    // NOTE(if your fprime uses buffered file I/O, re-enable this)
    /*bool status = true;
    if(this->mode == File::Mode::OPEN)
    {
      const Os::File::Status fileStatus = this->osFile.flush();
      if(fileStatus == Os::File::OP_OK)
      {
        status = true;
      }
      else
      {
        status = false;
      }
    }
    return status;*/
  }

  void BufferLogger::File ::
    close()
  {
    if (this->m_mode == File::Mode::OPEN) {
      // Close file
      this->m_osFile.close();
      // Write out the hash file to disk
      this->writeHashFile();
      // Update mode
      this->m_mode = File::Mode::CLOSED;
    }
  }

}
