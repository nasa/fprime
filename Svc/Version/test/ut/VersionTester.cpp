// ======================================================================
// \title  VersionTester.cpp
// \author sreddy
// \brief  cpp file for Version component test harness implementation class
// ======================================================================

#include "VersionTester.hpp"
#include "versions/version.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

VersionTester ::VersionTester()
    : VersionGTestBase("VersionTester", VersionTester::MAX_HISTORY_SIZE), component("Version") {
    this->initComponents();
    this->connectPorts();
}

VersionTester ::~VersionTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
// Clear/init test
void VersionTester ::clear_all() {
    this->clearHistory();
    ASSERT_EVENTS_FrameworkVersion_SIZE(0);
    ASSERT_EVENTS_ProjectVersion_SIZE(0);
    ASSERT_EVENTS_LibraryVersions_SIZE(0);
    ASSERT_EVENTS_CustomVersions_SIZE(0);
    ASSERT_TLM_FrameworkVersion_SIZE(0);
    ASSERT_TLM_ProjectVersion_SIZE(0);
    ASSERT_TLM_LibraryVersion01_SIZE(0);
    ASSERT_TLM_LibraryVersion02_SIZE(0);
    ASSERT_TLM_LibraryVersion03_SIZE(0);
    ASSERT_TLM_LibraryVersion04_SIZE(0);
    ASSERT_TLM_LibraryVersion05_SIZE(0);
    ASSERT_TLM_LibraryVersion06_SIZE(0);
    ASSERT_TLM_LibraryVersion07_SIZE(0);
    ASSERT_TLM_LibraryVersion08_SIZE(0);
    ASSERT_TLM_LibraryVersion09_SIZE(0);
    ASSERT_TLM_LibraryVersion10_SIZE(0);
    ASSERT_TLM_CustomVersion01_SIZE(0);
    ASSERT_TLM_CustomVersion02_SIZE(0);
    ASSERT_TLM_CustomVersion03_SIZE(0);
    ASSERT_TLM_CustomVersion04_SIZE(0);
    ASSERT_TLM_CustomVersion05_SIZE(0);
    ASSERT_TLM_CustomVersion06_SIZE(0);
    ASSERT_TLM_CustomVersion07_SIZE(0);
    ASSERT_TLM_CustomVersion08_SIZE(0);
    ASSERT_TLM_CustomVersion09_SIZE(0);
    ASSERT_TLM_CustomVersion10_SIZE(0);
}
// ----------------------------------------------------------------------
// Test STARTUP
// ----------------------------------------------------------------------
void VersionTester ::test_startup() {
    // this->invoke_to_run(0,0); //No longer running version on a scheduler but invoked after startTasks() in
    // RefTopology.cpp ASSERT_EVENTS_STARTUP_EVR_SIZE(1);
    // ASSERT_EVENTS_STARTUP_EVR(0,Project::Version::FRAMEWORK_VERSION,Project::Version::PROJECT_VERSION);
    this->component.config(true);
    /*
    ASSERT_TLM_FrameworkVersion(0, Project::Version::FRAMEWORK_VERSION);
    ASSERT_TLM_ProjectVersion(0, Project::Version::PROJECT_VERSION);
    ASSERT_EVENTS_FrameworkVersion_SIZE(1);
    ASSERT_EVENTS_FrameworkVersion(0, Project::Version::FRAMEWORK_VERSION);
    ASSERT_EVENTS_ProjectVersion_SIZE(1);
    ASSERT_EVENTS_ProjectVersion(0, Project::Version::FRAMEWORK_VERSION);
    // Library versions currently set to a null pointer
    // TODO: Need to figure out how to put in artificial sets to test them
    ASSERT_EVENTS_LibraryVersions_SIZE(12);
    ASSERT_EVENTS_LibraryVersions(0, "blah0 @ blah0");
    */
}

// ----------------------------------------------------------------------
// Test Commands
// ----------------------------------------------------------------------

