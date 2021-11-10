// ======================================================================
// \title  SystemResourcesComponentImpl.hpp
// \author Santos F. Fregoso
// \brief  hpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef SystemResources_HPP
#define SystemResources_HPP

#include "Svc/SystemResources/SystemResourcesComponentAc.hpp"

namespace Svc {

  class SystemResourcesComponentImpl :
    public SystemResourcesComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object SystemResources
      //!
      SystemResourcesComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object SystemResources
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object SystemResources
      //!
      ~SystemResourcesComponentImpl(void);

      typedef void (SystemResourcesComponentBase::*cpuTlmFunc)(F32, Fw::Time);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for run
      //!
      void run_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for SYS_RES_ENABLE command handler
      //! A command to enable or disable system resource telemetry
      void SYS_RES_ENABLE_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          SystemResourceEnabled enable /*!< whether or not system resource telemetry is enabled*/
      );

      //! Implementation for VERSION command handler
      //! Report version as EVR
      void VERSION_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    PRIVATE:

      I32 Cpu();
      I32 Mem();
      I32 PhysMem();
      void Version();
      void tlmCpu(U32 index, F32 value);
      F32 compCpuUtil(F32 used, F32 usedPrev, F32 total, F32 totalPrev);

 
    PRIVATE:

      static const U32 CPU_COUNT = 16; /*!< Maximum number of CPUs to report as telemetry */

    PRIVATE:

      cpuTlmFunc m_cpu_tlm_functions[CPU_COUNT]; /*!< Function pointer to specific CPU telemetry */
      U32 m_cpu_count; /*!< Number of CPUs used by the system */
      Os::SystemResources::memUtil m_mem; /*< RAM memory information */
      Os::SystemResources::physMemUtil m_physMem; /*< Physical memory information */
      Os::SystemResources::cpuUtil m_cpu[CPU_COUNT]; /*< CPU information for each CPU on the system */
      Os::SystemResources::cpuUtil m_cpu_prev[CPU_COUNT]; /*< Previous iteration CPU information */
      F32 m_cpu_avg; /*< Average of all CPU utilizations */
      bool m_enable; /*< Send telemetry when TRUE.  Don't send when FALSE */

    };

} // end namespace Svc

#endif
