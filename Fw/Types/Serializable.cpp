#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Types/StringType.hpp>
#include <cstdio>
#include <cstring>  // memcpy
#ifdef BUILD_UT
#include <Fw/Types/String.hpp>
#include <iomanip>
#endif

// Some macros/functions to optimize for architectures

namespace Fw {

Serializable::Serializable() {}

Serializable::~Serializable() {}

// ----------------------------------------------------------------------
#if FW_SERIALIZABLE_TO_STRING || FW_ENABLE_TEXT_LOGGING || BUILD_UT

void Serializable::toString(StringBase& text) const {
    text = "NOSPEC";  // set to not specified.
}

#endif

#ifdef BUILD_UT
std::ostream& operator<<(std::ostream& os, const Serializable& val) {
    Fw::String out;
    val.toString(out);

    os << out;

    return os;
}
#endif

SerialBufferBase::~SerialBufferBase() {}

LinearBufferBase::LinearBufferBase() : m_serLoc(0), m_deserLoc(0) {}

LinearBufferBase::~LinearBufferBase() {}

void LinearBufferBase::copyFrom(const LinearBufferBase& src) {
    this->m_serLoc = src.m_serLoc;
    this->m_deserLoc = src.m_deserLoc;
    FW_ASSERT(src.getBuffAddr());
    FW_ASSERT(this->getBuffAddr());
    // destination has to be same or bigger
    FW_ASSERT(src.getSize() <= this->getCapacity(), static_cast<FwAssertArgType>(src.getSize()),
              static_cast<FwAssertArgType>(this->getSize()));
    (void)memcpy(this->getBuffAddr(), src.getBuffAddr(), static_cast<size_t>(this->m_serLoc));
}

// Copy constructor doesn't make sense in this virtual class as there is nothing to copy. Derived classes should
// call the empty constructor and then call their own copy function
LinearBufferBase& LinearBufferBase::operator=(const LinearBufferBase& src) {  // lgtm[cpp/rule-of-two]
    this->copyFrom(src);
    return *this;
}

// serialization routines

SerializeStatus LinearBufferBase::serializeFrom(U8 val, Endianness mode) {
    if (this->m_serLoc + static_cast<Serializable::SizeType>(sizeof(val)) - 1 >= this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }
    FW_ASSERT(this->getBuffAddr());
    this->getBuffAddr()[this->m_serLoc] = val;
    this->m_serLoc += static_cast<Serializable::SizeType>(sizeof(val));
    this->m_deserLoc = 0;

    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::serializeFrom(I8 val, Endianness mode) {
    return serializeFrom(static_cast<U8>(val), mode);
}

#if FW_HAS_16_BIT == 1
SerializeStatus LinearBufferBase::serializeFrom(U16 val, Endianness mode) {
    if (this->m_serLoc + static_cast<Serializable::SizeType>(sizeof(val)) - 1 >= this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }
    FW_ASSERT(this->getBuffAddr());
    switch (mode) {
        case Endianness::BIG:
            // MSB first
            this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 8);
            this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val);
            break;
        case Endianness::LITTLE:
            // LSB first
            this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val);
            this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 8);
            break;
        default:
            FW_ASSERT(false);
            break;
    }
    this->m_serLoc += static_cast<Serializable::SizeType>(sizeof(val));
    this->m_deserLoc = 0;
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::serializeFrom(I16 val, Endianness mode) {
    return serializeFrom(static_cast<U16>(val), mode);
}
#endif
#if FW_HAS_32_BIT == 1
SerializeStatus LinearBufferBase::serializeFrom(U32 val, Endianness mode) {
    if (this->m_serLoc + static_cast<Serializable::SizeType>(sizeof(val)) - 1 >= this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }
    FW_ASSERT(this->getBuffAddr());
    switch (mode) {
        case Endianness::BIG:
            // MSB first
            this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 24);
            this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 16);
            this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 8);
            this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val);
            break;
        case Endianness::LITTLE:
            // LSB first
            this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val);
            this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 8);
            this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 16);
            this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val >> 24);
            break;
        default:
            FW_ASSERT(false);
            break;
    }
    this->m_serLoc += static_cast<Serializable::SizeType>(sizeof(val));
    this->m_deserLoc = 0;
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::serializeFrom(I32 val, Endianness mode) {
    return serializeFrom(static_cast<U32>(val), mode);
}
#endif

