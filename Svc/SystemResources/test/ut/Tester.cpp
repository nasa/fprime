// ====================================================================== 
// \title  SystemResources.hpp
// \author parallels
// \brief  cpp file for SystemResources test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) : 
#if FW_OBJECT_NAMES == 1
      SystemResourcesGTestBase("Tester", MAX_HISTORY_SIZE),
      component("SystemResources")
#else
      SystemResourcesGTestBase(MAX_HISTORY_SIZE),
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
    testMemRead(void) 
  {
    I32 ret;

    ret = this->component.Mem();

    ASSERT_EQ(ret, 0);
    ASSERT_NE(this->component.m_mem.memUsed, 0);
    ASSERT_NE(this->component.m_mem.memTotal, 0);

    fprintf(stderr, "Mem.Used=[%f], Mem.Total=[%f]\n", this->component.m_mem.memUsed, this->component.m_mem.memTotal);
  }

  void Tester ::
    testPhysMemRead(void) 
  {
    I32 ret;

    ret = this->component.PhysMem();

    ASSERT_EQ(ret, 0);
    ASSERT_NE(this->component.m_physMem.physMemUsed, 0);
    ASSERT_NE(this->component.m_physMem.physMemTotal, 0);

    fprintf(stderr, "PhysMem.Used=[%f], PhysMem.Total=[%f]\n", this->component.m_physMem.physMemUsed, this->component.m_physMem.physMemTotal);
  }

  void Tester ::
    testCpuUtilRead(void) 
  {
    I32 ret;

    for(U32 num = 0; num < 5; num++) {
        ret = this->component.Cpu();
    
        ASSERT_EQ(ret, 0);
    
        for(U32 i = 0; i < this->component.m_cpu_count; i++) {
    
            fprintf(stderr, "CPU[%d]: Used=[%f], Total=[%f], Util=[%f], Avg=[%f]\n", i, this->component.m_cpu[i].cpuUsed, this->component.m_cpu[i].cpuTotal, this->component.m_cpu_util, this->component.m_cpu_avg);
    
        }
    
        usleep(2000000);
    }

  }

  void Tester ::
    testSysResEnableCmd(void) 
  {
      SystemResourcesComponentImpl::SystemResourceEnabled enable;

      this->clearHistory();

      // Disable telemetry generation
      enable = SystemResourcesComponentImpl::SYS_RES_DISABLED;
      component.SYS_RES_ENABLE_cmdHandler(0, 0, enable);

      ASSERT_EQ(component.m_enable, false);

      this->invoke_to_run(0, 100);

      // Enable telemetry generation

      enable = SystemResourcesComponentImpl::SYS_RES_ENABLED;
      component.SYS_RES_ENABLE_cmdHandler(0, 0, enable);

      ASSERT_EQ(component.m_enable, true);

      this->invoke_to_run(0, 100);

  }

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
  {

    // run
    this->connect_to_run(
        0,
        this->component.get_run_InputPort(0)
    );

    // CmdDisp
    this->connect_to_CmdDisp(
        0,
        this->component.get_CmdDisp_InputPort(0)
    );

    // CmdStatus
    this->component.set_CmdStatus_OutputPort(
        0, 
        this->get_from_CmdStatus(0)
    );

    // CmdReg
    this->component.set_CmdReg_OutputPort(
        0, 
        this->get_from_CmdReg(0)
    );

    // Tlm
    this->component.set_Tlm_OutputPort(
        0, 
        this->get_from_Tlm(0)
    );

    // Time
    this->component.set_Time_OutputPort(
        0, 
        this->get_from_Time(0)
    );

    // Log
    this->component.set_Log_OutputPort(
        0, 
        this->get_from_Log(0)
    );

    // LogText
    this->component.set_LogText_OutputPort(
        0, 
        this->get_from_LogText(0)
    );




  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
