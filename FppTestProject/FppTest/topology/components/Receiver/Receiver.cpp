// ======================================================================
// \title  B.cpp
// \author bocchino
// \brief  cpp file for B component implementation class
// ======================================================================

#include <iostream>

#include "FppTest/topology/components/Receiver/Receiver.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Receiver ::Receiver(const char* const compName) : ReceiverComponentBase(compName) {}

Receiver ::~Receiver() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Receiver ::dataIn_handler(FwIndexType portNum, U32 data) {
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

void Receiver::printMessage(const Fw::StringBase& msg) {
    const auto instance = this->getInstance();
    std::cout << "Receiver (instance " << instance << "): " << msg.toChar() << "\n";
    fflush(stdout);
}

}  // namespace FppTest
