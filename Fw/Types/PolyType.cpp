#include <Fw/Types/PolyType.hpp>
#include <Fw/Types/Assert.hpp>
#include <stdio.h>

namespace Fw {

    // U8 methods

    PolyType::PolyType(void) {
        this->m_dataType = TYPE_NOTYPE;
    }

    PolyType::PolyType(U8 val) {
        this->m_dataType = TYPE_U8;
        this->m_val.u8Val = val;
    }

    PolyType::operator U8() {
        FW_ASSERT(TYPE_U8 == this->m_dataType);
        return this->m_val.u8Val;
    }

    void PolyType::get(U8& val) {
        FW_ASSERT(TYPE_U8 == this->m_dataType);
        val = this->m_val.u8Val;
    }

    bool PolyType::isU8(void) {
        return (TYPE_U8 == this->m_dataType);
    }

    U8 PolyType::operator=(U8 other) {
        this->m_dataType = TYPE_U8;
        return this->m_val.u8Val = other;
    }

    // I8 methods

    PolyType::PolyType(I8 val) {
        this->m_dataType = TYPE_I8;
        this->m_val.i8Val = val;
    }

    PolyType::operator I8() {
        FW_ASSERT(TYPE_I8 == this->m_dataType);
        return this->m_val.i8Val;
    }

    void PolyType::get(I8& val) {
        FW_ASSERT(TYPE_I8 == this->m_dataType);
        val = this->m_val.i8Val;
    }

    bool PolyType::isI8(void) {
        return (TYPE_I8 == this->m_dataType);
    }

    I8 PolyType::operator=(I8 other) {
        this->m_dataType = TYPE_I8;
        return this->m_val.i8Val = other;
    }

#if FW_HAS_16_BIT

    // U16 methods

    PolyType::PolyType(U16 val) {
        this->m_dataType = TYPE_U16;
        this->m_val.u16Val = val;
    }

    PolyType::operator U16() {
        FW_ASSERT(TYPE_U16 == this->m_dataType);
        return this->m_val.u16Val;
    }

    void PolyType::get(U16& val) {
        FW_ASSERT(TYPE_U16 == this->m_dataType);
        val = this->m_val.u16Val;
    }

    bool PolyType::isU16(void) {
        return (TYPE_U16 == this->m_dataType);
    }

    U16 PolyType::operator=(U16 other) {
        this->m_dataType = TYPE_U16;
        return this->m_val.u16Val = other;
    }

    // I16 methods

    PolyType::PolyType(I16 val) {
        this->m_dataType = TYPE_I16;
        this->m_val.i16Val = val;
    }

    PolyType::operator I16() {
        FW_ASSERT(TYPE_I16 == this->m_dataType);
        return this->m_val.i16Val;
    }

    void PolyType::get(I16& val) {
        FW_ASSERT(TYPE_I16 == this->m_dataType);
        val = this->m_val.i16Val;
    }

    bool PolyType::isI16(void) {
        return (TYPE_I16 == this->m_dataType);
    }

    I16 PolyType::operator=(I16 other) {
        this->m_dataType = TYPE_I16;
        return this->m_val.i16Val = other;
    }

#endif

#if FW_HAS_32_BIT

    // U32 methods

    PolyType::PolyType(U32 val) {
        this->m_dataType = TYPE_U32;
        this->m_val.u32Val = val;
    }

    PolyType::operator U32() {
        FW_ASSERT(TYPE_U32 == this->m_dataType);
        return this->m_val.u32Val;
    }

    void PolyType::get(U32& val) {
        FW_ASSERT(TYPE_U32 == this->m_dataType);
        val = this->m_val.u32Val;
    }

    bool PolyType::isU32(void) {
        return (TYPE_U32 == this->m_dataType);
    }

    U32 PolyType::operator=(U32 other) {
        this->m_dataType = TYPE_U32;
        return this->m_val.u32Val = other;
    }

    // I32 methods

    PolyType::PolyType(I32 val) {
        this->m_dataType = TYPE_I32;
        this->m_val.i32Val = val;
    }

    PolyType::operator I32() {
        FW_ASSERT(TYPE_I32 == this->m_dataType);
        return this->m_val.i32Val;
    }

