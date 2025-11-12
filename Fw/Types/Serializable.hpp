#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#ifdef BUILD_UT
#include <iostream>
#endif

#include <Fw/FPrimeBasicTypes.hpp>
#include "Fw/Deprecate.hpp"

namespace Fw {

class StringBase;  //!< forward declaration for string
typedef enum {
    FW_SERIALIZE_OK,               //!< Serialization/Deserialization operation was successful
    FW_SERIALIZE_FORMAT_ERROR,     //!< Data was the wrong format (e.g. wrong packet type)
    FW_SERIALIZE_NO_ROOM_LEFT,     //!< No room left in the buffer to serialize data
    FW_DESERIALIZE_BUFFER_EMPTY,   //!< Deserialization buffer was empty when trying to read more data
    FW_DESERIALIZE_FORMAT_ERROR,   //!< Deserialization data had incorrect values (unexpected data types)
    FW_DESERIALIZE_SIZE_MISMATCH,  //!< Data was left in the buffer, but not enough to deserialize
    FW_DESERIALIZE_TYPE_MISMATCH,  //!< Deserialized type ID didn't match
    FW_DESERIALIZE_IMMUTABLE,      //!< Attempted to deserialize into an immutable buffer
} SerializeStatus;

class SerialBufferBase;  //!< forward declaration
class LinearBufferBase;  //!< forward declaration

// TODO: Temporary backwards-compatibility hack. Remove this when all references to SerializeBufferBase are migrated.
using SerializeBufferBase = LinearBufferBase;

struct Serialization {
    enum t {
        INCLUDE_LENGTH,  //!< Include length as first token in serialization
        OMIT_LENGTH      //!< Omit length from serialization
    };
};

enum class Endianness {
    BIG,    //!< Big endian serialization
    LITTLE  //!< Little endian serialization
};

class Serializable {
  public:
    // Size type for backwards compatibility
    using SizeType = FwSizeType;

  public:
    //! \brief Serialize the contents of this object to a buffer
    //!
    //! This method serializes the object's data into the provided buffer.
    //! The serialization process converts the object's data into a format
    //! that can be stored or transmitted, and then writes it to the buffer.
    //!
    //! \param buffer Reference to the SerialBufferBase where data will be serialized
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeTo(SerialBufferBase& buffer, Endianness mode = Endianness::BIG) const = 0;

    //! \brief Deserialize the contents of this object from a buffer
    //!
    //! This method reads serialized data from the provided buffer and
    //! reconstructs the object's data from it. The buffer should contain
    //! data in the format produced by serializeTo().
    //!
    //! \param buffer Reference to the SerialBufferBase from which data will be deserialized
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeFrom(SerialBufferBase& buffer, Endianness mode = Endianness::BIG) = 0;

    //! TODO: this operator should be deleted, this must be done after RawTime is modified though
    // as it currently depends on this being defined
    Serializable& operator=(const Serializable& src) = default;

    // ----------------------------------------------------------------------
    // Legacy methods for backward compatibility
    // ----------------------------------------------------------------------

    DEPRECATED(SerializeStatus serialize(SerialBufferBase& buffer) const,
               "Use serializeTo(SerialBufferBase& buffer) instead") {
        return this->serializeTo(buffer);
    }

    DEPRECATED(SerializeStatus deserialize(SerialBufferBase& buffer),
               "Use deserializeFrom(SerialBufferBase& buffer) instead") {
        return this->deserializeFrom(buffer);
    }

#if FW_SERIALIZABLE_TO_STRING || FW_ENABLE_TEXT_LOGGING || BUILD_UT
    //! \brief Generate a human-readable string representation of this object
    //!
    //! This method converts the object's data into a textual representation
    //! that can be used for logging, debugging, or display purposes. The
    //! exact format of the string may vary depending on the implementation
    //! of the derived class.
    //!
    //! \param text Reference to a StringBase object where the text representation will be stored
    virtual void toString(StringBase& text) const;
#endif

#ifdef BUILD_UT
    friend std::ostream& operator<<(std::ostream& os, const Serializable& val);
#endif

  protected:
    //! \brief Default constructor
    //!
    //! Initializes a Serializable object with default values.
    Serializable();

    //! \brief Virtual destructor
    //!
    //! Ensures proper cleanup of derived classes.
    virtual ~Serializable();
};

class SerialBufferBase {
  public:
    //! \brief Virtual destructor
    //!
    //! Ensures proper cleanup of derived classes.
    virtual ~SerialBufferBase();

    // Serialization for built-in types

    //! \brief Serialize an 8-bit unsigned integer value
    //!
    //! This method serializes a single 8-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! NOTE: The 'mode' argument here for endianness has no effect as there is no
    //! concept of endianness for a U8. It has been left in for compatibility.
    //!
    //! \param val The 8-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(U8 val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize an 8-bit signed integer value
    //!
    //! This method serializes a single 8-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! NOTE: The 'mode' argument here for endianness has no effect as there is no
    //! concept of endianness for an I8. It has been left in for compatibility.
    //!
    //! \param val The 8-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(I8 val, Endianness mode = Endianness::BIG) = 0;

#if FW_HAS_16_BIT == 1
    //! \brief Serialize a 16-bit unsigned integer value
    //!
    //! This method serializes a single 16-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 16-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(U16 val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a 16-bit signed integer value
    //!
    //! This method serializes a single 16-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 16-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(I16 val, Endianness mode = Endianness::BIG) = 0;
#endif
#if FW_HAS_32_BIT == 1
    //! \brief Serialize a 32-bit unsigned integer value
    //!
    //! This method serializes a single 32-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 32-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(U32 val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a 32-bit signed integer value
    //!
    //! This method serializes a single 32-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 32-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(I32 val, Endianness mode = Endianness::BIG) = 0;
#endif
#if FW_HAS_64_BIT == 1
    //! \brief Serialize a 64-bit unsigned integer value
    //!
    //! This method serializes a single 64-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 64-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(U64 val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a 64-bit signed integer value
    //!
    //! This method serializes a single 64-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 64-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(I64 val, Endianness mode = Endianness::BIG) = 0;
#endif
    //! \brief Serialize a 32-bit floating point value
    //!
    //! This method serializes a single 32-bit floating point value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 32-bit floating point value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(F32 val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a 64-bit floating point value
    //!
    //! This method serializes a single 64-bit floating point value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 64-bit floating point value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(F64 val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a boolean value
    //!
    //! This method serializes a single boolean value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The boolean value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(bool val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a pointer value
    //!
    //! This method serializes a pointer value into the buffer. Note that only
    //! the pointer value itself is serialized, not the contents it points to.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The pointer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(const void* val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a data buffer
    //!
    //! This method serializes a buffer of bytes into the serialization buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param buff Pointer to the buffer containing data to serialize
    //! \param length Number of bytes to serialize from the buffer
    //! \param endianMode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(const U8* buff,
                                          FwSizeType length,
                                          Endianness endianMode = Endianness::BIG) = 0;

