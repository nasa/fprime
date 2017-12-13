// ====================================================================== 
// \title  File.cpp
// \author bocchino, dinkel
// \brief  Implementation for ASTERIA::BufferLogger::File
//
// \copyright
// Copyright (C) 2015-2017 California Institute of Technology.
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

#include "ASTERIA/Components/BufferLogger/BufferLogger.hpp"

namespace ASTERIA {

  // ----------------------------------------------------------------------
  // OpenErrors
  // ----------------------------------------------------------------------

  BufferLogger::File::OpenErrors ::
    OpenErrors(BufferLogger& bufferLogger) :
      bufferLogger(bufferLogger),
      errorOccurred(false)
  {
  
  }

  void BufferLogger::File::OpenErrors ::
    emit(
        const U32 status,
        Fw::LogStringArg& fileName
    )
  {
    if (!this->errorOccurred) {
      this->bufferLogger.log_WARNING_HI_LogFileOpenError(
          status,
          fileName
      );
    }
    this->errorOccurred = true;
  }

  void BufferLogger::File::OpenErrors ::
    clear(void)
  {
    this->errorOccurred = false;
  }

  // ----------------------------------------------------------------------
  // WriteErrors
  // ----------------------------------------------------------------------

  BufferLogger::File::WriteErrors ::
    WriteErrors(BufferLogger& bufferLogger) :
      bufferLogger(bufferLogger),
      errorOccurred(false)
  {
  
  }

  void BufferLogger::File::WriteErrors ::
    emit(
        const U32 status,
        const U32 bytesWritten,
        const U32 bytesAttempted,
        Fw::LogStringArg& fileName
    )
  {
    if (!this->errorOccurred) {
      this->bufferLogger.log_WARNING_HI_LogFileWriteError(
          status, 
          bytesWritten,
          bytesAttempted,
          fileName
      );
    }
    this->errorOccurred = true;
  }

  void BufferLogger::File::WriteErrors ::
    clear(void)
  {
    this->errorOccurred = false;
  }

  // ----------------------------------------------------------------------
  // Constructors and destructors
  // ----------------------------------------------------------------------

  BufferLogger::File ::
    File(
        BufferLogger& bufferLogger,
        const char *const prefix,
        const char *const suffix,
        const U32 maxSize,
        const U8 sizeOfSize
    ) :
      bufferLogger(bufferLogger),
      prefix(prefix),
      suffix(suffix),
      maxSize(maxSize),
      sizeOfSize(sizeOfSize),
      mode(Mode::CLOSED),
      bytesWritten(0),
      openErrors(bufferLogger),
      writeErrors(bufferLogger)
  {
    FW_ASSERT(sizeOfSize <= sizeof(U32), sizeOfSize);
    FW_ASSERT(maxSize > sizeOfSize, maxSize);
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
    formatName(
        String& name,
        const Fw::Time& time
    )
  {
    name.format(
        "%s_%d_%d_%06d%s",
        this->prefix.toChar(),
        time.getTimeBase(),
        time.getSeconds(),
        time.getUSeconds(),
        this->suffix.toChar()
    );
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
      this->bufferLogger.log_DIAGNOSTIC_LogFileClosed(logStringArg);
    }
  }

  // ----------------------------------------------------------------------
  // Private functions 
  // ----------------------------------------------------------------------

  void BufferLogger::File ::
    open(void)
  {
    FW_ASSERT(this->mode == File::Mode::CLOSED);

    Fw::Time timestamp = this->bufferLogger.getTime();
    this->name.format(
        "%s_%d_%d_%06d%s",
        this->prefix.toChar(),
        timestamp.getTimeBase(),
        timestamp.getSeconds(),
        timestamp.getUSeconds(),
        this->suffix.toChar()
    );

    const Os::File::Status status = this->asteriaFile.open(
        this->name.toChar(),
        Os::File::OPEN_WRITE
    );
    if (status == Os::File::OP_OK) {
      // Clear open errors state
      this->openErrors.clear();
      // Reset bytes written
      this->bytesWritten = 0;
      // Set mode
      this->mode = File::Mode::OPEN; 
    }
    else {
      Fw::LogStringArg string(this->name.toChar());
      this->openErrors.emit(status, string);
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
        const U32 length
    )
  {
    U32 size = length;
    const Os::File::Status fileStatus = this->asteriaFile.write(data, size);
    bool status;
    if (fileStatus == Os::File::OP_OK && size == length) {
      this->writeErrors.clear();
      this->bytesWritten += length;
      status = true;
    }
    else {
      Fw::LogStringArg string(this->name.toChar());
      this->writeErrors.emit(fileStatus, size, length, string);
      status = false;
    }
    return status;
  }

  void BufferLogger::File :: 
    writeHashFile(void)
  {
    ValidatedFile validatedFile(this->name.toChar());
    const Os::ValidateFile::Status status =
      validatedFile.createHashFile();
    if (status !=  Os::ValidateFile::VALIDATION_OK) {
      const String &hashFileName = validatedFile.getHashFileName();
      Fw::LogStringArg logStringArg(hashFileName.toChar());
      this->bufferLogger.log_WARNING_HI_LogFileValidationError(
          logStringArg,
          status
      );
    }
  }

  bool BufferLogger::File ::
  flush(void)
  {
    bool status = true;
    if(this->mode == File::Mode::OPEN)
    {
      const Os::File::Status fileStatus = this->asteriaFile.flush();
      if(fileStatus == Os::File::OP_OK)
      {
        status = true;
      }
      else
      {
        status = false;
      }
    }
    return status;
  }

  void BufferLogger::File ::
    close(void)
  {
    if (this->mode == File::Mode::OPEN) {
      // Close file
      this->asteriaFile.close();
      // Write out the hash file to disk
      this->writeHashFile();
      // Update mode
      this->mode = File::Mode::CLOSED;
    }
  }

}
