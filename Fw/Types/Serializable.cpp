#include <Fw/Types/Serializable.hpp>
#include <cstring> // memcpy
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringType.hpp>
#include <cstdio>
#include <FpConfig.hpp>
#ifdef BUILD_UT
#include <iomanip>
#include <Fw/Types/String.hpp>
#endif

// Some macros/functions to optimize for architectures

namespace Fw {

    Serializable::Serializable() {
    }

    Serializable::~Serializable() {
    }

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT

    void Serializable::toString(StringBase& text) const {
        text = "NOSPEC"; // set to not specified.
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

    SerializeBufferBase::SerializeBufferBase() :
            m_serLoc(0), m_deserLoc(0) {
    }

    SerializeBufferBase::~SerializeBufferBase() {
    }

    void SerializeBufferBase::copyFrom(const SerializeBufferBase& src) {
        this->m_serLoc = src.m_serLoc;
        this->m_deserLoc = src.m_deserLoc;
        FW_ASSERT(src.getBuffAddr());
        FW_ASSERT(this->getBuffAddr());
        // destination has to be same or bigger
        FW_ASSERT(src.getBuffLength() <= this->getBuffCapacity(),src.getBuffLength(),this->getBuffLength());
        (void) memcpy(this->getBuffAddr(),src.getBuffAddr(),this->m_serLoc);
    }

    // Copy constructor doesn't make sense in this virtual class as there is nothing to copy. Derived classes should
    // call the empty constructor and then call their own copy function
    SerializeBufferBase& SerializeBufferBase::operator=(const SerializeBufferBase &src) { // lgtm[cpp/rule-of-two]
        this->copyFrom(src);
        return *this;
    }

    // serialization routines

    SerializeStatus SerializeBufferBase::serialize(U8 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        this->getBuffAddr()[this->m_serLoc] = val;
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;

        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::serialize(I8 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val);
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }

#if FW_HAS_16_BIT==1
    SerializeStatus SerializeBufferBase::serialize(U16 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 8);
        this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val);
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::serialize(I16 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 8);
        this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val);
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }
#endif
#if FW_HAS_32_BIT==1
    SerializeStatus SerializeBufferBase::serialize(U32 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 24);
        this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 16);
        this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 8);
        this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val);
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::serialize(I32 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 24);
        this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 16);
        this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 8);
        this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val);
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }
#endif

#if FW_HAS_64_BIT==1
    SerializeStatus SerializeBufferBase::serialize(U64 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 56);
        this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 48);
        this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 40);
        this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val >> 32);
        this->getBuffAddr()[this->m_serLoc + 4] = static_cast<U8>(val >> 24);
        this->getBuffAddr()[this->m_serLoc + 5] = static_cast<U8>(val >> 16);
        this->getBuffAddr()[this->m_serLoc + 6] = static_cast<U8>(val >> 8);
        this->getBuffAddr()[this->m_serLoc + 7] = static_cast<U8>(val);
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::serialize(I64 val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(val)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        this->getBuffAddr()[this->m_serLoc + 0] = static_cast<U8>(val >> 56);
        this->getBuffAddr()[this->m_serLoc + 1] = static_cast<U8>(val >> 48);
        this->getBuffAddr()[this->m_serLoc + 2] = static_cast<U8>(val >> 40);
        this->getBuffAddr()[this->m_serLoc + 3] = static_cast<U8>(val >> 32);
        this->getBuffAddr()[this->m_serLoc + 4] = static_cast<U8>(val >> 24);
        this->getBuffAddr()[this->m_serLoc + 5] = static_cast<U8>(val >> 16);
        this->getBuffAddr()[this->m_serLoc + 6] = static_cast<U8>(val >> 8);
        this->getBuffAddr()[this->m_serLoc + 7] = static_cast<U8>(val);
        this->m_serLoc += sizeof(val);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }
#endif

#if FW_HAS_F64

    SerializeStatus SerializeBufferBase::serialize(F64 val) {
        // floating point values need to be byte-swapped as well, so copy to U64 and use that routine
        U64 u64Val;
        (void) memcpy(&u64Val, &val, sizeof(val));
        return this->serialize(u64Val);

    }