    //! \brief Serialize a byte buffer with optional length prefix
    //!
    //! This method serializes a buffer of bytes into the serialization buffer.
    //! If lengthMode is set to INCLUDE_LENGTH, the length is included as the first token.
    //! The endianness of the serialization can be controlled via the endianMode parameter.
    //!
    //! \param buff Pointer to the buffer containing data to serialize
    //! \param length Number of bytes to serialize from the buffer
    //! \param lengthMode Specifies whether to include length in serialization (INCLUDE_LENGTH or OMIT_LENGTH)
    //! \param endianMode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(const U8* buff,
                                          FwSizeType length,
                                          Serialization::t lengthMode,
                                          Endianness endianMode = Endianness::BIG) = 0;

    //! \brief Serialize another LinearBufferBase object
    //!
    //! This method serializes the contents of another LinearBufferBase object
    //! into this buffer. The endianness of the serialization can be controlled
    //! via the mode parameter.
    //!
    //! \param val Reference to the LinearBufferBase object to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(const LinearBufferBase& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a Serializable object
    //!
    //! This method serializes an object derived from the Serializable base class
    //! into this buffer. The endianness of the serialization can be controlled
    //! via the mode parameter.
    //!
    //! \param val Reference to the Serializable object to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeFrom(const Serializable& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Serialize a size value
    //!
    //! This method serializes a size value (typically used for buffer sizes)
    //! into this buffer. The endianness of the serialization can be controlled
    //! via the mode parameter.
    //!
    //! \param size The size value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeSize(const FwSizeType size, Endianness mode = Endianness::BIG) = 0;

    // Deserialization for built-in types

    //! \brief Deserialize an 8-bit unsigned integer value
    //!
    //! This method reads an 8-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 8-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(U8& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize an 8-bit signed integer value
    //!
    //! This method reads an 8-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 8-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(I8& val, Endianness mode = Endianness::BIG) = 0;

#if FW_HAS_16_BIT == 1
    //! \brief Deserialize a 16-bit unsigned integer value
    //!
    //! This method reads a 16-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 16-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(U16& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a 16-bit signed integer value
    //!
    //! This method reads a 16-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 16-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(I16& val, Endianness mode = Endianness::BIG) = 0;
#endif

#if FW_HAS_32_BIT == 1
    //! \brief Deserialize a 32-bit unsigned integer value
    //!
    //! This method reads a 32-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 32-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(U32& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a 32-bit signed integer value
    //!
    //! This method reads a 32-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 32-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(I32& val, Endianness mode = Endianness::BIG) = 0;
#endif
#if FW_HAS_64_BIT == 1
    //! \brief Deserialize a 64-bit unsigned integer value
    //!
    //! This method reads a 64-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 64-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(U64& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a 64-bit signed integer value
    //!
    //! This method reads a 64-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 64-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(I64& val, Endianness mode = Endianness::BIG) = 0;
