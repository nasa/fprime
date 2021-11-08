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

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SystemResourcesComponentImpl ::
    SystemResourcesComponentImpl(
        const char *const compName
    ) : SystemResourcesComponentBase(compName),
	    m_tick_count(0),
	    m_sample_rate(1),
        m_cpu_count(0)
  {

      if(Os::SystemResources::getCpuCount(m_cpu_count) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {
          m_cpu_count = 0;
      }

      m_cpu_count = (m_cpu_count >= CPU_COUNT) ? CPU_COUNT - 1: m_cpu_count;
     
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
    FW_ASSERT(tick_time_hz != 0);

    if((m_tick_count % (tick_time_hz / m_sample_rate)) == 0)
    {

        Cpu();
        Mem();
        PhysMem();
    }

    m_tick_count++;
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
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void SystemResourcesComponentImpl ::
    VERSION_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


  I32 SystemResourcesComponentImpl::Cpu()
  {
      Os::SystemResources::SystemResourcesStatus status; 
      Os::SystemResources::cpuUtil cpu; 
      U32 count = 0;
      F32 cpuAvg = 0;
      Fw::Time t = Fw::Time();

      if((status = Os::SystemResources::getCpuUtil(cpu, true)) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {

          return -1;
      }


      for(U32 i = 0; i < m_cpu_count && i < CPU_COUNT; i++) {

            status = Os::SystemResources::getCpuUtil(cpu, false, i);

            if(status == Os::SystemResources::SYSTEM_RESOURCES_OK) {

                cpuAvg += cpu.cpuUsed;
        //FW_ASSERT(this->m_ise_tlm_fns[i]); // make sure fn pointer is set
        //(this->*m_ise_tlm_fns[i])(ise_mv[i], t);
                FW_ASSERT(this->m_cpu_tlm_functions[i]); // make sure fn pointer is set
                (this->*m_cpu_tlm_functions[i])(cpu.cpuUsed, t); 
                count++;
            }
      }

      cpuAvg /= count;
      this->tlmWrite_CPU(cpuAvg);;

      return 0;
  }

  I32 SystemResourcesComponentImpl::Mem()
  {
      Os::SystemResources::SystemResourcesStatus status; 
      Os::SystemResources::memUtil mem; 

      if((status = Os::SystemResources::getMemUtil(mem)) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {

          return -1;
      }

      this->tlmWrite_MEM_TOTAL(mem.memTotal);
      this->tlmWrite_MEM_UTIL(mem.memUsed);

      return 0;
  }

  I32 SystemResourcesComponentImpl::PhysMem()
  {
      Os::SystemResources::SystemResourcesStatus status; 
      Os::SystemResources::physMemUtil physMem; 

      if((status = Os::SystemResources::getPhysMemUtil(physMem)) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {

          return -1;
      }

      this->tlmWrite_PHYS_MEM_TOTAL(physMem.physMemTotal);
      this->tlmWrite_PHYS_MEM_UTIL(physMem.physMemUsed);

      return 0;
  }

} // end namespace Svc