#endif

    SerializeStatus SerializeBufferBase::serialize(F32 val) {

        // floating point values need to be byte-swapped as well, so copy to U32 and use that routine
        U32 u32Val;
        (void) memcpy(&u32Val, &val, sizeof(val));
        return this->serialize(u32Val);

    }

    SerializeStatus SerializeBufferBase::serialize(bool val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(U8)) - 1 >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }

        FW_ASSERT(this->getBuffAddr());
        if (val) {
            this->getBuffAddr()[this->m_serLoc + 0] = FW_SERIALIZE_TRUE_VALUE;
        } else {
            this->getBuffAddr()[this->m_serLoc + 0] = FW_SERIALIZE_FALSE_VALUE;
        }

        this->m_serLoc += sizeof(U8);
        this->m_deserLoc = 0;
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::serialize(const void* val) {
        if (this->m_serLoc + static_cast<NATIVE_UINT_TYPE>(sizeof(void*)) - 1
                >= this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }

        return this->serialize(reinterpret_cast<POINTER_CAST>(val));

    }

    SerializeStatus SerializeBufferBase::serialize(const U8* buff, NATIVE_UINT_TYPE length, bool noLength) {
        // First serialize length
        SerializeStatus stat;
        if (not noLength) {
            stat = this->serialize(static_cast<FwBuffSizeType>(length));
            if (stat != FW_SERIALIZE_OK) {
                return stat;
            }
        }

        // make sure we have enough space
        if (this->m_serLoc + length > this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }

        // copy buffer to our buffer
        (void) memcpy(&this->getBuffAddr()[this->m_serLoc], buff, length);
        this->m_serLoc += length;
        this->m_deserLoc = 0;

        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::serialize(const Serializable &val) {
        return val.serialize(*this);
    }

    SerializeStatus SerializeBufferBase::serialize(
            const SerializeBufferBase& val) {
        NATIVE_UINT_TYPE size = val.getBuffLength();
        if (this->m_serLoc + size + static_cast<NATIVE_UINT_TYPE>(sizeof(FwBuffSizeType))
                > this->getBuffCapacity()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }

        // First, serialize size
        SerializeStatus stat = this->serialize(static_cast<FwBuffSizeType>(size));
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        FW_ASSERT(this->getBuffAddr());
        FW_ASSERT(val.getBuffAddr());
        // serialize buffer
        (void) memcpy(&this->getBuffAddr()[this->m_serLoc], val.getBuffAddr(), size);
        this->m_serLoc += size;
        this->m_deserLoc = 0;

        return FW_SERIALIZE_OK;
    }

    // deserialization routines

    SerializeStatus SerializeBufferBase::deserialize(U8 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        val = this->getBuffAddr()[this->m_deserLoc + 0];
        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::deserialize(I8 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        val = static_cast<I8>(this->getBuffAddr()[this->m_deserLoc + 0]);
        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }

#if FW_HAS_16_BIT==1
    SerializeStatus SerializeBufferBase::deserialize(U16 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        val = static_cast<U16>(
            ((this->getBuffAddr()[this->m_deserLoc + 1]) << 0) |
            ((this->getBuffAddr()[this->m_deserLoc + 0]) << 8)
        );
        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::deserialize(I16 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        val = static_cast<I16>(
            ((this->getBuffAddr()[this->m_deserLoc + 1]) << 0) |
            ((this->getBuffAddr()[this->m_deserLoc + 0]) << 8)
        );
        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }
#endif
#if FW_HAS_32_BIT==1
    SerializeStatus SerializeBufferBase::deserialize(U32 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        val = (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 3]) << 0)
                | (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 2]) << 8)
                | (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 1]) << 16)
                | (static_cast<U32>(this->getBuffAddr()[this->m_deserLoc + 0]) << 24);
        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::deserialize(I32 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        val = (static_cast<I32>(this->getBuffAddr()[this->m_deserLoc + 3]) << 0)
                | (static_cast<I32>(this->getBuffAddr()[this->m_deserLoc + 2]) << 8)
                | (static_cast<I32>(this->getBuffAddr()[this->m_deserLoc + 1]) << 16)
                | (static_cast<I32>(this->getBuffAddr()[this->m_deserLoc + 0]) << 24);
        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }
#endif

#if FW_HAS_64_BIT==1

    SerializeStatus SerializeBufferBase::deserialize(U64 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        val = (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 7]) << 0)
                | (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 6]) << 8)
                | (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 5]) << 16)
                | (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 4]) << 24)
                | (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 3]) << 32)
                | (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 2]) << 40)
                | (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 1]) << 48)
                | (static_cast<U64>(this->getBuffAddr()[this->m_deserLoc + 0]) << 56);

        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::deserialize(I64 &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(val))) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // read from current location
        FW_ASSERT(this->getBuffAddr());
        // MSB first
        val = (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 7]) << 0)
                | (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 6]) << 8)
                | (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 5]) << 16)
                | (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 4]) << 24)
                | (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 3]) << 32)
                | (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 2]) << 40)
                | (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 1]) << 48)
                | (static_cast<I64>(this->getBuffAddr()[this->m_deserLoc + 0]) << 56);
        this->m_deserLoc += sizeof(val);
        return FW_SERIALIZE_OK;
    }