#endif
    //! \brief Deserialize a 32-bit floating point value
    //!
    //! This method reads a 32-bit floating point value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 32-bit floating point value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(F32& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a 64-bit floating point value
    //!
    //! This method reads a 64-bit floating point value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 64-bit floating point value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(F64& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a boolean value
    //!
    //! This method reads a boolean value from the deserialization buffer
    //! and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized boolean value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(bool& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a pointer value
    //!
    //! This method reads a pointer value from the deserialization buffer
    //! and stores it in the provided reference. Note that only the pointer
    //! value itself is deserialized, not the contents it points to. The
    //! endianness of the deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized pointer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(void*& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a data buffer
    //!
    //! This method reads a buffer of bytes from the deserialization buffer
    //! and stores them in the provided buffer. The endianness of the
    //! deserialization can be controlled via the endianMode parameter.
    //!
    //! \param buff Pointer to the buffer where deserialized data will be stored
    //! \param length Reference to store the actual number of bytes deserialized
    //! \param endianMode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(U8* buff, FwSizeType& length, Endianness endianMode = Endianness::BIG) = 0;

    //! \brief Deserialize a byte buffer with optional length prefix
    //!
    //! This method reads a buffer of bytes from the deserialization buffer
    //! and stores them in the provided buffer. If lengthMode indicates that
    //! a length prefix was included, it will be read from the buffer first.
    //! The endianness of the deserialization can be controlled via the
    //! endianMode parameter.
    //!
    //! \param buff Pointer to the buffer where deserialized data will be stored
    //! \param length Reference to store the actual number of bytes deserialized
    //! \param lengthMode Specifies whether length was included in serialization (INCLUDE_LENGTH or OMIT_LENGTH)
    //! \param endianMode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(U8* buff,
                                          FwSizeType& length,
                                          Serialization::t lengthMode,
                                          Endianness endianMode = Endianness::BIG) = 0;

    //! \brief Deserialize a Serializable object
    //!
    //! This method reads data from the deserialization buffer and reconstructs
    //! a Serializable object from it. The endianness of the deserialization
    //! can be controlled via the mode parameter.
    //!
    //! \param val Reference to the Serializable object that will be populated with deserialized data
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(Serializable& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a LinearBufferBase object
    //!
    //! This method reads data from the deserialization buffer and reconstructs
    //! a LinearBufferBase object from it. The endianness of the deserialization
    //! can be controlled via the mode parameter.
    //!
    //! \param val Reference to the LinearBufferBase object that will be populated with deserialized data
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeTo(LinearBufferBase& val, Endianness mode = Endianness::BIG) = 0;

    //! \brief Deserialize a size value
    //!
    //! This method reads a size value (typically used for buffer sizes)
    //! from the deserialization buffer. The endianness of the deserialization
    //! can be controlled via the mode parameter.
    //!
    //! \param size Reference to store the deserialized size value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeSize(FwSizeType& size, Endianness mode = Endianness::BIG) = 0;

    //! \brief Copy raw bytes from the source (this) into a destination buffer and advance source offset
    //!
    //! Copies exactly `size` bytes starting at the current deserialization pointer of `this` into `dest`.
    //! This operation does not prepend a length field and does not interpret the data.
    //!
    //! Preconditions:
    //! - `size` bytes must remain in the source (`getDeserializeSizeLeft() >= size`).
    //! - Destination must have sufficient capacity (`dest.getCapacity() >= size`).
    //!
    //! Post-conditions on success:
    //! - `dest` contains exactly the copied bytes and its previous contents are discarded.
    //! - `this` has advanced its deserialization pointer by `size` bytes.
    //!
    //! \param dest Destination serialization buffer to receive the bytes (its contents are replaced)
    //! \param size Number of bytes to copy from the source
    //! \return `FW_SERIALIZE_OK` on success; `FW_SERIALIZE_NO_ROOM_LEFT` if destination capacity is insufficient;
    //!         `FW_DESERIALIZE_SIZE_MISMATCH` if source does not contain `size` bytes remaining
    virtual SerializeStatus copyRaw(SerialBufferBase& dest, Serializable::SizeType size) = 0;

    //! \brief Append raw bytes to destination (no length) and advance source offset
    //!
    //! Appends exactly `size` bytes from the current deserialization pointer of `this` into `dest` using
    //! `Serialization::OMIT_LENGTH`, preserving any existing bytes already serialized in `dest`.
    //!
    //! Preconditions:
    //! - `size` bytes must remain in the source (`getDeserializeSizeLeft() >= size`).
    //! - Destination must have space for the append (`dest.getCapacity() >= dest.getSize() + size`).
    //!
    //! Post-conditions on success:
    //! - `dest` gains `size` additional bytes at the end; no length token is written.
    //! - `this` has advanced its deserialization pointer by `size` bytes.
    //!
    //! \param dest Destination serialization buffer to append to
    //! \param size Number of bytes to copy from the source and append to dest
    //! \return `FW_SERIALIZE_OK` on success; `FW_SERIALIZE_NO_ROOM_LEFT` if destination capacity is insufficient;
    //!         `FW_DESERIALIZE_SIZE_MISMATCH` if source does not contain `size` bytes remaining
    virtual SerializeStatus copyRawOffset(SerialBufferBase& dest, Serializable::SizeType size) = 0;

    //! \brief Reset serialization pointer to beginning of buffer
    //!
    //! This method resets the serialization pointer to the beginning of the buffer,
    //! allowing the buffer to be reused for new serialization operations. Any
    //! data that was previously serialized in the buffer will be overwritten.
    virtual void resetSer() = 0;

    //! \brief Reset deserialization pointer to beginning of buffer
    //!
    //! This method resets the deserialization pointer to the beginning of the buffer,
    //! allowing the buffer to be reused for new deserialization operations. The buffer
    //! contents are not modified, but the pointer is reset to allow reading from the
    //! start of the data.
    virtual void resetDeser() = 0;

    //! \brief Move serialization pointer to specified offset
    //!
    //! This method moves the serialization pointer to the specified offset within
    //! the buffer. This allows for skipping over data or positioning the serializer
    //! at a specific location in the buffer.
    //!
    //! \param offset The offset to move the serialization pointer to
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus moveSerToOffset(FwSizeType offset) = 0;

    //! \brief Move deserialization pointer to specified offset
    //!
    //! This method moves the deserialization pointer to the specified offset within
    //! the buffer. This allows for skipping over data or positioning the deserializer
    //! at a specific location in the buffer.
    //!
    //! \param offset The offset to move the deserialization pointer to
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus moveDeserToOffset(FwSizeType offset) = 0;

    //! \brief Skip specified number of bytes during serialization
    //!
    //! This method advances the serialization pointer by the specified number of bytes
    //! without writing any data. This can be used to reserve space in the buffer or skip
    //! over data that will be written later.
    //!
    //! \param numBytesToSkip Number of bytes to skip during serialization
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus serializeSkip(FwSizeType numBytesToSkip) = 0;

    //! \brief Skip specified number of bytes during deserialization
    //!
    //! This method advances the deserialization pointer by the specified number of bytes
    //! without reading any data. This can be used to skip over data in the buffer that
    //! is not needed or to advance to the next relevant data segment.
    //!
    //! \param numBytesToSkip Number of bytes to skip during deserialization
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus deserializeSkip(FwSizeType numBytesToSkip) = 0;

    //! \brief Get buffer capacity
    //!
    //! This method returns the total capacity of the buffer, which is the maximum
    //! amount of data that can be stored in the buffer. This is not the same as
    //! the current size, which indicates how much data is currently in the buffer.
    //!
    //! \return The capacity of the buffer in bytes
    virtual Serializable::SizeType getCapacity() const = 0;

    //! \brief Get current buffer size
    //!
    //! This method returns the current size of the buffer, which indicates how
    //! much data is currently stored in the buffer. This may be less than or
    //! equal to the buffer's capacity.
    //!
    //! \return The current size of the buffer in bytes
    virtual Serializable::SizeType getSize() const = 0;

    //! \brief Get remaining deserialization buffer size
    //!
    //! This method returns the amount of data that remains to be deserialized
    //! from the buffer. It indicates how much data is left starting from the
    //! current deserialization pointer to the end of the valid size (returned by getSize()).
    //!
    //! \return The remaining size of the deserialization buffer in bytes
    virtual Serializable::SizeType getDeserializeSizeLeft() const = 0;

    //! \brief Get remaining serialization buffer size
    //!
    //! This method returns the amount of space available for serialization
    //! in the buffer. It indicates how much data can still be written to the
    //! buffer starting from the current serialization pointer to the end of
    //! the buffer's capacity.
    //!
    //! \return The remaining size of the serialization buffer in bytes
    virtual Serializable::SizeType getSerializeSizeLeft() const = 0;

    //! \brief Set buffer contents from external source
    //!
    //! This method sets the contents of the buffer from an external source.
    //! It copies the specified number of bytes from the source pointer into
    //! the buffer and updates the buffer size accordingly.
    //!
    //! \param src Pointer to the external data source
    //! \param length Number of bytes to copy from the source
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus setBuff(const U8* src, Serializable::SizeType length) = 0;

    //! \brief Set buffer length manually
    //!
    //! This method manually sets the length of the buffer without modifying
    //! its contents. This can be used after filling the buffer with data through
    //! other means to indicate how much valid data is in the buffer.
    //!
    //! \param length The new length to set for the buffer
    //! \return SerializeStatus indicating the result of the operation
    virtual SerializeStatus setBuffLen(Serializable::SizeType length) = 0;
};

