// ======================================================================
// \title  BufferManager/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for BufferManager component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BufferManager_GTEST_BASE_HPP
#define BufferManager_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for telemetry history assertions
// ----------------------------------------------------------------------

#define ASSERT_TLM_SIZE(size) \
  this->assertTlm_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_BufferManager_NumAllocatedBuffers_SIZE(size) \
  this->assertTlm_BufferManager_NumAllocatedBuffers_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_BufferManager_NumAllocatedBuffers(index, value) \
  this->assertTlm_BufferManager_NumAllocatedBuffers(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_BufferManager_AllocatedSize_SIZE(size) \
  this->assertTlm_BufferManager_AllocatedSize_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_BufferManager_AllocatedSize(index, value) \
  this->assertTlm_BufferManager_AllocatedSize(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ClearedErrorState_SIZE(size) \
  this->assertEvents_ClearedErrorState_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_StoreSizeExceeded_SIZE(size) \
  this->assertEvents_StoreSizeExceeded_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_TooManyBuffers_SIZE(size) \
  this->assertEvents_TooManyBuffers_size(__FILE__, __LINE__, size)

namespace Svc {

  //! \class BufferManagerGTestBase
  //! \brief Auto-generated base class for BufferManager component Google Test harness
  //!
  class BufferManagerGTestBase :
    public BufferManagerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object BufferManagerGTestBase
      //!
      BufferManagerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object BufferManagerGTestBase
      //!
      virtual ~BufferManagerGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Telemetry
      // ----------------------------------------------------------------------

      //! Assert size of telemetry history
      //!
      void assertTlm_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: BufferManager_NumAllocatedBuffers
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_BufferManager_NumAllocatedBuffers_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_BufferManager_NumAllocatedBuffers(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: BufferManager_AllocatedSize
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_BufferManager_AllocatedSize_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_BufferManager_AllocatedSize(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ClearedErrorState
      // ----------------------------------------------------------------------

      void assertEvents_ClearedErrorState_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: StoreSizeExceeded
      // ----------------------------------------------------------------------

      void assertEvents_StoreSizeExceeded_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: TooManyBuffers
      // ----------------------------------------------------------------------

      void assertEvents_TooManyBuffers_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Svc

#endif