#if FW_HAS_64_BIT == 1
SerializeStatus LinearBufferBase::serializeFrom(U64 val, Endianness mode) {
    if (this->m_serLoc + static_cast<Serializable::SizeType>(sizeof(val)) - 1 >= this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }
    FW_ASSERT(this->getBuffAddr());
    switch (mode) {
        case Endianness::BIG:
            // MSB first
            this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 56);
            this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 48);
            this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 40);
            this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val >> 32);
            this->getBuffAddr()[this->m_serLoc + 4] = static_cast<U8>(val >> 24);
            this->getBuffAddr()[this->m_serLoc + 5] = static_cast<U8>(val >> 16);
            this->getBuffAddr()[this->m_serLoc + 6] = static_cast<U8>(val >> 8);
            this->getBuffAddr()[this->m_serLoc + 7] = static_cast<U8>(val);
            break;
        case Endianness::LITTLE:
            // LSB first
            this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val);
            this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 8);
            this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 16);
            this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val >> 24);
            this->getBuffAddr()[this->m_serLoc + 4] = static_cast<U8>(val >> 32);
            this->getBuffAddr()[this->m_serLoc + 5] = static_cast<U8>(val >> 40);
            this->getBuffAddr()[this->m_serLoc + 6] = static_cast<U8>(val >> 48);
            this->getBuffAddr()[this->m_serLoc + 7] = static_cast<U8>(val >> 56);
            break;
        default:
            FW_ASSERT(false);
            break;
    }
    this->m_serLoc += static_cast<Serializable::SizeType>(sizeof(val));
    this->m_deserLoc = 0;
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::serializeFrom(I64 val, Endianness mode) {
    return serializeFrom(static_cast<U64>(val), mode);
}
#endif

SerializeStatus LinearBufferBase::serializeFrom(F64 val, Endianness mode) {
    // floating point values need to be byte-swapped as well, so copy to U64 and use that routine
    U64 u64Val;
    (void)memcpy(&u64Val, &val, sizeof(val));
    return this->serializeFrom(u64Val, mode);
}

SerializeStatus LinearBufferBase::serializeFrom(F32 val, Endianness mode) {
    // floating point values need to be byte-swapped as well, so copy to U32 and use that routine
    U32 u32Val;
    (void)memcpy(&u32Val, &val, sizeof(val));
    return this->serializeFrom(u32Val, mode);
}

