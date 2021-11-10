// ======================================================================
// \title  SystemResourcesComponentImpl.cpp
// \author parallels
// \brief  cpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Svc/SystemResources/SystemResourcesComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <version.hpp>
#include <math.h> //isnan()

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SystemResourcesComponentImpl ::
    SystemResourcesComponentImpl(
        const char *const compName
    ) : SystemResourcesComponentBase(compName),
        m_cpu_count(0),
        m_enable(true)
  {

      if(Os::SystemResources::getCpuCount(m_cpu_count) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {
          m_cpu_count = 0;
      }

      m_cpu_count = (m_cpu_count >= CPU_COUNT) ? CPU_COUNT: m_cpu_count;

      for(U32 i = 0; i < CPU_COUNT; i++) {

          m_cpu_prev[i].cpuUsed = 0;
          m_cpu_prev[i].cpuTotal = 0;
      }

  }

  void SystemResourcesComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    SystemResourcesComponentBase::init(instance);

    m_cpu_tlm_functions[0] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_00;
    m_cpu_tlm_functions[1] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_01;
    m_cpu_tlm_functions[2] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_02;
    m_cpu_tlm_functions[3] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_03;
    m_cpu_tlm_functions[4] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_04;
    m_cpu_tlm_functions[5] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_05;
    m_cpu_tlm_functions[6] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_06;
    m_cpu_tlm_functions[7] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_07;
    m_cpu_tlm_functions[8] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_08;
    m_cpu_tlm_functions[9] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_09;
    m_cpu_tlm_functions[10] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_10;
    m_cpu_tlm_functions[11] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_11;
    m_cpu_tlm_functions[12] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_12;
    m_cpu_tlm_functions[13] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_13;
    m_cpu_tlm_functions[14] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_14;
    m_cpu_tlm_functions[15] = &Svc::SystemResourcesComponentImpl::tlmWrite_CPU_15;

  }

  SystemResourcesComponentImpl ::
    ~SystemResourcesComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SystemResourcesComponentImpl ::
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE tick_time_hz
    )
  {

    if(m_enable)
    {
        Cpu();
        Mem();
        PhysMem();
        Version();
    }

  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void SystemResourcesComponentImpl ::
    SYS_RES_ENABLE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        SystemResourceEnabled enable
    )
  {

    m_enable = (enable == SYS_RES_ENABLED);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void SystemResourcesComponentImpl ::
    VERSION_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    Fw::LogStringArg version_string(VERSION);

    this->log_ACTIVITY_LO_SYS_RES_VERSION(version_string);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  F32 SystemResourcesComponentImpl::compCpuUtil(F32 used, F32 usedPrev, F32 total, F32 totalPrev)
  {
      F32 util;

      // Compute CPU % Utilization
      util = ((used - usedPrev) / (total - totalPrev)) * 100;

      // Validate.  If sampled too fast Total-TotalPrev will be set to 0
      if((total - totalPrev) == 0 || isnan(util)) {

          util = 0;
      }

      return util;

  }

  I32 SystemResourcesComponentImpl::Cpu()
  {
      Os::SystemResources::SystemResourcesStatus status; 
      U32 count = 0;
      F32 cpuAvg = 0;
      Fw::Time t = Fw::Time();


      for(U32 i = 0; i < m_cpu_count && i < CPU_COUNT; i++) {

            status = Os::SystemResources::getCpuUtil(m_cpu[i], false, i);

            if(status == Os::SystemResources::SYSTEM_RESOURCES_OK) {
                F32 cpuUtil;


                cpuUtil = compCpuUtil(m_cpu[i].cpuUsed, m_cpu_prev[i].cpuUsed, m_cpu[i].cpuTotal, m_cpu_prev[i].cpuTotal);

                cpuAvg += cpuUtil;

                // Send telemetry
                FW_ASSERT(this->m_cpu_tlm_functions[i]); // make sure fn pointer is set
                (this->*m_cpu_tlm_functions[i])(cpuUtil, t); 

                // Store cpu used and total 
                m_cpu_prev[i] = m_cpu[i];

                count++;
            }
            else {
       
                return -1;
            }
      }

      cpuAvg /= count;
      this->tlmWrite_CPU(cpuAvg);;
      m_cpu_avg = cpuAvg;

      return 0;
  }

  I32 SystemResourcesComponentImpl::Mem()
  {
      Os::SystemResources::SystemResourcesStatus status; 

      if((status = Os::SystemResources::getMemUtil(m_mem)) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {

          return -1;
      }

      this->tlmWrite_MEM_TOTAL(m_mem.memTotal);
      this->tlmWrite_MEM_UTIL(m_mem.memUsed);

      return 0;
  }

  I32 SystemResourcesComponentImpl::PhysMem()
  {
      Os::SystemResources::SystemResourcesStatus status; 

      if((status = Os::SystemResources::getPhysMemUtil(m_physMem)) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {

          return -1;
      }

      this->tlmWrite_PHYS_MEM_TOTAL(m_physMem.physMemTotal);
      this->tlmWrite_PHYS_MEM_UTIL(m_physMem.physMemUsed);

      return 0;
  }

  void SystemResourcesComponentImpl::Version()
  {
      Fw::TlmString version_string(VERSION);

      this->tlmWrite_VERSION(version_string);
  }
} // end namespace Svc
