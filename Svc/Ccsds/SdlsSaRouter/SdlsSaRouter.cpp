// ======================================================================
// \title  SdlsSaRouter.cpp
// \author lestarch-autobot
// \brief  cpp file for SdlsSaRouter component implementation class
// ======================================================================

#include "Svc/Ccsds/SdlsSaRouter/SdlsSaRouter.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SdlsSaRouter ::SdlsSaRouter(const char* const compName) : SdlsSaRouterComponentBase(compName) {
    // Load the compile-time SA-to-port map from configuration
    const SdlsCfg::SaMap saMap;
    for (SdlsCfg::SaMap::SizeType i = 0; i < SdlsCfg::SaMap::SIZE; i++) {
        const Fw::Success status =
            this->m_saMap.insert(saMap[i].get_securityAssociationIndex(), saMap[i].get_portIndex());
        FW_ASSERT(status == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(i));
    }
}

SdlsSaRouter ::~SdlsSaRouter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void SdlsSaRouter ::dataIn_handler(FwIndexType portNum,
                                   U16 securityAssociationIndex,
                                   Fw::Buffer& data,
                                   const ComCfg::FrameContext& context) {
    FwIndexType outputPort = 0;
    const Fw::Success found = this->m_saMap.find(securityAssociationIndex, outputPort);
    Svc::Ccsds::SdlsStatus errorStatus = Svc::Ccsds::SdlsStatus::UNKNOWN_SA;
    if (found == Fw::Success::SUCCESS) {
        if ((outputPort >= 0) && (outputPort < this->getNum_saDataOut_OutputPorts()) &&
            (this->isConnected_saDataOut_OutputPort(outputPort))) {
            this->saDataOut_out(outputPort, securityAssociationIndex, data, context);
            return;
        }
        errorStatus = Svc::Ccsds::SdlsStatus::UNKNOWN_PORT;
    }
    // Routing failed: pass the error status forward with the untouched buffer, tracking it
    // so the eventual ownership return routes back upstream via bufferReturnOut
    const Fw::Success inserted =
        this->m_outstanding.insert(data.getContext(), static_cast<FwIndexType>(ROUTER_ERROR_PORT));
    if (inserted != Fw::Success::SUCCESS) {
        // Tracking table full: drop the request and return the buffer upstream immediately
        this->log_WARNING_HI_TrackingTableFull();
        this->bufferReturnOut_out(0, data, context);
        return;
    }
    this->dataOut_out(0, errorStatus, data, context);
}

void SdlsSaRouter ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    FwIndexType outputPort = 0;
    const Fw::Success found = this->m_outstanding.find(data.getContext(), outputPort);
    if (found != Fw::Success::SUCCESS) {
        // Untracked buffer (e.g. lost to a context-key collision): return it upstream
        this->log_WARNING_HI_UntrackedBufferReturned();
        this->bufferReturnOut_out(0, data, context);
        return;
    }
    (void)this->m_outstanding.remove(data.getContext(), outputPort);
    if (outputPort == ROUTER_ERROR_PORT) {
        // Buffer was forwarded by the router itself on a routing error: return it upstream
        this->bufferReturnOut_out(0, data, context);
    } else {
        this->saDataReturnOut_out(outputPort, data, context);
    }
}

void SdlsSaRouter ::saBufferReturnIn_handler(FwIndexType portNum,
                                             Fw::Buffer& data,
                                             const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(0, data, context);
}

void SdlsSaRouter ::saDataIn_handler(FwIndexType portNum,
                                     const Svc::Ccsds::SdlsStatus& status,
                                     Fw::Buffer& data,
                                     const ComCfg::FrameContext& context) {
    const Fw::Success inserted = this->m_outstanding.insert(data.getContext(), portNum);
    if (inserted != Fw::Success::SUCCESS) {
        // Tracking table full: drop the data and return ownership to the downstream component
        this->log_WARNING_HI_TrackingTableFull();
        this->saDataReturnOut_out(portNum, data, context);
        return;
    }
    this->dataOut_out(0, status, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
