// ======================================================================
// \title  Receiver.cpp
// \author tumbar
// \brief  cpp file for Receiver component implementation class
// ======================================================================

#include "FppTest/topology/components/Receiver/Receiver.hpp"

#include "Fw/Types/SerialBuffer.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Receiver ::Receiver(const char* const compName)
    : ReceiverComponentBase(compName), numMessages(0), lastPortNum(-1), recv(m_data, sizeof(m_data)), m_data{} {}

Receiver ::~Receiver() = default;

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Receiver ::arrayArgsAsync_handler(FwIndexType portNum,
                                       const FormalParamArray& a,
                                       FormalParamArray& aRef,
                                       const FormalAliasArray& b,
                                       FormalAliasArray& bRef,
                                       const FormalAliasStringArray& c,
                                       FormalAliasStringArray& cRef) {
    lastPortNum = portNum;
    numMessages++;
    a.serializeTo(recv);
    aRef.serializeTo(recv);
    b.serializeTo(recv);
    bRef.serializeTo(recv);
    c.serializeTo(recv);
    cRef.serializeTo(recv);
}

void Receiver ::arrayArgsGuarded_handler(FwIndexType portNum,
                                         const FormalParamArray& a,
                                         FormalParamArray& aRef,
                                         const FormalAliasArray& b,
                                         FormalAliasArray& bRef,
                                         const FormalAliasStringArray& c,
                                         FormalAliasStringArray& cRef) {
    lastPortNum = portNum;
    numMessages++;
    a.serializeTo(recv);
    aRef.serializeTo(recv);
    b.serializeTo(recv);
    bRef.serializeTo(recv);
    c.serializeTo(recv);
    cRef.serializeTo(recv);
}

void Receiver ::arrayArgsSync_handler(FwIndexType portNum,
                                      const FormalParamArray& a,
                                      FormalParamArray& aRef,
                                      const FormalAliasArray& b,
                                      FormalAliasArray& bRef,
                                      const FormalAliasStringArray& c,
                                      FormalAliasStringArray& cRef) {
    lastPortNum = portNum;
    numMessages++;
    a.serializeTo(recv);
    aRef.serializeTo(recv);
    b.serializeTo(recv);
    bRef.serializeTo(recv);
    c.serializeTo(recv);
    cRef.serializeTo(recv);
}

FormalParamArray Receiver ::arrayReturnGuarded_handler(FwIndexType portNum,
                                                       const FormalParamArray& a,
                                                       FormalParamArray& aRef) {
    lastPortNum = portNum;
    numMessages++;
    a.serializeTo(recv);
    aRef.serializeTo(recv);
    return aRef;
}

FormalParamArray Receiver ::arrayReturnSync_handler(FwIndexType portNum,
                                                    const FormalParamArray& a,
                                                    FormalParamArray& aRef) {
    lastPortNum = portNum;
    numMessages++;
    a.serializeTo(recv);
    aRef.serializeTo(recv);
    return aRef;
}

FormalAliasStringArray Receiver ::arrayStringAliasReturnGuarded_handler(FwIndexType portNum,
                                                                        const FormalParamArray& a,
                                                                        FormalParamArray& aRef) {
    lastPortNum = portNum;
    numMessages++;
    a.serializeTo(recv);
    aRef.serializeTo(recv);
    return FormalAliasStringArray({"a", "b", "c"});
}

FormalAliasStringArray Receiver ::arrayStringAliasReturnSync_handler(FwIndexType portNum,
                                                                     const FormalParamArray& a,
                                                                     FormalParamArray& aRef) {
    lastPortNum = portNum;
    numMessages++;
    a.serializeTo(recv);
    aRef.serializeTo(recv);
    return FormalAliasStringArray({"a", "b", "c"});
}

void Receiver ::enumArgsAsync_handler(FwIndexType portNum,
                                      const FormalParamEnum& en,
                                      FormalParamEnum& enRef,
                                      const FormalAliasEnum& enA,
                                      FormalAliasEnum& enARef) {
    lastPortNum = portNum;
    numMessages++;
    en.serializeTo(recv);
    enRef.serializeTo(recv);
    enA.serializeTo(recv);
    enARef.serializeTo(recv);
}

void Receiver ::enumArgsGuarded_handler(FwIndexType portNum,
                                        const FormalParamEnum& en,
                                        FormalParamEnum& enRef,
                                        const FormalAliasEnum& enA,
                                        FormalAliasEnum& enARef) {
    lastPortNum = portNum;
    numMessages++;
    en.serializeTo(recv);
    enRef.serializeTo(recv);
    enA.serializeTo(recv);
    enARef.serializeTo(recv);
}

