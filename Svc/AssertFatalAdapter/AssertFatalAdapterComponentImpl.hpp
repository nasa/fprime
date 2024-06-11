// ======================================================================
// \title  AssertFatalAdapterImpl.hpp
// \author tcanham
// \brief  hpp file for AssertFatalAdapter component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef AssertFatalAdapter_HPP
#define AssertFatalAdapter_HPP

#include "Svc/AssertFatalAdapter/AssertFatalAdapterComponentAc.hpp"

namespace Svc {

  class AssertFatalAdapterComponentImpl :
    public AssertFatalAdapterComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object AssertFatalAdapter
      //!
      AssertFatalAdapterComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object AssertFatalAdapter
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object AssertFatalAdapter
      //!
      ~AssertFatalAdapterComponentImpl();

      //! Report the assert as a FATAL
      void reportAssert(
          FILE_NAME_ARG file,
          NATIVE_UINT_TYPE lineNo,
          NATIVE_UINT_TYPE numArgs,
          FwAssertArgType arg1,
          FwAssertArgType arg2,
          FwAssertArgType arg3,
          FwAssertArgType arg4,
          FwAssertArgType arg5,
          FwAssertArgType arg6
          );

    private:

      class AssertFatalAdapter : public Fw::AssertHook {
          public:
              AssertFatalAdapter();
              ~AssertFatalAdapter();
              void regAssertReporter(AssertFatalAdapterComponentImpl* compPtr);
          private:
              void reportAssert(
                  FILE_NAME_ARG file,
                  NATIVE_UINT_TYPE lineNo,
                  NATIVE_UINT_TYPE numArgs,
                  FwAssertArgType arg1,
                  FwAssertArgType arg2,
                  FwAssertArgType arg3,
                  FwAssertArgType arg4,
                  FwAssertArgType arg5,
                  FwAssertArgType arg6
                  );

              // Prevent actual assert since FATAL handler will deal with it
              void doAssert();

              AssertFatalAdapterComponentImpl* m_compPtr;
      };

      AssertFatalAdapter m_adapter;

    };

} // end namespace Svc

#endif