class LinearBufferBase : public SerialBufferBase {
    friend class SerializeBufferBaseTester;

  protected:
    //! \brief Copy assignment operator
    //!
    //! Assigns the contents of another LinearBufferBase instance to this one.
    //! This includes copying the buffer data, serialization location, and
    //! deserialization location from the source to this instance.
    //!
    //! \param src Reference to the source LinearBufferBase to copy from
    //! \return Reference to this instance after assignment
    LinearBufferBase& operator=(const LinearBufferBase& src);

  public:
    //! \brief Destructor
    //!
    //! Destroys a LinearBufferBase instance. This is a virtual destructor
    //! that allows proper cleanup of derived classes.
    virtual ~LinearBufferBase();

    // Serialization for built-in types

    //! \brief Serialize an 8-bit unsigned integer value
    //!
    //! This method serializes a single 8-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! NOTE: The 'mode' argument here for endianness has no effect as there is no
    //! concept of endianness for an U8. It has been left in for compatibility.
    //!
    //! \param val The 8-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(U8 val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize an 8-bit signed integer value
    //!
    //! This method serializes a single 8-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! NOTE: The 'mode' argument here for endianness has no effect as there is no
    //! concept of endianness for an I8. It has been left in for compatibility.
    //!
    //! \param val The 8-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(I8 val, Endianness mode = Endianness::BIG) override;

#if FW_HAS_16_BIT == 1
    //! \brief Serialize a 16-bit unsigned integer value
    //!
    //! This method serializes a single 16-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 16-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(U16 val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a 16-bit signed integer value
    //!
    //! This method serializes a single 16-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 16-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(I16 val, Endianness mode = Endianness::BIG) override;
#endif
#if FW_HAS_32_BIT == 1
    //! \brief Serialize a 32-bit unsigned integer value
    //!
    //! This method serializes a single 32-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 32-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(U32 val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a 32-bit signed integer value
    //!
    //! This method serializes a single 32-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 32-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(I32 val, Endianness mode = Endianness::BIG) override;
#endif
#if FW_HAS_64_BIT == 1
    //! \brief Serialize a 64-bit unsigned integer value
    //!
    //! This method serializes a single 64-bit unsigned integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 64-bit unsigned integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(U64 val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a 64-bit signed integer value
    //!
    //! This method serializes a single 64-bit signed integer value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 64-bit signed integer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(I64 val, Endianness mode = Endianness::BIG) override;
#endif
    //! \brief Serialize a 32-bit floating point value
    //!
    //! This method serializes a single 32-bit floating point value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 32-bit floating point value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(F32 val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a 64-bit floating point value
    //!
    //! This method serializes a single 64-bit floating point value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The 64-bit floating point value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(F64 val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a boolean value
    //!
    //! This method serializes a single boolean value into the buffer.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The boolean value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(bool val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a pointer value
    //!
    //! This method serializes a pointer value into the buffer. Note that only
    //! the pointer value itself is serialized, not the contents it points to.
    //! The endianness of the serialization can be controlled via the mode parameter.
    //!
    //! \param val The pointer value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(const void* val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a data buffer
    //!
    //! This method serializes a buffer of bytes into the serialization buffer.
    //! The endianness of the serialization can be controlled via the endianMode parameter.
    //!
    //! \param buff Pointer to the buffer containing data to serialize
    //! \param length Number of bytes to serialize from the buffer
    //! \param endianMode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(const U8* buff, FwSizeType length, Endianness endianMode = Endianness::BIG) override;

    //! \brief Serialize a byte buffer with optional length prefix
    //!
    //! This method serializes a buffer of bytes into the serialization buffer.
    //! If lengthMode is set to INCLUDE_LENGTH, the length is included as the first token.
    //! The endianness of the serialization can be controlled via the endianMode parameter.
    //!
    //! \param buff Pointer to the buffer containing data to serialize
    //! \param length Number of bytes to serialize from the buffer
    //! \param lengthMode Specifies whether to include length in serialization (INCLUDE_LENGTH or OMIT_LENGTH)
    //! \param endianMode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(const U8* buff,
                                  FwSizeType length,
                                  Serialization::t lengthMode,
                                  Endianness endianMode = Endianness::BIG) override;

    //! \brief Serialize another LinearBufferBase object
    //!
    //! This method serializes the contents of another LinearBufferBase object
    //! into this buffer. The endianness of the serialization can be controlled
    //! via the mode parameter.
    //!
    //! \param val Reference to the LinearBufferBase object to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(const LinearBufferBase& val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a Serializable object
    //!
    //! This method serializes an object derived from the Serializable base class
    //! into this buffer. The endianness of the serialization can be controlled
    //! via the mode parameter.
    //!
    //! \param val Reference to the Serializable object to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeFrom(const Serializable& val, Endianness mode = Endianness::BIG) override;

    //! \brief Serialize a size value
    //!
    //! This method serializes a size value (typically used for buffer sizes)
    //! into this buffer. The endianness of the serialization can be controlled
    //! via the mode parameter.
    //!
    //! \param size The size value to serialize
    //! \param mode Endianness mode for serialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeSize(const FwSizeType size, Endianness mode = Endianness::BIG) override;

    // Deserialization for built-in types

    //! \brief Deserialize an 8-bit unsigned integer value
    //!
    //! This method reads an 8-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 8-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(U8& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize an 8-bit signed integer value
    //!
    //! This method reads an 8-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 8-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(I8& val, Endianness mode = Endianness::BIG) override;

#if FW_HAS_16_BIT == 1
    //! \brief Deserialize a 16-bit unsigned integer value
    //!
    //! This method reads a 16-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 16-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(U16& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a 16-bit signed integer value
    //!
    //! This method reads a 16-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 16-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(I16& val, Endianness mode = Endianness::BIG) override;
#endif

#if FW_HAS_32_BIT == 1
    //! \brief Deserialize a 32-bit unsigned integer value
    //!
    //! This method reads a 32-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 32-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(U32& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a 32-bit signed integer value
    //!
    //! This method reads a 32-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 32-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(I32& val, Endianness mode = Endianness::BIG) override;
