// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

//! Handler for from_arrayArgsOut
//!
void from_arrayArgsOut_handler(FwIndexType portNum,              //!< The port number
                               const FormalParamArray& a,        //!< An array
                               FormalParamArray& aRef,           //!< An array ref
                               const FormalAliasArray& b,        //!< alias of an array
                               FormalAliasArray& bRef,           //!< alias of array ref
                               const FormalAliasStringArray& c,  //!< array of string aliases
                               FormalAliasStringArray& cRef      //!< array of string aliases ref
                               ) final;

//! Handler for from_arrayReturnOut
//!
FormalParamArray from_arrayReturnOut_handler(const FwIndexType portNum,  //!< The port number
                                             const FormalParamArray& a,  //!< An array
                                             FormalParamArray& aRef      //!< An array ref
                                             ) final;

//! Handler for from_enumArgsOut
//!
void from_enumArgsOut_handler(const FwIndexType portNum,   //!< The port number
                              const FormalParamEnum& en,   //!< An enum
                              FormalParamEnum& enRef,      //!< An enum ref
                              const FormalAliasEnum& enA,  //!< An enum alias
                              FormalAliasEnum& enARef      //!< An enum alias ref
                              ) final;

//! Handler for from_enumReturnOut
//!
FormalParamEnum from_enumReturnOut_handler(const FwIndexType portNum,  //!< The port number
                                           const FormalParamEnum& en,  //!< An enum
                                           FormalParamEnum& enRef      //!< An enum ref
                                           ) final;

//! Handler base-class function for from_arrayStringAliasReturnOut
FormalAliasStringArray from_arrayStringAliasReturnOut_handler(FwIndexType portNum,        //!< The port number
                                                              const FormalParamArray& a,  //!< An array
                                                              FormalParamArray& aRef      //!< An array ref
                                                              ) final;

//! Handler base-class function for from_arrayStringAliasReturnOut
FormalAliasString from_stringAliasReturnOut_handler(FwIndexType portNum,        //!< The port number
                                                    const Fw::StringBase& str,  //!< A string
                                                    Fw::StringBase& strRef      //!< A string ref
                                                    ) final;

//! Handler for from_stringArgsOut
//!
void from_stringArgsOut_handler(const FwIndexType portNum,
                                const Fw::StringBase& str80,
                                Fw::StringBase& str80Ref,
                                const Fw::StringBase& str100,
                                Fw::StringBase& str100Ref) final;

//! Handler for from_stringReturnOut
//!
Fw::String from_stringReturnOut_handler(const FwIndexType portNum,
                                        const Fw::StringBase& str,
                                        Fw::StringBase& strRef) final;

//! Handler for from_noArgsOut
//!
void from_noArgsOut_handler(const FwIndexType portNum  //!< The port number
                            ) final;

//! Handler for from_noArgsReturnOut
//!
bool from_noArgsReturnOut_handler(const FwIndexType portNum  //!< The port number
                                  ) final;

//! Handler for from_primitiveArgsOut
//!
void from_primitiveArgsOut_handler(const FwIndexType portNum,  //!< The port number
                                   U32 u32,
                                   U32& u32Ref,
                                   F32 f32,
                                   F32& f32Ref,
                                   bool b,
                                   bool& bRef) final;

//! Handler for from_primitiveReturnOut
//!
U32 from_primitiveReturnOut_handler(const FwIndexType portNum,  //!< The port number
                                    U32 u32,
                                    U32& u32Ref,
                                    F32 f32,
                                    F32& f32Ref,
                                    bool b,
                                    bool& bRef) final;

//! Handler for from_prmGetIn
//!
Fw::ParamValid from_prmGetIn_handler(const FwIndexType portNum,  //!< The port number
                                     FwPrmIdType id,             //!< Parameter ID
                                     Fw::ParamBuffer& val        //!< Buffer containing serialized parameter value
                                     ) final;

//! Handler for from_prmGetIn
//!
void from_prmSetIn_handler(const FwIndexType portNum,  //!< The port number
                           FwPrmIdType id,             //!< Parameter ID
                           Fw::ParamBuffer& val        //!< Buffer containing serialized parameter value
                           ) final;

//! Handler for from_structArgsOut
//!
void from_structArgsOut_handler(const FwIndexType portNum,   //!< The port number
                                const FormalParamStruct& s,  //!< A struct
                                FormalParamStruct& sRef      //!< A struct ref
                                ) final;

//! Handler for from_structReturnOut
//!
FormalParamStruct from_structReturnOut_handler(const FwIndexType portNum,   //!< The port number
                                               const FormalParamStruct& s,  //!< A struct
                                               FormalParamStruct& sRef      //!< A struct ref
                                               ) final;

void from_enumArgsHookOverflowed_handler(const FwIndexType portNum,
                                         const FormalParamEnum& en,
                                         FormalParamEnum& enRef,
                                         const FormalAliasEnum& enA,
                                         FormalAliasEnum& enARef);
