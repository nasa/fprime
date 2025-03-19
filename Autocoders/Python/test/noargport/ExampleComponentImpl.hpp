// ======================================================================
// \title  ExampleComponentImpl.hpp
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

#include "Autocoders/Python/test/noargport/ExampleComponentAc.hpp"

namespace ExampleComponents {

  class ExampleComponentImpl :
    public ExampleComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Example
      //!
      ExampleComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object Example
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance //!< The instance number
      );

      //! Destroy object Example
      //!
      ~ExampleComponentImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for noArgPort
      //!
      void noArgPort_handler(
          const FwIndexType portNum /*!< The port number*/
      );

      //! Handler implementation for asyncNoArgPort
      //!
      void asyncNoArgPort_handler(
          const FwIndexType portNum /*!< The port number*/
      );

      //! Handler implementation for guardedNoArgPort
      //!
      void guardedNoArgPort_handler(
          const FwIndexType portNum /*!< The port number*/
      );

      //! Handler implementation for exampleInput
      //!
      U32 exampleInput_handler(
          const FwIndexType portNum /*!< The port number*/
      );


    };

} // end namespace ExampleComponents

#endif
