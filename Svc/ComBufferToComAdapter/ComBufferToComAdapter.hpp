// ======================================================================
// \title  ComBufferToComAdapter.hpp
// \brief  hpp file for ComBufferToComAdapter component implementation class
// ======================================================================

#ifndef Svc_ComBufferToComAdapter_HPP
#define Svc_ComBufferToComAdapter_HPP

#include <Fw/Types/Assert.hpp>
#include <Svc/ComBufferToComAdapter/ComBufferToComAdapterComponentAc.hpp>

namespace Svc {

class ComBufferToComAdapter final : public ComBufferToComAdapterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComBufferToComAdapter
    explicit ComBufferToComAdapter(const char* compName);

    //! Destroy object ComBufferToComAdapter
    ~ComBufferToComAdapter();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comBufferSendIn
    //!
    //! Prepends the packetType (APID) to the front of the ComBuffer as a
    //! FwPacketDescriptorType and forwards the result on the legacy comOut port.
    void comBufferSendIn_handler(FwIndexType portNum,             //!< The port number
                                 Fw::ComBuffer& data,             //!< Buffer containing packet data
                                 const ComCfg::Apid& packetType,  //!< Packet APID
                                 U32 context                      //!< Call context value; meaning chosen by user
                                 ) override;
};

}  // namespace Svc

#endif
