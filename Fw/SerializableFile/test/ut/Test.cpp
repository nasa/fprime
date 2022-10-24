// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include <cstring>
#include <cstdio>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Fw/SerializableFile/SerializableFile.hpp>
#include <Fw/SerializableFile/test/TestSerializable/TestSerializableAc.hpp>

using namespace Fw;

int main(int argc, char **argv) {

  // Create local serializable:
  U32 element1 = 4294967284U; 
  I8 element2 = -18;
  F64 element3 = 3.14159;
  U32 size = Test::SERIALIZED_SIZE;
  Test config(element1, element2, element3);

  // Create the serializable file:
  MallocAllocator theMallocator;
  SerializableFile configFile(&theMallocator, size);
  SerializableFile::Status status;

  // Save the serializable to a file:
  printf("Testing save... ");
  status = configFile.save("test.ser", config);
  FW_ASSERT(SerializableFile::OP_OK == status, status);
  printf("Passed\n");

  // Load the serializable from a file:
  printf("Testing load... ");
  Test config2;
  status = configFile.load("test.ser", config2);
  FW_ASSERT(SerializableFile::OP_OK == status, status);
  printf("Passed\n");

  // Compare the results:
  printf("Testing compare... ");
  FW_ASSERT(config == config2);
  printf("Passed\n");

  // Test saving to impossible file:
  printf("Testing bad save... ");
  status = configFile.save("this/file/does/not/exist", config);
  FW_ASSERT(SerializableFile::FILE_OPEN_ERROR == status, status);
  printf("Passed\n");

  // Test reading from nonexistent file:
  printf("Testing bad load... ");
  Test config3;
  status = configFile.load("thisfiledoesnotexist.ser", config3);
  FW_ASSERT(SerializableFile::FILE_OPEN_ERROR == status, status);
  printf("Passed\n");

  return 0;
}