    void PolyType::get(I32& val) {
        FW_ASSERT(TYPE_I32 == this->m_dataType);
        val = this->m_val.i32Val;
    }

    bool PolyType::isI32(void) {
        return (TYPE_I32 == this->m_dataType);
    }

    I32 PolyType::operator=(I32 other) {
        this->m_dataType = TYPE_I32;
        return this->m_val.i32Val = other;
    }

#endif    
#if FW_HAS_64_BIT

    // U64 methods

    PolyType::PolyType(U64 val) {
        this->m_dataType = TYPE_U64;
        this->m_val.u64Val = val;
    }

    PolyType::operator U64() {
        FW_ASSERT(TYPE_U64 == this->m_dataType);
        return this->m_val.u64Val;
    }

    void PolyType::get(U64& val) {
        FW_ASSERT(TYPE_U64 == this->m_dataType);
        val = this->m_val.u64Val;
    }

    bool PolyType::isU64(void) {
        return (TYPE_U64 == this->m_dataType);
    }

    U64 PolyType::operator=(U64 other) {
        this->m_dataType = TYPE_U64;
        return this->m_val.u64Val = other;
    }

    // I64 methods

    PolyType::PolyType(I64 val) {
        this->m_dataType = TYPE_I64;
        this->m_val.u64Val = val;
    }

    PolyType::operator I64() {
        FW_ASSERT(TYPE_I64 == this->m_dataType);
        return this->m_val.i64Val;
    }

    void PolyType::get(I64& val) {
        FW_ASSERT(TYPE_I64 == this->m_dataType);
        val = this->m_val.i64Val;
    }

    bool PolyType::isI64(void) {
        return (TYPE_I64 == this->m_dataType);
    }

    I64 PolyType::operator=(I64 other) {
        this->m_dataType = TYPE_I64;
        return this->m_val.i64Val = other;
    }

#endif

#if FW_HAS_F64

    PolyType::PolyType(F64 val) {
        this->m_dataType = TYPE_F64;
        this->m_val.f64Val = val;
    }

    PolyType::operator F64() {
        FW_ASSERT(TYPE_F64 == this->m_dataType);
        return this->m_val.f64Val;
    }

    void PolyType::get(F64& val) {
        FW_ASSERT(TYPE_F64 == this->m_dataType);
        val = this->m_val.f64Val;
    }

    bool PolyType::isF64(void) {
        return (TYPE_F64 == this->m_dataType);
    }

    F64 PolyType::operator=(F64 other) {
        this->m_dataType = TYPE_F64;
        return this->m_val.f64Val = other;
    }

#endif
    PolyType::PolyType(F32 val) {
        this->m_dataType = TYPE_F32;
        this->m_val.f32Val = val;
    }

    PolyType::operator F32() {
        FW_ASSERT(TYPE_F32 == this->m_dataType);
        return this->m_val.f32Val;
    }

    void PolyType::get(F32& val) {
        FW_ASSERT(TYPE_F32 == this->m_dataType);
        val = this->m_val.f32Val;
    }

    bool PolyType::isF32(void) {
        return (TYPE_F32 == this->m_dataType);
    }

    F32 PolyType::operator=(F32 other) {
        this->m_dataType = TYPE_F32;
        return this->m_val.f32Val = other;
    }

    PolyType::PolyType(bool val) {
        this->m_dataType = TYPE_BOOL;
        this->m_val.boolVal = val;
    }

    PolyType::operator bool() {
        FW_ASSERT(TYPE_BOOL == this->m_dataType);
        return this->m_val.boolVal;
    }

    void PolyType::get(bool& val) {
        FW_ASSERT(TYPE_BOOL == this->m_dataType);
        val = this->m_val.boolVal;
    }

    bool PolyType::isBool(void) {
        return (TYPE_BOOL == this->m_dataType);
    }

    bool PolyType::operator=(bool other) {
        this->m_dataType = TYPE_BOOL;
        return this->m_val.boolVal = other;
    }

    PolyType::PolyType(void* val) {
        this->m_dataType = TYPE_PTR;
        this->m_val.ptrVal = val;
    }

    PolyType::operator void*() {
        FW_ASSERT(TYPE_PTR == this->m_dataType);
        return this->m_val.ptrVal;
    }

    void PolyType::get(void*& val) {
        FW_ASSERT(TYPE_PTR == this->m_dataType);
        val = this->m_val.ptrVal;
    }

