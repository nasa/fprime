// ----------------------------------------------------------------------
//
// ComLogger.cpp
//
// ----------------------------------------------------------------------

#include <Svc/ComLogger/ComLogger.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/ValidateFile.hpp>
#include <cstdio>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ComLogger ::
    ComLogger(const char* compName, const char* incomingFilePrefix, U32 maxFileSize, bool storeBufferLength) :
      ComLoggerComponentBase(compName),
      maxFileSize(maxFileSize),
      fileMode(CLOSED),
      byteCount(0),
      writeErrorOccurred(false),
      openErrorOccurred(false),
      storeBufferLength(storeBufferLength)
  {
    if( this->storeBufferLength ) {
      FW_ASSERT(maxFileSize > sizeof(U16), maxFileSize); // must be a positive integer greater than buffer length size
    }
    else {
      FW_ASSERT(maxFileSize > 0, maxFileSize); // must be a positive integer
    }
    FW_ASSERT(Fw::StringUtils::string_length(incomingFilePrefix, sizeof(this->filePrefix)) < sizeof(this->filePrefix),
      Fw::StringUtils::string_length(incomingFilePrefix, sizeof(this->filePrefix)), sizeof(this->filePrefix)); // ensure that file prefix is not too big

    // Set the file prefix:
    Fw::StringUtils::string_copy(this->filePrefix, incomingFilePrefix, sizeof(this->filePrefix));
  }

  void ComLogger ::
    init(
      NATIVE_INT_TYPE queueDepth, //!< The queue depth
      NATIVE_INT_TYPE instance //!< The instance number
    )
  {
    ComLoggerComponentBase::init(queueDepth, instance);
  }

  ComLogger ::
    ~ComLogger()
  {
    // Close file:
    // this->closeFile();
    // NOTE: the above did not work because we don't want to issue an event
    // in the destructor. This can cause "virtual method called" segmentation
    // faults.
    // So I am copying part of that function here.
    if( OPEN == this->fileMode ) {
      // Close file:
      this->file.close();

      // Write out the hash file to disk:
      this->writeHashFile();

      // Update mode:
      this->fileMode = CLOSED;

      // Send event:
      //Fw::LogStringArg logStringArg((char*) fileName);
      //this->log_DIAGNOSTIC_FileClosed(logStringArg);
    }
  }

  // ----------------------------------------------------------------------
  // Handler implementations
  // ----------------------------------------------------------------------

  void ComLogger ::
    comIn_handler(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FW_ASSERT(portNum == 0);

    // Get length of buffer:
    U32 size32 = data.getBuffLength();
    // ComLogger only writes 16-bit sizes to save space
    // on disk:
    FW_ASSERT(size32 < 65536, size32);
    U16 size = size32 & 0xFFFF;

    // Close the file if it will be too big:
    if( OPEN == this->fileMode ) {
      U32 projectedByteCount = this->byteCount + size;
      if( this->storeBufferLength ) {
        projectedByteCount += sizeof(size);
      }
      if( projectedByteCount > this->maxFileSize ) {
        this->closeFile();
      }
    }

    // Open the file if it there is not one open:
    if( CLOSED == this->fileMode ){
      this->openFile();
    }

    // Write to the file if it is open:
    if( OPEN == this->fileMode ) {
      this->writeComBufferToFile(data, size);
    }
  }

  void ComLogger ::
    CloseFile_cmdHandler(
      FwOpcodeType opCode,
      U32 cmdSeq
    )
  {
    this->closeFile();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void ComLogger ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
      // return key
      this->pingOut_out(0,key);
  }

  void ComLogger ::
    openFile(
    )
  {
    FW_ASSERT( CLOSED == this->fileMode );

    U32 bytesCopied;

    // Create filename:
    Fw::Time timestamp = getTime();
    memset(this->fileName, 0, sizeof(this->fileName));
    bytesCopied = snprintf(this->fileName, sizeof(this->fileName), "%s_%" PRI_FwTimeBaseStoreType "_%" PRIu32 "_%06" PRIu32 ".com",
      this->filePrefix, static_cast<FwTimeBaseStoreType>(timestamp.getTimeBase()), timestamp.getSeconds(), timestamp.getUSeconds());

    // "A return value of size or more means that the output was truncated"
    // See here: http://linux.die.net/man/3/snprintf
    FW_ASSERT( bytesCopied < sizeof(this->fileName) );

    // Create sha filename:
    bytesCopied = snprintf(this->hashFileName, sizeof(this->hashFileName), "%s_%" PRI_FwTimeBaseStoreType "_%" PRIu32 "_%06" PRIu32 ".com%s",
      this->filePrefix, static_cast<FwTimeBaseStoreType>(timestamp.getTimeBase()), timestamp.getSeconds(), timestamp.getUSeconds(), Utils::Hash::getFileExtensionString());
    FW_ASSERT( bytesCopied < sizeof(this->hashFileName) );

    Os::File::Status ret = file.open(this->fileName, Os::File::OPEN_WRITE);
    if( Os::File::OP_OK != ret ) {
      if( !this->openErrorOccurred ) { // throttle this event, otherwise a positive
                                       // feedback event loop can occur!
        Fw::LogStringArg logStringArg(this->fileName);
        this->log_WARNING_HI_FileOpenError(ret, logStringArg);
      }
      this->openErrorOccurred = true;
    } else {
      // Reset event throttle:
      this->openErrorOccurred = false;

      // Reset byte count:
      this->byteCount = 0;

      // Set mode:
      this->fileMode = OPEN;
    }
  }

  void ComLogger ::
    closeFile(
    )
  {
    if( OPEN == this->fileMode ) {
      // Close file:
      this->file.close();

      // Write out the hash file to disk:
      this->writeHashFile();

      // Update mode:
      this->fileMode = CLOSED;

      // Send event:
      Fw::LogStringArg logStringArg(this->fileName);
      this->log_DIAGNOSTIC_FileClosed(logStringArg);
    }
  }

  void ComLogger ::
    writeComBufferToFile(
      Fw::ComBuffer &data,
      U16 size
    )
  {
    if( this->storeBufferLength ) {
      U8 buffer[sizeof(size)];
      Fw::SerialBuffer serialLength(&buffer[0], sizeof(size));
      serialLength.serialize(size);
      if(this->writeToFile(serialLength.getBuffAddr(),
              static_cast<U16>(serialLength.getBuffLength()))) {
        this->byteCount += serialLength.getBuffLength();
      }
      else {
        return;
      }
    }

    // Write buffer to file:
    if(this->writeToFile(data.getBuffAddr(), size)) {
      this->byteCount += size;
    }
  }

  bool ComLogger ::
    writeToFile(
      void* data,
      U16 length
    )
  {
    NATIVE_INT_TYPE size = length;
    Os::File::Status ret = file.write(data, size);
    if( Os::File::OP_OK != ret || size != static_cast<NATIVE_INT_TYPE>(length) ) {
      if( !this->writeErrorOccurred ) { // throttle this event, otherwise a positive
                                        // feedback event loop can occur!
        Fw::LogStringArg logStringArg(this->fileName);
        this->log_WARNING_HI_FileWriteError(ret, size, length, logStringArg);
      }
      this->writeErrorOccurred = true;
      return false;
    }

    this->writeErrorOccurred = false;
    return true;
  }

  void ComLogger ::
    writeHashFile(
    )
  {
    Os::ValidateFile::Status validateStatus;
    validateStatus = Os::ValidateFile::createValidation(this->fileName, this->hashFileName);
    if( Os::ValidateFile::VALIDATION_OK != validateStatus ) {
      Fw::LogStringArg logStringArg1(this->fileName);
      Fw::LogStringArg logStringArg2(this->hashFileName);
      this->log_WARNING_LO_FileValidationError(logStringArg1, logStringArg2, validateStatus);
    }
  }
};