void VersionTester ::test_enable() {
    U32 cmd_seq = 9;
    this->sendCmd_ENABLE(0, cmd_seq, VersionEnabled::DISABLED);
    ASSERT_CMD_RESPONSE(0, 0, 9, Fw::CmdResponse::OK);
    VersionTester::test_setVer(false);
    //When verbosity is disabled, events are still generated but not EHAs
    ASSERT_EVENTS_CustomVersions_SIZE(10);
    ASSERT_TLM_CustomVersion01_SIZE(0);
    ASSERT_TLM_CustomVersion02_SIZE(0);
    ASSERT_TLM_CustomVersion03_SIZE(0);
    ASSERT_TLM_CustomVersion04_SIZE(0);
    ASSERT_TLM_CustomVersion05_SIZE(0);
    ASSERT_TLM_CustomVersion06_SIZE(0);
    ASSERT_TLM_CustomVersion07_SIZE(0);
    ASSERT_TLM_CustomVersion08_SIZE(0);
    ASSERT_TLM_CustomVersion09_SIZE(0);
    ASSERT_TLM_CustomVersion10_SIZE(0);

    cmd_seq = 9;
    this->sendCmd_ENABLE(0, cmd_seq, VersionEnabled::ENABLED);
    ASSERT_CMD_RESPONSE(0, 0, 9, Fw::CmdResponse::OK);
    VersionTester::test_setVer(true);
    //When verbosity is enabled, events and EHAs are generated
    ASSERT_EVENTS_CustomVersions_SIZE(10);
    ASSERT_TLM_CustomVersion01_SIZE(1);
    ASSERT_TLM_CustomVersion02_SIZE(1);
    ASSERT_TLM_CustomVersion03_SIZE(1);
    ASSERT_TLM_CustomVersion04_SIZE(1);
    ASSERT_TLM_CustomVersion05_SIZE(1);
    ASSERT_TLM_CustomVersion06_SIZE(1);
    ASSERT_TLM_CustomVersion07_SIZE(1);
    ASSERT_TLM_CustomVersion08_SIZE(1);
    ASSERT_TLM_CustomVersion09_SIZE(1);
    ASSERT_TLM_CustomVersion10_SIZE(1);
}

