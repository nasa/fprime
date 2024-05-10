// ======================================================================
// \title  VersionTester.cpp
// \author sreddy
// \brief  cpp file for Version component test harness implementation class
// ======================================================================

#include "VersionTester.hpp"
#include <version.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  VersionTester ::
    VersionTester() :
      VersionGTestBase("VersionTester", VersionTester::MAX_HISTORY_SIZE),
      component("Version")
  {
    this->initComponents();
    this->connectPorts();
  }

  VersionTester ::
    ~VersionTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // Test STARTUP
    // ----------------------------------------------------------------------
    void VersionTester :: test_startup() {
        this->invoke_to_run(0,0);
        ASSERT_EVENTS_STARTUP_EVR_SIZE(1);
        ASSERT_EVENTS_STARTUP_EVR(0,FRAMEWORK_VERSION,PROJECT_VERSION);
    }

    // ----------------------------------------------------------------------
    // Test Commands
    // ----------------------------------------------------------------------
  void VersionTester ::
    test_enable()
  {
    U32 cmd_seq = 9;
    this->sendCmd_ENABLE(0,cmd_seq,VersionEnabled::DISABLED);
    this->invoke_to_run(0,0);
    ASSERT_CMD_RESPONSE (0, 0, 9, Fw::CmdResponse::OK);

    cmd_seq = 9;
    this->sendCmd_ENABLE(0,cmd_seq,VersionEnabled::ENABLED);
    this->invoke_to_run(0,0);
    ASSERT_CMD_RESPONSE (0, 0, 9, Fw::CmdResponse::OK);
    ASSERT_TLM_FRAMEWORK_VERSION(0, FRAMEWORK_VERSION);
    ASSERT_TLM_PROJECT_VERSION(0, PROJECT_VERSION);
  }
  
  void VersionTester ::
    test_versions()
  {
    U32 cmd_seq = 9;
    this->sendCmd_VERSION(0,cmd_seq);
    this->invoke_to_run(0,0);
    ASSERT_CMD_RESPONSE (0, 0, 9, Fw::CmdResponse::OK);
    ASSERT_EVENTS_FRAMEWORK_VERSION_SIZE(1);
    ASSERT_EVENTS_FRAMEWORK_VERSION(0, FRAMEWORK_VERSION);
  }

  void VersionTester :: test_commands() {
    this->test_enable();
    this->test_versions();
  }

    // ----------------------------------------------------------------------
    // Test User Ports
    // ----------------------------------------------------------------------
 
  void VersionTester :: test_setVer() {
    
    Svc::VersionStatus status = Svc::VersionStatus::OK;
    Svc::VersPortStrings::StringSize80 set_ver_0 = "ver_0";
    Svc::VersPortStrings::StringSize80 set_ver_6 = "ver_6";
    Svc::VersPortStrings::StringSize80 set_ver_3 = "ver_3";
    Svc::VersPortStrings::StringSize80 set_ver_9 = "ver_9";

    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_00, set_ver_0, status);
    status = Svc::VersionStatus::FAILURE;
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, set_ver_3, status);
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, set_ver_6, status);
    status = Svc::VersionStatus::OK;
    this->invoke_to_setVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, set_ver_9, status);
  }

  void VersionTester :: test_getVer() {
    
    Svc::VersionStatus status;
    Svc::VersPortStrings::StringSize80 get_ver;

    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_00, get_ver, status);
    ASSERT_EQ(get_ver,"ver_0");
    ASSERT_EQ(status,Svc::VersionStatus::OK);
    
    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_03, get_ver, status);
    ASSERT_EQ(get_ver,"ver_3");
    ASSERT_EQ(status,Svc::VersionStatus::FAILURE);
    
    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_06, get_ver, status);
    ASSERT_EQ(get_ver,"ver_6");
    ASSERT_EQ(status,Svc::VersionStatus::FAILURE);
    
    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_09, get_ver, status);
    ASSERT_EQ(get_ver,"ver_9");
    ASSERT_EQ(status,Svc::VersionStatus::OK);
    
    this->invoke_to_getVersion(0, Svc::VersionCfg::VersionEnum::PROJECT_VERSION_05, get_ver, status);
    ASSERT_EQ(get_ver,"no_ver");
    ASSERT_EQ(status,Svc::VersionStatus::FAILURE);
  }

  void VersionTester::test_ports() {
    this->test_setVer();
    this->test_getVer();
  }


// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------
  /*
  void VersionTester::connectPorts() {
    // Connect custom ports 
    this->connect_to_run(0, this->component.get_run_InputPort(0));
    this->connect_to_getVersion (0, this->component.get_getVersion_InputPort(0));
    this->connect_to_setVersion (0, this->component.get_setVersion_InputPort(0));

    // CmdIn
    this->connect_to_cmdIn (0, this->component.get_cmdIn_InputPort(0));
    
    // CmdReg
    this->component.set_cmdRegOut_OutputPort(0,this->get_from_cmdRegOut(0));

    // CmdStatus
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_cmdResponseOut(0));

    // Tlm
    this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

    // Time
    this->component.set_timeCaller_OutputPort(0, this->get_from_timeCaller(0));

    // Log
    this->component.set_logOut_OutputPort(0, this->get_from_logOut(0));

    // LogText
    this->component.set_logTextOut_OutputPort(0, this->get_from_logTextOut(0));
  }

  void VersionTester::initComponents() {
    this->init();
    this->component.init(0);
  }
  */
}