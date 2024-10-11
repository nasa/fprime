#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#ifdef BUILD_UT
#include <iostream>
#endif

#include <FpConfig.hpp>
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
    using SizeType = NATIVE_UINT_TYPE;

  public:
    virtual SerializeStatus serialize(SerializeBufferBase& buffer) const = 0;  //!< serialize contents
    virtual SerializeStatus deserialize(SerializeBufferBase& buffer) = 0;      //!< deserialize to contents
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
  protected:
    SerializeBufferBase& operator=(const SerializeBufferBase& src);  //!< copy assignment operator

  public:
    virtual ~SerializeBufferBase();  //!< destructor

    // Serialization for built-in types

    SerializeStatus serialize(U8 val);  //!< serialize 8-bit unsigned int
    SerializeStatus serialize(I8 val);  //!< serialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    SerializeStatus serialize(U16 val);  //!< serialize 16-bit unsigned int
    SerializeStatus serialize(I16 val);  //!< serialize 16-bit signed int
#endif
#if FW_HAS_32_BIT == 1
    SerializeStatus serialize(U32 val);  //!< serialize 32-bit unsigned int
    SerializeStatus serialize(I32 val);  //!< serialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus serialize(U64 val);  //!< serialize 64-bit unsigned int
    SerializeStatus serialize(I64 val);  //!< serialize 64-bit signed int
#endif
    SerializeStatus serialize(F32 val);  //!< serialize 32-bit floating point
#if FW_HAS_F64
    SerializeStatus serialize(F64 val);  //!< serialize 64-bit floating point
#endif
    SerializeStatus serialize(bool val);  //!< serialize boolean

    SerializeStatus serialize(
        const void* val);  //!< serialize pointer (careful, only pointer value, not contents are serialized)

    //! serialize data buffer
    SerializeStatus serialize(const U8* buff, NATIVE_UINT_TYPE length, bool noLength);
    //! serialize data buffer
    SerializeStatus serialize(const U8* buff, NATIVE_UINT_TYPE length);

    //! \brief serialize a byte buffer of a given length
    //!
    //! Serialize bytes from `buff` up to `length`.  If `serializationMode` is set to `INCLUDE_LENGTH` then the
    //! length is included as the first token. Length may be omitted with `OMIT_LENGTH`.
    //!
    //! \param buff: buffer to serialize
    //! \param length: length of data to serialize
    //! \param mode: serialization type
    //! \return status of serialization
    SerializeStatus serialize(const U8* buff, FwSizeType length, Serialization::t mode);

    SerializeStatus serialize(const SerializeBufferBase& val);  //!< serialize a serialized buffer

    SerializeStatus serialize(const Serializable& val);  //!< serialize an object derived from serializable base class

    SerializeStatus serializeSize(const FwSizeType size);  //!< serialize a size value

    // Deserialization for built-in types

    SerializeStatus deserialize(U8& val);  //!< deserialize 8-bit unsigned int
    SerializeStatus deserialize(I8& val);  //!< deserialize 8-bit signed int

#if FW_HAS_16_BIT == 1
    SerializeStatus deserialize(U16& val);  //!< deserialize 16-bit unsigned int
    SerializeStatus deserialize(I16& val);  //!< deserialize 16-bit signed int
#endif

#if FW_HAS_32_BIT == 1
    SerializeStatus deserialize(U32& val);  //!< deserialize 32-bit unsigned int
    SerializeStatus deserialize(I32& val);  //!< deserialize 32-bit signed int
#endif
#if FW_HAS_64_BIT == 1
    SerializeStatus deserialize(U64& val);  //!< deserialize 64-bit unsigned int
    SerializeStatus deserialize(I64& val);  //!< deserialize 64-bit signed int
#endif
    SerializeStatus deserialize(F32& val);  //!< deserialize 32-bit floating point
#if FW_HAS_F64
    SerializeStatus deserialize(F64& val);  //!< deserialize 64-bit floating point
#endif
    SerializeStatus deserialize(bool& val);  //!< deserialize boolean

    SerializeStatus deserialize(void*& val);  //!< deserialize point value (careful, pointer value only, not contents)

    //! deserialize data buffer
    SerializeStatus deserialize(U8* buff, NATIVE_UINT_TYPE& length, bool noLength);

    //! deserialize data buffer
    SerializeStatus deserialize(U8* buff, NATIVE_UINT_TYPE& length);
    //! \brief deserialize a byte buffer of a given length
    //!
    //! Deserialize bytes into `buff` of `length` bytes.  If `serializationMode` is set to `INCLUDE_LENGTH` then
    //! the length is deserialized first followed by the bytes. Length may be omitted with `OMIT_LENGTH` and
    //! in this case `length` bytes will be deserialized. `length` will be filled with the amount of data
    //! deserialized.
    //!
    //! \param buff: buffer to hold deserialized data
    //! \param length: length of data to deserialize length is filled with deserialized length
    //! \param mode: deserialization type
    //! \return status of serialization
    SerializeStatus deserialize(U8* buff, FwSizeType& length, Serialization::t mode);

    SerializeStatus deserialize(Serializable& val);  //!< deserialize an object derived from serializable base class

    SerializeStatus deserialize(SerializeBufferBase& val);  //!< serialize a serialized buffer

    SerializeStatus deserializeSize(FwSizeType& size);  //!< deserialize a size value

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

  PROTECTED:
    SerializeBufferBase();  //!< default constructor

  PRIVATE:
    // Copy constructor can be used only by the implementation
    SerializeBufferBase(const SerializeBufferBase& src);  //!< constructor with buffer as source

    void copyFrom(const SerializeBufferBase& src);  //!< copy data from source buffer
    Serializable::SizeType m_serLoc;                //!< current offset in buffer of serialized data
    Serializable::SizeType m_deserLoc;              //!< current offset for deserialization
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

  PROTECTED:
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
    explicit ExternalSerializeBufferWithMemberCopy(const ExternalSerializeBufferWithMemberCopy& src)
        : ExternalSerializeBuffer(src.m_buff, src.m_buffSize) {}
    ExternalSerializeBufferWithMemberCopy& operator=(const ExternalSerializeBufferWithMemberCopy& src) {
        // Ward against self-assignment
        if (this != &src) {
            this->setExtBuffer(src.m_buff, src.m_buffSize);
        }
        return *this;
    }
};

}  // namespace Fw
#endif
