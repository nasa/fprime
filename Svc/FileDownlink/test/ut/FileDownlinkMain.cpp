// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "FileDownlinkTester.hpp"

TEST(FileDownlink, Downlink) {
  Svc::FileDownlinkTester tester;
  tester.downlink();
}

TEST(FileDownlink, FileOpenError) {
  Svc::FileDownlinkTester tester;
  tester.fileOpenError();
}

TEST(FileDownlink, CancelDownlink) {
  Svc::FileDownlinkTester tester;
  tester.cancelDownlink();
}

TEST(FileDownlink, CancelInIdleMode) {
  Svc::FileDownlinkTester tester;
  tester.cancelInIdleMode();
}

TEST(FileDownlink, DownlinkPartial) {
  Svc::FileDownlinkTester tester;
  tester.downlinkPartial();
}

TEST(FileDownlink, DownlinkTimeout) {
    Svc::FileDownlinkTester tester;
    tester.timeout();
}

TEST(FileDownlink, SendFilePort) {
    Svc::FileDownlinkTester tester;
    tester.sendFilePort();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
