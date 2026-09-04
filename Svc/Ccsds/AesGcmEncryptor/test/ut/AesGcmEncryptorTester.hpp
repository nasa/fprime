// ======================================================================
// \title  AESEncryptorTester.hpp
// \author vivi and claradavisb
// \brief  hpp file for AESEncryptor component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESEncryptorTester_HPP
#define Svc_Ccsds_AESEncryptorTester_HPP

#include "Svc/Encryption/AESEncryptor/AESEncryptor.hpp"
#include "Svc/Encryption/AESEncryptor/AESEncryptorGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AESEncryptorTester final : public AESEncryptorGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    //! Length of an AES-256 key, in bytes
    static const FwSizeType AES_256_KEY_LEN = 32;

    //! Length of the AES-GCM initialization vector, in bytes
    static const FwSizeType GCM_IV_LEN = 12;

    //! Length of the AES-GCM authentication tag, in bytes
    static const FwSizeType GCM_TAG_LEN = 16;

    //! Virtual channel for tests
    static const U8 TEST_VC_ID = 3;

    //! Security association index passed on encryptIn for tests
    static const U16 TEST_SPI = 0x00AB;

    //! Backing storage for buffers handed to the component
    static const FwSizeType TEST_BUFFER_SIZE = 1024;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object AESEncryptorTester
    AESEncryptorTester();

    //! Destroy object AESEncryptorTester
    ~AESEncryptorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! The emitted frame is IV | ciphertext | MAC, and decrypts back to the plaintext under
    //! an AAD built outside this component. Covers SVC-CCSDS-AES-ENCRYPTOR-001 and -002.
    void testEncryptNominal();

    //! The emitted ciphertext and MAC match what a reference implementation
    //! produces from the same key, the component's own IV, and an AAD.
    //! Covers SVC-CCSDS-AES-ENCRYPTOR-001 and SVC-CCSDS-AES-ENCRYPTOR-002.
    void testCiphertextAndMacMatch();

    //! Svc::Ccsds::Utils::SdlsTmAuthMask agrees with the mask this harness
    //! builds from the ground segment's contract. Covers SVC-CCSDS-AES-ENCRYPTOR-002.
    void testAuthMaskLayout();

    //! The virtual channel bound into the AAD is the one on the frame context, which is
    //! also the one Svc::Ccsds::TmFramer writes into the header. Covers SVC-CCSDS-AES-ENCRYPTOR-008.
    void testContextVcIdIsAuthenticated();

    //! Two frames with identical plaintext get different IVs and different ciphertext.
    //! Covers SVC-CCSDS-AES-ENCRYPTOR-003.
    void testIvIsFreshPerFrame();

    //! A zero-length plaintext still produces a well-formed IV and MAC.
    //! Covers SVC-CCSDS-AES-ENCRYPTOR-001.
    void testEmptyPlaintext();

    //! The largest plaintext that fits is accepted, and one byte more is refused.
    //! Covers SVC-CCSDS-AES-ENCRYPTOR-005.
    void testOutputCapacityBoundary();

    //! A key the key manager could not supply yields KEY_ERROR.
    //! Covers SVC-CCSDS-AES-ENCRYPTOR-004.
    void testKeyUnavailable();

    //! A key of the wrong length yields KEY_ERROR rather than being used.
    //! Covers SVC-CCSDS-AES-ENCRYPTOR-004.
    void testWrongKeySize();

    //! The plaintext buffer is handed back to its sender, and is left unmodified.
    //! Covers SVC-CCSDS-AES-ENCRYPTOR-006.
    void testPlaintextReturnedUnmodified();

    //! The output store is marked available again on encryptReturnIn, so a following frame is
    //! encrypted rather than dropped. Covers SVC-CCSDS-AES-ENCRYPTOR-007.
    void testOutputBufferReclaimed();

    //! A frame arriving while the previous one is still in flight is dropped rather than
    //! written over unsent ciphertext. Covers SVC-CCSDS-AES-ENCRYPTOR-009.
    void testOutputBufferBusy();

    //! Back-to-back frames succeed when the buffer is returned from inside the encryptOut
    //! call, as CcsdsSdlsFramer does on a synchronous pipeline. Covers SVC-CCSDS-AES-ENCRYPTOR-009.
    void testSynchronousReturn();


  private:
    // ----------------------------------------------------------------------
    // Handler overrides
    // ----------------------------------------------------------------------

    //! Stand in for the key manager, supplying what setKey() last configured
    Svc::Ccsds::SdlsStatus from_keyGet_handler(FwIndexType portNum,
                                               U16 securityAssociationIndex,
                                               Svc::Ccsds::SdlsKeyBuffer& key) override;

    //! Records the emitted frame, and when returnSynchronously() is set, hands it straight
    //! back on encryptReturnIn from inside this call
    void from_encryptOut_handler(FwIndexType portNum,
                                 const Svc::Ccsds::SdlsStatus& status,
                                 Fw::Buffer& data,
                                 const ComCfg::FrameContext& context) override;

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Set the key, and the status, this harness returns on keyGet
    void setKey(const U8* key, FwSizeType keyLen, Svc::Ccsds::SdlsStatus status);

    //! Fill the harness storage with a recognizable pattern and hand it to the component
    Fw::Buffer sendEncrypt(FwSizeType plainLen, U16 spi, U8 vcId = TEST_VC_ID);

    //! Assert that exactly one buffer came out on encryptOut carrying the given status
    void assertStatus(Svc::Ccsds::SdlsStatus status);

    //! Decrypt the emitted frame with the AAD this harness builds for the given VC and SA,
    //! and assert it yields the expected plaintext. This is what proves the component's AAD.
    void assertFrameDecryptsTo(const U8* expected, FwSizeType expectedLen, U8 vcId, U16 spi);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    AESEncryptor component;

    //! Key handed out on keyGet
    U8 m_key[AES_256_KEY_LEN];

    //! Length of the key handed out on keyGet
    FwSizeType m_keyLen;

    //! Status returned on keyGet
    Svc::Ccsds::SdlsStatus m_keyStatus;

    //! Backing storage for the plaintext handed to the component
    U8 m_storage[TEST_BUFFER_SIZE];

    //! Whether to return the emitted buffer from inside from_encryptOut_handler
    bool m_returnSynchronously;

};

}  // namespace Ccsds

}  // namespace Svc

#endif
