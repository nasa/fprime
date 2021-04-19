// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Tester.hpp"
#include <fstream>


#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) : 
#if FW_OBJECT_NAMES == 1
      ActiveTextLoggerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("ActiveTextLogger")
#else
      ActiveTextLoggerGTestBase(MAX_HISTORY_SIZE),
      component()
#endif
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester(void) 
  {
    
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------

  void Tester ::
  run_nominal_test(void)
  {
      printf("Testing writing to console\n");

      // Verifying initial state:
      ASSERT_FALSE(this->component.m_log_file.m_openFile);

      FwEventIdType id = 1;
      Fw::Time timeTag(TB_NONE,3,6);
      Fw::TextLogSeverity severity = Fw::TEXT_LOG_ACTIVITY_HI;
      Fw::TextLogString text("This component is the greatest!");
      this->invoke_to_TextLogger(0,id,timeTag,severity,text);
      this->component.doDispatch();

      id = 2;
      timeTag.set(TB_PROC_TIME,4,7);
      severity = Fw::TEXT_LOG_ACTIVITY_LO;
      text = "This component is the probably the greatest!";
      this->invoke_to_TextLogger(0,id,timeTag,severity,text);
      this->component.doDispatch();

      // This will output in a different format b/c WORKSTATION_TIME
      id = 3;
      timeTag.set(TB_WORKSTATION_TIME,5,876);
      severity = Fw::TEXT_LOG_WARNING_LO;
      text = "This component is maybe the greatest!";
      this->invoke_to_TextLogger(0,id,timeTag,severity,text);
      this->component.doDispatch();

      // No way to verify internal interface was called, but
      // can manually see the print out to the console

      printf("Testing writing to console and file\n");

      // Setup file for writing to:
      bool stat = this->component.set_log_file("test_file",512);

      ASSERT_TRUE(stat);
      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      EXPECT_STREQ("test_file",this->component.m_log_file.m_fileName.toChar());
      ASSERT_EQ(0U, this->component.m_log_file.m_currentFileSize);
      ASSERT_EQ(512U, this->component.m_log_file.m_maxFileSize);

      id = 4;
      timeTag.set(TB_NONE,5,8);
      severity = Fw::TEXT_LOG_WARNING_LO;
      const char* severityString = "WARNING_LO";
      text = "This component may be the greatest!";
      this->invoke_to_TextLogger(0,id,timeTag,severity,text);
      this->component.doDispatch();

      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(strlen(text.toChar())+32, this->component.m_log_file.m_currentFileSize);
      ASSERT_EQ(512U, this->component.m_log_file.m_maxFileSize);
      U32 past_size = strlen(text.toChar())+32;

      // Read file to verify contents:
      std::ifstream stream1("test_file");
      char oldLine[256];
      while(stream1) {
          char buf[256];
          stream1.getline(buf,256);
          if (stream1) {
              std::cout << "readLine: " << buf << std::endl;
              char textStr[512];
              sprintf(textStr,
                      "EVENT: (%d) (%d:%d,%d) %s: %s",
                       id,timeTag.getTimeBase(),timeTag.getSeconds(),timeTag.getUSeconds(),severityString,text.toChar());
              ASSERT_EQ(0,strcmp(textStr,buf));
              strcpy(oldLine,buf);
          }
      }
      stream1.close();

      id = 5;
      timeTag.set(TB_PROC_TIME,6,9);
      severity = Fw::TEXT_LOG_WARNING_HI;
      severityString = "WARNING_HI";
      text = "This component is probably not the greatest!";
      this->invoke_to_TextLogger(0,id,timeTag,severity,text);
      this->component.doDispatch();

      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(past_size+strlen(text.toChar())+32, this->component.m_log_file.m_currentFileSize);
      ASSERT_EQ(512U, this->component.m_log_file.m_maxFileSize);

      // Test predicted size matches actual:
      U64 fileSize = 0;
      Os::FileSystem::getFileSize("test_file",fileSize);
      ASSERT_EQ(fileSize,this->component.m_log_file.m_currentFileSize);

      // Read file to verify contents:
      std::ifstream stream2("test_file");
      U32 iter = 0;
      while(stream2) {
          char buf[256];
          stream2.getline(buf,256);
          if (stream2) {
              std::cout << "readLine: " << buf << std::endl;
              // Verify first printed line is still there
              if (iter == 0) {
                  ASSERT_EQ(0,strcmp(oldLine,buf));
              }
              // Verify new printed line
              else {
                  char textStr[512];
                  sprintf(textStr,
                          "EVENT: (%d) (%d:%d,%d) %s: %s",
                           id,timeTag.getTimeBase(),timeTag.getSeconds(),timeTag.getUSeconds(),severityString,text.toChar());
                  ASSERT_EQ(0,strcmp(textStr,buf));
              }
          }
          ++iter;
      }
      stream2.close();

      // Clean up:
      remove("test_file");

  }

  void Tester ::
  run_off_nominal_test(void)
  {
      // TODO file errors- use the Os/Stubs?

      U64 tmp;

      printf("Testing writing text that is larger than FW_INTERNAL_INTERFACE_STRING_MAX_SIZE\n");
      // Cant test this b/c max size of TextLogString is 256 and
      // FW_INTERNAL_INTERFACE_STRING_MAX_SIZE is 512

      printf("Testing writing more than the max file size\n");

      // Setup file for writing to:
      bool stat = this->component.set_log_file("test_file_max",45);

      ASSERT_TRUE(stat);
      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(0,strcmp("test_file_max",this->component.m_log_file.m_fileName.toChar()));
      ASSERT_EQ(0U, this->component.m_log_file.m_currentFileSize);
      ASSERT_EQ(45U, this->component.m_log_file.m_maxFileSize);
      ASSERT_EQ(Os::FileSystem::OP_OK,
                Os::FileSystem::getFileSize("test_file_max",tmp));

      // Write once to the file:
      FwEventIdType id = 1;
      Fw::Time timeTag(TB_NONE,3,6);
      Fw::TextLogSeverity severity = Fw::TEXT_LOG_ACTIVITY_HI;
      const char* severityString = "ACTIVITY_HI";
      Fw::TextLogString text("abcd");
      this->invoke_to_TextLogger(0,id,timeTag,severity,text);
      this->component.doDispatch();

      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(strlen(text.toChar())+33, this->component.m_log_file.m_currentFileSize);
      ASSERT_EQ(45U, this->component.m_log_file.m_maxFileSize);
      U32 past_size = strlen(text.toChar())+33;

      // Read file to verify contents:
      std::ifstream stream1("test_file_max");
      char oldLine[256];
      while(stream1) {
          char buf[256];
          stream1.getline(buf,256);
          if (stream1) {
              std::cout << "readLine: " << buf << std::endl;
              char textStr[512];
              sprintf(textStr,
                      "EVENT: (%d) (%d:%d,%d) %s: %s",
                       id,timeTag.getTimeBase(),timeTag.getSeconds(),timeTag.getUSeconds(),severityString,text.toChar());
              ASSERT_EQ(0,strcmp(textStr,buf));
              strcpy(oldLine,buf);
          }
      }
      stream1.close();

      // Write again to the file going over the max:
      text = "This will go over max size of the file!";
      this->invoke_to_TextLogger(0,id,timeTag,severity,text);
      this->component.doDispatch();

      // Verify file was closed and size didn't increase:
      ASSERT_FALSE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(past_size, this->component.m_log_file.m_currentFileSize);
      ASSERT_EQ(45U, this->component.m_log_file.m_maxFileSize);

      // Read file to verify contents didn't change:
      std::ifstream stream2("test_file_max");
      while(stream2) {
          char buf[256];
          stream2.getline(buf,256);
          if (stream2) {
              std::cout << "readLine: " << buf << std::endl;
              ASSERT_EQ(0,strcmp(oldLine,buf));
          }
      }
      stream2.close();

      printf("Testing with file name that already exists\n");

      // Setup file for writing to that is a duplicate name:
      stat = this->component.set_log_file("test_file_max",50);

      // Verify made file with 0 suffix:
      ASSERT_TRUE(stat);
      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(0,strcmp("test_file_max0",this->component.m_log_file.m_fileName.toChar()));
      ASSERT_EQ(0U, this->component.m_log_file.m_currentFileSize);
      ASSERT_EQ(50U, this->component.m_log_file.m_maxFileSize);
      ASSERT_EQ(Os::FileSystem::OP_OK,
              Os::FileSystem::getFileSize("test_file_max0",tmp));

      printf("Testing file name larger than 80 char\n");

      // Setup filename larger than 80 char:
      char longFileName[81];
      for (U32 i = 0; i < 80; ++i) {
          longFileName[i] = 'a';
      }
      longFileName[80] = 0;

      stat = this->component.set_log_file(longFileName,50);

      // Verify file not made:
      ASSERT_FALSE(stat);
      ASSERT_FALSE(this->component.m_log_file.m_openFile);
      ASSERT_NE(Os::FileSystem::OP_OK,
               Os::FileSystem::getFileSize(longFileName,tmp));

      printf("Testing file name larger than 79 char and file already exists\n");
      char longFileNameDup[80];
      for (U32 i = 0; i < 79; ++i) {
          longFileNameDup[i] = 'a';
      }
      longFileNameDup[79] = 0;

      stat = this->component.set_log_file(longFileNameDup,50);

      // Verify file made successful:
      ASSERT_TRUE(stat);
      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(Os::FileSystem::OP_OK,
                Os::FileSystem::getFileSize(longFileNameDup,tmp));

      // Try to make the same one again:
      stat = this->component.set_log_file(longFileNameDup,50);

      // Verify file not made:
      ASSERT_FALSE(stat);
      ASSERT_FALSE(this->component.m_log_file.m_openFile);
      char longFileNameDupS[81];
      strcat(longFileNameDupS,"0");
      ASSERT_NE(Os::FileSystem::OP_OK,
              Os::FileSystem::getFileSize(longFileNameDupS,tmp));

      printf("Testing with file name that already exists and the next 10 suffixes\n");

      // Create 11 files with the same name, and verify 11th fails, and re-uses the original
      const char* baseName = "test_mult_dup";

      // Create first file:
      stat = this->component.set_log_file(baseName,50);

      ASSERT_TRUE(stat);
      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      ASSERT_EQ(0,strcmp(baseName,this->component.m_log_file.m_fileName.toChar()));
      ASSERT_EQ(Os::FileSystem::OP_OK,
                Os::FileSystem::getFileSize(baseName,tmp));

      // Create 10 more, which all should succeed:
      char baseNameWithSuffix[128];
      U32 i;
      for (i = 0; i < 10; ++i) {

          //printf("<< %i\n",i);

          stat = this->component.set_log_file(baseName,50);

          sprintf(baseNameWithSuffix,"%s%d",baseName,i);
          ASSERT_TRUE(stat);
          ASSERT_TRUE(this->component.m_log_file.m_openFile);
          ASSERT_EQ(0,strcmp(baseNameWithSuffix,this->component.m_log_file.m_fileName.toChar()));
          ASSERT_EQ(Os::FileSystem::OP_OK,
                    Os::FileSystem::getFileSize(baseNameWithSuffix,tmp));
      }

      // Create 11th which will fail and re-use the original:
      stat = this->component.set_log_file(baseName,50);

      sprintf(baseNameWithSuffix,"%s%d",baseName,i);
      ASSERT_TRUE(stat);
      ASSERT_TRUE(this->component.m_log_file.m_openFile);
      printf("<< %s %s\n",baseName,this->component.m_log_file.m_fileName.toChar());
      ASSERT_EQ(0,strcmp(baseName,this->component.m_log_file.m_fileName.toChar()));
      ASSERT_EQ(Os::FileSystem::OP_OK,
                Os::FileSystem::getFileSize(baseName,tmp));

      // Clean up:
      remove("test_file_max");
      remove("test_file_max0");
      remove(longFileNameDup);
      remove(baseName);
      for (i = 0; i < 10; ++i) {
          sprintf(baseNameWithSuffix,"%s%d",baseName,i);
          remove(baseNameWithSuffix);
      }

  }

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
  {

    // TextLogger
    this->connect_to_TextLogger(
        0,
        this->component.get_TextLogger_InputPort(0)
    );




  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace Svc
