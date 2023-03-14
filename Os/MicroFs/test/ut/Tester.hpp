// ======================================================================
// \title  DataRecorder/test/ut/Tester.hpp
// \author watney
// \brief  hpp file for DataRecorder test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Os/MicroFs/MicroFs.hpp>
#include <Fw/Types/MallocAllocator.hpp>


#include "RulesHeaders.hpp"

namespace Os {

  class Tester
  {
      #include "MyRules.hpp"

      enum {
        MAX_BINS = 10,
        MAX_FILES_PER_BIN = 10,
        MAX_TOTAL_FILES = MAX_BINS * MAX_FILES_PER_BIN,
        FILE_SIZE = 100
      };

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester();

      //! Destroy object Tester
      //!
      ~Tester();

      Fw::MallocAllocator alloc;
      Os::MicroFsConfig testCfg;
      Os::File fileDesc[MAX_TOTAL_FILES];
      BYTE buffOut[FILE_SIZE];
      NATIVE_INT_TYPE curPtr;

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //!
      void InitTest();
      void OpenWriteReadTest();
      void OpenWriteTwiceReadOnceTest();
      void OpenWriteOnceReadTwiceTest();
      void OneFileReadDirectory();
      void OpenStressTest();
      void OpenFreeSpaceTest();
      void OddTest();

      // Helper functions
      void clearFileBuffer();

    private:

     

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

      I16 getIndex(const char *fileName);

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      // Free space test values
      FwSizeType m_expTotalBytes;
      FwSizeType m_expFreeBytes;
      FileSystem::Status m_expStat;  //! Expected status for internal filesystem calls

  };

} // end namespace Os

#endif
