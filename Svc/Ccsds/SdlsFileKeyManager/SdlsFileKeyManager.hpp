// ======================================================================
// \title  SdlsFileKeyManager.hpp
// \author lestarch-autobot
// \brief  hpp file for SdlsFileKeyManager component implementation class
// ======================================================================

#ifndef Svc_Ccsds_SdlsFileKeyManager_HPP
#define Svc_Ccsds_SdlsFileKeyManager_HPP

#include "Fw/Types/FileNameString.hpp"
#include "Svc/Ccsds/SdlsFileKeyManager/SdlsFileKeyManagerComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class SdlsFileKeyManager final : public SdlsFileKeyManagerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction, configuration, and destruction
    // ----------------------------------------------------------------------

    //! Construct SdlsFileKeyManager object
    SdlsFileKeyManager(const char* const compName  //!< The component name
    );

    //! Destroy SdlsFileKeyManager object
    ~SdlsFileKeyManager();

    //! Configure the key file path and the key length to read from the file
    void configure(const char* path,   //!< Path to the key file
                   FwSizeType keySize  //!< Key length in bytes; must be in (0, MAX_SDLS_KEY_SIZE]
    );

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for keyGet
    //!
    //! Port to receive a key request: fills the provided buffer with the key
    Svc::Ccsds::SdlsStatus keyGet_handler(FwIndexType portNum,  //!< The port number
                                          Svc::Ccsds::SdlsKeyBuffer& key) override;

    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! Path to the key file
    Fw::FileNameString m_path;

    //! Key length in bytes to read from the file
    FwSizeType m_keySize;

    //! Whether configure() has been called
    bool m_configured;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
