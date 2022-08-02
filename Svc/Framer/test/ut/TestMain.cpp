// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Fw/Test/UnitTest.hpp"
#include "Os/Log.hpp"
#include "Tester.hpp"

// Enable the console logging provided by Os::Log
Os::Log logger;

TEST(Nominal, Com) {
    COMMENT("Send one Fw::Com buffer to the framer (nominal behavior)");
    REQUIREMENT("SVC-FRAMER-001");
    REQUIREMENT("SVC-FRAMER-003");
    Svc::Tester tester;
    tester.test_com();
}

TEST(Nominal, Buffer) {
    COMMENT("Send one Fw::Buffer to the framer (nominal behavior)");
    REQUIREMENT("SVC-FRAMER-002");
    REQUIREMENT("SVC-FRAMER-003");
    Svc::Tester tester;
    tester.test_buffer();
}

TEST(Nominal, ManySends) {
    COMMENT("Send several buffers");
    REQUIREMENT("SVC-FRAMER-001");
    REQUIREMENT("SVC-FRAMER-002");
    REQUIREMENT("SVC-FRAMER-003");
    Svc::Tester tester;
    tester.test_com(27);
    tester.test_buffer(27);
    tester.test_com(31);
    tester.test_buffer(31);
}

TEST(SendError, Buffer) {
    COMMENT("Send one Fw::Buffer to the framer (send error)");
    REQUIREMENT("SVC-FRAMER-002");
    REQUIREMENT("SVC-FRAMER-003");
    Svc::Tester tester;
    tester.setSendStatus(Drv::SendStatus::SEND_ERROR);
    tester.test_buffer();
}

TEST(SendError, Com) {
    COMMENT("Send one Fw::Com buffer to the framer (send error)");
    REQUIREMENT("SVC-FRAMER-001");
    REQUIREMENT("SVC-FRAMER-003");
    Svc::Tester tester;
    tester.setSendStatus(Drv::SendStatus::SEND_ERROR);
    tester.test_com();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