#endif
#if FW_HAS_64_BIT == 1
    //! \brief Deserialize a 64-bit unsigned integer value
    //!
    //! This method reads a 64-bit unsigned integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 64-bit unsigned integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(U64& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a 64-bit signed integer value
    //!
    //! This method reads a 64-bit signed integer value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 64-bit signed integer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(I64& val, Endianness mode = Endianness::BIG) override;
#endif
    //! \brief Deserialize a 32-bit floating point value
    //!
    //! This method reads a 32-bit floating point value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 32-bit floating point value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(F32& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a 64-bit floating point value
    //!
    //! This method reads a 64-bit floating point value from the deserialization
    //! buffer and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized 64-bit floating point value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(F64& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a boolean value
    //!
    //! This method reads a boolean value from the deserialization buffer
    //! and stores it in the provided reference. The endianness of the
    //! deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized boolean value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(bool& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a pointer value
    //!
    //! This method reads a pointer value from the deserialization buffer
    //! and stores it in the provided reference. Note that only the pointer
    //! value itself is deserialized, not the contents it points to. The
    //! endianness of the deserialization can be controlled via the mode parameter.
    //!
    //! \param val Reference to store the deserialized pointer value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(void*& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a data buffer
    //!
    //! This method reads a buffer of bytes from the deserialization buffer
    //! and stores them in the provided buffer. The endianness of the
    //! deserialization can be controlled via the endianMode parameter.
    //!
    //! \param buff Pointer to the buffer where deserialized data will be stored
    //! \param length Reference to store the actual number of bytes deserialized
    //! \param endianMode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(U8* buff, FwSizeType& length, Endianness endianMode = Endianness::BIG) override;

    //! \brief Deserialize a byte buffer with optional length prefix
    //!
    //! This method reads a buffer of bytes from the deserialization buffer
    //! and stores them in the provided buffer. If lengthMode indicates that
    //! a length prefix was included, it will be read from the buffer first.
    //! The endianness of the deserialization can be controlled via the
    //! endianMode parameter.
    //!
    //! \param buff Pointer to the buffer where deserialized data will be stored
    //! \param length Reference to store the actual number of bytes deserialized
    //! \param lengthMode Specifies whether length was included in serialization (INCLUDE_LENGTH or OMIT_LENGTH)
    //! \param endianMode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(U8* buff,
                                  FwSizeType& length,
                                  Serialization::t lengthMode,
                                  Endianness endianMode = Endianness::BIG) override;

    //! \brief Deserialize a Serializable object
    //!
    //! This method reads data from the deserialization buffer and reconstructs
    //! a Serializable object from it. The endianness of the deserialization
    //! can be controlled via the mode parameter.
    //!
    //! \param val Reference to the Serializable object that will be populated with deserialized data
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(Serializable& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a LinearBufferBase object
    //!
    //! This method reads data from the deserialization buffer and reconstructs
    //! a LinearBufferBase object from it. The endianness of the deserialization
    //! can be controlled via the mode parameter.
    //!
    //! \param val Reference to the LinearBufferBase object that will be populated with deserialized data
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeTo(LinearBufferBase& val, Endianness mode = Endianness::BIG) override;

    //! \brief Deserialize a size value
    //!
    //! This method reads a size value (typically used for buffer sizes)
    //! from the deserialization buffer. The endianness of the deserialization
    //! can be controlled via the mode parameter.
    //!
    //! \param size Reference to store the deserialized size value
    //! \param mode Endianness mode for deserialization (default is Endianness::BIG)
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeSize(FwSizeType& size, Endianness mode = Endianness::BIG) override;

    DEPRECATED(SerializeStatus serialize(const LinearBufferBase& val),
               "Use serializeFrom(const SerialBufferBase& val) instead");
    DEPRECATED(SerializeStatus deserialize(LinearBufferBase& val), "Use deserializeTo(SerialBufferBase& val) instead");

    //! \brief Reset serialization pointer to beginning of buffer
    //!
    //! This method resets the serialization pointer to the beginning of the buffer,
    //! allowing the buffer to be reused for new serialization operations. Any
    //! data that was previously serialized in the buffer will be overwritten.
    //!
    //! \return None
    void resetSer() override;

    //! \brief Reset deserialization pointer to beginning of buffer
    //!
    //! This method resets the deserialization pointer to the beginning of the buffer,
    //! allowing the buffer to be reused for new deserialization operations. The buffer
    //! contents are not modified, but the pointer is reset to allow reading from the
    //! start of the data.
    //!
    //! \return None
    void resetDeser() override;

    //! \brief Move serialization pointer to specified offset
    //!
    //! This method moves the serialization pointer to the specified offset within
    //! the buffer. This allows for skipping over data or positioning the serializer
    //! at a specific location in the buffer.
    //!
    //! \param offset The offset to move the serialization pointer to
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus moveSerToOffset(FwSizeType offset) override;

    //! \brief Move deserialization pointer to specified offset
    //!
    //! This method moves the deserialization pointer to the specified offset within
    //! the buffer. This allows for skipping over data or positioning the deserializer
    //! at a specific location in the buffer.
    //!
    //! \param offset The offset to move the deserialization pointer to
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus moveDeserToOffset(FwSizeType offset) override;

    //! \brief Skip specified number of bytes during serialization
    //!
    //! This method advances the serialization pointer by the specified number of bytes
    //! without writing any data. This can be used to reserve space in the buffer or skip
    //! over data that will be written later.
    //!
    //! \param numBytesToSkip Number of bytes to skip during serialization
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus serializeSkip(FwSizeType numBytesToSkip) override;

    //! \brief Skip specified number of bytes during deserialization
    //!
    //! This method advances the deserialization pointer by the specified number of bytes
    //! without reading any data. This can be used to skip over data in the buffer that
    //! is not needed or to advance to the next relevant data segment.
    //!
    //! \param numBytesToSkip Number of bytes to skip during deserialization
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus deserializeSkip(FwSizeType numBytesToSkip) override;

    DEPRECATED(Serializable::SizeType getBuffCapacity() const, "Use getCapacity() instead");
    DEPRECATED(Serializable::SizeType getBuffLength() const, "Use getSize() instead");
    DEPRECATED(Serializable::SizeType getBuffLeft(), "Use getDeserializeSizeLeft() instead");

    //! \brief Get buffer capacity
    //!
    //! This method returns the total capacity of the buffer, which is the maximum
    //! amount of data that can be stored in the buffer. This is not the same as
    //! the current size, which indicates how much data is currently in the buffer.
    //!
    //! \return The capacity of the buffer in bytes
    Serializable::SizeType getCapacity() const override = 0;

    //! \brief Get current buffer size
    //!
    //! This method returns the current size of the buffer, which indicates how
    //! much data is currently stored in the buffer. This may be less than or
    //! equal to the buffer's capacity.
    //!
    //! \return The current size of the buffer in bytes
    Serializable::SizeType getSize() const override;

    //! \brief Get remaining deserialization buffer size
    //!
    //! This method returns the amount of data that remains to be deserialized
    //! from the buffer. It indicates how much data is left starting from the
    //! current deserialization pointer to the end of the buffer.
    //!
    //! \return The remaining size of the deserialization buffer in bytes
    Serializable::SizeType getDeserializeSizeLeft() const override;

    //! \brief Get remaining serialization buffer size
    //!
    //! This method returns the amount of space available for serialization
    //! in the buffer. It indicates how much data can still be written to the
    //! buffer starting from the current serialization pointer to the end of
    //! the buffer's capacity.
    //!
    //! \return The remaining size of the serialization buffer in bytes
    Serializable::SizeType getSerializeSizeLeft() const override;

    //! \brief Get buffer address for data filling (non-const version)
    //!
    //! This method returns a pointer to the buffer's data area where data can
    //! be written. This is the non-const version of the method, allowing the
    //! buffer contents to be modified.
    //!
    //! \return Pointer to the buffer's data area
    virtual U8* getBuffAddr() = 0;

    //! \brief Get buffer address for data reading (const version)
    //!
    //! This method returns a const pointer to the buffer's data area where data
    //! can be read. This is the const version of the method, preventing modification
    //! of the buffer contents.
    //!
    //! \return Const pointer to the buffer's data area
    virtual const U8* getBuffAddr() const = 0;

    //! \brief Get address of remaining non-deserialized data
    //!
    //! This method returns a const pointer to the portion of the buffer that
    //! has not yet been deserialized. This can be used to examine remaining
    //! data or to determine how much data is left to process.
    //!
    //! \return Const pointer to the remaining non-deserialized data
    const U8* getBuffAddrLeft() const;

    //! \brief Get address of end of serialization (DANGEROUS!)
    //!
    //! This method returns a pointer to the current end of serialized data
    //! in the buffer. This is a dangerous operation as it requires knowledge
    //! of the maximum buffer size and proper adjustment when done.
    //!
    //! \return Pointer to the end of serialized data
    U8* getBuffAddrSer();

    //! \brief Set buffer contents from external source
    //!
    //! This method sets the contents of the buffer from an external source.
    //! It copies the specified number of bytes from the source pointer into
    //! the buffer and updates the buffer size accordingly.
    //!
    //! \param src Pointer to the external data source
    //! \param length Number of bytes to copy from the source
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus setBuff(const U8* src, Serializable::SizeType length) override;

    //! \brief Set buffer length manually
    //!
    //! This method manually sets the length of the buffer without modifying
    //! its contents. This can be used after filling the buffer with data through
    //! other means to indicate how much valid data is in the buffer.
    //!
    //! \param length The new length to set for the buffer
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus setBuffLen(Serializable::SizeType length) override;

    //! \brief Copy raw bytes from this buffer to destination and advance source offset
    //!
    //! This method copies exactly `size` bytes from the current position in this
    //! buffer to the destination buffer. It advances the source buffer's position
    //! by the number of bytes copied.
    //!
    //! \param dest Destination buffer to receive the copied data
    //! \param size Number of bytes to copy
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus copyRaw(SerialBufferBase& dest, Serializable::SizeType size) override;

    //! \brief Append raw bytes to destination from this buffer and advance source offset
    //!
    //! This method appends exactly `size` bytes from the current position in this
    //! buffer to the destination buffer. It uses `Serialization::OMIT_LENGTH` mode,
    //! meaning no length token is written. The source buffer's position is advanced
    //! by the number of bytes copied.
    //!
    //! \param dest Destination buffer to append data to
    //! \param size Number of bytes to append
    //! \return SerializeStatus indicating the result of the operation
    SerializeStatus copyRawOffset(SerialBufferBase& dest, Serializable::SizeType size) override;

    // ----------------------------------------------------------------------
    // Deprecated Serialization methods
    // ----------------------------------------------------------------------

    DEPRECATED(SerializeStatus serialize(U8 val), "Use serializeFrom(U8 val) instead");
    DEPRECATED(SerializeStatus serialize(I8 val), "Use serializeFrom(I8 val) instead");
#if FW_HAS_16_BIT == 1
    DEPRECATED(SerializeStatus serialize(U16 val), "Use serializeFrom(U16 val) instead");
    DEPRECATED(SerializeStatus serialize(I16 val), "Use serializeFrom(I16 val) instead");
#endif
#if FW_HAS_32_BIT == 1
    DEPRECATED(SerializeStatus serialize(U32 val), "Use serializeFrom(U32 val) instead");
    DEPRECATED(SerializeStatus serialize(I32 val), "Use serializeFrom(I32 val) instead");
#endif
#if FW_HAS_64_BIT == 1
    DEPRECATED(SerializeStatus serialize(U64 val), "Use serializeFrom(U64 val) instead");
    DEPRECATED(SerializeStatus serialize(I64 val), "Use serializeFrom(I64 val) instead");
#endif

    DEPRECATED(SerializeStatus serialize(F32 val), "Use serializeFrom(F32 val) instead");
    DEPRECATED(SerializeStatus serialize(F64 val), "Use serializeFrom(F64 val) instead");
    DEPRECATED(SerializeStatus serialize(bool val), "Use serializeFrom(bool val) instead");
    DEPRECATED(SerializeStatus serialize(const void* val), "Use serializeFrom(const void* val) instead");
    DEPRECATED(SerializeStatus serialize(const U8* buff, FwSizeType length, bool noLength),
               "Use serialize(const U8* buff, FwSizeType length, Serialization::t mode) instead");
    DEPRECATED(SerializeStatus serialize(const U8* buff, FwSizeType length),
               "Use serializeFrom(const U8* buff, FwSizeType length) instead");
    DEPRECATED(SerializeStatus serialize(const U8* buff, FwSizeType length, Serialization::t mode),
               "Use serializeFrom(const U8* buff, FwSizeType length, Serialization::t mode) instead");
    DEPRECATED(SerializeStatus serialize(const Serializable& val),
               "Use serializeFrom(const Serializable& val) instead");

    DEPRECATED(SerializeStatus deserialize(U8& val), "Use deserializeTo(U8& val) instead");
    DEPRECATED(SerializeStatus deserialize(I8& val), "Use deserializeTo(I8& val) instead");
#if FW_HAS_16_BIT == 1
    DEPRECATED(SerializeStatus deserialize(U16& val), "Use deserializeTo(U16& val) instead");
    DEPRECATED(SerializeStatus deserialize(I16& val), "Use deserializeTo(I16& val) instead");
#endif
#if FW_HAS_32_BIT == 1
    DEPRECATED(SerializeStatus deserialize(U32& val), "Use deserializeTo(U32& val) instead");
    DEPRECATED(SerializeStatus deserialize(I32& val), "Use deserializeTo(I32& val) instead");
#endif
#if FW_HAS_64_BIT == 1
    DEPRECATED(SerializeStatus deserialize(U64& val), "Use deserializeTo(U64& val) instead");
    DEPRECATED(SerializeStatus deserialize(I64& val), "Use deserializeTo(I64& val) instead");
#endif

    DEPRECATED(SerializeStatus deserialize(F32& val), "Use deserializeTo(F32& val) instead");
    DEPRECATED(SerializeStatus deserialize(F64& val), "Use deserializeTo(F64& val) instead");
    DEPRECATED(SerializeStatus deserialize(bool& val), "Use deserializeTo(bool& val) instead");
    DEPRECATED(SerializeStatus deserialize(void*& val), "Use deserializeTo(void*& val) instead");
    DEPRECATED(SerializeStatus deserialize(U8* buff, FwSizeType& length, bool noLength),
               "Use deserialize(U8* buff, FwSizeType& length, Serialization::t mode) instead");
    DEPRECATED(SerializeStatus deserialize(U8* buff, FwSizeType& length),
               "Use deserializeTo(U8* buff, FwSizeType& length) instead");
    DEPRECATED(SerializeStatus deserialize(U8* buff, FwSizeType& length, Serialization::t mode),
               "Use deserializeTo(U8* buff, FwSizeType& length, Serialization::t mode) instead");
    DEPRECATED(SerializeStatus deserialize(Serializable& val), "Use deserializeTo(Serializable& val) instead");

#ifdef BUILD_UT
    //! \brief Equality comparison operator
    //!
    //! Compares this LinearBufferBase instance with another for equality.
    //! Two buffers are considered equal if they have the same contents and size.
    //!
    //! \param other Reference to the other LinearBufferBase instance to compare with
    //! \return true if the buffers are equal, false otherwise
    bool operator==(const LinearBufferBase& other) const;

    //! \brief Stream insertion operator for LinearBufferBase
    //!
    //! Allows a LinearBufferBase instance to be output to a stream (e.g., for debugging).
    //! This provides a human-readable representation of the buffer's contents.
    //!
    //! \param os Reference to the output stream
    //! \param buff Reference to the LinearBufferBase instance to output
    //! \return Reference to the output stream
    friend std::ostream& operator<<(std::ostream& os, const LinearBufferBase& buff);
#endif

  protected:
    //! \brief Default constructor
    //!
    //! Initializes a LinearBufferBase instance with default values.
    //! Sets the serialization and deserialization locations to zero.
    LinearBufferBase();

    //! \brief Copy constructor (protected)
    //!
    //! Creates a copy of another LinearBufferBase instance. This constructor
    //! is protected and intended for use only by derived classes or internal
    //! implementation details.
    //!
    //! \param src Reference to the source LinearBufferBase to copy from
    LinearBufferBase(const LinearBufferBase& src);

    //! \brief Copy data from source buffer
    //!
    //! Copies the contents from another LinearBufferBase instance to this one.
    //! This includes copying the buffer data, serialization location, and
    //! deserialization location.
    //!
    //! \param src Reference to the source LinearBufferBase to copy data from
    void copyFrom(const LinearBufferBase& src);

    Serializable::SizeType m_serLoc;    //!< current offset in buffer of serialized data
    Serializable::SizeType m_deserLoc;  //!< current offset for deserialization

  private:
};

