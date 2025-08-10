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
class SerializeBufferBase;  //!< forward declaration

class Serializable {
  public:
    // Size type for backwards compatibility
    using SizeType = FwSizeType;

  public:
    virtual SerializeStatus serializeTo(SerializeBufferBase& buffer) const = 0;  //!< serialize contents to buffer

    virtual SerializeStatus deserializeFrom(SerializeBufferBase& buffer) = 0;  //!< deserialize contents from buffer

    // ----------------------------------------------------------------------
    // Legacy methods for backward compatibility
    // ----------------------------------------------------------------------

    SerializeStatus serialize(SerializeBufferBase& buffer) const { return this->serializeTo(buffer); }

    SerializeStatus deserialize(SerializeBufferBase& buffer) { return this->deserializeFrom(buffer); }

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

class Serialization {
  public:
    enum t {
        INCLUDE_LENGTH,  //!< Include length as first token in serialization
        OMIT_LENGTH      //!< Omit length from serialization
    };
};

class SerializeBufferBase {
    friend class SerializeBufferBaseTester;

  protected:
    SerializeBufferBase& operator=(const SerializeBufferBase& src);  //!< copy assignment operator

  public:
    virtual ~SerializeBufferBase();  //!< destructor

    // Serialization for built-in types

    SerializeStatus serializeFrom(U8 val);  //!< serialize 8-bit unsigned int
    SerializeStatus serializeFrom(I8 val);  //!< serialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    SerializeStatus serializeFrom(U16 val);  //!< serialize 16-bit unsigned int
    SerializeStatus serializeFrom(I16 val);  //!< serialize 16-bit signed int
#endif
#if FW_HAS_32_BIT == 1
    SerializeStatus serializeFrom(U32 val);  //!< serialize 32-bit unsigned int
    SerializeStatus serializeFrom(I32 val);  //!< serialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus serializeFrom(U64 val);  //!< serialize 64-bit unsigned int
    SerializeStatus serializeFrom(I64 val);  //!< serialize 64-bit signed int
#endif
    SerializeStatus serializeFrom(F32 val);   //!< serialize 32-bit floating point
    SerializeStatus serializeFrom(F64 val);   //!< serialize 64-bit floating point
    SerializeStatus serializeFrom(bool val);  //!< serialize boolean

    SerializeStatus serializeFrom(
        const void* val);  //!< serialize pointer (careful, only pointer value, not contents are serialized)

    //! serialize data buffer
    SerializeStatus serializeFrom(const U8* buff, FwSizeType length);

    //! \brief serialize a byte buffer of a given length
    //!
    //! Serialize bytes from `buff` up to `length`.  If `serializationMode` is set to `INCLUDE_LENGTH` then the
    //! length is included as the first token. Length may be omitted with `OMIT_LENGTH`.
    //!
    //! \param buff: buffer to serialize
    //! \param length: length of data to serialize
    //! \param mode: serialization type
    //! \return status of serialization
    SerializeStatus serializeFrom(const U8* buff, FwSizeType length, Serialization::t mode);

    SerializeStatus serializeFrom(const SerializeBufferBase& val);  //!< serialize a serialized buffer

    SerializeStatus serializeFrom(
        const Serializable& val);  //!< serialize an object derived from serializable base class

    SerializeStatus serializeSize(const FwSizeType size);  //!< serialize a size value

    // Deserialization for built-in types

    SerializeStatus deserializeTo(U8& val);  //!< deserialize 8-bit unsigned int
    SerializeStatus deserializeTo(I8& val);  //!< deserialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    SerializeStatus deserializeTo(U16& val);  //!< deserialize 16-bit unsigned int
    SerializeStatus deserializeTo(I16& val);  //!< deserialize 16-bit signed int
#endif

#if FW_HAS_32_BIT == 1
    SerializeStatus deserializeTo(U32& val);  //!< deserialize 32-bit unsigned int
    SerializeStatus deserializeTo(I32& val);  //!< deserialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus deserializeTo(U64& val);  //!< deserialize 64-bit unsigned int
    SerializeStatus deserializeTo(I64& val);  //!< deserialize 64-bit signed int
#endif
    SerializeStatus deserializeTo(F32& val);   //!< deserialize 32-bit floating point
    SerializeStatus deserializeTo(F64& val);   //!< deserialize 64-bit floating point
    SerializeStatus deserializeTo(bool& val);  //!< deserialize boolean

    SerializeStatus deserializeTo(void*& val);  //!< deserialize point value (careful, pointer value only, not contents)

    SerializeStatus deserializeTo(U8* buff, FwSizeType& length);  //!< deserialize data buffer

    //! \brief deserialize a byte buffer of a given length
    //!
    //! The `mode` parameter specifies whether the serialized length should be read from the buffer.
    //! \param buff: buffer to deserialize into
    //! \param length: length of the buffer, updated with the actual deserialized length
    //! \param mode: deserialization type
    //! \return status of serialization
    SerializeStatus deserializeTo(U8* buff, FwSizeType& length, Serialization::t mode);

    SerializeStatus deserializeTo(Serializable& val);  //!< deserialize an object derived from serializable base class

    SerializeStatus deserializeTo(SerializeBufferBase& val);  //!< serialize a serialized buffer

    SerializeStatus deserializeSize(FwSizeType& size);  //!< deserialize a size value

    // ----------------------------------------------------------------------
    // Serialization methods
    // ----------------------------------------------------------------------

