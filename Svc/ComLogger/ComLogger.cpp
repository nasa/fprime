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
      m_maxFileSize(maxFileSize),
      m_fileMode(CLOSED),
      m_byteCount(0),
      m_writeErrorOccurred(false),
      m_openErrorOccurred(false),
      m_storeBufferLength(storeBufferLength),
      m_initialized(true)
  {
    this->init_log_file(incomingFilePrefix, maxFileSize, storeBufferLength);
  }

  ComLogger ::
    ComLogger(const char* compName) :
      ComLoggerComponentBase(compName),
      m_filePrefix(),
      m_maxFileSize(0),
      m_fileMode(CLOSED),
      m_fileName(),
      m_hashFileName(),
      m_byteCount(0),
      m_writeErrorOccurred(false),
      m_openErrorOccurred(false),
      m_storeBufferLength(),
      m_initialized(false)
  {
  }

  void ComLogger ::
    init_log_file(const char* incomingFilePrefix, U32 maxFileSize, bool storeBufferLength)
  {
    FW_ASSERT(incomingFilePrefix != nullptr);
    this->m_maxFileSize = maxFileSize;
    this->m_storeBufferLength = storeBufferLength;
    if( this->m_storeBufferLength ) {
      FW_ASSERT(maxFileSize > sizeof(U16), static_cast<FwAssertArgType>(maxFileSize));
    }

    FW_ASSERT(Fw::StringUtils::string_length(incomingFilePrefix, static_cast<FwSizeType>(sizeof(this->m_filePrefix))) < sizeof(this->m_filePrefix),
      static_cast<FwAssertArgType>(Fw::StringUtils::string_length(incomingFilePrefix, static_cast<FwSizeType>(sizeof(this->m_filePrefix)))),
      static_cast<FwAssertArgType>(sizeof(this->m_filePrefix))); // ensure that file prefix is not too big

    (void)Fw::StringUtils::string_copy(this->m_filePrefix, incomingFilePrefix, static_cast<FwSizeType>(sizeof(this->m_filePrefix)));

    this->m_initialized = true;
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
    if( OPEN == this->m_fileMode ) {
      // Close file:
      this->m_file.close();

      // Write out the hash file to disk:
      this->writeHashFile();

      // Update mode:
      this->m_fileMode = CLOSED;

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
    FW_ASSERT(size32 < 65536, static_cast<FwAssertArgType>(size32));
    U16 size = size32 & 0xFFFF;

    // Close the file if it will be too big:
    if( OPEN == this->m_fileMode ) {
      U32 projectedByteCount = this->m_byteCount + size;
      if( this->m_storeBufferLength ) {
        projectedByteCount += static_cast<U32>(sizeof(size));
      }
      if( projectedByteCount > this->m_maxFileSize ) {
        this->closeFile();
      }
    }

    // Open the file if it there is not one open:
    if( CLOSED == this->m_fileMode ){
      this->openFile();
    }

    // Write to the file if it is open:
    if( OPEN == this->m_fileMode ) {
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
    FW_ASSERT( CLOSED == this->m_fileMode );

    if( !this->m_initialized ){
        this->log_WARNING_LO_FileNotInitialized();
        return;
    }

    U32 bytesCopied;

    // Create filename:
    Fw::Time timestamp = getTime();
    memset(this->m_fileName, 0, sizeof(this->m_fileName));
    bytesCopied = static_cast<U32>(snprintf(
      this->m_fileName,
      sizeof(this->m_fileName),
      "%s_%" PRI_FwTimeBaseStoreType "_%" PRIu32 "_%06" PRIu32 ".com",
      this->m_filePrefix,
      static_cast<FwTimeBaseStoreType>(timestamp.getTimeBase()),
      timestamp.getSeconds(),
      timestamp.getUSeconds()));

    // "A return value of size or more means that the output was truncated"
    // See here: http://linux.die.net/man/3/snprintf
    FW_ASSERT( bytesCopied < sizeof(this->m_fileName) );

    // Create sha filename:
    bytesCopied = static_cast<U32>(snprintf(
      this->m_hashFileName,
      sizeof(this->m_hashFileName),
      "%s_%" PRI_FwTimeBaseStoreType "_%" PRIu32 "_%06" PRIu32 ".com%s",
      this->m_filePrefix,
      static_cast<FwTimeBaseStoreType>(timestamp.getTimeBase()),
      timestamp.getSeconds(),
      timestamp.getUSeconds(),
      Utils::Hash::getFileExtensionString()));
    FW_ASSERT( bytesCopied < sizeof(this->m_hashFileName) );

    Os::File::Status ret = m_file.open(this->m_fileName, Os::File::OPEN_WRITE);
    if( Os::File::OP_OK != ret ) {
      if( !this->m_openErrorOccurred ) { // throttle this event, otherwise a positive
                                       // feedback event loop can occur!
        Fw::LogStringArg logStringArg(this->m_fileName);
        this->log_WARNING_HI_FileOpenError(ret, logStringArg);
      }
      this->m_openErrorOccurred = true;
    } else {
      // Reset event throttle:
      this->m_openErrorOccurred = false;

      // Reset byte count:
      this->m_byteCount = 0;

      // Set mode:
      this->m_fileMode = OPEN;
    }
  }

  void ComLogger ::
    closeFile(
    )
  {
    if( OPEN == this->m_fileMode ) {
      // Close file:
      this->m_file.close();

      // Write out the hash file to disk:
      this->writeHashFile();

      // Update mode:
      this->m_fileMode = CLOSED;

      // Send event:
      Fw::LogStringArg logStringArg(this->m_fileName);
      this->log_DIAGNOSTIC_FileClosed(logStringArg);
    }
  }

  void ComLogger ::
    writeComBufferToFile(
      Fw::ComBuffer &data,
      U16 size
    )
  {
    if( this->m_storeBufferLength ) {
      U8 buffer[sizeof(size)];
      Fw::SerialBuffer serialLength(&buffer[0], sizeof(size));
      serialLength.serialize(size);
      if(this->writeToFile(serialLength.getBuffAddr(),
              static_cast<U16>(serialLength.getBuffLength()))) {
        this->m_byteCount += serialLength.getBuffLength();
      }
      else {
        return;
      }
    }

    // Write buffer to file:
    if(this->writeToFile(data.getBuffAddr(), size)) {
      this->m_byteCount += size;
    }
  }

  bool ComLogger ::
    writeToFile(
      void* data,
      U16 length
    )
  {
    FwSignedSizeType size = length;
    Os::File::Status ret = m_file.write(reinterpret_cast<const U8*>(data), size);
    if( Os::File::OP_OK != ret || size != static_cast<NATIVE_INT_TYPE>(length) ) {
      if( !this->m_writeErrorOccurred ) { // throttle this event, otherwise a positive
                                        // feedback event loop can occur!
        Fw::LogStringArg logStringArg(this->m_fileName);
        this->log_WARNING_HI_FileWriteError(ret, static_cast<U32>(size), length, logStringArg);
      }
      this->m_writeErrorOccurred = true;
      return false;
    }

    this->m_writeErrorOccurred = false;
    return true;
  }

  void ComLogger ::
    writeHashFile(
    )
  {
    Os::ValidateFile::Status validateStatus;
    validateStatus = Os::ValidateFile::createValidation(this->m_fileName, this->m_hashFileName);
    if( Os::ValidateFile::VALIDATION_OK != validateStatus ) {
      Fw::LogStringArg logStringArg1(this->m_fileName);
      Fw::LogStringArg logStringArg2(this->m_hashFileName);
      this->log_WARNING_LO_FileValidationError(logStringArg1, logStringArg2, validateStatus);
    }
  }
}
