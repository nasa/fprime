#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Ref/BlockDriver/BlockDriver.hpp>

namespace Ref {

BlockDriver::BlockDriver(const char* compName) : BlockDriverComponentBase(compName), m_cycles(0) {}

BlockDriver::~BlockDriver() {}

void BlockDriver::BufferIn_handler(FwIndexType portNum, Drv::DataBuffer& buffer) {
    // just a pass-through
    this->BufferOut_out(0, buffer);
}

void BlockDriver::Sched_handler(FwIndexType portNum, U32 context) {
    this->tlmWrite_BD_Cycles(this->m_cycles++);
}

void BlockDriver::PingIn_handler(const FwIndexType portNum, U32 key) {
    // call ping output port
    this->PingOut_out(0, key);
}

}  // namespace Ref
