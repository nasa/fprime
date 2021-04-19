// ----------------------------------------------------------------------
//
// ComLogger.hpp
//
// ----------------------------------------------------------------------

#ifndef Svc_ComLogger_HPP
#define Svc_ComLogger_HPP

#include "Svc/ComLogger/ComLoggerComponentAc.hpp"
#include <Os/File.hpp>
#include <Os/Mutex.hpp>
#include <Fw/Types/Assert.hpp>
#include <Utils/Hash/Hash.hpp>

#include <limits.h>
#include <stdio.h>
#include <cstdarg>

namespace Svc {

  class ComLogger :
    public ComLoggerComponentBase
  {
      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

    public:

      // CONSTRUCTOR:
      // filePrefix: string to prepend the file name with, ie. "thermal_telemetry"
      // maxFileSize: the maximum size a file should reach before being closed and a new one opened
      // storeBufferLength: if true, store the length of each com buffer before storing the buffer itself,
      //                    otherwise just store the com buffer. false might be advantageous in a system
      //                    where you can ensure that all buffers given to the ComLogger are the same size
      //                    in which case you do not need the overhead. Or you store an id which you can
      //                    match to an expected size on the ground during post processing.
      ComLogger(const char* compName, const char* filePrefix, U32 maxFileSize, bool storeBufferLength=true);

      void init(
          NATIVE_INT_TYPE queueDepth, //!< The queue depth
          NATIVE_INT_TYPE instance //!< The instance number
      );

      ~ComLogger(void);

      // ----------------------------------------------------------------------
      // Handler implementations
      // ----------------------------------------------------------------------

    PRIVATE:

      void comIn_handler(
          NATIVE_INT_TYPE portNum,
          Fw::ComBuffer &data,
          U32 context
      );

      void CloseFile_cmdHandler(
          FwOpcodeType opCode,
          U32 cmdSeq
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      // ----------------------------------------------------------------------
      // Constants:
      // ----------------------------------------------------------------------
      // The maximum size of a filename
      enum { 
        MAX_FILENAME_SIZE = NAME_MAX, // as defined in limits.h
        MAX_PATH_SIZE = PATH_MAX
      };

      // The filename data:
      U8 filePrefix[MAX_FILENAME_SIZE + MAX_PATH_SIZE];
      U32 maxFileSize;

      // ----------------------------------------------------------------------
      // Internal state:
      // ----------------------------------------------------------------------
      enum FileMode {
          CLOSED = 0,
          OPEN = 1
      };

      FileMode fileMode;
      Os::File file;
      U8 fileName[MAX_FILENAME_SIZE + MAX_PATH_SIZE];
      U8 hashFileName[MAX_FILENAME_SIZE + MAX_PATH_SIZE];
      U32 byteCount;
      bool writeErrorOccurred;
      bool openErrorOccurred;
      bool storeBufferLength;
      
      // ----------------------------------------------------------------------
      // File functions:
      // ---------------------------------------------------------------------- 
      void openFile(
      );

      void closeFile(
      );

      void writeComBufferToFile(
        Fw::ComBuffer &data,
        U16 size
      );

      // ----------------------------------------------------------------------
      // Helper functions:
      // ---------------------------------------------------------------------- 

      bool writeToFile(
        void* data, 
        U16 length
      );

      void writeHashFile(
      );
  };
};

#endif