SerializeStatus LinearBufferBase::serializeFrom(bool val, Endianness mode) {
    if (this->m_serLoc + static_cast<Serializable::SizeType>(sizeof(U8)) - 1 >= this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }

    FW_ASSERT(this->getBuffAddr());
    if (val) {
        this->getBuffAddr()[this->m_serLoc + 0] = FW_SERIALIZE_TRUE_VALUE;
    } else {
        this->getBuffAddr()[this->m_serLoc + 0] = FW_SERIALIZE_FALSE_VALUE;
    }

    this->m_serLoc += static_cast<Serializable::SizeType>(sizeof(U8));
    this->m_deserLoc = 0;
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::serializeFrom(const void* val, Endianness mode) {
    if (this->m_serLoc + static_cast<Serializable::SizeType>(sizeof(void*)) - 1 >= this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }

    return this->serializeFrom(reinterpret_cast<PlatformPointerCastType>(val), mode);
}

SerializeStatus LinearBufferBase::serializeFrom(const U8* buff, Serializable::SizeType length, Endianness endianMode) {
    return this->serializeFrom(buff, static_cast<FwSizeType>(length), Serialization::INCLUDE_LENGTH, endianMode);
}

SerializeStatus LinearBufferBase::serializeFrom(const U8* buff,
                                                FwSizeType length,
                                                Serialization::t lengthMode,
                                                Endianness endianMode) {  // First serialize length
    SerializeStatus stat;
    if (lengthMode == Serialization::INCLUDE_LENGTH) {
        stat = this->serializeFrom(static_cast<FwSizeStoreType>(length), endianMode);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
    }

    // make sure we have enough space
    if (this->m_serLoc + length > this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }

    // copy buffer to our buffer
    (void)memcpy(&this->getBuffAddr()[this->m_serLoc], buff, static_cast<size_t>(length));
    this->m_serLoc += static_cast<Serializable::SizeType>(length);
    this->m_deserLoc = 0;

    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::serializeFrom(const Serializable& val, Endianness mode) {
    return val.serializeTo(*this, mode);
}

SerializeStatus LinearBufferBase::serializeFrom(const LinearBufferBase& val, Endianness mode) {
    Serializable::SizeType size = val.getSize();
    if (this->m_serLoc + size + static_cast<Serializable::SizeType>(sizeof(FwSizeStoreType)) > this->getCapacity()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }

    // First, serialize size
    SerializeStatus stat = this->serializeFrom(static_cast<FwSizeStoreType>(size), mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    FW_ASSERT(this->getBuffAddr());
    FW_ASSERT(val.getBuffAddr());
    // serialize buffer
    (void)memcpy(&this->getBuffAddr()[this->m_serLoc], val.getBuffAddr(), static_cast<size_t>(size));
    this->m_serLoc += size;
    this->m_deserLoc = 0;

    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::serializeSize(const FwSizeType size, Endianness mode) {
    SerializeStatus status = FW_SERIALIZE_OK;
    if ((size < std::numeric_limits<FwSizeStoreType>::min()) || (size > std::numeric_limits<FwSizeStoreType>::max())) {
        status = FW_SERIALIZE_FORMAT_ERROR;
    }
    if (status == FW_SERIALIZE_OK) {
        status = this->serializeFrom(static_cast<FwSizeStoreType>(size), mode);
    }
    return status;
}

// deserialization routines

SerializeStatus LinearBufferBase::deserializeTo(U8& val, Endianness mode) {
    // check for room
    if (this->getSize() == this->m_deserLoc) {
        return FW_DESERIALIZE_BUFFER_EMPTY;
    } else if (this->getSize() - this->m_deserLoc < static_cast<Serializable::SizeType>(sizeof(val))) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    // read from current location
    FW_ASSERT(this->getBuffAddr());
    val = this->getBuffAddr()[this->m_deserLoc + 0];
    this->m_deserLoc += static_cast<Serializable::SizeType>(sizeof(val));
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(I8& val, Endianness mode) {
    // check for room
    if (this->getSize() == this->m_deserLoc) {
        return FW_DESERIALIZE_BUFFER_EMPTY;
    } else if (this->getSize() - this->m_deserLoc < static_cast<Serializable::SizeType>(sizeof(val))) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    // read from current location
    FW_ASSERT(this->getBuffAddr());
    val = static_cast<I8>(this->getBuffAddr()[this->m_deserLoc + 0]);
    this->m_deserLoc += static_cast<Serializable::SizeType>(sizeof(val));
    return FW_SERIALIZE_OK;
}

#if FW_HAS_16_BIT == 1
SerializeStatus LinearBufferBase::deserializeTo(U16& val, Endianness mode) {
    // check for room
    if (this->getSize() == this->m_deserLoc) {
        return FW_DESERIALIZE_BUFFER_EMPTY;
    } else if (this->getSize() - this->m_deserLoc < static_cast<Serializable::SizeType>(sizeof(val))) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    // read from current location
    FW_ASSERT(this->getBuffAddr());
    switch (mode) {
        case Endianness::BIG:
            // MSB first
            val = static_cast<U16>(((this->getBuffAddr()[this->m_deserLoc + 1]) << 0) |
                                   ((this->getBuffAddr()[this->m_deserLoc + 0]) << 8));
            break;
        case Endianness::LITTLE:
            // LSB first
            val = static_cast<U16>(((this->getBuffAddr()[this->m_deserLoc + 0]) << 0) |
                                   ((this->getBuffAddr()[this->m_deserLoc + 1]) << 8));
            break;
        default:
            FW_ASSERT(false);
            break;
    }
    this->m_deserLoc += static_cast<Serializable::SizeType>(sizeof(val));
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(I16& val, Endianness mode) {
    U16 unsignVal;
    SerializeStatus res = deserializeTo(unsignVal, mode);
    if (res == SerializeStatus::FW_SERIALIZE_OK) {
        val = static_cast<I16>(unsignVal);
    }
    return res;
}
#endif
#if FW_HAS_32_BIT == 1
SerializeStatus LinearBufferBase::deserializeTo(U32& val, Endianness mode) {
    // check for room
    if (this->getSize() == this->m_deserLoc) {
        return FW_DESERIALIZE_BUFFER_EMPTY;
    } else if (this->getSize() - this->m_deserLoc < static_cast<Serializable::SizeType>(sizeof(val))) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    // read from current location
    FW_ASSERT(this->getBuffAddr());
    switch (mode) {
        case Endianness::BIG:
            // MSB first
            val = (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 3]) << 0) |
                  (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 2]) << 8) |
                  (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 1]) << 16) |
                  (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 0]) << 24);
            break;
        case Endianness::LITTLE:
            // LSB first
            val = (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 0]) << 0) |
                  (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 1]) << 8) |
                  (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 2]) << 16) |
                  (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 3]) << 24);
            break;
        default:
            FW_ASSERT(false);
            break;
    }
    this->m_deserLoc += static_cast<Serializable::SizeType>(sizeof(val));
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(I32& val, Endianness mode) {
    U32 unsignVal;
    SerializeStatus res = deserializeTo(unsignVal, mode);
    if (res == SerializeStatus::FW_SERIALIZE_OK) {
        val = static_cast<I32>(unsignVal);
    }
    return res;
}
#endif

