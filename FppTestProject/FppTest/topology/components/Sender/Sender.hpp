// ======================================================================
// \title  Sender.hpp
// \author tumbar
// \brief  hpp file for Sender component implementation class
// ======================================================================

#ifndef FppTest_Sender_HPP
#define FppTest_Sender_HPP

#include "FppTest/topology/components/Sender/SenderComponentAc.hpp"

namespace FppTest {

class Sender final : public SenderComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Sender object
    Sender(const char* const compName  //!< The component name
    );

    //! Destroy Sender object
    ~Sender();

    // //! Invoke output port arrayArgsOut
    //   void arrayArgsOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const FormalParamArray& a, //!< An array
    //       FormalParamArray& aRef, //!< An array ref
    //       const FormalAliasArray& b, //!< alias of an array
    //       FormalAliasArray& bRef, //!< alias of array ref
    //       const FormalAliasStringArray& c, //!< array of string aliases
    //       FormalAliasStringArray& cRef //!< array of string aliases ref
    //   );
    //
    //   //! Invoke output port arrayReturnOut
    //   FormalParamArray arrayReturnOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const FormalParamArray& a, //!< An array
    //       FormalParamArray& aRef //!< An array ref
    //   );
    //
    //   //! Invoke output port arrayStringAliasReturnOut
    //   FormalAliasStringArray arrayStringAliasReturnOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const FormalParamArray& a, //!< An array
    //       FormalParamArray& aRef //!< An array ref
    //   );
    //
    //   //! Invoke output port enumArgsOut
    //   void enumArgsOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const FormalParamEnum& en, //!< An enum
    //       FormalParamEnum& enRef, //!< An enum ref
    //       const FormalAliasEnum& enA, //!< An enum alias
    //       FormalAliasEnum& enARef //!< An enum alias ref
    //   );
    //
    //   //! Invoke output port enumReturnOut
    //   FormalParamEnum enumReturnOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const FormalParamEnum& en, //!< An enum
    //       FormalParamEnum& enRef //!< An enum ref
    //   );
    //
    //   //! Invoke output port noArgsOut
    //   void noArgsOut_out(
    //       FwIndexType portNum //!< The port number
    //   );
    //
    //   //! Invoke output port noArgsReturnOut
    //   bool noArgsReturnOut_out(
    //       FwIndexType portNum //!< The port number
    //   );
    //
    //   //! Invoke output port primitiveArgsOut
    //   void primitiveArgsOut_out(
    //       FwIndexType portNum, //!< The port number
    //       U32 u32,
    //       U32& u32Ref,
    //       F32 f32,
    //       F32& f32Ref,
    //       bool b,
    //       bool& bRef
    //   );
    //
    //   //! Invoke output port primitiveReturnOut
    //   U32 primitiveReturnOut_out(
    //       FwIndexType portNum, //!< The port number
    //       U32 u32,
    //       U32& u32Ref,
    //       F32 f32,
    //       F32& f32Ref,
    //       bool b,
    //       bool& bRef
    //   );
    //
    //   //! Invoke output port prmGetIn
    //   Fw::ParamValid prmGetIn_out(
    //       FwIndexType portNum, //!< The port number
    //       FwPrmIdType id, //!< Parameter ID
    //       Fw::ParamBuffer& val //!< Buffer containing serialized parameter value.
    //                            //!< Unmodified if param not found.
    //   );
    //
    //   //! Invoke output port prmSetIn
    //   void prmSetIn_out(
    //       FwIndexType portNum, //!< The port number
    //       FwPrmIdType id, //!< Parameter ID
    //       Fw::ParamBuffer& val //!< Buffer containing serialized parameter value
    //   );
    //
    //   //! Invoke output port stringAliasReturnOut
    //   Fw::String stringAliasReturnOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const Fw::StringBase& str, //!< A string
    //       Fw::StringBase& strRef //!< A string ref
    //   );
    //
    //   //! Invoke output port stringArgsOut
    //   void stringArgsOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const Fw::StringBase& str80, //!< A string of size 80
    //       Fw::StringBase& str80Ref,
    //       const Fw::StringBase& str100, //!< A string of size 100
    //       Fw::StringBase& str100Ref
    //   );
    //
    //   //! Invoke output port stringReturnOut
    //   Fw::String stringReturnOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const Fw::StringBase& str, //!< A string
    //       Fw::StringBase& strRef //!< A string ref
    //   );
    //
    //   //! Invoke output port structArgsOut
    //   void structArgsOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const FormalParamStruct& s, //!< A struct
    //       FormalParamStruct& sRef //!< A struct ref
    //   );
    //
    //   //! Invoke output port structReturnOut
    //   FormalParamStruct structReturnOut_out(
    //       FwIndexType portNum, //!< The port number
    //       const FormalParamStruct& s, //!< A struct
    //       FormalParamStruct& sRef //!< A struct ref
    //   );

};

}  // namespace FppTest

#endif
