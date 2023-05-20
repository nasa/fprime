// ----------------------------------------------------------------------
// Tester.hpp
// ----------------------------------------------------------------------

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "../../ComLogger.hpp"
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <cstdio>

#define QUEUE_DEPTH 10
#define FILE_STR "test"
#define MAX_ENTRIES_PER_FILE 5
#define COM_BUFFER_LENGTH 4u
#define MAX_BYTES_PER_FILE (MAX_ENTRIES_PER_FILE*COM_BUFFER_LENGTH + MAX_ENTRIES_PER_FILE*sizeof(U16))
#define MAX_BYTES_PER_FILE_NO_LENGTH (MAX_ENTRIES_PER_FILE*COM_BUFFER_LENGTH)

namespace Svc {
  class Tester :
    public ComLoggerGTestBase
  {

    public:
      Tester(const char *const compName);

      // This constructor will construct comLogger with its
      // standard constructor
      Tester(const char *const compName, bool standardCLInit);
      ~Tester();

      void testLogging();
      void testLoggingNoLength();
      void openError();
      void writeError();
      void closeFileCommand();
      void testLoggingWithInit();
      void noInitError();
    private:
      void connectPorts();
      void initComponents();
      void dispatchOne();
      void dispatchAll();
      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

      ComLogger comLogger;
  };

};

#endif