#if FW_HAS_64_BIT == 1

SerializeStatus LinearBufferBase::deserializeTo(U64& val, Endianness mode) {
    // check for room
    if (this->getSize() == this->m_deserLoc) {
        return FW_DESERIALIZE_BUFFER_EMPTY;
    } else if (this->getSize() - this->m_deserLoc < static_cast<Serializable::SizeType>(sizeof(val))) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    // read from current location
    FW_ASSERT(this->getBuffAddr());
    switch (mode) {
        case Endianness::BIG:
            // MSB first
            val = (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 7]) << 0) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 6]) << 8) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 5]) << 16) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 4]) << 24) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 3]) << 32) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 2]) << 40) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 1]) << 48) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 0]) << 56);
            break;
        case Endianness::LITTLE:
            // LSB first
            val = (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 0]) << 0) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 1]) << 8) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 2]) << 16) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 3]) << 24) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 4]) << 32) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 5]) << 40) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 6]) << 48) |
                  (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 7]) << 56);
            break;
        default:
            FW_ASSERT(false);
            break;
    }
    this->m_deserLoc += static_cast<Serializable::SizeType>(sizeof(val));
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(I64& val, Endianness mode) {
    U64 unsignVal;
    SerializeStatus res = deserializeTo(unsignVal, mode);
    if (res == SerializeStatus::FW_SERIALIZE_OK) {
        val = static_cast<I64>(unsignVal);
    }
    return res;
}
#endif

