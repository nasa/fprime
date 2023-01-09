// ======================================================================
// \title  Example.hpp
// \author T. Chieu
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

      //! Handler implementation for arrayArgsIn
      //!
      void arrayArgsIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortArray &a, /*!< 
      An array
      */
          PortArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler implementation for arrayReturnIn
      //!
      PortArray arrayReturnIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortArray &a, /*!< 
      An array
      */
          PortArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler implementation for enumArgsIn
      //!
      void enumArgsIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortEnum &e, /*!< 
      An enum
      */
          PortEnum &eRef /*!< 
      An enum ref
      */
      );

      //! Handler implementation for enumReturnIn
      //!
      PortEnum enumReturnIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortEnum &e, /*!< 
      An enum
      */
          PortEnum &eRef /*!< 
      An enum ref
      */
      );

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

      //! Handler implementation for stringArgsIn
      //!
      void stringArgsIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const str80String &str80, /*!< 
      A string of size 80
      */
          str80RefString &str80Ref, 
          const str100String &str100, /*!< 
      A string of size 100
      */
          str100RefString &str100Ref 
      );

      //! Handler implementation for structArgsIn
      //!
      void structArgsIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortStruct &s, /*!< 
      A struct
      */
          PortStruct &sRef /*!< 
      A struct ref
      */
      );

      //! Handler implementation for structReturnIn
      //!
      PortStruct structReturnIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortStruct &s, /*!< 
      A struct
      */
          PortStruct &sRef /*!< 
      A struct ref
      */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined serial input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for serialIn
      //!
      void serialIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

    public:

      //! Enables checking the serialization status of serial port invocations
      Fw::SerializeStatus serializeStatus;

    };


#endif