    bool PolyType::isPtr(void) {
        return (TYPE_PTR == this->m_dataType);
    }

    void* PolyType::operator=(void* other) {
        this->m_dataType = TYPE_PTR;
        return this->m_val.ptrVal = other;
    }

    PolyType::~PolyType(void) {
    }

    const PolyType& PolyType::operator=(const PolyType &src) {
        this->m_dataType = src.m_dataType;
        this->m_val = src.m_val;
        return src;
    }

    bool PolyType::operator!=(const PolyType &other) const {
        return !operator==(other);
    }

    bool PolyType::operator==(const PolyType &other) const {

        // if type doesn't match, not equal
        if (this->m_dataType != other.m_dataType) {
            return false;
        } else {
        // check based on type
            bool valIsEqual = false;
            switch (this->m_dataType) {
                case TYPE_U8:
                    valIsEqual = (this->m_val.u8Val == other.m_val.u8Val);
                    break;
                case TYPE_I8:
                    valIsEqual = (this->m_val.i8Val == other.m_val.i8Val);
                    break;
#if FW_HAS_16_BIT
                case TYPE_U16:
                    valIsEqual = (this->m_val.u16Val == other.m_val.u16Val);
                    break;
                case TYPE_I16:
                    valIsEqual = (this->m_val.i16Val == other.m_val.i16Val);
                    break;
#endif
#if FW_HAS_32_BIT
                case TYPE_U32:
                    valIsEqual = (this->m_val.u32Val == other.m_val.u32Val);
                    break;
                case TYPE_I32:
                    valIsEqual = (this->m_val.i32Val == other.m_val.i32Val);
                    break;
#endif
#if FW_HAS_64_BIT
                case TYPE_U64:
                    valIsEqual = (this->m_val.u64Val == other.m_val.u64Val);
                    break;
                case TYPE_I64:
                    valIsEqual = (this->m_val.i64Val == other.m_val.i64Val);
                    break;
#endif
                case TYPE_BOOL:
                    valIsEqual = (this->m_val.boolVal == other.m_val.boolVal);
                    break;
                case TYPE_PTR:
                    valIsEqual = (this->m_val.ptrVal == other.m_val.ptrVal);
                    break;
#if FW_HAS_64_BIT
                case TYPE_F64: // fall through, shouldn't test floating point
#endif
                case TYPE_F32: // fall through, shouldn't test floating point
                case TYPE_NOTYPE:
                    valIsEqual = false;
                    break;
                default:
                    FW_ASSERT(0,static_cast<NATIVE_INT_TYPE>(this->m_dataType));
                    return false; // for compiler
                }
            return valIsEqual;
        }

    }

    SerializeStatus PolyType::serialize(SerializeBufferBase& buffer) const {

        // store type
        SerializeStatus stat = buffer.serialize(static_cast<FwEnumStoreType> (this->m_dataType));

        // switch on type
        switch (this->m_dataType) {
            case TYPE_U8:
                stat = buffer.serialize(this->m_val.u8Val);
                break;
            case TYPE_I8:
                stat = buffer.serialize(this->m_val.i8Val);
                break;
#if FW_HAS_16_BIT
            case TYPE_U16:
                stat = buffer.serialize(this->m_val.u16Val);
                break;
            case TYPE_I16:
                stat = buffer.serialize(this->m_val.i16Val);
                break;
#endif
#if FW_HAS_32_BIT
            case TYPE_U32:
                stat = buffer.serialize(this->m_val.u32Val);
                break;
            case TYPE_I32:
                stat = buffer.serialize(this->m_val.i32Val);
                break;
#endif
#if FW_HAS_64_BIT
            case TYPE_U64:
                stat = buffer.serialize(this->m_val.u64Val);
                break;
            case TYPE_I64:
                stat = buffer.serialize(this->m_val.i64Val);
                break;
            case TYPE_F64:
                stat = buffer.serialize(this->m_val.f64Val);
                break;
#endif				
            case TYPE_F32:
                stat = buffer.serialize(this->m_val.f32Val);
                break;
            case TYPE_BOOL:
                stat = buffer.serialize(this->m_val.boolVal);
                break;
            case TYPE_PTR:
                stat = buffer.serialize(this->m_val.ptrVal);
                break;
            default:
                stat = FW_SERIALIZE_FORMAT_ERROR;
                break;
            }
        
        return stat;
    }