#endif

#if FW_HAS_F64

    SerializeStatus SerializeBufferBase::deserialize(F64 &val) {

        // deserialize as 64-bit int to handle endianness
        U64 tempVal;
        SerializeStatus stat = this->deserialize(tempVal);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        // copy to argument
        (void) memcpy(&val, &tempVal, sizeof(val));

        return FW_SERIALIZE_OK;
    }

#endif

    SerializeStatus SerializeBufferBase::deserialize(bool &val) {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < static_cast<NATIVE_UINT_TYPE>(sizeof(U8))) {
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

        this->m_deserLoc += sizeof(U8);
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::deserialize(void*& val) {
        return this->deserialize(reinterpret_cast<POINTER_CAST&>(val));
    }

    SerializeStatus SerializeBufferBase::deserialize(F32 &val) {
        // deserialize as 64-bit int to handle endianness
        U32 tempVal;
        SerializeStatus stat = this->deserialize(tempVal);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        (void) memcpy(&val, &tempVal, sizeof(val));

        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::deserialize(U8* buff, NATIVE_UINT_TYPE& length, bool noLength) {

        FW_ASSERT(this->getBuffAddr());

        if (not noLength) {
            FwBuffSizeType storedLength;

            SerializeStatus stat = this->deserialize(storedLength);

            if (stat != FW_SERIALIZE_OK) {
                return stat;
            }

            // make sure it fits
            if ((storedLength > this->getBuffLeft()) or (storedLength > length)) {
                return FW_DESERIALIZE_SIZE_MISMATCH;
            }

            (void) memcpy(buff, &this->getBuffAddr()[this->m_deserLoc], storedLength);

            length = static_cast<NATIVE_UINT_TYPE>(storedLength);

        } else {
            // make sure enough is left
            if (length > this->getBuffLeft()) {
                return FW_DESERIALIZE_SIZE_MISMATCH;
            }

            (void) memcpy(buff, &this->getBuffAddr()[this->m_deserLoc], length);
        }

        this->m_deserLoc += length;
        return FW_SERIALIZE_OK;
    }

    SerializeStatus SerializeBufferBase::deserialize(Serializable &val) {
        return val.deserialize(*this);
    }

    SerializeStatus SerializeBufferBase::deserialize(SerializeBufferBase& val) {

        FW_ASSERT(val.getBuffAddr());
        SerializeStatus stat = FW_SERIALIZE_OK;

        FwBuffSizeType storedLength;

        stat = this->deserialize(storedLength);

        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        // make sure destination has enough room

        if ((storedLength > val.getBuffCapacity()) or (storedLength > this->getBuffLeft()) ) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }

        FW_ASSERT(this->getBuffAddr());
        (void) memcpy(val.getBuffAddr(), &this->getBuffAddr()[this->m_deserLoc],
                storedLength);

        stat = val.setBuffLen(storedLength);

        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        this->m_deserLoc += storedLength;

        return FW_SERIALIZE_OK;
    }

    void SerializeBufferBase::resetSer() {
        this->m_deserLoc = 0;
        this->m_serLoc = 0;
    }

    void SerializeBufferBase::resetDeser() {
        this->m_deserLoc = 0;
    }

    SerializeStatus SerializeBufferBase::deserializeSkip(NATIVE_UINT_TYPE numBytesToSkip)
    {
        // check for room
        if (this->getBuffLength() == this->m_deserLoc) {
            return FW_DESERIALIZE_BUFFER_EMPTY;
        } else if (this->getBuffLength() - this->m_deserLoc < numBytesToSkip) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }
        // update location in buffer to skip the value
        this->m_deserLoc += numBytesToSkip;
        return FW_SERIALIZE_OK;
    }

    NATIVE_UINT_TYPE SerializeBufferBase::getBuffLength() const {
        return this->m_serLoc;
    }

    SerializeStatus SerializeBufferBase::setBuff(const U8* src, NATIVE_UINT_TYPE length) {
        if (this->getBuffCapacity() < length) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        } else {
            FW_ASSERT(src);
            FW_ASSERT(this->getBuffAddr());
            memcpy(this->getBuffAddr(), src, length);
            this->m_serLoc = length;
            this->m_deserLoc = 0;
            return FW_SERIALIZE_OK;
        }
    }

    SerializeStatus SerializeBufferBase::setBuffLen(NATIVE_UINT_TYPE length) {
        if (this->getBuffCapacity() < length) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        } else {
            this->m_serLoc = length;
            this->m_deserLoc = 0;
            return FW_SERIALIZE_OK;
        }
    }

    NATIVE_UINT_TYPE SerializeBufferBase::getBuffLeft() const {
        return this->m_serLoc - this->m_deserLoc;
    }

    SerializeStatus SerializeBufferBase::copyRaw(SerializeBufferBase& dest, NATIVE_UINT_TYPE size) {
        // make sure there is sufficient size in destination
        if (dest.getBuffCapacity() < size) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        // otherwise, set destination buffer to data from deserialization pointer plus size
        SerializeStatus stat = dest.setBuff(&this->getBuffAddr()[this->m_deserLoc],size);
        if (stat == FW_SERIALIZE_OK) {
            this->m_deserLoc += size;
        }
        return stat;

    }

    SerializeStatus SerializeBufferBase::copyRawOffset(SerializeBufferBase& dest, NATIVE_UINT_TYPE size) {
        // make sure there is sufficient size in destination
        if (dest.getBuffCapacity() < size + dest.getBuffLength()) {
            return FW_SERIALIZE_NO_ROOM_LEFT;
        }
        // make sure there is sufficient buffer in source
        if (this->getBuffLeft() < size) {
            return FW_DESERIALIZE_SIZE_MISMATCH;
        }

        // otherwise, serialize bytes to destination without writing length
        SerializeStatus stat = dest.serialize(&this->getBuffAddr()[this->m_deserLoc], size, true);
        if (stat == FW_SERIALIZE_OK) {
            this->m_deserLoc += size;
        }
        return stat;

    }

    // return address of buffer not yet deserialized. This is used
    // to copy the remainder of a buffer.
    const U8* SerializeBufferBase::getBuffAddrLeft() const {
        return &this->getBuffAddr()[this->m_deserLoc];
    }

    //!< gets address of end of serialization. Used to manually place data at the end
    U8* SerializeBufferBase::getBuffAddrSer() {
        return &this->getBuffAddr()[this->m_serLoc];
    }

