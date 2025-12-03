// ======================================================================
// \title  AsyncByteStreamBufferAdapter.hpp
// \author bocchino
// \brief  hpp file for AsyncByteStreamBufferAdapter component implementation class
// ======================================================================

#ifndef Drv_AsyncByteStreamBufferAdapter_HPP
#define Drv_AsyncByteStreamBufferAdapter_HPP

#include "Drv/AsyncByteStreamBufferAdapter/AsyncByteStreamBufferAdapterComponentAc.hpp"

namespace Drv {

class AsyncByteStreamBufferAdapter final : public AsyncByteStreamBufferAdapterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AsyncByteStreamBufferAdapter object
    AsyncByteStreamBufferAdapter(const char* const compName  //!< The component name
    );

    //! Destroy AsyncByteStreamBufferAdapter object
    ~AsyncByteStreamBufferAdapter();

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

    //! Handler implementation for byteStreamDriver
    //!
    //! Port for receiving ready signals from the driver
    //! Sample connection: byteStreamDriver.ready -> byteStreamDriverClient.byteStreamReady
    void byteStreamDriverReady_handler(FwIndexType portNum  //!< The port number
                                       ) override;

    //! Handler implementation for fromByteStreamDriver
    //!
    //! Port for receiving data from the driver
    //! Sample connection: byteStreamDriver.$recv -> byteStreamDriverClient.fromDriver
    void fromByteStreamDriver_handler(FwIndexType portNum,  //!< The port number
                                      Fw::Buffer& buffer,
                                      const Drv::ByteStreamStatus& status) override;

    //! Handler implementation for toByteStreamDriverReturn
    //!
    //! Port for receiving buffers sent on toByteStreamDriver and then returned
    //! Sample connection: driver.sendReturnOut -> client.toByteStreamDriverReturn
    void toByteStreamDriverReturn_handler(FwIndexType portNum,  //!< The port number
                                          Fw::Buffer& buffer,
                                          const Drv::ByteStreamStatus& status) override;

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! Whether the driver is ready
    bool m_driverIsReady = false;
};

}  // namespace Drv

#endif
