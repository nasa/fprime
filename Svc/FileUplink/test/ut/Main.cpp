// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(FileUplink, SendFile) {
  Svc::Tester tester;
  tester.sendFile();
}

TEST(FileUplink, BadChecksum) {
  Svc::Tester tester;
  tester.badChecksum();
}

TEST(FileUplink, FileOpenError) {
  Svc::Tester tester;
  tester.fileOpenError();
}

TEST(FileUplink, FileWriteError) {
  Svc::Tester tester;
  tester.fileWriteError();
}

TEST(FileUplink, StartPacketInDataMode) {
  Svc::Tester tester;
  tester.startPacketInDataMode();
}

TEST(FileUplink, DataPacketInStartMode) {
  Svc::Tester tester;
  tester.dataPacketInStartMode();
}

TEST(FileUplink, EndPacketInStartMode) {
  Svc::Tester tester;
  tester.endPacketInStartMode();
}

TEST(FileUplink, PacketOutOfBounds) {
  Svc::Tester tester;
  tester.packetOutOfBounds();
}

TEST(FileUplink, PacketOutOfOrder) {
  Svc::Tester tester;
  tester.packetOutOfOrder();
}

TEST(FileUplink, CancelPacketInStartMode) {
  Svc::Tester tester;
  tester.cancelPacketInStartMode();
}

TEST(FileUplink, CancelPacketInDataMode) {
  Svc::Tester tester;
  tester.cancelPacketInDataMode();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
