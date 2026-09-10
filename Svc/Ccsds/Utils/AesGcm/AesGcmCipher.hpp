// ======================================================================
// \title  AesGcmCipher.hpp
// \brief  AES-256-GCM one-shot encrypt / decrypt over a preallocated OpenSSL context
// ======================================================================

#ifndef Svc_Ccsds_Utils_AesGcmCipher_HPP
#define Svc_Ccsds_Utils_AesGcmCipher_HPP

#include <openssl/evp.h>

#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/ByteArray.hpp"
#include "Fw/Types/ConstByteArray.hpp"
#include "Svc/Ccsds/Types/SdlsKeyBuffer.hpp"
#include "Svc/Ccsds/Types/SdlsStatusEnumAc.hpp"

namespace Svc {
namespace Ccsds {
namespace Utils {

//! Owns one AES-256-GCM cipher context for the lifetime of the object, so a frame operation
//! only re-keys it and allocates nothing. Shared by the SDLS AES-GCM encryptor and decryptor.
class AesGcmCipher final {
  public:
    //! Length of an AES-256 key, in bytes
    static constexpr FwSizeType KEY_LEN = 32;
    //! Length of the AES-GCM initialization vector, in bytes
    static constexpr FwSizeType IV_LEN = 12;
    //! Length of the AES-GCM authentication tag (the SDLS MAC), in bytes
    static constexpr FwSizeType TAG_LEN = 16;

    //! Which primitive the context is initialized for
    enum class Direction {
        ENCRYPT,
        DECRYPT,
    };

    //! Fetch the algorithm and build the context; asserts if OpenSSL cannot supply AES-256-GCM
    explicit AesGcmCipher(Direction direction);

    //! Free the context and the fetched algorithm
    ~AesGcmCipher();

    AesGcmCipher(const AesGcmCipher&) = delete;
    AesGcmCipher& operator=(const AesGcmCipher&) = delete;
    AesGcmCipher(AesGcmCipher&&) = delete;
    AesGcmCipher& operator=(AesGcmCipher&&) = delete;

    //! Encrypt plaintext under key and iv, authenticating aad, writing ciphertext and tag.
    //! The key is wiped before returning. ciphertext must hold plaintext.size bytes.
    //! \return true on success
    bool encrypt(Svc::Ccsds::SdlsKeyBuffer& key,  //!< KEY_LEN-byte key, wiped on return
                 Fw::ConstByteArray iv,           //!< IV_LEN bytes
                 Fw::ConstByteArray aad,          //!< Additional authenticated data
                 Fw::ConstByteArray plaintext,    //!< Data to encrypt
                 Fw::ByteArray ciphertext,        //!< Receives plaintext.size bytes
                 Fw::ByteArray tag                //!< Receives TAG_LEN bytes
    );

    //! Decrypt data in place under key and iv, then verify tag over aad and the ciphertext.
    //! The key is wiped before returning. On MAC_VERIFICATION_FAILURE data holds garbage.
    //! \return SUCCESS, MAC_VERIFICATION_FAILURE, or DECRYPTION_FAILURE
    Svc::Ccsds::SdlsStatus decrypt(Svc::Ccsds::SdlsKeyBuffer& key,  //!< KEY_LEN-byte key, wiped on return
                                   Fw::ConstByteArray iv,           //!< IV_LEN bytes
                                   Fw::ConstByteArray aad,          //!< Additional authenticated data
                                   Fw::ByteArray data,              //!< Ciphertext, replaced by plaintext
                                   Fw::ConstByteArray tag           //!< TAG_LEN-byte tag to verify
    );

  private:
    //! Re-key the context with key and iv, wipe the key, and absorb the AAD
    bool init(Svc::Ccsds::SdlsKeyBuffer& key, Fw::ConstByteArray iv, Fw::ConstByteArray aad);

    //! Whether the context was built for encryption or decryption
    const Direction m_direction;

    //! AES-256-GCM implementation, fetched once at construction
    EVP_CIPHER* m_cipher;

    //! Cipher context, created once at construction and re-keyed per operation
    EVP_CIPHER_CTX* m_ctx;
};

}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc

#endif
