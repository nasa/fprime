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
    FW_DESERIALIZE_TYPE_MISMATCH   //!< Deserialized type ID didn't match
} SerializeStatus;

class SerialBufferBase;     //!< forward declaration
class LinearBufferBase;  //!< forward declaration

// TODO: Temporary backwards-compatibility hack. 
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
    virtual SerializeStatus serializeTo(SerialBufferBase& buffer,
                                        Endianness mode = Endianness::BIG) const = 0;  //!< serialize contents to buffer

    virtual SerializeStatus deserializeFrom(
        SerialBufferBase& buffer,
        Endianness mode = Endianness::BIG) = 0;  //!< deserialize contents from buffer

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
    virtual void toString(StringBase& text) const;  //!< generate text from serializable
#endif

#ifdef BUILD_UT
    friend std::ostream& operator<<(std::ostream& os, const Serializable& val);
#endif

  protected:
    Serializable();           //!< Default constructor
    virtual ~Serializable();  //!< destructor
};

class SerialBufferBase {
  public:
    virtual ~SerialBufferBase();  //!< destructor
    // Serialization for built-in types

    virtual SerializeStatus serializeFrom(U8 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 8-bit unsigned int
    virtual SerializeStatus serializeFrom(I8 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    virtual SerializeStatus serializeFrom(U16 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 16-bit unsigned int
    virtual SerializeStatus serializeFrom(I16 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 16-bit signed int
#endif
#if FW_HAS_32_BIT == 1
    virtual SerializeStatus serializeFrom(U32 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 32-bit unsigned int
    virtual SerializeStatus serializeFrom(I32 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    virtual SerializeStatus serializeFrom(U64 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 64-bit unsigned int
    virtual SerializeStatus serializeFrom(I64 val, Endianness mode = Endianness::BIG) = 0;  //!< serialize 64-bit signed int
#endif
    virtual SerializeStatus serializeFrom(F32 val, Endianness mode = Endianness::BIG) = 0;   //!< serialize 32-bit floating point
    virtual SerializeStatus serializeFrom(F64 val, Endianness mode = Endianness::BIG) = 0;   //!< serialize 64-bit floating point
    virtual SerializeStatus serializeFrom(bool val, Endianness mode = Endianness::BIG) = 0;  //!< serialize boolean

    virtual SerializeStatus serializeFrom(const void* val,
                                  Endianness mode = Endianness::BIG) = 0;  //!< serialize pointer (careful, only pointer
                                                                       //!< value, not contents are serialized)

    //! serialize data buffer
    virtual SerializeStatus serializeFrom(const U8* buff, FwSizeType length, Endianness endianMode = Endianness::BIG) = 0;

    //! \brief serialize a byte buffer of a given length
    //!
    //! Serialize bytes from `buff` up to `length`.  If `serializationMode` is set to `INCLUDE_LENGTH` then the
    //! length is included as the first token. Length may be omitted with `OMIT_LENGTH`.
    //!
    //! \param buff: buffer to serialize
    //! \param length: length of data to serialize
    //! \param mode: serialization type
    //! \return status of serialization
    virtual SerializeStatus serializeFrom(const U8* buff,
                                  FwSizeType length,
                                  Serialization::t lengthMode,
                                  Endianness endianMode = Endianness::BIG) = 0;

    virtual SerializeStatus serializeFrom(const LinearBufferBase& val,
                                  Endianness mode = Endianness::BIG) = 0;  //!< serialize a serialized buffer

    virtual SerializeStatus serializeFrom(
        const Serializable& val,
        Endianness mode = Endianness::BIG) = 0;  //!< serialize an object derived from serializable base class

    virtual SerializeStatus serializeSize(const FwSizeType size,
                                  Endianness mode = Endianness::BIG) = 0;  //!< serialize a size value

    // Deserialization for built-in types

    virtual SerializeStatus deserializeTo(U8& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 8-bit unsigned int
    virtual SerializeStatus deserializeTo(I8& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    virtual SerializeStatus deserializeTo(U16& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 16-bit unsigned int
    virtual SerializeStatus deserializeTo(I16& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 16-bit signed int
#endif

#if FW_HAS_32_BIT == 1
    virtual SerializeStatus deserializeTo(U32& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 32-bit unsigned int
    virtual SerializeStatus deserializeTo(I32& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    virtual SerializeStatus deserializeTo(U64& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 64-bit unsigned int
    virtual SerializeStatus deserializeTo(I64& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize 64-bit signed int
#endif
    virtual SerializeStatus deserializeTo(F32& val, Endianness mode = Endianness::BIG) = 0;   //!< deserialize 32-bit floating point
    virtual SerializeStatus deserializeTo(F64& val, Endianness mode = Endianness::BIG) = 0;   //!< deserialize 64-bit floating point
    virtual SerializeStatus deserializeTo(bool& val, Endianness mode = Endianness::BIG) = 0;  //!< deserialize boolean

    virtual SerializeStatus deserializeTo(
        void*& val,
        Endianness mode = Endianness::BIG) = 0;  //!< deserialize point value (careful, pointer value only, not contents)

    virtual SerializeStatus deserializeTo(U8* buff,
                                  FwSizeType& length,
                                  Endianness endianMode = Endianness::BIG) = 0;  //!< deserialize data buffer

    //! \brief deserialize a byte buffer of a given length
    //!
    //! The `mode` parameter specifies whether the serialized length should be read from the buffer.
    //! \param buff: buffer to deserialize into
    //! \param length: length of the buffer, updated with the actual deserialized length
    //! \param mode: deserialization type
    //! \return status of serialization
    virtual SerializeStatus deserializeTo(U8* buff,
                                  FwSizeType& length,
                                  Serialization::t lengthMode,
                                  Endianness endianMode = Endianness::BIG) = 0;

    virtual SerializeStatus deserializeTo(
        Serializable& val,
        Endianness mode = Endianness::BIG) = 0;  //!< deserialize an object derived from serializable base class

    virtual SerializeStatus deserializeTo(LinearBufferBase& val,
                                  Endianness mode = Endianness::BIG) = 0;  //!< serialize a serialized buffer

    virtual SerializeStatus deserializeSize(FwSizeType& size, Endianness mode = Endianness::BIG) = 0;  //!< deserialize a size value

    //! \brief Copy raw bytes from the source (this) into a destination buffer and advance source offset
    //!
    //! Copies exactly `size` bytes starting at the current deserialization pointer of `this` into `dest`.
    //! This operation does not prepend a length field and does not interpret the data.
    //!
    //! Preconditions:
    //! - `size` bytes must remain in the source (`getDeserializeSizeLeft() >= size`).
    //! - Destination must have sufficient capacity (`dest.getCapacity() >= size`).
    //!
    //! Postconditions on success:
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
    //! Postconditions on success:
    //! - `dest` gains `size` additional bytes at the end; no length token is written.
    //! - `this` has advanced its deserialization pointer by `size` bytes.
    //!
    //! \param dest Destination serialization buffer to append to
    //! \param size Number of bytes to copy from the source and append to dest
    //! \return `FW_SERIALIZE_OK` on success; `FW_SERIALIZE_NO_ROOM_LEFT` if destination capacity is insufficient;
    //!         `FW_DESERIALIZE_SIZE_MISMATCH` if source does not contain `size` bytes remaining
    virtual SerializeStatus copyRawOffset(SerialBufferBase& dest, Serializable::SizeType size) = 0;

    virtual void resetSer() = 0;    //!< reset to beginning of buffer to reuse for serialization
    virtual void resetDeser() = 0;  //!< reset deserialization to beginning

    virtual SerializeStatus moveSerToOffset(FwSizeType offset) = 0;    //!< Moves serialization to the specified offset
    virtual SerializeStatus moveDeserToOffset(FwSizeType offset) = 0;  //!< Moves deserialization to the specified offset

    virtual SerializeStatus serializeSkip(
        FwSizeType numBytesToSkip) = 0;  //!< Skips the number of specified bytes for serialization
    virtual SerializeStatus deserializeSkip(
        FwSizeType numBytesToSkip) = 0;  //!< Skips the number of specified bytes for deserialization

    virtual Serializable::SizeType getCapacity() const = 0; //!< returns capacity, not current size, of buffer
    virtual Serializable::SizeType getSize() const = 0;  //!< returns current buffer size
    virtual Serializable::SizeType getDeserializeSizeLeft() const = 0; //!< returns how much deserialization buffer is left
    virtual Serializable::SizeType getSerializeSizeLeft() const = 0; //!< returns how much serialization buffer is left

    virtual SerializeStatus setBuff(const U8* src, Serializable::SizeType length) = 0;  //!< sets buffer contents and size
    virtual SerializeStatus setBuffLen(Serializable::SizeType length) = 0;  //!< sets buffer length manually after filling with data
};

class LinearBufferBase : public SerialBufferBase {
    friend class SerializeBufferBaseTester;

  protected:
    LinearBufferBase& operator=(const LinearBufferBase& src);  //!< copy assignment operator

  public:
    virtual ~LinearBufferBase();  //!< destructor

    // Serialization for built-in types

    SerializeStatus serializeFrom(U8 val, Endianness mode = Endianness::BIG) override;  //!< serialize 8-bit unsigned int
    SerializeStatus serializeFrom(I8 val, Endianness mode = Endianness::BIG) override;  //!< serialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    SerializeStatus serializeFrom(U16 val, Endianness mode = Endianness::BIG) override;  //!< serialize 16-bit unsigned int
    SerializeStatus serializeFrom(I16 val, Endianness mode = Endianness::BIG) override;  //!< serialize 16-bit signed int
#endif
#if FW_HAS_32_BIT == 1
    SerializeStatus serializeFrom(U32 val, Endianness mode = Endianness::BIG) override;  //!< serialize 32-bit unsigned int
    SerializeStatus serializeFrom(I32 val, Endianness mode = Endianness::BIG) override;  //!< serialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus serializeFrom(U64 val, Endianness mode = Endianness::BIG) override;  //!< serialize 64-bit unsigned int
    SerializeStatus serializeFrom(I64 val, Endianness mode = Endianness::BIG) override;  //!< serialize 64-bit signed int
#endif
    SerializeStatus serializeFrom(F32 val, Endianness mode = Endianness::BIG) override;   //!< serialize 32-bit floating point
    SerializeStatus serializeFrom(F64 val, Endianness mode = Endianness::BIG) override;   //!< serialize 64-bit floating point
    SerializeStatus serializeFrom(bool val, Endianness mode = Endianness::BIG) override;  //!< serialize boolean

    SerializeStatus serializeFrom(const void* val,
                                  Endianness mode = Endianness::BIG) override;  //!< serialize pointer (careful, only pointer
                                                                       //!< value, not contents are serialized)

    //! serialize data buffer
    SerializeStatus serializeFrom(const U8* buff, FwSizeType length, Endianness endianMode = Endianness::BIG) override;

    //! \brief serialize a byte buffer of a given length
    //!
    //! Serialize bytes from `buff` up to `length`.  If `serializationMode` is set to `INCLUDE_LENGTH` then the
    //! length is included as the first token. Length may be omitted with `OMIT_LENGTH`.
    //!
    //! \param buff: buffer to serialize
    //! \param length: length of data to serialize
    //! \param mode: serialization type
    //! \return status of serialization
    SerializeStatus serializeFrom(const U8* buff,
                                  FwSizeType length,
                                  Serialization::t lengthMode,
                                  Endianness endianMode = Endianness::BIG) override;

    SerializeStatus serializeFrom(const LinearBufferBase& val,
                                  Endianness mode = Endianness::BIG) override;  //!< serialize a serialized buffer

    SerializeStatus serializeFrom(
        const Serializable& val,
        Endianness mode = Endianness::BIG) override;  //!< serialize an object derived from serializable base class

    SerializeStatus serializeSize(const FwSizeType size,
                                  Endianness mode = Endianness::BIG) override;  //!< serialize a size value

    // Deserialization for built-in types

    SerializeStatus deserializeTo(U8& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 8-bit unsigned int
    SerializeStatus deserializeTo(I8& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    SerializeStatus deserializeTo(U16& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 16-bit unsigned int
    SerializeStatus deserializeTo(I16& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 16-bit signed int
#endif

#if FW_HAS_32_BIT == 1
    SerializeStatus deserializeTo(U32& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 32-bit unsigned int
    SerializeStatus deserializeTo(I32& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus deserializeTo(U64& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 64-bit unsigned int
    SerializeStatus deserializeTo(I64& val, Endianness mode = Endianness::BIG) override;  //!< deserialize 64-bit signed int
#endif
    SerializeStatus deserializeTo(F32& val, Endianness mode = Endianness::BIG) override;   //!< deserialize 32-bit floating point
    SerializeStatus deserializeTo(F64& val, Endianness mode = Endianness::BIG) override;   //!< deserialize 64-bit floating point
    SerializeStatus deserializeTo(bool& val, Endianness mode = Endianness::BIG) override;  //!< deserialize boolean

    SerializeStatus deserializeTo(
        void*& val,
        Endianness mode = Endianness::BIG) override;  //!< deserialize point value (careful, pointer value only, not contents)

    SerializeStatus deserializeTo(U8* buff,
                                  FwSizeType& length,
                                  Endianness endianMode = Endianness::BIG) override;  //!< deserialize data buffer

    //! \brief deserialize a byte buffer of a given length
    //!
    //! The `mode` parameter specifies whether the serialized length should be read from the buffer.
    //! \param buff: buffer to deserialize into
    //! \param length: length of the buffer, updated with the actual deserialized length
    //! \param mode: deserialization type
    //! \return status of serialization
    SerializeStatus deserializeTo(U8* buff,
                                  FwSizeType& length,
                                  Serialization::t lengthMode,
                                  Endianness endianMode = Endianness::BIG) override;

    SerializeStatus deserializeTo(
        Serializable& val,
        Endianness mode = Endianness::BIG) override;  //!< deserialize an object derived from serializable base class

    SerializeStatus deserializeTo(LinearBufferBase& val,
                                  Endianness mode = Endianness::BIG) override;  //!< serialize a serialized buffer

    SerializeStatus deserializeSize(FwSizeType& size, Endianness mode = Endianness::BIG) override;  //!< deserialize a size value

    DEPRECATED(SerializeStatus serialize(const LinearBufferBase& val),
               "Use serializeFrom(const SerialBufferBase& val) instead");
    DEPRECATED(SerializeStatus deserialize(LinearBufferBase& val),
               "Use deserializeTo(SerialBufferBase& val) instead");

    void resetSer() override;    //!< reset to beginning of buffer to reuse for serialization
    void resetDeser() override;  //!< reset deserialization to beginning

    SerializeStatus moveSerToOffset(FwSizeType offset) override;    //!< Moves serialization to the specified offset
    SerializeStatus moveDeserToOffset(FwSizeType offset) override;  //!< Moves deserialization to the specified offset

    SerializeStatus serializeSkip(
        FwSizeType numBytesToSkip) override;  //!< Skips the number of specified bytes for serialization
    SerializeStatus deserializeSkip(
        FwSizeType numBytesToSkip) override;  //!< Skips the number of specified bytes for deserialization

    DEPRECATED(Serializable::SizeType getBuffCapacity() const, "Use getCapacity() instead");
    DEPRECATED(Serializable::SizeType getBuffLength() const, "Use getSize() instead"); 
    DEPRECATED(Serializable::SizeType getBuffLeft(), "Use getDeserializeSizeLeft() instead");

    virtual Serializable::SizeType getCapacity() const override = 0;     //!< returns capacity, not current size, of buffer
    Serializable::SizeType getSize() const override;                     //!< returns current buffer size
    Serializable::SizeType getDeserializeSizeLeft() const override;      //!< returns how much deserialization buffer is left
    Serializable::SizeType getSerializeSizeLeft() const override;        //!< returns how much serialization space is left

    virtual U8* getBuffAddr() = 0;                               //!< gets buffer address for data filling
    virtual const U8* getBuffAddr() const = 0;  //!< gets buffer address for data reading, const version
    const U8* getBuffAddrLeft() const;          //!< gets address of remaining non-deserialized data.
    U8* getBuffAddrSer();  //!< gets address of end of serialization. DANGEROUS! Need to know max buffer size and adjust
                           //!< when done
    SerializeStatus setBuff(const U8* src, Serializable::SizeType length) override;  //!< sets buffer contents and size
    SerializeStatus setBuffLen(Serializable::SizeType length) override;  //!< sets buffer length manually after filling with data
    
    SerializeStatus copyRaw(SerialBufferBase& dest, Serializable::SizeType size) override;
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
    bool operator==(const LinearBufferBase& other) const;
    friend std::ostream& operator<<(std::ostream& os, const LinearBufferBase& buff);
#endif

  protected:
    LinearBufferBase();              //!< default constructor
    Serializable::SizeType m_serLoc;    //!< current offset in buffer of serialized data
    Serializable::SizeType m_deserLoc;  //!< current offset for deserialization

  private:
    // Copy constructor can be used only by the implementation
    LinearBufferBase(const LinearBufferBase& src);  //!< constructor with buffer as source

    void copyFrom(const LinearBufferBase& src);  //!< copy data from source buffer
};

// Helper classes for building buffers with external storage

//! External serialize buffer with no copy semantics
class ExternalSerializeBuffer : public LinearBufferBase {
  public:
    ExternalSerializeBuffer(U8* buffPtr, Serializable::SizeType size);  //!< construct with external buffer
    ExternalSerializeBuffer();                                          //!< default constructor
    ~ExternalSerializeBuffer() {}                                       //!< destructor
    //! Set the external buffer
    //! This action also resets the serialization and deserialization pointers
    void setExtBuffer(U8* buffPtr, Serializable::SizeType size);
    void clear();                                                          //!< clear external buffer
    ExternalSerializeBuffer(const ExternalSerializeBuffer& src) = delete;  //!< deleted copy constructor

    // pure virtual functions
    DEPRECATED(Serializable::SizeType getBuffCapacity() const, "Use getCapacity() instead");
    Serializable::SizeType getCapacity() const;
    U8* getBuffAddr();
    const U8* getBuffAddr() const;

    //! deleted copy assignment operator
    ExternalSerializeBuffer& operator=(const LinearBufferBase& src) = delete;

  protected:
    // data members
    U8* m_buff;                         //!< pointer to external buffer
    Serializable::SizeType m_buffSize;  //!< size of external buffer
};

//! External serialize buffer with data copy semantics
//!
//! Use this when the object esb on the left-hand side of an assignment esb = sbb
//! is guaranteed to have a valid buffer
class ExternalSerializeBufferWithDataCopy final : public ExternalSerializeBuffer {
  public:
    ExternalSerializeBufferWithDataCopy(U8* buffPtr, Serializable::SizeType size)
        : ExternalSerializeBuffer(buffPtr, size) {}
    ExternalSerializeBufferWithDataCopy() : ExternalSerializeBuffer() {}
    ~ExternalSerializeBufferWithDataCopy() {}
    ExternalSerializeBufferWithDataCopy(const LinearBufferBase& src) = delete;
    ExternalSerializeBufferWithDataCopy& operator=(LinearBufferBase& src) {
        (void)LinearBufferBase::operator=(src);
        return *this;
    }
};

//! External serialize buffer with member copy semantics
//!
//! Use this when the object esb1 on the left-hand side of an assignment esb1 = esb2
//! has an invalid buffer, and you want to move the buffer of esb2 into it.
//! In this case there should usually be no more uses of esb2 after the assignment.
class ExternalSerializeBufferWithMemberCopy final : public ExternalSerializeBuffer {
  public:
    ExternalSerializeBufferWithMemberCopy(U8* buffPtr, Serializable::SizeType size)
        : ExternalSerializeBuffer(buffPtr, size) {}
    ExternalSerializeBufferWithMemberCopy() : ExternalSerializeBuffer() {}
    ~ExternalSerializeBufferWithMemberCopy() {}
    ExternalSerializeBufferWithMemberCopy(const ExternalSerializeBufferWithMemberCopy& src)
        : ExternalSerializeBuffer(src.m_buff, src.m_buffSize) {
        this->m_serLoc = src.m_serLoc;
        this->m_deserLoc = src.m_deserLoc;
    }
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
