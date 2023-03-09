
    
#include "Tester.hpp"
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Os/MicroFs/MicroFs.hpp>
#include <Fw/Types/MallocAllocator.hpp>


  // ------------------------------------------------------------------------------------------------------
  // Rule:  InitFileSystem
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::InitFileSystem::InitFileSystem(U32 numBins, U32 fileSize, U32 numFiles) :
        STest::Rule<Os::Tester>("InitFileSystem")
  {
    this->numBins = numBins;
    this->fileSize = fileSize;
    this->numFiles = numFiles;
  }


  bool Os::Tester::InitFileSystem::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::InitFileSystem::action(
            Os::Tester& state //!< The test state
        ) 
  {
    printf("--> Rule: %s \n", this->name);

    state.testCfg.numBins = this->numBins;
    state.testCfg.bins[0].fileSize = this->fileSize;
    state.testCfg.bins[0].numFiles = this->numFiles;
    
    Os::MicroFsInit(state.testCfg,0, state.alloc);

  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  OpenFile
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::OpenFile::OpenFile() :
        STest::Rule<Os::Tester>("OpenFile")
  {
  }


  bool Os::Tester::OpenFile::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::OpenFile::action(
            Os::Tester& state //!< The test state
        ) 
  {
    printf("--> Rule: %s \n", this->name);

    Os::File::Status stat = state.f.open("/bin0/file0", Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::OP_OK, stat);
  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  Cleanup
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::Cleanup::Cleanup() :
        STest::Rule<Os::Tester>("Cleanup")
  {
  }


  bool Os::Tester::Cleanup::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::Cleanup::action(
            Os::Tester& state //!< The test state
        ) 
  {
    printf("--> Rule: %s \n", this->name);

    Os::MicroFsCleanup(0,state.alloc);

  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  WriteData
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::WriteData::WriteData() :
        STest::Rule<Os::Tester>("WriteData")
  {
  }


  bool Os::Tester::WriteData::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::WriteData::action(
            Os::Tester& state //!< The test state
        ) 
  {
    printf("--> Rule: %s \n", this->name);

    // BYTE buffOut[testCfg.bins[0].fileSize];
    // memset(buffOut,0xFF,sizeof(buffOut));
    // NATIVE_INT_TYPE size = sizeof(buffOut);
    // ASSERT_EQ(Os::File::OP_OK,f.write(buffOut,size));
    // ASSERT_EQ(sizeof(buffOut),size);
  }

