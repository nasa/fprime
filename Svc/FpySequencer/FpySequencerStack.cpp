#include <cstring>
#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

template <typename T>
T FpySequencer::Stack::pop() {
    static_assert(sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "size must be 1, 2, 4, 8");
    FW_ASSERT(this->size >= sizeof(T), static_cast<FwAssertArgType>(this->size),
              static_cast<FwAssertArgType>(sizeof(T)));
    // first make a byte array which can definitely store our val
    U8 valBytes[8] = {0};
    // now move top of stack into byte array and shrink stack
    memcpy(valBytes, this->top() - sizeof(T), sizeof(T));
    this->size -= static_cast<Fpy::StackSizeType>(sizeof(T));

    // now do appropriate byteswap on byte array
    if (sizeof(T) == 8) {
        return static_cast<T>((static_cast<T>(valBytes[7]) << 0) | (static_cast<T>(valBytes[6]) << 8) |
                              (static_cast<T>(valBytes[5]) << 16) | (static_cast<T>(valBytes[4]) << 24) |
                              (static_cast<T>(valBytes[3]) << 32) | (static_cast<T>(valBytes[2]) << 40) |
                              (static_cast<T>(valBytes[1]) << 48) | (static_cast<T>(valBytes[0]) << 56));
    } else if (sizeof(T) == 4) {
        return static_cast<T>((static_cast<T>(valBytes[3]) << 0) | (static_cast<T>(valBytes[2]) << 8) |
                              (static_cast<T>(valBytes[1]) << 16) | (static_cast<T>(valBytes[0]) << 24));
    } else if (sizeof(T) == 2) {
        return static_cast<T>((static_cast<T>(valBytes[1]) << 0) | (static_cast<T>(valBytes[0]) << 8));
    } else {
        return static_cast<T>(valBytes[0]);
    }
}

template U8 FpySequencer::Stack::pop();
template U16 FpySequencer::Stack::pop();
template U32 FpySequencer::Stack::pop();
template U64 FpySequencer::Stack::pop();
template I8 FpySequencer::Stack::pop();
template I16 FpySequencer::Stack::pop();
template I32 FpySequencer::Stack::pop();
template I64 FpySequencer::Stack::pop();

template <>
F32 FpySequencer::Stack::pop<F32>() {
    U32 endianness = this->pop<U32>();
    F32 val;
    memcpy(&val, &endianness, sizeof(val));
    return val;
}

template <>
F64 FpySequencer::Stack::pop<F64>() {
    U64 endianness = this->pop<U64>();
    F64 val;
    memcpy(&val, &endianness, sizeof(val));
    return val;
}

template <typename T>
void FpySequencer::Stack::push(T val) {
    static_assert(sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "size must be 1, 2, 4, 8");
    FW_ASSERT(this->size + sizeof(val) < Fpy::MAX_STACK_SIZE, static_cast<FwAssertArgType>(this->size),
              static_cast<FwAssertArgType>(sizeof(T)));
    // first make a byte array which can definitely store our val
    U8 valBytes[8] = {0};
    // convert val to unsigned to avoid undefined behavior for bitshifts of signed types
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT valUnsigned = static_cast<UnsignedT>(val);
    if (sizeof(T) == 8) {
        valBytes[0] = static_cast<U8>(valUnsigned >> 56);
        valBytes[1] = static_cast<U8>(valUnsigned >> 48);
        valBytes[2] = static_cast<U8>(valUnsigned >> 40);
        valBytes[3] = static_cast<U8>(valUnsigned >> 32);
        valBytes[4] = static_cast<U8>(valUnsigned >> 24);
        valBytes[5] = static_cast<U8>(valUnsigned >> 16);
        valBytes[6] = static_cast<U8>(valUnsigned >> 8);
        valBytes[7] = static_cast<U8>(valUnsigned >> 0);
    } else if (sizeof(T) == 4) {
        valBytes[0] = static_cast<U8>(valUnsigned >> 24);
        valBytes[1] = static_cast<U8>(valUnsigned >> 16);
        valBytes[2] = static_cast<U8>(valUnsigned >> 8);
        valBytes[3] = static_cast<U8>(valUnsigned >> 0);
    } else if (sizeof(T) == 2) {
        valBytes[0] = static_cast<U8>(valUnsigned >> 8);
        valBytes[1] = static_cast<U8>(valUnsigned >> 0);
    } else {
        valBytes[0] = static_cast<U8>(valUnsigned);
    }
    memcpy(this->top(), valBytes, sizeof(T));
    this->size += static_cast<Fpy::StackSizeType>(sizeof(T));
}

template void FpySequencer::Stack::push(U8);
template void FpySequencer::Stack::push(U16);
template void FpySequencer::Stack::push(U32);
template void FpySequencer::Stack::push(U64);
template void FpySequencer::Stack::push(I8);
template void FpySequencer::Stack::push(I16);
template void FpySequencer::Stack::push(I32);
template void FpySequencer::Stack::push(I64);

template <>
void FpySequencer::Stack::push<F32>(F32 val) {
    U32 endianness;
    memcpy(&endianness, &val, sizeof(val));
    this->push(endianness);
}

template <>
void FpySequencer::Stack::push<F64>(F64 val) {
    U64 endianness;
    memcpy(&endianness, &val, sizeof(val));
    this->push(endianness);
}

// pops a byte array from the top of the stack into the destination array
// does not convert endianness
void FpySequencer::Stack::pop(U8* dest, Fpy::StackSizeType destSize) {
    FW_ASSERT(this->size >= destSize, static_cast<FwAssertArgType>(this->size), static_cast<FwAssertArgType>(destSize));
    memcpy(dest, this->top() - destSize, destSize);
    this->size -= destSize;
}

// pushes a byte array to the top of the stack from the source array
// leaves the source array unmodified
// does not convert endianness
void FpySequencer::Stack::push(U8* src, Fpy::StackSizeType srcSize) {
    FW_ASSERT(this->size + srcSize < Fpy::MAX_STACK_SIZE, static_cast<FwAssertArgType>(this->size),
              static_cast<FwAssertArgType>(srcSize));
    memcpy(this->top(), src, srcSize);
    this->size += srcSize;
}

// pushes zero bytes to the stack
void FpySequencer::Stack::pushZeroes(Fpy::StackSizeType byteCount) {
    FW_ASSERT(this->size + byteCount < Fpy::MAX_STACK_SIZE, static_cast<FwAssertArgType>(this->size),
              static_cast<FwAssertArgType>(byteCount));
    memset(this->top(), 0, byteCount);
    this->size += byteCount;
}

U8* FpySequencer::Stack::top() {
    return &this->bytes[this->size];
}

U8* FpySequencer::Stack::lvars() {
    return this->bytes + this->lvarOffset();
}

Fpy::StackSizeType FpySequencer::Stack::lvarOffset() {
    // at the moment, because we only have one stack frame,
    // lvars always start at 0
    return 0;
}

}  // namespace Svc
