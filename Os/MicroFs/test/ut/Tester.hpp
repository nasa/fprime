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
        BUFFER_SIZE = 100
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
      Os::File f;
      BYTE buffOut[BUFFER_SIZE];
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

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

  };

} // end namespace Os

#endif