    SerializeStatus PolyType::deserialize(SerializeBufferBase& buffer) {
        // get type
        FwEnumStoreType des;
        SerializeStatus stat = buffer.deserialize(des);

        if (stat != FW_SERIALIZE_OK) {
            return stat;
        } else {
            this->m_dataType = static_cast<Type>(des);
            // switch on type
            switch (this->m_dataType) {
                case TYPE_U8:
                    return buffer.deserialize(this->m_val.u8Val);
                case TYPE_I8:
                    return buffer.deserialize(this->m_val.i8Val);
#if FW_HAS_16_BIT
                case TYPE_U16:
                    return buffer.deserialize(this->m_val.u16Val);
                case TYPE_I16:
                    return buffer.deserialize(this->m_val.i16Val);
#endif                    
#if FW_HAS_32_BIT
                case TYPE_U32:
                    return buffer.deserialize(this->m_val.u32Val);
                case TYPE_I32:
                    return buffer.deserialize(this->m_val.i32Val);
#endif                    
#if FW_HAS_64_BIT
                case TYPE_U64:
                    return buffer.deserialize(this->m_val.u64Val);
                case TYPE_I64:
                    return buffer.deserialize(this->m_val.i64Val);
                case TYPE_F64:
                    return buffer.deserialize(this->m_val.f64Val);
#endif              
                case TYPE_F32:
                    return buffer.deserialize(this->m_val.f32Val);
                case TYPE_BOOL:
                    return buffer.deserialize(this->m_val.boolVal);
                case TYPE_PTR:
                    return buffer.deserialize(this->m_val.ptrVal);
                default:
                    return FW_DESERIALIZE_FORMAT_ERROR;
            }
        }

    }

#if FW_OBJECT_TO_STRING

    void PolyType::toString(StringBase& dest) const {
    	this->toString(dest,false);
    }

    void PolyType::toString(StringBase& dest, bool append) const {
        char valString[80];

        switch (this->m_dataType) {
            case TYPE_U8:
                (void) snprintf(valString, sizeof(valString), "%d ", this->m_val.u8Val);
                break;
            case TYPE_I8:
            	(void) snprintf(valString, sizeof(valString), "%d ", this->m_val.i8Val);
                break;
#if FW_HAS_16_BIT
            case TYPE_U16:
            	(void) snprintf(valString, sizeof(valString), "%d ", this->m_val.u16Val);
                break;
            case TYPE_I16:
            	(void) snprintf(valString, sizeof(valString), "%d ", this->m_val.i16Val);
                break;
#endif                
#if FW_HAS_32_BIT
            case TYPE_U32:
            	(void) snprintf(valString, sizeof(valString), "%d ", this->m_val.u32Val);
                break;
            case TYPE_I32:
            	(void) snprintf(valString, sizeof(valString), "%d ", this->m_val.i32Val);
                break;
#endif                
#if FW_HAS_64_BIT
            case TYPE_U64:
            	(void) snprintf(valString, sizeof(valString), "%llu ", (unsigned long long)this->m_val.u64Val);
                break;
            case TYPE_I64:
            	(void) snprintf(valString, sizeof(valString), "%lld ", (long long)this->m_val.i64Val);
                break;
#endif
#if FW_HAS_F64
            case TYPE_F64:
            	(void) snprintf(valString, sizeof(valString), "%lg ", this->m_val.f64Val);
                break;
#endif
            case TYPE_F32:
            	(void) snprintf(valString, sizeof(valString), "%g ", this->m_val.f32Val);
                break;
            case TYPE_BOOL:
            	(void) snprintf(valString, sizeof(valString), "%s ", this->m_val.boolVal?"T":"F");
                break;
            case TYPE_PTR:
            	(void) snprintf(valString, sizeof(valString), "%p ", this->m_val.ptrVal);
                break;
            default:
            	(void) snprintf(valString, sizeof(valString), "%s ", "NT");
                break;
        }
        
        // NULL terminate
        valString[sizeof(valString)-1] = 0;

        if (append) {
            dest += valString;
        } else {
            dest = valString;
        }

    }

#endif
}