void VersionTester ::test_versions() {
    U32 cmd_seq = 9;
    this->clear_all();
    this->sendCmd_VERSION(0, cmd_seq, Svc::VersionType::FRAMEWORK);
    ASSERT_CMD_RESPONSE(0, 1, 9, Fw::CmdResponse::OK);
    ASSERT_TLM_FrameworkVersion_SIZE(1);
    ASSERT_TLM_FrameworkVersion(0, Project::Version::FRAMEWORK_VERSION);
    ASSERT_EVENTS_FrameworkVersion_SIZE(1);
    ASSERT_EVENTS_FrameworkVersion(0, Project::Version::FRAMEWORK_VERSION);

    this->clear_all();
    this->sendCmd_VERSION(0, cmd_seq, Svc::VersionType::PROJECT);
    ASSERT_CMD_RESPONSE(0, 1, 9, Fw::CmdResponse::OK);
    ASSERT_TLM_ProjectVersion_SIZE(1);
    ASSERT_TLM_ProjectVersion(0, Project::Version::FRAMEWORK_VERSION);
    ASSERT_EVENTS_ProjectVersion_SIZE(1);
    ASSERT_EVENTS_ProjectVersion(0, Project::Version::FRAMEWORK_VERSION);

    this->clear_all();
    this->sendCmd_VERSION(0, cmd_seq, Svc::VersionType::LIBRARY);
    ASSERT_CMD_RESPONSE(0, 1, 9, Fw::CmdResponse::OK);
    // printf ("\nfirst lib element : %s\n\n", Project::Version::LIBRARY_VERSIONS[0]);
    ASSERT_EVENTS_LibraryVersions_SIZE(12);

    ASSERT_TLM_LibraryVersion01_SIZE(1);
    ASSERT_TLM_LibraryVersion01(0, "blah0 @ blah0");
    ASSERT_EVENTS_LibraryVersions(0, "blah0 @ blah0");

    ASSERT_TLM_LibraryVersion02_SIZE(1);
    ASSERT_TLM_LibraryVersion02(0, "blah1 @ blah1");
    ASSERT_EVENTS_LibraryVersions(1, "blah1 @ blah1");

    ASSERT_TLM_LibraryVersion03_SIZE(1);
    ASSERT_TLM_LibraryVersion03(0, "blah2 @ blah2");
    ASSERT_EVENTS_LibraryVersions(2, "blah2 @ blah2");

    ASSERT_TLM_LibraryVersion04_SIZE(1);
    ASSERT_TLM_LibraryVersion04(0, "blah3 @ blah3");
    ASSERT_EVENTS_LibraryVersions(3, "blah3 @ blah3");

    ASSERT_TLM_LibraryVersion05_SIZE(1);
    ASSERT_TLM_LibraryVersion05(0, "blah4 @ blah4");
    ASSERT_EVENTS_LibraryVersions(4, "blah4 @ blah4");

    ASSERT_TLM_LibraryVersion06_SIZE(1);
    ASSERT_TLM_LibraryVersion06(0, "blah5 @ blah5");
    ASSERT_EVENTS_LibraryVersions(5, "blah5 @ blah5");

    ASSERT_TLM_LibraryVersion07_SIZE(1);
    ASSERT_TLM_LibraryVersion07(0, "blah6 @ blah6");
    ASSERT_EVENTS_LibraryVersions(6, "blah6 @ blah6");

    ASSERT_TLM_LibraryVersion08_SIZE(1);
    ASSERT_TLM_LibraryVersion08(0, "blah7 @ blah7");
    ASSERT_EVENTS_LibraryVersions(7, "blah7 @ blah7");

    ASSERT_TLM_LibraryVersion09_SIZE(1);
    ASSERT_TLM_LibraryVersion09(0, "blah8 @ blah8");
    ASSERT_EVENTS_LibraryVersions(8, "blah8 @ blah8");

    ASSERT_TLM_LibraryVersion10_SIZE(1);
    ASSERT_TLM_LibraryVersion10(0, "blah9 @ blah9");
    ASSERT_EVENTS_LibraryVersions(9, "blah9 @ blah9");

    ASSERT_EVENTS_LibraryVersions(10, "blah10 @ blah10");

    ASSERT_EVENTS_LibraryVersions(11, "blah11 @ blah11");

    this->clear_all();
    Svc::CustomVersionDb custom_data_struct;
    this->test_setVer(false);
    this->clear_all();
    this->sendCmd_VERSION(0, cmd_seq, Svc::VersionType::CUSTOM);
    ASSERT_CMD_RESPONSE(0, 1, 9, Fw::CmdResponse::OK);

    ASSERT_EVENTS_CustomVersions_SIZE(10);

    ASSERT_EVENTS_CustomVersions(2, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_02, "ver_2");
    custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_02, Fw::String("ver_2"), Svc::VersionStatus::OK);
    ASSERT_TLM_CustomVersion03(0, custom_data_struct);
    ASSERT_TLM_CustomVersion03_SIZE(1);

    ASSERT_EVENTS_CustomVersions(3, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, "ver_3");
    custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, Fw::String("ver_3"), Svc::VersionStatus::FAILURE);
    ASSERT_TLM_CustomVersion04(0, custom_data_struct);
    ASSERT_TLM_CustomVersion04_SIZE(1);

    ASSERT_EVENTS_CustomVersions(6, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, "ver_6");
    custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, Fw::String("ver_6"), Svc::VersionStatus::FAILURE);
    ASSERT_TLM_CustomVersion07(0, custom_data_struct);
    ASSERT_TLM_CustomVersion07_SIZE(1);

    ASSERT_EVENTS_CustomVersions(9, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, "ver_9");
    custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, Fw::String("ver_9"), Svc::VersionStatus::OK);
    ASSERT_TLM_CustomVersion10(0, custom_data_struct);
    ASSERT_TLM_CustomVersion10_SIZE(1);

    this->clear_all();
    this->sendCmd_VERSION(0, cmd_seq, Svc::VersionType::ALL);
    ASSERT_CMD_RESPONSE(0, 1, 9, Fw::CmdResponse::OK);
    ASSERT_TLM_FrameworkVersion(0, Project::Version::FRAMEWORK_VERSION);
    ASSERT_TLM_ProjectVersion(0, Project::Version::PROJECT_VERSION);
    ASSERT_EVENTS_FrameworkVersion_SIZE(1);
    ASSERT_EVENTS_FrameworkVersion(0, Project::Version::FRAMEWORK_VERSION);
    ASSERT_EVENTS_ProjectVersion_SIZE(1);
    ASSERT_EVENTS_ProjectVersion(0, Project::Version::FRAMEWORK_VERSION);
    ASSERT_EVENTS_CustomVersions_SIZE(10);
    ASSERT_EVENTS_LibraryVersions_SIZE(12);
}

void VersionTester ::test_commands() {
    this->test_enable();
    this->test_versions();
}

// ----------------------------------------------------------------------
// Test User Ports
// ----------------------------------------------------------------------

