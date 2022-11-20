// ======================================================================
// \title  Example.hpp
// \author joshuaa
// \brief  hpp file for Example component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Example_HPP
#define Example_HPP

#include "FppTest/port/no_arg/ExampleComponentAc.hpp"

class Example :
  public ExampleComponentBase
{

  public:

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object Example
    //!
    Example(
        const char *const compName /*!< The component name*/
    );

    //! Initialize object Example
    //!
    void init(
        const NATIVE_INT_TYPE instance //!< The instance number
    );

    //! Destroy object Example
    //!
    ~Example();

  PRIVATE:

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for noArgPort
    //!
    void noArgsIn_handler(
        const NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Handler implementation for asyncNoArgPort
    //!
    void noArgsOut_handler(
        const NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Handler implementation for guardedNoArgPort
    //!
    U32 returnU32In_handler(
        const NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Handler implementation for exampleInput
    //!
    U32 returnU32Out_handler(
        const NATIVE_INT_TYPE portNum /*!< The port number*/
    );


  };

#endif