void Receiver ::enumArgsHook_handler(FwIndexType portNum,
                                     const FormalParamEnum& en,
                                     FormalParamEnum& enRef,
                                     const FormalAliasEnum& enA,
                                     FormalAliasEnum& enARef) {
    lastPortNum = portNum;
    numMessages++;
    en.serializeTo(recv);
    enRef.serializeTo(recv);
    enA.serializeTo(recv);
    enARef.serializeTo(recv);
}

void Receiver ::enumArgsSync_handler(FwIndexType portNum,
                                     const FormalParamEnum& en,
                                     FormalParamEnum& enRef,
                                     const FormalAliasEnum& enA,
                                     FormalAliasEnum& enARef) {
    lastPortNum = portNum;
    numMessages++;
    en.serializeTo(recv);
    enRef.serializeTo(recv);
    enA.serializeTo(recv);
    enARef.serializeTo(recv);
}

FormalParamEnum Receiver ::enumReturnGuarded_handler(FwIndexType portNum,
                                                     const FormalParamEnum& en,
                                                     FormalParamEnum& enRef) {
    lastPortNum = portNum;
    numMessages++;
    en.serializeTo(recv);
    enRef.serializeTo(recv);
    return enRef;
}

FormalParamEnum Receiver ::enumReturnSync_handler(FwIndexType portNum,
                                                  const FormalParamEnum& en,
                                                  FormalParamEnum& enRef) {
    lastPortNum = portNum;
    numMessages++;
    en.serializeTo(recv);
    enRef.serializeTo(recv);
    return enRef;
}

void Receiver ::noArgsAsync_handler(FwIndexType portNum) {
    lastPortNum = portNum;
    numMessages++;
}

void Receiver ::noArgsGuarded_handler(FwIndexType portNum) {
    lastPortNum = portNum;
    numMessages++;
}

bool Receiver ::noArgsReturnGuarded_handler(FwIndexType portNum) {
    lastPortNum = portNum;
    numMessages++;
    return true;
}

bool Receiver ::noArgsReturnSync_handler(FwIndexType portNum) {
    lastPortNum = portNum;
    numMessages++;
    return true;
}

void Receiver ::noArgsSync_handler(FwIndexType portNum) {
    lastPortNum = portNum;
    numMessages++;
}

void Receiver ::primitiveArgsAsync_handler(FwIndexType portNum,
                                           U32 u32,
                                           U32& u32Ref,
                                           F32 f32,
                                           F32& f32Ref,
                                           bool b,
                                           bool& bRef) {
    lastPortNum = portNum;
    numMessages++;
    recv.serializeFrom(u32);
    recv.serializeFrom(u32Ref);
    recv.serializeFrom(f32);
    recv.serializeFrom(f32Ref);
    recv.serializeFrom(b);
    recv.serializeFrom(bRef);
}

void Receiver ::primitiveArgsGuarded_handler(FwIndexType portNum,
                                             U32 u32,
                                             U32& u32Ref,
                                             F32 f32,
                                             F32& f32Ref,
                                             bool b,
                                             bool& bRef) {
    lastPortNum = portNum;
    numMessages++;
    recv.serializeFrom(u32);
    recv.serializeFrom(u32Ref);
    recv.serializeFrom(f32);
    recv.serializeFrom(f32Ref);
    recv.serializeFrom(b);
    recv.serializeFrom(bRef);
}

void Receiver ::primitiveArgsSync_handler(FwIndexType portNum,
                                          U32 u32,
                                          U32& u32Ref,
                                          F32 f32,
                                          F32& f32Ref,
                                          bool b,
                                          bool& bRef) {
    lastPortNum = portNum;
    numMessages++;
    recv.serializeFrom(u32);
    recv.serializeFrom(u32Ref);
    recv.serializeFrom(f32);
    recv.serializeFrom(f32Ref);
    recv.serializeFrom(b);
    recv.serializeFrom(bRef);
}

U32 Receiver ::primitiveReturnGuarded_handler(FwIndexType portNum,
                                              U32 u32,
                                              U32& u32Ref,
                                              F32 f32,
                                              F32& f32Ref,
                                              bool b,
                                              bool& bRef) {
    lastPortNum = portNum;
    numMessages++;
    recv.serializeFrom(u32);
    recv.serializeFrom(u32Ref);
    recv.serializeFrom(f32);
    recv.serializeFrom(f32Ref);
    recv.serializeFrom(b);
    recv.serializeFrom(bRef);
    return u32;
}

U32 Receiver ::primitiveReturnSync_handler(FwIndexType portNum,
                                           U32 u32,
                                           U32& u32Ref,
                                           F32 f32,
                                           F32& f32Ref,
                                           bool b,
                                           bool& bRef) {
    lastPortNum = portNum;
    numMessages++;
    recv.serializeFrom(u32);
    recv.serializeFrom(u32Ref);
    recv.serializeFrom(f32);
    recv.serializeFrom(f32Ref);
    recv.serializeFrom(b);
    recv.serializeFrom(bRef);
    return u32;
}

