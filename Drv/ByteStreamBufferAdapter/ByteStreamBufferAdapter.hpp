// ======================================================================
// \title  ByteStreamBufferAdapter.hpp
// \author bocchino
// \brief  hpp file for ByteStreamBufferAdapter component implementation class
// ======================================================================

#ifndef Drv_ByteStreamBufferAdapter_HPP
#define Drv_ByteStreamBufferAdapter_HPP

#include "Drv/ByteStreamBufferAdapter/ByteStreamBufferAdapterComponentAc.hpp"

namespace Drv {

class ByteStreamBufferAdapter final : public ByteStreamBufferAdapterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ByteStreamBufferAdapter object
    ByteStreamBufferAdapter(const char* const compName  //!< The component name
    );

    //! Destroy ByteStreamBufferAdapter object
    ~ByteStreamBufferAdapter();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferIn
    //!
    //! Port for receiving buffers
    void bufferIn_handler(FwIndexType portNum,  //!< The port number
                          Fw::Buffer& fwBuffer  //!< The buffer
                          ) override;

    //! Handler implementation for bufferOutReturn
    //!
    //! Port for receiving buffers sent on bufferOut and then returned
    void bufferOutReturn_handler(FwIndexType portNum,  //!< The port number
                                 Fw::Buffer& fwBuffer  //!< The buffer
                                 ) override;

    //! Handler implementation for byteStreamIn
    //!
    //! Port for receiving data from the driver
    //! Sample connection: byteStreamDriver.$recv -> byteStreamDriverClient.byteStreamIn
    void fromByteStreamDriver_handler(FwIndexType portNum,  //!< The port number
                                      Fw::Buffer& buffer,
                                      const Drv::ByteStreamStatus& status) override;

    //! Handler implementation for byteStreamReady
    //!
    //! Port for receiving ready signals from the driver
    //! Sample connection: byteStreamDriver.ready -> byteStreamDriverClient.byteStreamDriverReady
    void byteStreamDriverReady_handler(FwIndexType portNum  //!< The port number
                                       ) override;

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! Whether the driver is ready
    bool m_driverIsReady = false;
};

}  // namespace Drv

#endif
