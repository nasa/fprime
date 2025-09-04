// ----------------------------------------------------------------------
//
// ComSplitter.cpp
//
// ----------------------------------------------------------------------

#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/ComSplitter/ComSplitter.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ComSplitter ::ComSplitter(const char* compName) : ComSplitterComponentBase(compName) {}

ComSplitter ::~ComSplitter() {}

// ----------------------------------------------------------------------
// Handler implementations
// ----------------------------------------------------------------------

void ComSplitter ::comIn_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    FW_ASSERT(portNum == 0);

    FwIndexType numPorts = getNum_comOut_OutputPorts();
    FW_ASSERT(numPorts > 0);

    for (FwIndexType i = 0; i < numPorts; i++) {
        if (isConnected_comOut_OutputPort(i)) {
            // Need to make a copy because we are passing by reference!:
            Fw::ComBuffer dataToSend = data;
            comOut_out(i, dataToSend, 0);
        }
    }
}

}  // namespace Svc