Fw::String Receiver ::stringAliasReturnGuarded_handler(FwIndexType portNum,
                                                       const Fw::StringBase& str,
                                                       Fw::StringBase& strRef) {
    lastPortNum = portNum;
    numMessages++;
    str.serializeTo(recv);
    strRef.serializeTo(recv);
    return str;
}

Fw::String Receiver ::stringAliasReturnSync_handler(FwIndexType portNum,
                                                    const Fw::StringBase& str,
                                                    Fw::StringBase& strRef) {
    lastPortNum = portNum;
    numMessages++;
    str.serializeTo(recv);
    strRef.serializeTo(recv);
    return str;
}

void Receiver ::stringArgsAsync_handler(FwIndexType portNum,
                                        const Fw::StringBase& str80,
                                        Fw::StringBase& str80Ref,
                                        const Fw::StringBase& str100,
                                        Fw::StringBase& str100Ref) {
    lastPortNum = portNum;
    numMessages++;
    str80.serializeTo(recv);
    str80Ref.serializeTo(recv);
    str100.serializeTo(recv);
    str100Ref.serializeTo(recv);
}

void Receiver ::stringArgsGuarded_handler(FwIndexType portNum,
                                          const Fw::StringBase& str80,
                                          Fw::StringBase& str80Ref,
                                          const Fw::StringBase& str100,
                                          Fw::StringBase& str100Ref) {
    lastPortNum = portNum;
    numMessages++;
    str80.serializeTo(recv);
    str80Ref.serializeTo(recv);
    str100.serializeTo(recv);
    str100Ref.serializeTo(recv);
}

void Receiver ::stringArgsSync_handler(FwIndexType portNum,
                                       const Fw::StringBase& str80,
                                       Fw::StringBase& str80Ref,
                                       const Fw::StringBase& str100,
                                       Fw::StringBase& str100Ref) {
    lastPortNum = portNum;
    numMessages++;
    str80.serializeTo(recv);
    str80Ref.serializeTo(recv);
    str100.serializeTo(recv);
    str100Ref.serializeTo(recv);
}

Fw::String Receiver ::stringReturnGuarded_handler(FwIndexType portNum,
                                                  const Fw::StringBase& str,
                                                  Fw::StringBase& strRef) {
    lastPortNum = portNum;
    numMessages++;
    str.serializeTo(recv);
    strRef.serializeTo(recv);
    return str;
}

Fw::String Receiver ::stringReturnSync_handler(FwIndexType portNum, const Fw::StringBase& str, Fw::StringBase& strRef) {
    lastPortNum = portNum;
    numMessages++;
    str.serializeTo(recv);
    strRef.serializeTo(recv);
    return str;
}

void Receiver ::structArgsAsync_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    lastPortNum = portNum;
    numMessages++;
    s.serializeTo(recv);
    sRef.serializeTo(recv);
}

void Receiver ::structArgsGuarded_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    lastPortNum = portNum;
    numMessages++;
    s.serializeTo(recv);
    sRef.serializeTo(recv);
}

void Receiver ::structArgsSync_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    lastPortNum = portNum;
    numMessages++;
    s.serializeTo(recv);
    sRef.serializeTo(recv);
}

FormalParamStruct Receiver ::structReturnGuarded_handler(FwIndexType portNum,
                                                         const FormalParamStruct& s,
                                                         FormalParamStruct& sRef) {
    lastPortNum = portNum;
    numMessages++;
    s.serializeTo(recv);
    // sRef.serializeTo(recv);
    return s;
}

FormalParamStruct Receiver ::structReturnSync_handler(FwIndexType portNum,
                                                      const FormalParamStruct& s,
                                                      FormalParamStruct& sRef) {
    lastPortNum = portNum;
    numMessages++;
    s.serializeTo(recv);
    // sRef.serializeTo(recv);
    return s;
}

// ----------------------------------------------------------------------
// Overflow hook implementations for typed input ports
// ----------------------------------------------------------------------

void Receiver ::enumArgsHook_overflowHook(FwIndexType portNum,
                                          const FormalParamEnum& en,
                                          FormalParamEnum& enRef,
                                          const FormalAliasEnum& enA,
                                          FormalAliasEnum& enARef) {
    lastPortNum = portNum;
    numMessages++;
    en.serializeTo(recv);
    enRef.serializeTo(recv);
    enA.serializeTo(recv);
    enARef.serializeTo(recv);
}

}  // namespace FppTest
