// ======================================================================
// \title  Receiver.cpp
// \author tumbar
// \brief  cpp file for Receiver component implementation class
// ======================================================================

#include "FppTest/topology/components/Receiver/Receiver.hpp"

#include "FppTest/topology/ports/SenderIdEnumAc.hpp"
#include "Fw/Types/SerialBuffer.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Receiver ::Receiver(const char* const compName)
    : ReceiverComponentBase(compName), m_data{}, m_recv(m_data, sizeof(m_data)) {}

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
    m_recv.resetSer();
    a.serializeTo(m_recv);
    aRef.serializeTo(m_recv);
    b.serializeTo(m_recv);
    bRef.serializeTo(m_recv);
    c.serializeTo(m_recv);
    cRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::ASYNC, portNum, TestDeploymentPort::ARRAY_ARGS_ASYNC, data);
}

void Receiver ::arrayArgsGuarded_handler(FwIndexType portNum,
                                         const FormalParamArray& a,
                                         FormalParamArray& aRef,
                                         const FormalAliasArray& b,
                                         FormalAliasArray& bRef,
                                         const FormalAliasStringArray& c,
                                         FormalAliasStringArray& cRef) {
    m_recv.resetSer();
    a.serializeTo(m_recv);
    aRef.serializeTo(m_recv);
    b.serializeTo(m_recv);
    bRef.serializeTo(m_recv);
    c.serializeTo(m_recv);
    cRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::ARRAY_ARGS_GUARDED, data);
}

void Receiver ::arrayArgsSync_handler(FwIndexType portNum,
                                      const FormalParamArray& a,
                                      FormalParamArray& aRef,
                                      const FormalAliasArray& b,
                                      FormalAliasArray& bRef,
                                      const FormalAliasStringArray& c,
                                      FormalAliasStringArray& cRef) {
    m_recv.resetSer();
    a.serializeTo(m_recv);
    aRef.serializeTo(m_recv);
    b.serializeTo(m_recv);
    bRef.serializeTo(m_recv);
    c.serializeTo(m_recv);
    cRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::ARRAY_ARGS_SYNC, data);
}

FormalParamArray Receiver ::arrayReturnGuarded_handler(FwIndexType portNum,
                                                       const FormalParamArray& a,
                                                       FormalParamArray& aRef) {
    m_recv.resetSer();
    a.serializeTo(m_recv);
    aRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::ARRAY_RETURN_GUARDED, data);
    return a;
}

FormalParamArray Receiver ::arrayReturnSync_handler(FwIndexType portNum,
                                                    const FormalParamArray& a,
                                                    FormalParamArray& aRef) {
    m_recv.resetSer();
    a.serializeTo(m_recv);
    aRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::ARRAY_RETURN_SYNC, data);
    return a;
}

FormalAliasStringArray Receiver ::arrayStringAliasReturnGuarded_handler(FwIndexType portNum,
                                                                        const FormalParamArray& a,
                                                                        FormalParamArray& aRef) {
    m_recv.resetSer();
    a.serializeTo(m_recv);
    aRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_GUARDED, data);
    return FormalAliasStringArray({"a", "b", "c"});
}

FormalAliasStringArray Receiver ::arrayStringAliasReturnSync_handler(FwIndexType portNum,
                                                                     const FormalParamArray& a,
                                                                     FormalParamArray& aRef) {
    m_recv.resetSer();
    a.serializeTo(m_recv);
    aRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_SYNC, data);
    return FormalAliasStringArray({"a", "b", "c"});
}

void Receiver ::enumArgsAsync_handler(FwIndexType portNum,
                                      const FormalParamEnum& en,
                                      FormalParamEnum& enRef,
                                      const FormalAliasEnum& enA,
                                      FormalAliasEnum& enARef) {
    m_recv.resetSer();
    en.serializeTo(m_recv);
    enRef.serializeTo(m_recv);
    enA.serializeTo(m_recv);
    enARef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::ASYNC, portNum, TestDeploymentPort::ENUM_ARGS_ASYNC, data);
}

void Receiver ::enumArgsGuarded_handler(FwIndexType portNum,
                                        const FormalParamEnum& en,
                                        FormalParamEnum& enRef,
                                        const FormalAliasEnum& enA,
                                        FormalAliasEnum& enARef) {
    m_recv.resetSer();
    en.serializeTo(m_recv);
    enRef.serializeTo(m_recv);
    enA.serializeTo(m_recv);
    enARef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::ENUM_ARGS_GUARDED, data);
}

void Receiver ::enumArgsHook_handler(FwIndexType portNum,
                                     const FormalParamEnum& en,
                                     FormalParamEnum& enRef,
                                     const FormalAliasEnum& enA,
                                     FormalAliasEnum& enARef) {
    FW_ASSERT(0);
}

