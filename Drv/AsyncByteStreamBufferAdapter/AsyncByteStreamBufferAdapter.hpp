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

    //! Handler implementation for driverReady
    //!
    //! Port for receiving ready signals from the driver
    //! Sample connection: byteStreamDriver.ready -> byteStreamDriverClient.byteStreamReady
    void driverReady_handler(FwIndexType portNum  //!< The port number
                             ) override;

    //! Handler implementation for fromDriver
    //!
    //! Port for receiving data from the driver
    //! Sample connection: byteStreamDriver.$recv -> byteStreamDriverClient.fromDriver
    void fromDriver_handler(FwIndexType portNum,  //!< The port number
                            Fw::Buffer& buffer,
                            const Drv::ByteStreamStatus& status) override;

    //! Handler implementation for toDriverReturn
    //!
    //! Port for receiving buffers sent on toDriver and then returned
    //! Sample connection: driver.sendReturnOut -> client.toDriverReturn
    void toDriverReturn_handler(FwIndexType portNum,  //!< The port number
                                Fw::Buffer& buffer,
                                const Drv::ByteStreamStatus& status) override;
};

}  // namespace Drv

#endif
