// ======================================================================
// \title  SystemResourcesComponentImpl.cpp
// \author sfregoso
// \brief  cpp file for SystemResources component implementation class
//
// \copyright
// Copyright 2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <cmath>  //isnan()
#include <Svc/SystemResources/SystemResources.hpp>
#include <version.hpp>
#include <FpConfig.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

SystemResources ::SystemResources(const char* const compName)
    : SystemResourcesComponentBase(compName), m_cpu_count(0), m_enable(true) {

    // Structure initializations
    m_mem.used = 0;
    m_mem.total = 0;
    for (U32 i = 0; i < CPU_COUNT; i++) {
        m_cpu[i].used = 0;
        m_cpu[i].total = 0;
        m_cpu_prev[i].used = 0;
        m_cpu_prev[i].total = 0;
    }

    if (Os::SystemResources::getCpuCount(m_cpu_count) == Os::SystemResources::SYSTEM_RESOURCES_ERROR) {
        m_cpu_count = 0;
    }

    m_cpu_count = (m_cpu_count >= CPU_COUNT) ? CPU_COUNT : m_cpu_count;

    m_cpu_tlm_functions[0] = &Svc::SystemResources::tlmWrite_CPU_00;
    m_cpu_tlm_functions[1] = &Svc::SystemResources::tlmWrite_CPU_01;
    m_cpu_tlm_functions[2] = &Svc::SystemResources::tlmWrite_CPU_02;
    m_cpu_tlm_functions[3] = &Svc::SystemResources::tlmWrite_CPU_03;
    m_cpu_tlm_functions[4] = &Svc::SystemResources::tlmWrite_CPU_04;
    m_cpu_tlm_functions[5] = &Svc::SystemResources::tlmWrite_CPU_05;
    m_cpu_tlm_functions[6] = &Svc::SystemResources::tlmWrite_CPU_06;
    m_cpu_tlm_functions[7] = &Svc::SystemResources::tlmWrite_CPU_07;
    m_cpu_tlm_functions[8] = &Svc::SystemResources::tlmWrite_CPU_08;
    m_cpu_tlm_functions[9] = &Svc::SystemResources::tlmWrite_CPU_09;
    m_cpu_tlm_functions[10] = &Svc::SystemResources::tlmWrite_CPU_10;
    m_cpu_tlm_functions[11] = &Svc::SystemResources::tlmWrite_CPU_11;
    m_cpu_tlm_functions[12] = &Svc::SystemResources::tlmWrite_CPU_12;
    m_cpu_tlm_functions[13] = &Svc::SystemResources::tlmWrite_CPU_13;
    m_cpu_tlm_functions[14] = &Svc::SystemResources::tlmWrite_CPU_14;
    m_cpu_tlm_functions[15] = &Svc::SystemResources::tlmWrite_CPU_15;
}

void SystemResources ::init(const NATIVE_INT_TYPE instance) {
    SystemResourcesComponentBase::init(instance);
}

SystemResources ::~SystemResources() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void SystemResources ::run_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE tick_time_hz) {
    if (m_enable) {
        Cpu();
        Mem();
        PhysMem();
        Version();
    }
}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void SystemResources ::ENABLE_cmdHandler(const FwOpcodeType opCode,
                                         const U32 cmdSeq,
                                         SystemResourceEnabled enable) {
    m_enable = (enable == SystemResourceEnabled::ENABLED);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void SystemResources ::VERSION_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    Fw::LogStringArg version_string(FRAMEWORK_VERSION);
    this->log_ACTIVITY_LO_FRAMEWORK_VERSION(version_string);

    version_string = PROJECT_VERSION;
    this->log_ACTIVITY_LO_PROJECT_VERSION(version_string);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

F32 SystemResources::compCpuUtil(Os::SystemResources::CpuTicks current, Os::SystemResources::CpuTicks previous) {
    F32 util = 100.0f;
    // Prevent divide by zero on fast-sample
    if ((current.total - previous.total) != 0) {
        // Compute CPU % Utilization
        util = (static_cast<F32>(current.used - previous.used) / static_cast<F32>(current.total - previous.total)) *
               100.0f;
        util = std::isnan(util) ? 100.0f : util;
    }
    return util;
}

void SystemResources::Cpu() {
    U32 count = 0;
    F32 cpuAvg = 0;

    for (U32 i = 0; i < m_cpu_count && i < CPU_COUNT; i++) {
        Os::SystemResources::SystemResourcesStatus status = Os::SystemResources::getCpuTicks(m_cpu[i], i);
        // Best-effort calculations and telemetry
        if (status == Os::SystemResources::SYSTEM_RESOURCES_OK) {
            F32 cpuUtil = compCpuUtil(m_cpu[i], m_cpu_prev[i]);
            cpuAvg += cpuUtil;

            // Send telemetry using telemetry output table
            FW_ASSERT(this->m_cpu_tlm_functions[i]);
            (this->*m_cpu_tlm_functions[i])(cpuUtil, Fw::Time());

            // Store cpu used and total
            m_cpu_prev[i] = m_cpu[i];
            count++;
        }
    }

    cpuAvg = (count == 0) ? 0.0f : (cpuAvg / static_cast<F32>(count));
    this->tlmWrite_CPU(cpuAvg);
}

void SystemResources::Mem() {
    if (Os::SystemResources::getMemUtil(m_mem) == Os::SystemResources::SYSTEM_RESOURCES_OK) {
        this->tlmWrite_MEMORY_TOTAL(m_mem.total / 1024);
        this->tlmWrite_MEMORY_USED(m_mem.used / 1024);
    }
}

void SystemResources::PhysMem() {
    U64 total = 0;
    U64 free = 0;

    if (Os::FileSystem::getFreeSpace("/", total, free) == Os::FileSystem::OP_OK) {
        this->tlmWrite_NON_VOLATILE_FREE(free / 1024);
        this->tlmWrite_NON_VOLATILE_TOTAL(total / 1024);
    }
}

void SystemResources::Version() {
    Fw::TlmString version_string(FRAMEWORK_VERSION);
    this->tlmWrite_FRAMEWORK_VERSION(version_string);

    version_string= PROJECT_VERSION;
    this->tlmWrite_PROJECT_VERSION(version_string);
}
}  // end namespace Svc
