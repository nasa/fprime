// ======================================================================
// \title  AESDecryptorTester.hpp
// \author vivi and claradavisb
// \brief  hpp file for AESDecryptor component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESDecryptorTester_HPP
#define Svc_Ccsds_AESDecryptorTester_HPP

#include "Svc/Encryption/AESDecryptor/AESDecryptor.hpp"
#include "Svc/Encryption/AESDecryptor/AESDecryptorGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AESDecryptorTester final : public AESDecryptorGTestBase {
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

    //! Length of the AES-GCM authentication tag (the SDLS MAC), in bytes
    static const FwSizeType GCM_TAG_LEN = 16;

    //! Virtual channel the component is configured for in these tests
    static const U8 TEST_VC_ID = 5;

    //! Security association index passed on decryptIn in these tests
    static const U16 TEST_SPI = 0x1234;

    //! Backing storage for buffers handed to the component
    static const FwSizeType TEST_BUFFER_SIZE = 256;

    //! Storage for a buffer past the configured input bound
    static const FwSizeType OVERSIZE_STORAGE = SdlsCfg::AesMaxInputSize + 1;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object AESDecryptorTester
    AESDecryptorTester();

    //! Destroy object AESDecryptorTester
    ~AESDecryptorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! A frame built by an implementation outside this repository decrypts to the expected
    //! plaintext. Covers SVC-CCSDS-AES-DECRYPTOR-001 and SVC-CCSDS-AES-DECRYPTOR-002.
    void testKnownAnswer();

    //! Svc::Ccsds::Utils::SdlsTcAuthMask agrees, byte for byte, with the mask this harness
    //! builds from the ground segment's contract. Covers SVC-CCSDS-AES-DECRYPTOR-002.
    void testAuthMaskLayout();

    //! A well-formed frame decrypts in place, leaving the plaintext at IV_LEN into the
    //! original allocation. Covers SVC-CCSDS-AES-DECRYPTOR-001.
    void testDecryptNominal();

    //! A frame carrying no ciphertext at all is still authenticated and accepted.
    //! Covers SVC-CCSDS-AES-DECRYPTOR-001.
    void testEmptyCiphertext();

    //! The emitted buffer keeps the allocation context and origin of the buffer that arrived,
    //! so it remains deallocatable. Covers SVC-CCSDS-AES-DECRYPTOR-006.
    void testAllocationContextPreserved();

    //! A single flipped ciphertext bit fails the MAC check. Covers SVC-CCSDS-AES-DECRYPTOR-003.
    void testTamperedCiphertext();

    //! A single flipped MAC bit fails the MAC check. Covers SVC-CCSDS-AES-DECRYPTOR-003.
    void testTamperedMac();

    //! A single flipped IV bit fails the MAC check. Covers SVC-CCSDS-AES-DECRYPTOR-003.
    void testTamperedIv();

    //! A frame authenticated for another virtual channel is rejected, which is the whole
    //! point of putting the VC in the AAD. Covers SVC-CCSDS-AES-DECRYPTOR-002.
    void testWrongVcId();

    //! A frame authenticated under another security association is rejected.
    //! Covers SVC-CCSDS-AES-DECRYPTOR-002.
    void testWrongSecurityAssociation();

    //! configure() may be called again to change the virtual channel, keeping the cipher
    //! context it built the first time. Covers SVC-CCSDS-AES-DECRYPTOR-002.
    void testReconfigureChangesVc();

    //! A good frame still decrypts after a rejected one. The cipher context is built once and
    //! reused, so a failed MAC check must not leave it unusable. Covers SVC-CCSDS-AES-DECRYPTOR-003.
    void testRecoversAfterMacFailure();

    //! A buffer too short to hold an IV and a MAC is rejected without touching the key.
    //! Covers SVC-CCSDS-AES-DECRYPTOR-004.
    void testShortBuffer();

    //! A buffer larger than the deployment's frame can carry is rejected, likewise before a
    //! key is requested. Covers SVC-CCSDS-AES-DECRYPTOR-004.
    void testOversizeBuffer();

    //! A key the key manager could not supply yields KEY_ERROR.
    //! Covers SVC-CCSDS-AES-DECRYPTOR-005.
    void testKeyUnavailable();

    //! A key of the wrong length yields KEY_ERROR rather than decrypting under it.
    //! Covers SVC-CCSDS-AES-DECRYPTOR-005.
    void testWrongKeySize();

    //! A buffer returned on decryptReturnIn goes back to its sender.
    //! Covers SVC-CCSDS-AES-DECRYPTOR-007.
    void testBufferReturn();

  private:
    // ----------------------------------------------------------------------
    // Handler overrides
    // ----------------------------------------------------------------------

    //! Stand in for the key manager, supplying whatever setKey() last configured
    Svc::Ccsds::SdlsStatus from_keyGet_handler(FwIndexType portNum, Svc::Ccsds::SdlsKeyBuffer& key) override;

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Set the key, and the status, this harness returns on keyGet
    void setKey(const U8* key, FwSizeType keyLen, Svc::Ccsds::SdlsStatus status);

    //! Build IV | ciphertext | MAC into the harness storage for the given plaintext, and
    //! return a buffer wrapping it
    Fw::Buffer buildFrame(const U8* plaintext, FwSizeType plainLen, U8 vcId, U16 spi);

    //! Hand a frame to the component
    void sendDecrypt(Fw::Buffer& data, U16 spi);

    //! Assert that exactly one buffer came out on decryptOut carrying the given status
    void assertStatus(Svc::Ccsds::SdlsStatus status);

    //! Assert that exactly one buffer came out on decryptOut carrying the given plaintext
    void assertPlaintext(const U8* expected, FwSizeType expectedLen);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    AESDecryptor component;

    //! Key handed out on keyGet
    U8 m_key[AES_256_KEY_LEN];

    //! Length of the key handed out on keyGet
    FwSizeType m_keyLen;

    //! Status returned on keyGet
    Svc::Ccsds::SdlsStatus m_keyStatus;

    //! Backing storage for the frames handed to the component
    U8 m_storage[TEST_BUFFER_SIZE];

    //! Backing storage for the oversize-rejection test
    U8 m_oversize[OVERSIZE_STORAGE];
};

}  // namespace Ccsds

}  // namespace Svc

#endif
