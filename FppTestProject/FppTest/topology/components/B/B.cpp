// ======================================================================
// \title  B.cpp
// \author bocchino
// \brief  cpp file for B component implementation class
// ======================================================================

#include <iostream>

#include "FppTest/topology/components/B/B.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

B ::B(const char* const compName) : BComponentBase(compName) {}

B ::~B() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void B ::dataIn_handler(FwIndexType portNum, U32 data) {
    Fw::String msg;
    msg.format("dataIn_handler: received %" PRI_U32, data);
    this->printMessage(msg);
    if (this->isConnected_dataOut_OutputPort(0)) {
        this->dataOut_out(0, data);
    }
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void B::printMessage(const Fw::StringBase& msg) {
    const auto instance = this->getInstance();
    std::cout << "B (instance " << instance << "): " << msg.toChar() << "\n";
    fflush(stdout);
}

}  // namespace FppTest
