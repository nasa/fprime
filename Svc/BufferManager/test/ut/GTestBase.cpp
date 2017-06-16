// ======================================================================
// \title  BufferManager/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for BufferManager component Google Test harness base class
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

#include "GTestBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  BufferManagerGTestBase ::
    BufferManagerGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        BufferManagerTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  BufferManagerGTestBase ::
    ~BufferManagerGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertTlm_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->tlmSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all telemetry histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: BufferManager_NumAllocatedBuffers
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_NumAllocatedBuffers_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_BufferManager_NumAllocatedBuffers->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel BufferManager_NumAllocatedBuffers\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_BufferManager_NumAllocatedBuffers->size() << "\n";
  }

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_NumAllocatedBuffers(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_BufferManager_NumAllocatedBuffers->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel BufferManager_NumAllocatedBuffers\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_BufferManager_NumAllocatedBuffers->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_BufferManager_NumAllocatedBuffers& e =
      this->tlmHistory_BufferManager_NumAllocatedBuffers->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel BufferManager_NumAllocatedBuffers\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: BufferManager_AllocatedSize
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_AllocatedSize_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_BufferManager_AllocatedSize->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel BufferManager_AllocatedSize\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_BufferManager_AllocatedSize->size() << "\n";
  }

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_AllocatedSize(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_BufferManager_AllocatedSize->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel BufferManager_AllocatedSize\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_BufferManager_AllocatedSize->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_BufferManager_AllocatedSize& e =
      this->tlmHistory_BufferManager_AllocatedSize->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel BufferManager_AllocatedSize\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: BufferManager_AllocationQueueEmpty
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_BufferManager_AllocationQueueEmpty_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_BufferManager_AllocationQueueEmpty)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event BufferManager_AllocationQueueEmpty\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_BufferManager_AllocationQueueEmpty << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: BufferManager_AllocationQueueFull
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_BufferManager_AllocationQueueFull_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_BufferManager_AllocationQueueFull)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event BufferManager_AllocationQueueFull\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_BufferManager_AllocationQueueFull << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: BufferManager_IDMismatch
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_BufferManager_IDMismatch_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_BufferManager_IDMismatch->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event BufferManager_IDMismatch\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_BufferManager_IDMismatch->size() << "\n";
  }

  void BufferManagerGTestBase ::
    assertEvents_BufferManager_IDMismatch(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 expected,
        const U32 saw
    ) const
  {
    ASSERT_GT(this->eventHistory_BufferManager_IDMismatch->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event BufferManager_IDMismatch\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_BufferManager_IDMismatch->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_BufferManager_IDMismatch& e =
      this->eventHistory_BufferManager_IDMismatch->at(index);
    ASSERT_EQ(expected, e.expected)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument expected at index "
      << index
      << " in history of event BufferManager_IDMismatch\n"
      << "  Expected: " << expected << "\n"
      << "  Actual:   " << e.expected << "\n";
    ASSERT_EQ(saw, e.saw)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument saw at index "
      << index
      << " in history of event BufferManager_IDMismatch\n"
      << "  Expected: " << saw << "\n"
      << "  Actual:   " << e.saw << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: BufferManager_StoreSizeExceeded
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_BufferManager_StoreSizeExceeded_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_BufferManager_StoreSizeExceeded)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event BufferManager_StoreSizeExceeded\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_BufferManager_StoreSizeExceeded << "\n";
  }

} // end namespace Svc