SerializeStatus LinearBufferBase::deserializeTo(F64& val, Endianness mode) {
    // deserialize as 64-bit int to handle endianness
    U64 tempVal;
    SerializeStatus stat = this->deserializeTo(tempVal, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }
    // copy to argument
    (void)memcpy(&val, &tempVal, sizeof(val));

    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(bool& val, Endianness mode) {
    // check for room
    if (this->getSize() == this->m_deserLoc) {
        return FW_DESERIALIZE_BUFFER_EMPTY;
    } else if (this->getSize() - this->m_deserLoc < static_cast<Serializable::SizeType>(sizeof(U8))) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    // read from current location
    FW_ASSERT(this->getBuffAddr());
    if (FW_SERIALIZE_TRUE_VALUE == this->getBuffAddr()[this->m_deserLoc + 0]) {
        val = true;
    } else if (FW_SERIALIZE_FALSE_VALUE == this->getBuffAddr()[this->m_deserLoc + 0]) {
        val = false;
    } else {
        return FW_DESERIALIZE_FORMAT_ERROR;
    }

    this->m_deserLoc += static_cast<Serializable::SizeType>(sizeof(U8));
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(void*& val, Endianness mode) {
    // Deserialize as pointer cast, then convert to void*
    PlatformPointerCastType pointerCastVal = 0;
    const SerializeStatus stat = this->deserializeTo(pointerCastVal, mode);
    if (stat == FW_SERIALIZE_OK) {
        val = reinterpret_cast<void*>(pointerCastVal);
    }
    return stat;
}

SerializeStatus LinearBufferBase::deserializeTo(F32& val, Endianness mode) {
    // deserialize as 64-bit int to handle endianness
    U32 tempVal;
    SerializeStatus stat = this->deserializeTo(tempVal, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }
    (void)memcpy(&val, &tempVal, sizeof(val));

    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(U8* buff, Serializable::SizeType& length, Endianness endianMode) {
    FwSizeType length_in_out = static_cast<FwSizeType>(length);
    SerializeStatus status = this->deserializeTo(buff, length_in_out, Serialization::INCLUDE_LENGTH, endianMode);
    length = static_cast<Serializable::SizeType>(length_in_out);
    return status;
}

SerializeStatus LinearBufferBase::deserializeTo(U8* buff,
                                                Serializable::SizeType& length,
                                                Serialization::t lengthMode,
                                                Endianness endianMode) {
    FW_ASSERT(this->getBuffAddr());

    if (lengthMode == Serialization::INCLUDE_LENGTH) {
        FwSizeStoreType storedLength;

        SerializeStatus stat = this->deserializeTo(storedLength, endianMode);

        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        // make sure it fits
        if ((storedLength > this->getDeserializeSizeLeft()) or (storedLength > length)) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }

        (void)memcpy(buff, &this->getBuffAddr()[this->m_deserLoc], static_cast<size_t>(storedLength));

        length = static_cast<FwSizeType>(storedLength);

    } else {
        // make sure enough is left
        if (length > this->getDeserializeSizeLeft()) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }

        (void)memcpy(buff, &this->getBuffAddr()[this->m_deserLoc], static_cast<size_t>(length));
    }

    this->m_deserLoc += static_cast<Serializable::SizeType>(length);
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeTo(Serializable& val, Endianness mode) {
    return val.deserializeFrom(*this, mode);
}

SerializeStatus LinearBufferBase::deserializeTo(LinearBufferBase& val, Endianness mode) {
    FW_ASSERT(val.getBuffAddr());
    SerializeStatus stat = FW_SERIALIZE_OK;

    FwSizeStoreType storedLength;

    stat = this->deserializeTo(storedLength, mode);

    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    // make sure destination has enough room

    if ((storedLength > val.getCapacity()) or (storedLength > this->getDeserializeSizeLeft())) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }

    FW_ASSERT(this->getBuffAddr());
    (void)memcpy(val.getBuffAddr(), &this->getBuffAddr()[this->m_deserLoc], static_cast<size_t>(storedLength));

    stat = val.setBuffLen(storedLength);

    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    this->m_deserLoc += storedLength;

    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::deserializeSize(FwSizeType& size, Endianness mode) {
    FwSizeStoreType storedSize = 0;
    Fw::SerializeStatus status = this->deserializeTo(storedSize, mode);
    if (status == FW_SERIALIZE_OK) {
        size = static_cast<FwSizeType>(storedSize);
    }
    return status;
}

void LinearBufferBase::resetSer() {
    this->m_deserLoc = 0;
    this->m_serLoc = 0;
}

void LinearBufferBase::resetDeser() {
    this->m_deserLoc = 0;
}

SerializeStatus LinearBufferBase::serializeSkip(FwSizeType numBytesToSkip) {
    Fw::SerializeStatus status = FW_SERIALIZE_OK;
    // compute new deser loc
    const FwSizeType newSerLoc = this->m_serLoc + numBytesToSkip;
    // check for room
    if (newSerLoc <= this->getCapacity()) {
        // update deser loc
        this->m_serLoc = static_cast<Serializable::SizeType>(newSerLoc);
    } else {
        status = FW_SERIALIZE_NO_ROOM_LEFT;
    }
    return status;
}

SerializeStatus LinearBufferBase::deserializeSkip(FwSizeType numBytesToSkip) {
    // check for room
    if (this->getSize() == this->m_deserLoc) {
        return FW_DESERIALIZE_BUFFER_EMPTY;
    } else if (this->getSize() - this->m_deserLoc < numBytesToSkip) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    // update location in buffer to skip the value
    this->m_deserLoc += static_cast<Serializable::SizeType>(numBytesToSkip);
    return FW_SERIALIZE_OK;
}

SerializeStatus LinearBufferBase::moveSerToOffset(FwSizeType offset) {
    // Reset serialization
    this->resetSer();
    // Advance to offset
    return this->serializeSkip(offset);
}
SerializeStatus LinearBufferBase::moveDeserToOffset(FwSizeType offset) {
    // Reset deserialization
    this->resetDeser();
    // Advance to offset
    return this->deserializeSkip(offset);
}

Serializable::SizeType LinearBufferBase::getSize() const {
    return this->m_serLoc;
}

SerializeStatus LinearBufferBase::setBuff(const U8* src, Serializable::SizeType length) {
    if (this->getCapacity() < length) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    } else {
        FW_ASSERT(src);
        FW_ASSERT(this->getBuffAddr());
        (void)memcpy(this->getBuffAddr(), src, static_cast<size_t>(length));
        this->m_serLoc = length;
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }
}

SerializeStatus LinearBufferBase::setBuffLen(Serializable::SizeType length) {
    if (this->getCapacity() < length) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    } else {
        this->m_serLoc = length;
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }
}