void Receiver ::enumArgsSync_handler(FwIndexType portNum,
                                     const FormalParamEnum& en,
                                     FormalParamEnum& enRef,
                                     const FormalAliasEnum& enA,
                                     FormalAliasEnum& enARef) {
    m_recv.resetSer();
    en.serializeTo(m_recv);
    enRef.serializeTo(m_recv);
    enA.serializeTo(m_recv);
    enARef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::ENUM_ARGS_SYNC, data);
}

FormalParamEnum Receiver ::enumReturnGuarded_handler(FwIndexType portNum,
                                                     const FormalParamEnum& en,
                                                     FormalParamEnum& enRef) {
    m_recv.resetSer();
    en.serializeTo(m_recv);
    enRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::ENUM_RETURN_GUARDED, data);
    return en;
}

FormalParamEnum Receiver ::enumReturnSync_handler(FwIndexType portNum,
                                                  const FormalParamEnum& en,
                                                  FormalParamEnum& enRef) {
    m_recv.resetSer();
    en.serializeTo(m_recv);
    enRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::ENUM_RETURN_SYNC, data);
    return en;
}

void Receiver ::noArgsAsync_handler(FwIndexType portNum) {
    m_recv.resetSer();
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::ASYNC, portNum, TestDeploymentPort::NO_ARGS_ASYNC, data);
}

void Receiver ::noArgsGuarded_handler(FwIndexType portNum) {
    m_recv.resetSer();
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::NO_ARGS_GUARDED, data);
}

bool Receiver ::noArgsReturnGuarded_handler(FwIndexType portNum) {
    m_recv.resetSer();
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::NO_ARGS_RETURN_GUARDED, data);
    return true;
}

bool Receiver ::noArgsReturnSync_handler(FwIndexType portNum) {
    m_recv.resetSer();
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::NO_ARGS_RETURN_SYNC, data);
    return true;
}

void Receiver ::noArgsSync_handler(FwIndexType portNum) {
    m_recv.resetSer();
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::NO_ARGS_SYNC, data);
}

void Receiver ::primitiveArgsAsync_handler(FwIndexType portNum,
                                           U32 u32,
                                           U32& u32Ref,
                                           F32 f32,
                                           F32& f32Ref,
                                           bool b,
                                           bool& bRef) {
    m_recv.resetSer();
    m_recv.serializeFrom(u32);
    m_recv.serializeFrom(u32Ref);
    m_recv.serializeFrom(f32);
    m_recv.serializeFrom(f32Ref);
    m_recv.serializeFrom(b);
    m_recv.serializeFrom(bRef);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::ASYNC, portNum, TestDeploymentPort::PRIMITIVE_ARGS_ASYNC, data);
}

void Receiver ::primitiveArgsGuarded_handler(FwIndexType portNum,
                                             U32 u32,
                                             U32& u32Ref,
                                             F32 f32,
                                             F32& f32Ref,
                                             bool b,
                                             bool& bRef) {
    m_recv.resetSer();
    m_recv.serializeFrom(u32);
    m_recv.serializeFrom(u32Ref);
    m_recv.serializeFrom(f32);
    m_recv.serializeFrom(f32Ref);
    m_recv.serializeFrom(b);
    m_recv.serializeFrom(bRef);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::PRIMITIVE_ARGS_GUARDED, data);
}

void Receiver ::primitiveArgsSync_handler(FwIndexType portNum,
                                          U32 u32,
                                          U32& u32Ref,
                                          F32 f32,
                                          F32& f32Ref,
                                          bool b,
                                          bool& bRef) {
    m_recv.resetSer();
    m_recv.serializeFrom(u32);
    m_recv.serializeFrom(u32Ref);
    m_recv.serializeFrom(f32);
    m_recv.serializeFrom(f32Ref);
    m_recv.serializeFrom(b);
    m_recv.serializeFrom(bRef);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::PRIMITIVE_ARGS_SYNC, data);
}

U32 Receiver ::primitiveReturnGuarded_handler(FwIndexType portNum,
                                              U32 u32,
                                              U32& u32Ref,
                                              F32 f32,
                                              F32& f32Ref,
                                              bool b,
                                              bool& bRef) {
    m_recv.resetSer();
    m_recv.serializeFrom(u32);
    m_recv.serializeFrom(u32Ref);
    m_recv.serializeFrom(f32);
    m_recv.serializeFrom(f32Ref);
    m_recv.serializeFrom(b);
    m_recv.serializeFrom(bRef);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::PRIMITIVE_RETURN_GUARDED, data);
    return u32;
}

