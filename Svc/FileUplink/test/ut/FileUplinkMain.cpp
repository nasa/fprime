// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "FileUplinkTester.hpp"

TEST(FileUplink, SendFile) {
  Svc::FileUplinkTester tester;
  tester.sendFile();
}

TEST(FileUplink, BadChecksum) {
  Svc::FileUplinkTester tester;
  tester.badChecksum();
}

TEST(FileUplink, FileOpenError) {
  Svc::FileUplinkTester tester;
  tester.fileOpenError();
}

TEST(FileUplink, FileWriteError) {
  Svc::FileUplinkTester tester;
  tester.fileWriteError();
}

TEST(FileUplink, StartPacketInDataMode) {
  Svc::FileUplinkTester tester;
  tester.startPacketInDataMode();
}

TEST(FileUplink, DataPacketInStartMode) {
  Svc::FileUplinkTester tester;
  tester.dataPacketInStartMode();
}

TEST(FileUplink, EndPacketInStartMode) {
  Svc::FileUplinkTester tester;
  tester.endPacketInStartMode();
}

TEST(FileUplink, PacketOutOfBounds) {
  Svc::FileUplinkTester tester;
  tester.packetOutOfBounds();
}

TEST(FileUplink, PacketOutOfOrder) {
  Svc::FileUplinkTester tester;
  tester.packetOutOfOrder();
}

TEST(FileUplink, CancelPacketInStartMode) {
  Svc::FileUplinkTester tester;
  tester.cancelPacketInStartMode();
}

TEST(FileUplink, CancelPacketInDataMode) {
  Svc::FileUplinkTester tester;
  tester.cancelPacketInDataMode();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