Serializable::SizeType LinearBufferBase::getDeserializeSizeLeft() const {
    FW_ASSERT(this->m_serLoc >= this->m_deserLoc, static_cast<FwAssertArgType>(this->m_serLoc),
              static_cast<FwAssertArgType>(this->m_deserLoc));
    return this->m_serLoc - this->m_deserLoc;
}

Serializable::SizeType LinearBufferBase::getSerializeSizeLeft() const {
    FW_ASSERT(static_cast<FwAssertArgType>(this->m_serLoc));
    return this->getCapacity() - this->m_serLoc;
}

SerializeStatus LinearBufferBase::copyRaw(SerialBufferBase& dest, Serializable::SizeType size) {
    // make sure there is sufficient size in destination
    if (dest.getCapacity() < size) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }
    // make sure there is sufficient buffer in source
    if (this->getDeserializeSizeLeft() < size) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }

    // otherwise, set destination buffer to data from deserialization pointer plus size
    SerializeStatus stat = dest.setBuff(&this->getBuffAddr()[this->m_deserLoc], size);
    if (stat == FW_SERIALIZE_OK) {
        this->m_deserLoc += size;
    }
    return stat;
}

SerializeStatus LinearBufferBase::copyRawOffset(SerialBufferBase& dest, Serializable::SizeType size) {
    // make sure there is sufficient size in destination
    if (dest.getCapacity() < size + dest.getSize()) {
        return FW_SERIALIZE_NO_ROOM_LEFT;
    }
    // make sure there is sufficient buffer in source
    if (this->getDeserializeSizeLeft() < size) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }

    // otherwise, serialize bytes to destination without writing length
    SerializeStatus stat = dest.serializeFrom(&this->getBuffAddr()[this->m_deserLoc], size, Serialization::OMIT_LENGTH);
    if (stat == FW_SERIALIZE_OK) {
        this->m_deserLoc += size;
    }
    return stat;
}

// return address of buffer not yet deserialized. This is used
// to copy the remainder of a buffer.
const U8* LinearBufferBase::getBuffAddrLeft() const {
    return &this->getBuffAddr()[this->m_deserLoc];
}

//!< gets address of end of serialization. Used to manually place data at the end
U8* LinearBufferBase::getBuffAddrSer() {
    return &this->getBuffAddr()[this->m_serLoc];
}