U32 Receiver ::primitiveReturnSync_handler(FwIndexType portNum,
                                           U32 u32,
                                           U32& u32Ref,
                                           F32 f32,
                                           F32& f32Ref,
                                           bool b,
                                           bool& bRef) {
    m_recv.resetSer();
    m_recv.serializeFrom(u32);
    m_recv.serializeFrom(u32Ref);
    m_recv.serializeFrom(f32);
    m_recv.serializeFrom(f32Ref);
    m_recv.serializeFrom(b);
    m_recv.serializeFrom(bRef);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::PRIMITIVE_RETURN_SYNC, data);
    return u32;
}

Fw::String Receiver ::stringAliasReturnGuarded_handler(FwIndexType portNum,
                                                       const Fw::StringBase& str,
                                                       Fw::StringBase& strRef) {
    m_recv.resetSer();
    str.serializeTo(m_recv);
    strRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::STRING_ALIAS_RETURN_GUARDED, data);
    return str;
}

Fw::String Receiver ::stringAliasReturnSync_handler(FwIndexType portNum,
                                                    const Fw::StringBase& str,
                                                    Fw::StringBase& strRef) {
    m_recv.resetSer();
    str.serializeTo(m_recv);
    strRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::STRING_ALIAS_RETURN_SYNC, data);
    return str;
}

void Receiver ::stringArgsAsync_handler(FwIndexType portNum,
                                        const Fw::StringBase& str80,
                                        Fw::StringBase& str80Ref,
                                        const Fw::StringBase& str100,
                                        Fw::StringBase& str100Ref) {
    m_recv.resetSer();
    str80.serializeTo(m_recv);
    str80Ref.serializeTo(m_recv);
    str100.serializeTo(m_recv);
    str100Ref.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::ASYNC, portNum, TestDeploymentPort::STRING_ARGS_ASYNC, data);
}

void Receiver ::stringArgsGuarded_handler(FwIndexType portNum,
                                          const Fw::StringBase& str80,
                                          Fw::StringBase& str80Ref,
                                          const Fw::StringBase& str100,
                                          Fw::StringBase& str100Ref) {
    m_recv.resetSer();
    str80.serializeTo(m_recv);
    str80Ref.serializeTo(m_recv);
    str100.serializeTo(m_recv);
    str100Ref.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::STRING_ARGS_GUARDED, data);
}

void Receiver ::stringArgsSync_handler(FwIndexType portNum,
                                       const Fw::StringBase& str80,
                                       Fw::StringBase& str80Ref,
                                       const Fw::StringBase& str100,
                                       Fw::StringBase& str100Ref) {
    m_recv.resetSer();
    str80.serializeTo(m_recv);
    str80Ref.serializeTo(m_recv);
    str100.serializeTo(m_recv);
    str100Ref.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::STRING_ARGS_SYNC, data);
}

Fw::String Receiver ::stringReturnGuarded_handler(FwIndexType portNum,
                                                  const Fw::StringBase& str,
                                                  Fw::StringBase& strRef) {
    m_recv.resetSer();
    str.serializeTo(m_recv);
    strRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::STRING_RETURN_GUARDED, data);
    return str;
}

Fw::String Receiver ::stringReturnSync_handler(FwIndexType portNum, const Fw::StringBase& str, Fw::StringBase& strRef) {
    m_recv.resetSer();
    str.serializeTo(m_recv);
    strRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::STRING_RETURN_SYNC, data);
    return str;
}

void Receiver ::structArgsAsync_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    m_recv.resetSer();
    s.serializeTo(m_recv);
    sRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::ASYNC, portNum, TestDeploymentPort::STRUCT_ARGS_ASYNC, data);
}

void Receiver ::structArgsGuarded_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    m_recv.resetSer();
    s.serializeTo(m_recv);
    sRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::STRUCT_ARGS_GUARDED, data);
}

void Receiver ::structArgsSync_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    m_recv.resetSer();
    s.serializeTo(m_recv);
    sRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::STRUCT_ARGS_SYNC, data);
}

FormalParamStruct Receiver ::structReturnGuarded_handler(FwIndexType portNum,
                                                         const FormalParamStruct& s,
                                                         FormalParamStruct& sRef) {
    m_recv.resetSer();
    s.serializeTo(m_recv);
    // sRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::GUARDED, portNum, TestDeploymentPort::STRUCT_RETURN_GUARDED, data);
    return s;
}

FormalParamStruct Receiver ::structReturnSync_handler(FwIndexType portNum,
                                                      const FormalParamStruct& s,
                                                      FormalParamStruct& sRef) {
    m_recv.resetSer();
    s.serializeTo(m_recv);
    // sRef.serializeTo(m_recv);
    Fw::Buffer data(m_data, m_recv.getSize());
    replyOut_out(SenderId::SYNC, portNum, TestDeploymentPort::STRUCT_RETURN_SYNC, data);
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
    FW_ASSERT(0);
}

}  // namespace FppTest
