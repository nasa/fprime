// ======================================================================
// \title  Receiver.cpp
// \author tumbar
// \brief  cpp file for Receiver component implementation class
// ======================================================================

#include "FppTest/topology/components/Receiver/Receiver.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Receiver ::Receiver(const char* const compName) : ReceiverComponentBase(compName) {}

Receiver ::~Receiver() {}

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
    // TODO
}

void Receiver ::arrayArgsGuarded_handler(FwIndexType portNum,
                                         const FormalParamArray& a,
                                         FormalParamArray& aRef,
                                         const FormalAliasArray& b,
                                         FormalAliasArray& bRef,
                                         const FormalAliasStringArray& c,
                                         FormalAliasStringArray& cRef) {
    // TODO
}

void Receiver ::arrayArgsSync_handler(FwIndexType portNum,
                                      const FormalParamArray& a,
                                      FormalParamArray& aRef,
                                      const FormalAliasArray& b,
                                      FormalAliasArray& bRef,
                                      const FormalAliasStringArray& c,
                                      FormalAliasStringArray& cRef) {
    // TODO
}

FormalParamArray Receiver ::arrayReturnGuarded_handler(FwIndexType portNum,
                                                       const FormalParamArray& a,
                                                       FormalParamArray& aRef) {
    // TODO return
}

FormalParamArray Receiver ::arrayReturnSync_handler(FwIndexType portNum,
                                                    const FormalParamArray& a,
                                                    FormalParamArray& aRef) {
    // TODO return
}

FormalAliasStringArray Receiver ::arrayStringAliasReturnGuarded_handler(FwIndexType portNum,
                                                                        const FormalParamArray& a,
                                                                        FormalParamArray& aRef) {
    // TODO return
}

FormalAliasStringArray Receiver ::arrayStringAliasReturnSync_handler(FwIndexType portNum,
                                                                     const FormalParamArray& a,
                                                                     FormalParamArray& aRef) {
    // TODO return
}

void Receiver ::enumArgsAsync_handler(FwIndexType portNum,
                                      const FormalParamEnum& en,
                                      FormalParamEnum& enRef,
                                      const FormalAliasEnum& enA,
                                      FormalAliasEnum& enARef) {
    // TODO
}

void Receiver ::enumArgsGuarded_handler(FwIndexType portNum,
                                        const FormalParamEnum& en,
                                        FormalParamEnum& enRef,
                                        const FormalAliasEnum& enA,
                                        FormalAliasEnum& enARef) {
    // TODO
}

void Receiver ::enumArgsHook_handler(FwIndexType portNum,
                                     const FormalParamEnum& en,
                                     FormalParamEnum& enRef,
                                     const FormalAliasEnum& enA,
                                     FormalAliasEnum& enARef) {
    // TODO
}

void Receiver ::enumArgsSync_handler(FwIndexType portNum,
                                     const FormalParamEnum& en,
                                     FormalParamEnum& enRef,
                                     const FormalAliasEnum& enA,
                                     FormalAliasEnum& enARef) {
    // TODO
}

FormalParamEnum Receiver ::enumReturnGuarded_handler(FwIndexType portNum,
                                                     const FormalParamEnum& en,
                                                     FormalParamEnum& enRef) {
    // TODO return
}

FormalParamEnum Receiver ::enumReturnSync_handler(FwIndexType portNum,
                                                  const FormalParamEnum& en,
                                                  FormalParamEnum& enRef) {
    // TODO return
}

void Receiver ::noArgsAsync_handler(FwIndexType portNum) {
    // TODO
}

void Receiver ::noArgsGuarded_handler(FwIndexType portNum) {
    // TODO
}

bool Receiver ::noArgsReturnGuarded_handler(FwIndexType portNum) {
    // TODO return
}

bool Receiver ::noArgsReturnSync_handler(FwIndexType portNum) {
    // TODO return
}

void Receiver ::noArgsSync_handler(FwIndexType portNum) {
    // TODO
}

