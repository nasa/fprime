// ======================================================================
// \title  SystemResourcesComponentImpl.hpp
// \author Santos F. Fregoso
// \brief  hpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef SystemResources_HPP
#define SystemResources_HPP

#include "Svc/SystemResources/SystemResourcesComponentAc.hpp"
#include "Os/SystemResources.hpp"
#include "Os/FileSystem.hpp"

namespace Svc {

class SystemResources : public SystemResourcesComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object SystemResources
    //!
    SystemResources(const char* const compName /*!< The component name*/
    );

    //! Initialize object SystemResources
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object SystemResources
    //!
    ~SystemResources(void);

    typedef void (SystemResourcesComponentBase::*cpuTlmFunc)(F32, Fw::Time);

    PRIVATE :

        // ----------------------------------------------------------------------
        // Handler implementations for user-defined typed input ports
        // ----------------------------------------------------------------------

        //! Handler implementation for run
        //!
        void
        run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                    NATIVE_UINT_TYPE context       /*!< The call order*/
        );

  private:
    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    //! Implementation for SYS_RES_ENABLE command handler
    //! A command to enable or disable system resource telemetry
    void ENABLE_cmdHandler(
        const FwOpcodeType opCode,   /*!< The opcode*/
        const U32 cmdSeq,            /*!< The command sequence number*/
        SystemResourceEnabled enable /*!< whether or not system resource telemetry is enabled*/
    );

    //! Implementation for VERSION command handler
    //! Report version as EVR
    void VERSION_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                            const U32 cmdSeq           /*!< The command sequence number*/
    );

  private:
    void Cpu();
    void Mem();
    void PhysMem();
    void Version();
    F32 compCpuUtil(Os::SystemResources::CpuTicks current, Os::SystemResources::CpuTicks previous);


    static const U32 CPU_COUNT = 16; /*!< Maximum number of CPUs to report as telemetry */

    cpuTlmFunc m_cpu_tlm_functions[CPU_COUNT];       /*!< Function pointer to specific CPU telemetry */
    U32 m_cpu_count;                                     /*!< Number of CPUs used by the system */
    Os::SystemResources::MemUtil m_mem;                  /*!< RAM memory information */
    Os::SystemResources::CpuTicks m_cpu[CPU_COUNT];      /*!< CPU information for each CPU on the system */
    Os::SystemResources::CpuTicks m_cpu_prev[CPU_COUNT]; /*!< Previous iteration CPU information */
    bool m_enable;                                       /*!< Send telemetry when TRUE.  Don't send when FALSE */
};

}  // end namespace Svc

#endif