// Helper classes for building buffers with external storage

//! \brief External serialize buffer with no copy semantics
//!
//! This class provides a serialization buffer that uses an external buffer
//! without copying data. It is designed for scenarios where you want to avoid
//! data copying for performance reasons and are willing to manage the buffer
//! lifetime manually.
//!
//! \note The external buffer must remain valid for the lifetime of the
//! ExternalSerializeBuffer instance. The class does not take ownership of
//! the buffer or copy its contents.
class ExternalSerializeBuffer : public LinearBufferBase {
  public:
    //! \brief Construct with external buffer
    //!
    //! Creates an ExternalSerializeBuffer instance that uses the provided
    //! external buffer. The buffer must remain valid for the lifetime of
    //! this instance.
    //!
    //! \param buffPtr Pointer to the external buffer
    //! \param size Size of the external buffer in bytes
    ExternalSerializeBuffer(U8* buffPtr, Serializable::SizeType size);

    //! \brief Default constructor
    //!
    //! Creates an ExternalSerializeBuffer instance without an external buffer.
    //! The buffer must be set later using setExtBuffer().
    ExternalSerializeBuffer();

    //! \brief Destructor
    //!
    //! Destroys the ExternalSerializeBuffer instance. Note that this does
    //! not free the external buffer if one was set.
    ~ExternalSerializeBuffer() {}