void Receiver ::primitiveArgsAsync_handler(FwIndexType portNum,
                                           U32 u32,
                                           U32& u32Ref,
                                           F32 f32,
                                           F32& f32Ref,
                                           bool b,
                                           bool& bRef) {
    // TODO
}

void Receiver ::primitiveArgsGuarded_handler(FwIndexType portNum,
                                             U32 u32,
                                             U32& u32Ref,
                                             F32 f32,
                                             F32& f32Ref,
                                             bool b,
                                             bool& bRef) {
    // TODO
}

void Receiver ::primitiveArgsSync_handler(FwIndexType portNum,
                                          U32 u32,
                                          U32& u32Ref,
                                          F32 f32,
                                          F32& f32Ref,
                                          bool b,
                                          bool& bRef) {
    // TODO
}

U32 Receiver ::primitiveReturnGuarded_handler(FwIndexType portNum,
                                              U32 u32,
                                              U32& u32Ref,
                                              F32 f32,
                                              F32& f32Ref,
                                              bool b,
                                              bool& bRef) {
    // TODO return
}

U32 Receiver ::primitiveReturnSync_handler(FwIndexType portNum,
                                           U32 u32,
                                           U32& u32Ref,
                                           F32 f32,
                                           F32& f32Ref,
                                           bool b,
                                           bool& bRef) {
    // TODO return
}

Fw::String Receiver ::stringAliasReturnGuarded_handler(FwIndexType portNum,
                                                       const Fw::StringBase& str,
                                                       Fw::StringBase& strRef) {
    // TODO return
}

Fw::String Receiver ::stringAliasReturnSync_handler(FwIndexType portNum,
                                                    const Fw::StringBase& str,
                                                    Fw::StringBase& strRef) {
    // TODO return
}

void Receiver ::stringArgsAsync_handler(FwIndexType portNum,
                                        const Fw::StringBase& str80,
                                        Fw::StringBase& str80Ref,
                                        const Fw::StringBase& str100,
                                        Fw::StringBase& str100Ref) {
    // TODO
}

void Receiver ::stringArgsGuarded_handler(FwIndexType portNum,
                                          const Fw::StringBase& str80,
                                          Fw::StringBase& str80Ref,
                                          const Fw::StringBase& str100,
                                          Fw::StringBase& str100Ref) {
    // TODO
}

void Receiver ::stringArgsSync_handler(FwIndexType portNum,
                                       const Fw::StringBase& str80,
                                       Fw::StringBase& str80Ref,
                                       const Fw::StringBase& str100,
                                       Fw::StringBase& str100Ref) {
    // TODO
}

Fw::String Receiver ::stringReturnGuarded_handler(FwIndexType portNum,
                                                  const Fw::StringBase& str,
                                                  Fw::StringBase& strRef) {
    // TODO return
}

Fw::String Receiver ::stringReturnSync_handler(FwIndexType portNum, const Fw::StringBase& str, Fw::StringBase& strRef) {
    // TODO return
}

void Receiver ::structArgsAsync_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    // TODO
}

void Receiver ::structArgsGuarded_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    // TODO
}

void Receiver ::structArgsSync_handler(FwIndexType portNum, const FormalParamStruct& s, FormalParamStruct& sRef) {
    // TODO
}

FormalParamStruct Receiver ::structReturnGuarded_handler(FwIndexType portNum,
                                                         const FormalParamStruct& s,
                                                         FormalParamStruct& sRef) {
    // TODO return
}

FormalParamStruct Receiver ::structReturnSync_handler(FwIndexType portNum,
                                                      const FormalParamStruct& s,
                                                      FormalParamStruct& sRef) {
    // TODO return
}

// ----------------------------------------------------------------------
// Overflow hook implementations for typed input ports
// ----------------------------------------------------------------------

void Receiver ::enumArgsHook_overflowHook(FwIndexType portNum,
                                          const FormalParamEnum& en,
                                          FormalParamEnum& enRef,
                                          const FormalAliasEnum& enA,
                                          FormalAliasEnum& enARef) {
    // TODO
}

}  // namespace FppTest