    SerializeStatus serialize(U8 val);
    SerializeStatus serialize(I8 val);
#if FW_HAS_16_BIT == 1
    SerializeStatus serialize(U16 val);
    SerializeStatus serialize(I16 val);
#endif
#if FW_HAS_32_BIT == 1
    SerializeStatus serialize(U32 val);
    SerializeStatus serialize(I32 val);
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus serialize(U64 val);
    SerializeStatus serialize(I64 val);
#endif
    SerializeStatus serialize(F32 val);
    SerializeStatus serialize(F64 val);
    SerializeStatus serialize(bool val);
    SerializeStatus serialize(const void* val);
    DEPRECATED(SerializeStatus serialize(const U8* buff, FwSizeType length, bool noLength),
               "Use serialize(const U8* buff, FwSizeType length, Serialization::t mode) instead");
    SerializeStatus serialize(const U8* buff, FwSizeType length);
    SerializeStatus serialize(const U8* buff, FwSizeType length, Serialization::t mode);
    SerializeStatus serialize(const Serializable& val);
    SerializeStatus serialize(const SerializeBufferBase& val);

    SerializeStatus deserialize(U8& val);
    SerializeStatus deserialize(I8& val);
#if FW_HAS_16_BIT == 1
    SerializeStatus deserialize(U16& val);
    SerializeStatus deserialize(I16& val);
#endif
#if FW_HAS_32_BIT == 1
    SerializeStatus deserialize(U32& val);
    SerializeStatus deserialize(I32& val);
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus deserialize(U64& val);
    SerializeStatus deserialize(I64& val);
#endif
    SerializeStatus deserialize(F32& val);
    SerializeStatus deserialize(F64& val);
    SerializeStatus deserialize(bool& val);
    SerializeStatus deserialize(void*& val);
    DEPRECATED(SerializeStatus deserialize(U8* buff, FwSizeType& length, bool noLength),
               "Use deserialize(U8* buff, FwSizeType& length, Serialization::t mode) instead");
    SerializeStatus deserialize(U8* buff, FwSizeType& length);
    SerializeStatus deserialize(U8* buff, FwSizeType& length, Serialization::t mode);
    SerializeStatus deserialize(Serializable& val);
    SerializeStatus deserialize(SerializeBufferBase& val);

    void resetSer();    //!< reset to beginning of buffer to reuse for serialization
    void resetDeser();  //!< reset deserialization to beginning

    SerializeStatus moveSerToOffset(FwSizeType offset);    //!< Moves serialization to the specified offset
    SerializeStatus moveDeserToOffset(FwSizeType offset);  //!< Moves deserialization to the specified offset

    SerializeStatus serializeSkip(
        FwSizeType numBytesToSkip);  //!< Skips the number of specified bytes for serialization
    SerializeStatus deserializeSkip(
        FwSizeType numBytesToSkip);  //!< Skips the number of specified bytes for deserialization
    virtual Serializable::SizeType getBuffCapacity() const = 0;  //!< returns capacity, not current size, of buffer
    Serializable::SizeType getBuffLength() const;                //!< returns current buffer size
    Serializable::SizeType getBuffLeft() const;                  //!< returns how much deserialization buffer is left
    virtual U8* getBuffAddr() = 0;                               //!< gets buffer address for data filling
    virtual const U8* getBuffAddr() const = 0;  //!< gets buffer address for data reading, const version
    const U8* getBuffAddrLeft() const;          //!< gets address of remaining non-deserialized data.
    U8* getBuffAddrSer();  //!< gets address of end of serialization. DANGEROUS! Need to know max buffer size and adjust
                           //!< when done
    SerializeStatus setBuff(const U8* src, Serializable::SizeType length);  //!< sets buffer contents and size
    SerializeStatus setBuffLen(Serializable::SizeType length);  //!< sets buffer length manually after filling with data
    SerializeStatus copyRaw(
        SerializeBufferBase& dest,
        Serializable::SizeType size);  //!< directly copies buffer without looking for a size in the stream.
                                       // Will increment deserialization pointer
    SerializeStatus copyRawOffset(
        SerializeBufferBase& dest,
        Serializable::SizeType size);  //!< directly copies buffer without looking for a size in the stream.
                                       // Will increment deserialization pointer

#ifdef BUILD_UT
    bool operator==(const SerializeBufferBase& other) const;
    friend std::ostream& operator<<(std::ostream& os, const SerializeBufferBase& buff);
#endif

  protected:
    SerializeBufferBase();              //!< default constructor
    Serializable::SizeType m_serLoc;    //!< current offset in buffer of serialized data
    Serializable::SizeType m_deserLoc;  //!< current offset for deserialization

  private:
    // Copy constructor can be used only by the implementation
    SerializeBufferBase(const SerializeBufferBase& src);  //!< constructor with buffer as source

    void copyFrom(const SerializeBufferBase& src);  //!< copy data from source buffer
};

// Helper classes for building buffers with external storage

//! External serialize buffer with no copy semantics
class ExternalSerializeBuffer : public SerializeBufferBase {
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
    Serializable::SizeType getBuffCapacity() const;
    U8* getBuffAddr();
    const U8* getBuffAddr() const;

    //! deleted copy assignment operator
    ExternalSerializeBuffer& operator=(const SerializeBufferBase& src) = delete;

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
    ExternalSerializeBufferWithDataCopy(const SerializeBufferBase& src) = delete;
    ExternalSerializeBufferWithDataCopy& operator=(SerializeBufferBase& src) {
        (void)SerializeBufferBase::operator=(src);
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