    //! \brief Set the external buffer
    //!
    //! Sets a new external buffer for this instance. This action also resets
    //! the serialization and deserialization pointers to the beginning of
    //! the new buffer.
    //!
    //! \param buffPtr Pointer to the new external buffer
    //! \param size Size of the new external buffer in bytes
    void setExtBuffer(U8* buffPtr, Serializable::SizeType size);

    //! \brief Clear external buffer
    //!
    //! Clears the external buffer reference. After calling this method,
    //! the buffer is effectively empty and cannot be used for serialization
    //! or deserialization until a new buffer is set.
    void clear();

    //! \brief Deleted copy constructor
    //!
    //! The copy constructor is deleted to prevent copying instances of
    //! ExternalSerializeBuffer, as this could lead to issues with buffer
    //! management.
    ExternalSerializeBuffer(const ExternalSerializeBuffer& src) = delete;

    DEPRECATED(Serializable::SizeType getBuffCapacity() const, "Use getCapacity() instead");

    //! \brief Get buffer capacity
    //!
    //! This method returns the total capacity of the buffer, which is the maximum
    //! amount of data that can be stored in the buffer. This is not the same as
    //! the current size, which indicates how much data is currently in the buffer.
    //!
    //! \return The capacity of the buffer in bytes
    Serializable::SizeType getCapacity() const;

