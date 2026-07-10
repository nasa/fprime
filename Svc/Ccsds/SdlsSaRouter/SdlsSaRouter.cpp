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

void SdlsSaRouter ::decryptIn_handler(FwIndexType portNum,
                                      U16 securityAssociationIndex,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) {
    FwIndexType outputPort = 0;
    const Fw::Success found = this->m_saMap.find(securityAssociationIndex, outputPort);
    Svc::Ccsds::SdlsStatus errorStatus = Svc::Ccsds::SdlsStatus::UNKNOWN_SA;
    if (found == Fw::Success::SUCCESS) {
        if ((outputPort >= 0) && (outputPort < this->getNum_saDecryptOut_OutputPorts()) &&
            (this->isConnected_saDecryptOut_OutputPort(outputPort))) {
            this->saDecryptOut_out(outputPort, securityAssociationIndex, data, context);
            return;
        }
        errorStatus = Svc::Ccsds::SdlsStatus::UNKNOWN_PORT;
    }
    // Routing failed: pass the error status forward with the untouched buffer, tracking it
    // so the eventual ownership return routes back upstream via bufferReturnOut
    const Fw::Success inserted =
        this->m_outstanding.insert(data.getContext(), static_cast<FwIndexType>(ROUTER_ERROR_PORT));
    FW_ASSERT(inserted == Fw::Success::SUCCESS);
    this->decryptOut_out(0, errorStatus, data, context);
}

void SdlsSaRouter ::decryptReturnIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    FwIndexType outputPort = 0;
    const Fw::Success found = this->m_outstanding.find(data.getContext(), outputPort);
    FW_ASSERT(found == Fw::Success::SUCCESS);
    (void)this->m_outstanding.remove(data.getContext(), outputPort);
    if (outputPort == ROUTER_ERROR_PORT) {
        // Buffer was forwarded by the router itself on a routing error: return it upstream
        this->bufferReturnOut_out(0, data, context);
    } else {
        this->saDecryptReturnOut_out(outputPort, data, context);
    }
}

void SdlsSaRouter ::saBufferReturnIn_handler(FwIndexType portNum,
                                             Fw::Buffer& data,
                                             const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(0, data, context);
}

void SdlsSaRouter ::saDecryptIn_handler(FwIndexType portNum,
                                        const Svc::Ccsds::SdlsStatus& status,
                                        Fw::Buffer& data,
                                        const ComCfg::FrameContext& context) {
    const Fw::Success inserted = this->m_outstanding.insert(data.getContext(), portNum);
    FW_ASSERT(inserted == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(portNum));
    this->decryptOut_out(0, status, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