void VersionTester ::test_setVer(bool is_enabled) {
    Svc::VersionStatus status = Svc::VersionStatus::OK;
    Fw::String set_ver = "ver_2";

    // Create a db to compare against set values
    Svc::CustomVersionDb custom_data_struct;
    // printf("\nTesting the very first port invocation\n");

    // Start Clean
    this->clear_all();

    // this->sendCmd_ENABLE(0,9,VersionEnabled::ENABLED);
    set_ver = "ver_0";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_00, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(1);
        ASSERT_EVENTS_CustomVersions(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_00, "ver_0");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_00, set_ver, Svc::VersionStatus::OK);
        ASSERT_TLM_CustomVersion01(0, custom_data_struct);
    }

    set_ver = "ver_1";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_01, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(2);
        ASSERT_EVENTS_CustomVersions(1, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_01, "ver_1");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_01, set_ver, Svc::VersionStatus::OK);
        ASSERT_TLM_CustomVersion02(0, custom_data_struct);
    }

    set_ver = "ver_2";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_02, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(3);
        ASSERT_EVENTS_CustomVersions(2, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_02, "ver_2");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_02, set_ver, Svc::VersionStatus::OK);
        ASSERT_TLM_CustomVersion03(0, custom_data_struct);
    }

    status = Svc::VersionStatus::FAILURE;

    set_ver = "ver_3";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(4);
        ASSERT_EVENTS_CustomVersions(3, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, "ver_3");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, set_ver, Svc::VersionStatus::FAILURE);
        ASSERT_TLM_CustomVersion04(0, custom_data_struct);
    }

    set_ver = "ver_4";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_04, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(5);
        ASSERT_EVENTS_CustomVersions(4, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_04, "ver_4");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_04, set_ver, Svc::VersionStatus::FAILURE);
        ASSERT_TLM_CustomVersion05(0, custom_data_struct);
    }

    set_ver = "ver_5";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_05, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(6);
        ASSERT_EVENTS_CustomVersions(5, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_05, "ver_5");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_05, set_ver, Svc::VersionStatus::FAILURE);
        ASSERT_TLM_CustomVersion06(0, custom_data_struct);
    }

    set_ver = "ver_6";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(7);
        ASSERT_EVENTS_CustomVersions(6, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, "ver_6");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, set_ver, Svc::VersionStatus::FAILURE);
        ASSERT_TLM_CustomVersion07(0, custom_data_struct);
    }

    status = Svc::VersionStatus::OK;

    set_ver = "ver_7";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_07, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(8);
        ASSERT_EVENTS_CustomVersions(7, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_07, "ver_7");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_07, set_ver, Svc::VersionStatus::OK);
        ASSERT_TLM_CustomVersion08(0, custom_data_struct);
    }

    set_ver = "ver_8";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_08, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(9);
        ASSERT_EVENTS_CustomVersions(8, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_08, "ver_8");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_08, set_ver, Svc::VersionStatus::OK);
        ASSERT_TLM_CustomVersion09(0, custom_data_struct);
    }

    set_ver = "ver_9";
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, set_ver, status);
    if (is_enabled == true) {
        ASSERT_EVENTS_CustomVersions_SIZE(10);
        ASSERT_EVENTS_CustomVersions(9, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, "ver_9");
        custom_data_struct.set(Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, set_ver, Svc::VersionStatus::OK);
        ASSERT_TLM_CustomVersion10(0, custom_data_struct);
    }
}

void VersionTester ::test_getVer() {
    Svc::VersionStatus status;
    Fw::String get_ver;

    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_02, get_ver, status);
    ASSERT_EQ(get_ver, "ver_2");
    ASSERT_EQ(status, Svc::VersionStatus::OK);

    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, get_ver, status);
    ASSERT_EQ(get_ver, "ver_3");
    ASSERT_EQ(status, Svc::VersionStatus::FAILURE);

    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, get_ver, status);
    ASSERT_EQ(get_ver, "ver_6");
    ASSERT_EQ(status, Svc::VersionStatus::FAILURE);

    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, get_ver, status);
    ASSERT_EQ(get_ver, "ver_9");
    ASSERT_EQ(status, Svc::VersionStatus::OK);

    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_05, get_ver, status);
    ASSERT_EQ(get_ver, "ver_5");
    ASSERT_EQ(status, Svc::VersionStatus::FAILURE);
}

void VersionTester::test_ports() {
    this->sendCmd_ENABLE(0, 9, VersionEnabled::ENABLED);
    this->test_setVer(true);
    this->test_getVer();
}
}  // namespace Svc
