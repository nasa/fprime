// ======================================================================
// \title  BufferManager/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for BufferManager component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
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
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->tlmSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all telemetry histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: BufferManager_NumAllocatedBuffers
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_NumAllocatedBuffers_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_BufferManager_NumAllocatedBuffers->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel BufferManager_NumAllocatedBuffers\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_BufferManager_NumAllocatedBuffers->size() << "\n";
  }

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_NumAllocatedBuffers(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_BufferManager_NumAllocatedBuffers->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel BufferManager_NumAllocatedBuffers\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_BufferManager_NumAllocatedBuffers->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_BufferManager_NumAllocatedBuffers& e =
      this->tlmHistory_BufferManager_NumAllocatedBuffers->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel BufferManager_NumAllocatedBuffers\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: BufferManager_AllocatedSize
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_AllocatedSize_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_BufferManager_AllocatedSize->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel BufferManager_AllocatedSize\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_BufferManager_AllocatedSize->size() << "\n";
  }

  void BufferManagerGTestBase ::
    assertTlm_BufferManager_AllocatedSize(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_BufferManager_AllocatedSize->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel BufferManager_AllocatedSize\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_BufferManager_AllocatedSize->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_BufferManager_AllocatedSize& e =
      this->tlmHistory_BufferManager_AllocatedSize->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel BufferManager_AllocatedSize\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ClearedErrorState
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_ClearedErrorState_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_ClearedErrorState)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ClearedErrorState\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_ClearedErrorState << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: StoreSizeExceeded
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_StoreSizeExceeded_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_StoreSizeExceeded)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event StoreSizeExceeded\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_StoreSizeExceeded << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: TooManyBuffers
  // ----------------------------------------------------------------------

  void BufferManagerGTestBase ::
    assertEvents_TooManyBuffers_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_TooManyBuffers)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event TooManyBuffers\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_TooManyBuffers << "\n";
  }

} // end namespace Svc