    //! \brief Get buffer address for data filling (non-const version)
    //!
    //! This method returns a pointer to the buffer's data area where data can
    //! be written. This is the non-const version of the method, allowing the
    //! buffer contents to be modified.
    //!
    //! \return Pointer to the buffer's data area
    U8* getBuffAddr();

    //! \brief Get buffer address for data reading (const version)
    //!
    //! This method returns a const pointer to the buffer's data area where data
    //! can be read. This is the const version of the method, preventing modification
    //! of the buffer contents.
    //!
    //! \return Const pointer to the buffer's data area
    const U8* getBuffAddr() const;

    //! \brief Deleted copy assignment operator
    //!
    //! The copy assignment operator is deleted to prevent copying instances of
    //! ExternalSerializeBuffer, as this could lead to issues with buffer
    //! management.
    ExternalSerializeBuffer& operator=(const LinearBufferBase& src) = delete;

  protected:
    // data members
    U8* m_buff;                         //!< pointer to external buffer
    Serializable::SizeType m_buffSize;  //!< size of external buffer
};

//! \brief External serialize buffer with data copy semantics
//!
//! This class provides a serialization buffer that uses an external buffer
//! and performs data copying during assignment operations. It is designed
//! for scenarios where you want to ensure that the buffer always contains
//! valid data and are willing to pay the performance cost of copying.
//!
//! \note This class should be used when the object on the left-hand side of
//! an assignment (esb = sbb) is guaranteed to have a valid buffer.
//!
//! \see ExternalSerializeBuffer for a version without data copying
class ExternalSerializeBufferWithDataCopy final : public ExternalSerializeBuffer {
  public:
    //! \brief Construct with external buffer
    //!
    //! Creates an ExternalSerializeBufferWithDataCopy instance that uses the
    //! provided external buffer.
    //!
    //! \param buffPtr Pointer to the external buffer
    //! \param size Size of the external buffer in bytes
    ExternalSerializeBufferWithDataCopy(U8* buffPtr, Serializable::SizeType size)
        : ExternalSerializeBuffer(buffPtr, size) {}

    //! \brief Default constructor
    //!
    //! Creates an ExternalSerializeBufferWithDataCopy instance without an
    //! external buffer. The buffer must be set later using setExtBuffer().
    ExternalSerializeBufferWithDataCopy() : ExternalSerializeBuffer() {}

    //! \brief Destructor
    //!
    //! Destroys the ExternalSerializeBufferWithDataCopy instance.
    ~ExternalSerializeBufferWithDataCopy() {}

    //! \brief Deleted copy constructor
    //!
    //! The copy constructor is deleted to prevent copying instances of
    //! ExternalSerializeBufferWithDataCopy, as this could lead to issues
    //! with buffer management.
    ExternalSerializeBufferWithDataCopy(const LinearBufferBase& src) = delete;

    //! \brief Copy assignment operator with data copying
    //!
    //! This assignment operator copies data from the source buffer to the
    //! destination buffer. It ensures that the destination buffer always
    //! contains valid data after the assignment.
    //!
    //! \param src Reference to the source buffer to copy data from
    //! \return Reference to this instance
    ExternalSerializeBufferWithDataCopy& operator=(LinearBufferBase& src) {
        (void)LinearBufferBase::operator=(src);
        return *this;
    }
};

//! \brief External serialize buffer with member copy semantics
//!
//! This class provides a serialization buffer that uses an external buffer
//! and performs member copying during assignment operations. It is designed
//! for scenarios where you want to move data between buffers efficiently.
//!
//! \note This class should be used when the object on the left-hand side of
//! an assignment (esb1 = esb2) has an invalid buffer, and you want to move
//! the buffer of esb2 into it. In this case, there should usually be no more
//! uses of esb2 after the assignment.
//!
//! \see ExternalSerializeBuffer for a version without data copying
//! \see ExternalSerializeBufferWithDataCopy for a version with data copying
class ExternalSerializeBufferWithMemberCopy final : public ExternalSerializeBuffer {
  public:
    //! \brief Construct with external buffer
    //!
    //! Creates an ExternalSerializeBufferWithMemberCopy instance that uses the
    //! provided external buffer.
    //!
    //! \param buffPtr Pointer to the external buffer
    //! \param size Size of the external buffer in bytes
    ExternalSerializeBufferWithMemberCopy(U8* buffPtr, Serializable::SizeType size)
        : ExternalSerializeBuffer(buffPtr, size) {}

    //! \brief Default constructor
    //!
    //! Creates an ExternalSerializeBufferWithMemberCopy instance without an
    //! external buffer. The buffer must be set later using setExtBuffer().
    ExternalSerializeBufferWithMemberCopy() : ExternalSerializeBuffer() {}

    //! \brief Destructor
    //!
    //! Destroys the ExternalSerializeBufferWithMemberCopy instance.
    ~ExternalSerializeBufferWithMemberCopy() {}

    //! \brief Copy constructor with member copying
    //!
    //! This constructor copies members from the source instance, including
    //! the buffer pointer, size, serialization location, and deserialization
    //! location.
    //!
    //! \param src Reference to the source instance to copy from
    ExternalSerializeBufferWithMemberCopy(const ExternalSerializeBufferWithMemberCopy& src)
        : ExternalSerializeBuffer(src.m_buff, src.m_buffSize) {
        this->m_serLoc = src.m_serLoc;
        this->m_deserLoc = src.m_deserLoc;
    }

    //! \brief Copy assignment operator with member copying
    //!
    //! This assignment operator copies members from the source instance,
    //! including the buffer pointer, size, serialization location, and
    //! deserialization location. It guards against self-assignment.
    //!
    //! \param src Reference to the source instance to copy from
    //! \return Reference to this instance
    ExternalSerializeBufferWithMemberCopy& operator=(const ExternalSerializeBufferWithMemberCopy& src) {
        // Ward against self-assignment
        if (this != &src) {
            this->setExtBuffer(src.m_buff, src.m_buffSize);
            this->m_serLoc = src.m_serLoc;
            this->m_deserLoc = src.m_deserLoc;
        }
        return *this;
    }
};

}  // namespace Fw
#endif
