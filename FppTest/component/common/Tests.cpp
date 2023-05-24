#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Telemetry tests
// ----------------------------------------------------------------------

TLM_TEST_DEFS

void Tester ::
  checkSerializeStatusSuccess()
{
  ASSERT_EQ(
    component.serializeStatus, 
    Fw::FW_SERIALIZE_OK
  );
}

void Tester ::
  checkSerializeStatusBufferEmpty()
{
  ASSERT_EQ(
    component.serializeStatus, 
    Fw::FW_DESERIALIZE_BUFFER_EMPTY
  );
}
