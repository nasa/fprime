// ======================================================================
// \title  Sender.cpp
// \author bocchino
// \brief  cpp file for Sender component implementation class
// ======================================================================

#include <iostream>

#include "FppTest/topology/components/Sender/Sender.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Sender::Sender(const char* const compName) : AComponentBase(compName) {}

Sender::~Sender() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Sender::dataIn_handler(FwIndexType portNum, U32 data) {
    Fw::String msg;
    msg.format("dataIn_handler: received %" PRI_U32, data);
    this->printMessage(msg);
}

// ----------------------------------------------------------------------
// Public interface
// ----------------------------------------------------------------------

void Sender::sendData(U32 data  //!< The data
) {
    Fw::String msg;
    if (this->isConnected_dataOut_OutputPort(0)) {
        msg.format("dataIn_handler: sending %" PRI_U32, data);
        this->printMessage(msg);
        this->dataOut_out(0, data);
    }
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void Sender::printMessage(const Fw::StringBase& msg) {
    const auto instance = this->getInstance();
    std::cout << "A (instance " << instance << "): " << msg.toChar() << "\n";
    fflush(stdout);
}

}  // namespace FppTest