#ifdef BUILD_UT
bool LinearBufferBase::operator==(const SerializeBufferBase& other) const {
    if (this->getSize() != other.getSize()) {
        return false;
    }

    const U8* us = this->getBuffAddr();
    const U8* them = other.getBuffAddr();

    FW_ASSERT(us);
    FW_ASSERT(them);

    for (Serializable::SizeType byte = 0; byte < this->getSize(); byte++) {
        if (us[byte] != them[byte]) {
            return false;
        }
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const LinearBufferBase& buff) {
    const U8* us = buff.getBuffAddr();

    FW_ASSERT(us);

    for (Serializable::SizeType byte = 0; byte < buff.getSize(); byte++) {
        os << "[" << std::setw(2) << std::hex << std::setfill('0') << us[byte] << "]" << std::dec;
    }

    return os;
}
#endif

ExternalSerializeBuffer::ExternalSerializeBuffer(U8* buffPtr, Serializable::SizeType size) {
    this->setExtBuffer(buffPtr, size);
}

ExternalSerializeBuffer::ExternalSerializeBuffer() {
    this->clear();
}

void ExternalSerializeBuffer::setExtBuffer(U8* buffPtr, Serializable::SizeType size) {
    FW_ASSERT(buffPtr != nullptr);
    this->clear();
    this->m_buff = buffPtr;
    this->m_buffSize = size;
}

void ExternalSerializeBuffer::clear() {
    this->resetSer();
    this->resetDeser();
    this->m_buff = nullptr;
    this->m_buffSize = 0;
}

Serializable::SizeType ExternalSerializeBuffer::getCapacity() const {
    return this->m_buffSize;
}

U8* ExternalSerializeBuffer::getBuffAddr() {
    return this->m_buff;
}

const U8* ExternalSerializeBuffer::getBuffAddr() const {
    return this->m_buff;
}

// ----------------------------------------------------------------------
// Deprecated method implementations for backward compatibility
// ----------------------------------------------------------------------

Serializable::SizeType LinearBufferBase::getBuffLength() const {
    return this->getSize();
}

Serializable::SizeType LinearBufferBase::getBuffLeft() {
    return this->getDeserializeSizeLeft();
}

SerializeStatus LinearBufferBase::serialize(U8 val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(I8 val) {
    return this->serializeFrom(val);
}
#if FW_HAS_16_BIT == 1
SerializeStatus LinearBufferBase::serialize(U16 val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(I16 val) {
    return this->serializeFrom(val);
}
#endif
#if FW_HAS_32_BIT == 1
SerializeStatus LinearBufferBase::serialize(U32 val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(I32 val) {
    return this->serializeFrom(val);
}
#endif
#if FW_HAS_64_BIT == 1
SerializeStatus LinearBufferBase::serialize(U64 val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(I64 val) {
    return this->serializeFrom(val);
}
#endif
SerializeStatus LinearBufferBase::serialize(F32 val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(F64 val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(bool val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(const void* val) {
    return this->serializeFrom(val);
}

// Deprecated method for backward compatibility
SerializeStatus LinearBufferBase::serialize(const U8* buff, FwSizeType length, bool noLength) {
    const Serialization::t mode = noLength ? Serialization::OMIT_LENGTH : Serialization::INCLUDE_LENGTH;
    return this->serializeFrom(buff, length, mode);
}

SerializeStatus LinearBufferBase::serialize(const U8* buff, FwSizeType length) {
    return this->serializeFrom(buff, length);
}
SerializeStatus LinearBufferBase::serialize(const U8* buff, FwSizeType length, Serialization::t mode) {
    return this->serializeFrom(buff, length, mode);
}
SerializeStatus LinearBufferBase::serialize(const Serializable& val) {
    return this->serializeFrom(val);
}
SerializeStatus LinearBufferBase::serialize(const LinearBufferBase& val) {
    return this->serializeFrom(val);
}

SerializeStatus LinearBufferBase::deserialize(U8& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(I8& val) {
    return this->deserializeTo(val);
}
#if FW_HAS_16_BIT == 1
SerializeStatus LinearBufferBase::deserialize(U16& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(I16& val) {
    return this->deserializeTo(val);
}
#endif
#if FW_HAS_32_BIT == 1
SerializeStatus LinearBufferBase::deserialize(U32& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(I32& val) {
    return this->deserializeTo(val);
}
#endif
#if FW_HAS_64_BIT == 1
SerializeStatus LinearBufferBase::deserialize(U64& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(I64& val) {
    return this->deserializeTo(val);
}
#endif
SerializeStatus LinearBufferBase::deserialize(F32& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(F64& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(bool& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(void*& val) {
    return this->deserializeTo(val);
}

// Deprecated method for backward compatibility
SerializeStatus LinearBufferBase::deserialize(U8* buff, FwSizeType& length, bool noLength) {
    const Serialization::t mode = noLength ? Serialization::OMIT_LENGTH : Serialization::INCLUDE_LENGTH;
    return this->deserializeTo(buff, length, mode);
}

SerializeStatus LinearBufferBase::deserialize(U8* buff, FwSizeType& length) {
    return this->deserializeTo(buff, length, Serialization::INCLUDE_LENGTH);
}

SerializeStatus LinearBufferBase::deserialize(U8* buff, FwSizeType& length, Serialization::t mode) {
    return this->deserializeTo(buff, length, mode);
}

SerializeStatus LinearBufferBase::deserialize(Serializable& val) {
    return this->deserializeTo(val);
}
SerializeStatus LinearBufferBase::deserialize(LinearBufferBase& val) {
    return this->deserializeTo(val);
}

Serializable::SizeType ExternalSerializeBuffer::getBuffCapacity() const {
    return this->getCapacity();
}

}  // namespace Fw