#ifdef BUILD_UT
    bool SerializeBufferBase::operator==(const SerializeBufferBase& other) const {
        if (this->getBuffLength() != other.getBuffLength()) {
            return false;
        }

        const U8* us = this->getBuffAddr();
        const U8* them = other.getBuffAddr();

        FW_ASSERT(us);
        FW_ASSERT(them);

        for (NATIVE_UINT_TYPE byte = 0; byte < this->getBuffLength(); byte++) {
            if (us[byte] != them[byte]) {
                return false;
            }
        }

        return true;
    }

    std::ostream& operator<<(std::ostream& os, const SerializeBufferBase& buff) {

        const U8* us = buff.getBuffAddr();

        FW_ASSERT(us);

        for (NATIVE_UINT_TYPE byte = 0; byte < buff.getBuffLength(); byte++) {
            os << "[" << std::setw(2) << std::hex << std::setfill('0') << us[byte] << "]" << std::dec;
        }

        return os;
    }
#endif

    ExternalSerializeBuffer::ExternalSerializeBuffer(U8* buffPtr, NATIVE_UINT_TYPE size) {
        this->setExtBuffer(buffPtr,size);
    }

    ExternalSerializeBuffer::ExternalSerializeBuffer() {
        this->clear();
    }

    void ExternalSerializeBuffer::setExtBuffer(U8* buffPtr, NATIVE_UINT_TYPE size) {
        FW_ASSERT(buffPtr);
        this->m_buff = buffPtr;
        this->m_buffSize = size;
    }

    void ExternalSerializeBuffer::clear() {
        this->m_buff = nullptr;
        this->m_buffSize = 0;
    }

    NATIVE_UINT_TYPE ExternalSerializeBuffer::getBuffCapacity() const {
        return this->m_buffSize;
    }

    U8* ExternalSerializeBuffer::getBuffAddr() {
        return this->m_buff;
    }

    const U8* ExternalSerializeBuffer::getBuffAddr() const {
        return this->m_buff;
    }

}

