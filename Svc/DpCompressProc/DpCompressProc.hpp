// ======================================================================
// \title  DpCompressProc.hpp
// \author kubiak
// \brief  hpp file for DpCompressProc component implementation class
// ======================================================================

#ifndef Svc_DpCompressProc_HPP
#define Svc_DpCompressProc_HPP

#include "Svc/DpCompressProc/DpCompressProcComponentAc.hpp"
#include "Svc/DpCompressProc/Types/CompressionMetadataSerializableAc.hpp"

namespace Svc {

class DpCompressProc final : public DpCompressProcComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct DpCompressProc object
    DpCompressProc(const char* const compName  //!< The component name
    );

    //! Destroy DpCompressProc object
    ~DpCompressProc();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    void serializeCompressionHeader(Fw::LinearBufferBase& serializer,
                                    const FwSizeStoreType compressed_payload_size,
                                    const CompressionMetadata& metadata);

    //! Handler implementation for procRequest
    void procRequest_handler(FwIndexType portNum,  //!< The port number
                             Fw::Buffer& fwBuffer  //!< The buffer
                             ) override;
};

}  // namespace Svc

#endif
