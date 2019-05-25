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
      bufferLogger(bufferLogger),
      prefix(""),
      suffix(""),
      baseName(""),
      fileCounter(0),
      maxSize(0),
      sizeOfSize(0),
      mode(Mode::CLOSED),
      bytesWritten(0)
  {
  }

  BufferLogger::File ::
    ~File(void)
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
      FW_ASSERT(this->mode == File::Mode::CLOSED);

      this->prefix = logFilePrefix;
      this->suffix = logFileSuffix;
      this->maxSize = maxFileSize;
      this->sizeOfSize = sizeOfSize;

      FW_ASSERT(sizeOfSize <= sizeof(U32), sizeOfSize);
      FW_ASSERT(maxSize > sizeOfSize, maxSize);
  }

  void BufferLogger::File ::
    setBaseName(
        const Fw::EightyCharString& baseName
    )
  {
      if (this->mode == File::Mode::OPEN) {
          this->closeAndEmitEvent();
      }
      this->baseName = baseName;
      this->fileCounter = 0;
      this->open();
  }

  void BufferLogger::File ::
    logBuffer(
        const U8 *const data,
        const U32 size
    )
  {
    // Close the file if it will be too big
    if (this->mode == File::Mode::OPEN) {
      const U32 projectedByteCount =
        this->bytesWritten + this->sizeOfSize + size;
      if (projectedByteCount > this->maxSize) {
        this->closeAndEmitEvent();
      }
    }
    // Open a file if necessary
    if (this->mode == File::Mode::CLOSED) {
      this->open();
    }
    // Write to the file if it is open
    if (this->mode == File::Mode::OPEN) {
      (void) this->writeBuffer(data, size);
    }
  }

  void BufferLogger::File ::
    closeAndEmitEvent(void)
  {
    if (this->mode == File::Mode::OPEN) {
      this->close();
      Fw::LogStringArg logStringArg(this->name.toChar());
      this->bufferLogger.log_DIAGNOSTIC_BL_LogFileClosed(logStringArg);
    }
  }

  // ----------------------------------------------------------------------
  // Private functions
  // ----------------------------------------------------------------------

  void BufferLogger::File ::
    open(void)
  {
    FW_ASSERT(this->mode == File::Mode::CLOSED);

    // NOTE(mereweth) - check that file path has been set and that initLog has been called
    if ((this->baseName.toChar()[0] == '\0') ||
        (this->sizeOfSize > sizeof(U32)) ||
        (this->maxSize <= this->sizeOfSize)) {
        this->bufferLogger.log_WARNING_HI_BL_NoLogFileOpenInitError();
        return;
    }

    if (this->fileCounter == 0) {
        this->name.format(
            "%s%s%s",
            this->prefix.toChar(),
            this->baseName.toChar(),
            this->suffix.toChar()
        );
    }
    else {
        this->name.format(
            "%s%s%d%s",
            this->prefix.toChar(),
            this->baseName.toChar(),
            this->fileCounter,
            this->suffix.toChar()
        );
    }

    const Os::File::Status status = this->osFile.open(
        this->name.toChar(),
        Os::File::OPEN_WRITE
    );
    if (status == Os::File::OP_OK) {
      this->fileCounter++;
      // Reset bytes written
      this->bytesWritten = 0;
      // Set mode
      this->mode = File::Mode::OPEN;
    }
    else {
      Fw::LogStringArg string(this->name.toChar());
      this->bufferLogger.log_WARNING_HI_BL_LogFileOpenError(status, string);
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
    U32 sizeRegister = size;
    U8 sizeBuffer[this->sizeOfSize];
    for (U8 i = 0; i < this->sizeOfSize; ++i) {
      sizeBuffer[this->sizeOfSize - i - 1] = sizeRegister & 0xFF;
      sizeRegister >>= 8;
    }
    const bool status = this->writeBytes(
        sizeBuffer,
        sizeof(sizeBuffer)
    );
    return status;
  }

  bool BufferLogger::File ::
    writeBytes(
        const void *const data,
        const NATIVE_UINT_TYPE length
    )
  {
    FW_ASSERT(length > 0, length);
    NATIVE_INT_TYPE size = length;
    const Os::File::Status fileStatus = this->osFile.write(data, size);
    bool status;
    if (fileStatus == Os::File::OP_OK && size == static_cast<NATIVE_INT_TYPE>(length)) {
      this->bytesWritten += length;
      status = true;
    }
    else {
      Fw::LogStringArg string(this->name.toChar());

      this->bufferLogger.log_WARNING_HI_BL_LogFileWriteError(fileStatus, size, length, string);
      status = false;
    }
    return status;
  }

  void BufferLogger::File ::
    writeHashFile(void)
  {
    Os::ValidatedFile validatedFile(this->name.toChar());
    const Os::ValidateFile::Status status =
      validatedFile.createHashFile();
    if (status !=  Os::ValidateFile::VALIDATION_OK) {
      const Fw::EightyCharString &hashFileName = validatedFile.getHashFileName();
      Fw::LogStringArg logStringArg(hashFileName.toChar());
      this->bufferLogger.log_WARNING_HI_BL_LogFileValidationError(
          logStringArg,
          status
      );
    }
  }

  bool BufferLogger::File ::
  flush(void)
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
    close(void)
  {
    if (this->mode == File::Mode::OPEN) {
      // Close file
      this->osFile.close();
      // Write out the hash file to disk
      this->writeHashFile();
      // Update mode
      this->mode = File::Mode::CLOSED;
    }
  }

}
