// ======================================================================
// \title  BufferManager/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for BufferManager component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
//
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
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

#define ASSERT_EVENTS_BufferManager_AllocationQueueEmpty_SIZE(size) \
  this->assertEvents_BufferManager_AllocationQueueEmpty_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BufferManager_AllocationQueueFull_SIZE(size) \
  this->assertEvents_BufferManager_AllocationQueueFull_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BufferManager_IDMismatch_SIZE(size) \
  this->assertEvents_BufferManager_IDMismatch_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BufferManager_IDMismatch(index, _expected, _saw) \
  this->assertEvents_BufferManager_IDMismatch(__FILE__, __LINE__, index, _expected, _saw)

#define ASSERT_EVENTS_BufferManager_StoreSizeExceeded_SIZE(size) \
  this->assertEvents_BufferManager_StoreSizeExceeded_size(__FILE__, __LINE__, size)

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
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: BufferManager_NumAllocatedBuffers
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_BufferManager_NumAllocatedBuffers_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_BufferManager_NumAllocatedBuffers(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: BufferManager_AllocatedSize
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_BufferManager_AllocatedSize_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_BufferManager_AllocatedSize(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BufferManager_AllocationQueueEmpty
      // ----------------------------------------------------------------------

      void assertEvents_BufferManager_AllocationQueueEmpty_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BufferManager_AllocationQueueFull
      // ----------------------------------------------------------------------

      void assertEvents_BufferManager_AllocationQueueFull_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BufferManager_IDMismatch
      // ----------------------------------------------------------------------

      void assertEvents_BufferManager_IDMismatch_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_BufferManager_IDMismatch(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 expected, /*!< The expected ID value*/
          const U32 saw /*!< The ID value seen*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BufferManager_StoreSizeExceeded
      // ----------------------------------------------------------------------

      void assertEvents_BufferManager_StoreSizeExceeded_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Svc

#endif
