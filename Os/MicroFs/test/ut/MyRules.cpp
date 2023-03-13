
    
#include "Tester.hpp"
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Os/MicroFs/MicroFs.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Fw/Test/UnitTest.hpp>



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

    ASSERT_LE(this->numBins, Os::MAX_MICROFS_BINS);

    state.testCfg.numBins = this->numBins;

    for (U16 i=0; i < this->numBins; i++)
    {
        state.testCfg.bins[i].fileSize = this->fileSize;
        state.testCfg.bins[i].numFiles = this->numFiles;
    }
    
    Os::MicroFsInit(state.testCfg, 0, state.alloc);

  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  OpenFile
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::OpenFile::OpenFile(const char *filename) :
        STest::Rule<Os::Tester>("OpenFile")
  {
    this->filename = filename;
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
    printf("--> Rule: %s %s\n", this->name, this->filename);

    Os::File::Status stat = state.f.open(this->filename, Os::File::OPEN_CREATE);
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
  
  Os::Tester::WriteData::WriteData(NATIVE_INT_TYPE size, U8 value) :
        STest::Rule<Os::Tester>("WriteData")
  {
    this->size = size;
    this->value = value;
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
    memset(state.buffOut + state.curPtr, this->value, this->size);
    NATIVE_INT_TYPE retSize = this->size;
    Os::File::Status stat = state.f.write(state.buffOut + state.curPtr, retSize);
    state.curPtr = state.curPtr + this->size;
    ASSERT_EQ(stat, Os::File::OP_OK);
    ASSERT_EQ(retSize, this->size);

    #if 0
    for (U16 i=0; i<Tester::BUFFER_SIZE; i++)
    {
        printf("%X ", state.buffOut[i]);
    }
    printf("\n");
    #endif


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


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  CloseFile
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::CloseFile::CloseFile() :
        STest::Rule<Os::Tester>("CloseFile")
  {
  }


  bool Os::Tester::CloseFile::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::CloseFile::action(
            Os::Tester& state //!< The test state
        ) 
  {
    printf("--> Rule: %s \n", this->name);

    // close file
    state.f.close();
  }


    


  // ------------------------------------------------------------------------------------------------------
  // Rule:  Listings
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::Listings::Listings() :
        STest::Rule<Os::Tester>("Listings")
  {
  }


  bool Os::Tester::Listings::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::Listings::action(
            Os::Tester& state //!< The test state
        ) 
  {
    printf("--> Rule: %s \n", this->name);

    Fw::String listDir;
    Fw::String expectedFile;
    Fw::String files[1];
    U32 numFiles = 10; // oversize to check return

    COMMENT("Listing /");
    listDir = "/";

    // get root directory listing
    ASSERT_EQ(Os::FileSystem::OP_OK,
        Os::FileSystem::readDirectory(listDir.toChar(),1, files, numFiles));
    ASSERT_EQ(1,numFiles);

    expectedFile.format("/%s0", MICROFS_BIN_STRING);
    ASSERT_EQ(0,strcmp(expectedFile.toChar(), files[0].toChar()));

    // get file listing
    listDir.format("/%s0",MICROFS_BIN_STRING);
    Fw::String msg;
    msg.format("Listing %s",listDir.toChar());
    COMMENT(msg.toChar());
    numFiles = 10;

    ASSERT_EQ(Os::FileSystem::OP_OK,
        Os::FileSystem::readDirectory(listDir.toChar(),1, files, numFiles));

    expectedFile.format("/%s0/%s0",MICROFS_BIN_STRING,MICROFS_FILE_STRING);
    printf("%s\n", expectedFile.toChar());

    ASSERT_EQ(0,strcmp(expectedFile.toChar(),files[0].toChar()));

    // check nonexistent bin

    listDir.format("/%s1",MICROFS_BIN_STRING);
    msg.format("Listing nonexistent %s",listDir.toChar());
    COMMENT(msg.toChar());
    numFiles = 10;

    ASSERT_EQ(Os::FileSystem::INVALID_PATH,
        Os::FileSystem::readDirectory(listDir.toChar(),1, files, numFiles));
  }

  // ------------------------------------------------------------------------------------------------------
  // Rule:  FreeSpace
  //
  // ------------------------------------------------------------------------------------------------------
  
  Os::Tester::FreeSpace::FreeSpace() :
        STest::Rule<Os::Tester>("FreeSpace")
  {
  }


  bool Os::Tester::FreeSpace::precondition(
            const Os::Tester& state //!< The test state
        ) 
  {
      return true;
  }

  
  void Os::Tester::FreeSpace::action(
            Os::Tester& state //!< The test state
        ) 
  {
    printf("--> Rule: %s \n", this->name);

    FwSizeType total;
    FwSizeType free;

    FileSystem::Status stat = FileSystem::getFreeSpace("", total, free);
    ASSERT_EQ(state.m_expFreeBytes,free);
    ASSERT_EQ(state.m_expTotalBytes,total);

  }

