// ======================================================================
// \title  ComToComBufferAdapter.hpp
// \brief  hpp file for ComToComBufferAdapter component implementation class
// ======================================================================

#ifndef Svc_ComToComBufferAdapter_HPP
#define Svc_ComToComBufferAdapter_HPP

#include <Fw/Types/Assert.hpp>
#include <Svc/ComToComBufferAdapter/ComToComBufferAdapterComponentAc.hpp>

namespace Svc {

class ComToComBufferAdapter final : public ComToComBufferAdapterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComToComBufferAdapter
    explicit ComToComBufferAdapter(const char* compName);

    //! Destroy object ComToComBufferAdapter
    ~ComToComBufferAdapter();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comIn
    //!
    //! Strips the prepended FwPacketDescriptorType off the legacy ComBuffer and
    //! forwards the remaining payload on comBufferSendOut with the descriptor as
    //! the packetType argument.
    void comIn_handler(FwIndexType portNum,  //!< The port number
                       Fw::ComBuffer& data,  //!< Buffer containing packet data
                       U32 context           //!< Call context value; meaning chosen by user
                       ) override;
};

}  // namespace Svc

#endif
