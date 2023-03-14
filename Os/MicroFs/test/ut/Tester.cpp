
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
  // OddTest
  // ----------------------------------------------------------------------
  void Tester ::
    OddTest()
  {
    const U16 NumberBins = 1;
    const U16 NumberFiles = 2;

    const char* File1 = "/bin0/file0";
    const char* File2 = "/bin0/file2";
    // const char* File3 = "/bin1/file0";
    // const char* File4 = "/bin1/file2";

    const U16 TotalFiles = NumberBins * NumberFiles;
    clearFileBuffer();

    // Instantiate the Rules
    InitFileSystem initFileSystem(NumberBins, FILE_SIZE, NumberFiles);
    OpenFile openFile1(File1);
    OpenFile openFile2(File2);
    // OpenFile openFile3(File3);
    // OpenFile openFile4(File4);
    Listings listings(NumberBins, NumberFiles);

    Cleanup cleanup;

    // Run the Rules
    initFileSystem.apply(*this);
    openFile1.apply(*this);
    openFile2.apply(*this);
    // openFile3.apply(*this);
    // openFile4.apply(*this);
    listings.apply(*this);

    cleanup.apply(*this);
  }

  // ----------------------------------------------------------------------
  // OpenStressTest
  // ----------------------------------------------------------------------
  void Tester ::
    OpenStressTest()
  {
    const U16 NumberBins = MAX_BINS;
    const U16 NumberFiles = MAX_FILES_PER_BIN;

    const U16 TotalFiles = NumberBins * NumberFiles;
    clearFileBuffer();

    // Instantiate the Rules
    OpenFile* openFile[TotalFiles];
    InitFileSystem initFileSystem(NumberBins, FILE_SIZE, NumberFiles);
    Cleanup cleanup;

    char fileName[TotalFiles][20];

    U16 fileIndex =  0;
    for (U16 bin=0; bin < NumberBins; bin++)
    {
      for (U16 file=0; file < NumberFiles; file++)
      {
        snprintf(fileName[fileIndex], 20, "/bin%d/file%d", bin, file);
        fileIndex++;
      }
    }
    

    // Run the Rules
    initFileSystem.apply(*this);

    for (U16 i = 0; i < TotalFiles; i++)
    {
      openFile[i] = new OpenFile(fileName[i]);
      openFile[i]->apply(*this);
    }

    cleanup.apply(*this);
  }

  // ----------------------------------------------------------------------
  // InitTest
  // ----------------------------------------------------------------------
  void Tester ::
    InitTest()
  {
    clearFileBuffer();

    // Instantiate the Rules
    const U16 NumberBins = MAX_BINS;
    const U16 NumberFiles = MAX_FILES_PER_BIN;

    InitFileSystem initFileSystem(NumberBins, FILE_SIZE, NumberFiles);
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
    clearFileBuffer();

    // Instantiate the Rules
    const U16 NumberBins = MAX_BINS;
    const U16 NumberFiles = MAX_FILES_PER_BIN;
    const char* FileName = "/bin9/file9";

    InitFileSystem initFileSystem(NumberBins, FILE_SIZE, NumberFiles);
    OpenFile openFile(FileName);
    ResetFile resetFile(FileName);
    Cleanup cleanup;
    WriteData writeData(FileName, FILE_SIZE, 0xFF);
    ReadData readData(FileName, FILE_SIZE);

    // Run the Rules
    initFileSystem.apply(*this);
    openFile.apply(*this);
    writeData.apply(*this);
    resetFile.apply(*this);
    readData.apply(*this);

    cleanup.apply(*this);
    
  }


  // ----------------------------------------------------------------------
  // OpenWriteTwiceReadOnceTest
  // ----------------------------------------------------------------------
  void Tester ::
    OpenWriteTwiceReadOnceTest()
  {
    clearFileBuffer();

    // Instantiate the Rules
    const U16 NumberBins = MAX_BINS;
    const U16 NumberFiles = MAX_FILES_PER_BIN;
    const char* FileName = "/bin0/file0";

    InitFileSystem initFileSystem(NumberBins, FILE_SIZE, NumberFiles);
    OpenFile openFile(FileName);
    ResetFile resetFile(FileName);
    Cleanup cleanup;
    WriteData writeData1(FileName, FILE_SIZE/2, 0x11);
    WriteData writeData2(FileName, FILE_SIZE/2, 0x22);
    ReadData readData(FileName, FILE_SIZE);

    // Run the Rules
    initFileSystem.apply(*this);
    openFile.apply(*this);
    writeData1.apply(*this);
    writeData2.apply(*this);
    resetFile.apply(*this);
    readData.apply(*this);

    cleanup.apply(*this);
    
  }
  // ----------------------------------------------------------------------
  // OpenWriteOnceReadTwiceTest
  // ----------------------------------------------------------------------
  void Tester ::
    OpenWriteOnceReadTwiceTest()
  {
    clearFileBuffer();

    // Instantiate the Rules
    const U16 NumberBins = MAX_BINS;
    const U16 NumberFiles = MAX_FILES_PER_BIN;
    const char* FileName = "/bin0/file0";
    
    InitFileSystem initFileSystem(NumberBins, FILE_SIZE, NumberFiles);
    OpenFile openFile(FileName);
    ResetFile resetFile(FileName);
    Cleanup cleanup;
    WriteData writeData(FileName, FILE_SIZE, 0xFF);
    ReadData readData(FileName, FILE_SIZE/2);

    // Run the Rules
    initFileSystem.apply(*this);
    openFile.apply(*this);
    writeData.apply(*this);
    resetFile.apply(*this);
    readData.apply(*this);
    readData.apply(*this);

    cleanup.apply(*this);
    
  }

  // ----------------------------------------------------------------------
  // OneFileReadDirectory
  // ----------------------------------------------------------------------
  void Tester ::
    OneFileReadDirectory()
  {
    clearFileBuffer();

    // Instantiate the Rules
    const U16 NumberBins = 2;
    const U16 NumberFiles = 2;
    const char* File1 = "/bin0/file0";
    const char* File2 = "/bin0/file1";
    const char* File3 = "/bin1/file0";
    const char* File4 = "/bin1/file1";

    InitFileSystem initFileSystem(NumberBins, FILE_SIZE, NumberFiles);
    OpenFile openFile1(File1);
    OpenFile openFile2(File2);
    OpenFile openFile3(File3);
    OpenFile openFile4(File4);
    Cleanup cleanup;
    WriteData writeData(File1, FILE_SIZE, 0xFF);
    CloseFile closeFile(File1);
    Listings listings(NumberBins, NumberFiles);

    // Run the Rules
    initFileSystem.apply(*this);
    openFile1.apply(*this);
    openFile2.apply(*this);
    openFile3.apply(*this);
    openFile4.apply(*this);
    //writeData.apply(*this);
    //closeFile.apply(*this);
    listings.apply(*this);

    cleanup.apply(*this);
    
  }

  // ----------------------------------------------------------------------
  // OpenWriteReadTest
  // ----------------------------------------------------------------------
  void Tester ::
    OpenFreeSpaceTest()
  {
    clearFileBuffer();

    // Instantiate the Rules
    U32 numBins = 1;
    U32 numFiles = 1;
    const char* FileName = "/bin0/file0";
    InitFileSystem initFileSystem(numBins, FILE_SIZE, numFiles);

    FreeSpace freeSpace;
    OpenFile openFile(FileName);
    // ResetFile resetFile;
    Cleanup cleanup;
    WriteData writeData(FileName, FILE_SIZE, 0xFF);

    // Run the Rules
    initFileSystem.apply(*this);
    this->m_expFreeBytes = 100;
    this->m_expTotalBytes = 100;
    freeSpace.apply(*this);
    openFile.apply(*this);
    writeData.apply(*this);
    this->m_expFreeBytes = 0;
    this->m_expTotalBytes = 100;
    freeSpace.apply(*this);

    cleanup.apply(*this);
    
  }


  // Helper functions
  void Tester::clearFileBuffer()
  {
        this->curPtr = 0;
        memset(this->buffOut, 0xA5, FILE_SIZE);
  }

  I16 Tester::getIndex(const char *fileName)
  {
    const char* filePathSpec = "/bin%d/file%d";

    FwNativeUIntType binIndex = 0;
    FwNativeUIntType fileIndex = 0;
    I16 stat = sscanf(fileName, filePathSpec, &binIndex, &fileIndex);
    if (stat != 2)
    {
      return -1;

    } else {
      return binIndex * MAX_BINS + fileIndex;
    }

  }


} // end namespace Os
