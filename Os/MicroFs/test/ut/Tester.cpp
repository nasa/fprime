
#include "Tester.hpp"
#include <stdio.h>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include <Fw/Types/MallocAllocator.hpp>


namespace Os {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester()
  {
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------
  // InitTest
  // ----------------------------------------------------------------------
  void Tester ::
    InitTest()
  {
    // Instantiate the Rules
    U32 numBins = 1;
    U32 fileSize = 100;
    U32 numFiles = 1;
    InitFileSystem initFileSystem(numBins, fileSize, numFiles);
    Cleanup cleanup;

    // Run the Rules
    initFileSystem.apply(*this);
    cleanup.apply(*this);


  }

  // ----------------------------------------------------------------------
  // OpenWriteReadTest
  // ----------------------------------------------------------------------
  void Tester ::
    OpenWriteReadTest()
  {

    // Instantiate the Rules
    U32 numBins = 1;
    U32 fileSize = 100;
    U32 numFiles = 1;
    InitFileSystem initFileSystem(numBins, fileSize, numFiles);
    OpenFile openFile;
    Cleanup cleanup;

    // Run the Rules
    initFileSystem.apply(*this);
    openFile.apply(*this);
    cleanup.apply(*this);
    
  }


} // end namespace Os
