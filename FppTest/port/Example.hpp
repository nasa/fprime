// ======================================================================
// \title  Example.hpp
// \author tiffany
// \brief  hpp file for Example component implementation class
// ======================================================================

#ifndef Example_HPP
#define Example_HPP

#include "FppTest/port/ExampleComponentAc.hpp"


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
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Example
      //!
      ~Example();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for noArgsIn
      //!
      void noArgsIn_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler implementation for noArgsReturnIn
      //!
      bool noArgsReturnIn_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler implementation for primitiveArgsIn
      //!
      void primitiveArgsIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler implementation for primitiveReturnIn
      //!
      U32 primitiveReturnIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );


    };


#endif
