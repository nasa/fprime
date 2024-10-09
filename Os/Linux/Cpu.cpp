// ======================================================================
// \title Os/Linux/Cpu.cpp
// \brief Linux implementation for Os::Cpu
// ======================================================================
#include <Os/Linux/Cpu.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/File.hpp>
#include <unistd.h>
#include <cstring>
namespace Os {
namespace Linux {
namespace Cpu {

// Proc FS /proc/stat file format example:
//
// cpu  270288 1598 88660 8470416 15185 9775 2990 867 0 0
// cpu0 67462 108 22104 2118172 3779 2540 648 287 0 0
// cpu1 74237 650 24059 2109680 2995 2447 667 93 0 0
// cpu2 69388 630 22033 2115177 4428 2424 649 378 0 0
// cpu3 59199 209 20462 2127387 3981 2363 1024 108 0 0
// ...

// Needed format, and compliant with kernel < 2.5
enum ProcCpuMeasures {
    CPU_NUMBER = 0,
    USER = 1,
    NICE = 2,
    SYSTEM = 3,
    IDLE = 4,
    MAX_CPU_TICK_TYPES = 8
};


using ProcCpuData = FwSizeType[ProcCpuMeasures::MAX_CPU_TICK_TYPES];
constexpr FwSizeType LINE_SIZE = 255; // log10(max(U64)) * 11 fields (kernel 2.6.33) = 220. Round to 256 - 1 (\0)

CpuInterface::Status getCpuData(FwSizeType cpu_index, ProcCpuData data) {
    Os::File file;
    // Open the procfs file
    constexpr char PROC_STAT_PATH[] = "/proc/stat";
    if (file.open(PROC_STAT_PATH, Os::File::Mode::OPEN_READ) != Os::File::Status::OP_OK) {

        return CpuInterface::Status::ERROR;
    }
    char proc_stat_line[LINE_SIZE + 1];
    // File starts with cpu line, then individual CPUs.
    for (FwSizeType i = 0; i < cpu_index + 2; i++) {
        FwSignedSizeType read_size = sizeof proc_stat_line - 1;
        Os::File::Status file_status = file.readline(reinterpret_cast<U8*>(proc_stat_line), read_size,
                                                     Os::File::WaitType::NO_WAIT);
        proc_stat_line[read_size + 1] = '\0'; // Null terminate
        if (file_status != Os::File::Status::OP_OK) {
            return CpuInterface::Status::ERROR;
        }
        // Make sure we've not strayed passed the cpu section
        if (::strncmp(proc_stat_line, "cpu", 3) != 0) {
            return CpuInterface::Status::ERROR;
        }
    }
    char* token_start = proc_stat_line + 3; // Length of "cpu"
    for (FwSizeType i = 0; i < ProcCpuMeasures::MAX_CPU_TICK_TYPES; i++) {
        FwSizeType token = 0;
        Fw::StringUtils::StringToNumberStatus status = Fw::StringUtils::string_to_number(
            token_start, static_cast<FwSizeType>(sizeof proc_stat_line) - static_cast<FwSizeType>(token_start - proc_stat_line), token, &token_start);
        // Check conversion success
        if (status != Fw::StringUtils::StringToNumberStatus::SUCCESSFUL_CONVERSION) {
            return CpuInterface::Status::ERROR;
        }
        data[i] = token;
        // Check cpu index is correct
        if (i == ProcCpuMeasures::CPU_NUMBER and data[i] != cpu_index) {
            return CpuInterface::Status::ERROR;
        }
    }
    return CpuInterface::Status::OP_OK;
}

CpuInterface::Status LinuxCpu::_getCount(FwSizeType& cpu_count) {
    long cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if ((cpus > 0) && (static_cast<FwSizeType>(cpus) < std::numeric_limits<FwSizeType>::max())) {
        cpu_count = static_cast<FwSizeType>(cpus);
        return Status::OP_OK;
    }
    cpu_count = 0;
    return Status::ERROR;
}

CpuInterface::Status LinuxCpu::_getTicks(Os::Cpu::Ticks& ticks, FwSizeType cpu_index) {
    FwSizeType count = 0;
    CpuInterface::Status status = this->_getCount(count);
    if (status != CpuInterface::Status::OP_OK) {
        return status;
    } else if (cpu_index >= count) {
        return CpuInterface::Status::ERROR;
    }
    ProcCpuData cpu_data = {0, 0, 0, 0, 0, 0, 0, 0};
    status = getCpuData(cpu_index, cpu_data);
    ticks.used = cpu_data[ProcCpuMeasures::USER] + cpu_data[ProcCpuMeasures::NICE] + cpu_data[ProcCpuMeasures::SYSTEM];
    ticks.total = ticks.used + cpu_data[ProcCpuMeasures::IDLE];

    return status;
}

CpuHandle* LinuxCpu::getHandle() {
    return &this->m_handle;
}

} // namespace Cpu
} // namespace Linux
} // namespace Os
