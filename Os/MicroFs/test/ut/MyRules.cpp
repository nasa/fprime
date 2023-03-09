
    
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
  
  Os::Tester::WriteData::WriteData(NATIVE_INT_TYPE size) :
        STest::Rule<Os::Tester>("WriteData")
  {
    this->size = size;
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

    ASSERT_LE(state.curPtr + this->size, Tester::BUFFER_SIZE);
    memset(state.buffOut + state.curPtr, 0xFF, this->size);
    state.curPtr = state.curPtr + this->size;
    NATIVE_INT_TYPE retSize = this->size;
    Os::File::Status stat = state.f.write(state.buffOut, retSize);
    ASSERT_EQ(stat, Os::File::OP_OK);
    ASSERT_EQ(retSize, this->size);

    for (U16 i=0; i<Tester::BUFFER_SIZE; i++)
    {
        printf("%X ", state.buffOut[i]);
    }
    printf("\n");


  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  ReadData
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::ReadData::ReadData(NATIVE_INT_TYPE size) :
        STest::Rule<Os::Tester>("ReadData")
  {
    this->size = size;
  }


  bool Os::Tester::ReadData::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::ReadData::action(
            Os::Tester& state //!< The test state
        ) 
  {
      printf("--> Rule: %s \n", this->name);

      BYTE buffIn[state.testCfg.bins[0].fileSize];
      NATIVE_INT_TYPE bufferSize = sizeof(buffIn);
      memset(buffIn,0xA5,sizeof(buffIn));
      ASSERT_LE(this->size, sizeof(buffIn));
      NATIVE_INT_TYPE retSize = this->size;
      Os::File::Status stat = state.f.read(buffIn, retSize);
      ASSERT_EQ(stat, Os::File::OP_OK);
      ASSERT_EQ(retSize, this->size);

      // Check the returned data
      ASSERT_LE(state.curPtr + this->size, Tester::BUFFER_SIZE);
      ASSERT_EQ(0,memcmp(buffIn, state.buffOut+state.curPtr, this->size));

  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  ResetFile
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::ResetFile::ResetFile() :
        STest::Rule<Os::Tester>("ResetFile")
  {
  }


  bool Os::Tester::ResetFile::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::ResetFile::action(
            Os::Tester& state //!< The test state
        ) 
  {
      printf("--> Rule: %s \n", this->name);

      // seek back to beginning
      ASSERT_EQ(Os::File::OP_OK, state.f.seek(0));
      state.curPtr = 0;
  }

