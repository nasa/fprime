# DpWriter Component Tests

## 1. Abstract State

### 1.1. Variables

| Variable | Type | Description | Initial Value |
|----------|------|-------------|---------------|
| `m_NumBuffersReceived` | `OnChangeChannel<U32>` | The number of buffers received | 0 |
| `m_NumBytesWritten` | `OnChangeChannel<U64>` | The number of bytes written | 0 |
| `m_NumErrors` | `OnChangeChannel<U32>` | The number of errors | 0 |
| `m_NumFailedWrites` | `OnChangeChannel<U32>` | The number of failed writes | 0 |
| `m_NumSuccessfulWrites` | `OnChangeChannel<U32>` | The number of successful writes | 0 |
| `m_bufferTooSmallForDataEventCount` | `FwSizeType` | The number of `BufferTooSmallForData` events since the last throttle clear |0 |
| `m_bufferTooSmallForPacketEventCount` | `FwSizeType` | The number of `BufferTooSmallForPacket` events since the last throttle clear |0 |
| `m_fileOpenErrorEventCount` | `FwSizeType` | The number of file open error events since the last throttle clear |0 |
| `m_fileWriteErrorEventCount` | `FwSizeType` | The number of file write error events since the last throttle clear |0 |
| `m_invalidBufferEventCount` | `FwSizeType` | The number of buffer invalid events since the last throttle clear |0 |
| `m_invalidHeaderEventCount` | `FwSizeType` | The number of invalid packet descriptor events since the last throttle clear |0 |
| `m_invalidHeaderHashEventCount` | `FwSizeType` | The number of invalid header hash events since the last throttle clear |0 |

## 2. Rule Groups

### 2.1. FileOpenStatus

This rule group manages the file open status in the test harness.

#### 2.1.1. OK

This rule sets the file open status to `Os::File::OP_OK`, simulating a system state
in which a file open call succeeds.

**Precondition:**
`Os::Stub::File::Test::StaticData::data.openStatus != Os::File::OP_OK`.

**Action:**
`Os::Stub::File::Test::StaticData::data.openStatus = Os::File::OP_OK`.

**Test:**

1. Apply rule `FileOpenStatus::Error`.
1. Apply rule `FileOpenStatus::OK`.

**Requirements tested:**
None (helper rule).

#### 2.1.2. Error

This rule sets the file open status to an error value, simulating a system state
in which a file open call fails.

**Precondition:**
`Os::Stub::File::Test::StaticData::data.openStatus == Os::File::OP_OK`.

**Action:**
Set `Os::Stub::File::Test::StaticData::data.openStatus` to a random
value other than `Os::File::OP_OK`.

**Test:**

1. Apply rule `FileOpenStatus::Error`.

**Requirements tested:**
None (helper rule).

### 2.2. FileWriteStatus

This rule group manages the file write status in the test harness.

#### 2.2.1. OK

This rule sets the file open status to `Os::File::OP_OK`, simulating a system state
in which a file write call succeeds.

**Precondition:**
`Os::Stub::File::Test::StaticData::data.writeStatus != Os::File::OP_OK`.

**Action:**
`Os::Stub::File::Test::StaticData::data.writeStatus = Os::File::OP_OK`.

**Test:**

1. Apply rule `FileWriteStatus::Error`.
1. Apply rule `FileWriteStatus::OK`.

**Requirements tested:**
None (helper rule).

#### 2.2.2. Error

This rule sets the file write status to an error value, simulating a system state
in which a file write call fails.

**Precondition:**
`Os::Stub::File::Test::StaticData::data.writeStatus == Os::File::OP_OK`.

**Action:**
Set `Os::Stub::File::Test::StaticData::data.writeStatus` to a random value
other than `Os::File::OP_OK`.

**Test:**

1. Apply rule `FileWriteStatus::Error`.

**Requirements tested:**
None (helper rule).

### 2.3. SchedIn

This rule group sends test input to the `schedIn` port.

#### 2.3.1. OK

This rule invokes `schedIn` with nominal input.

**Precondition:** `true`

**Action:**

1. Clear history.
1. Invoke `schedIn` with a random context.
1. Check telemetry.

**Test:**

1. Apply rule `SchedIn::OK`.

**Requirements tested:**
`SVC-DPWRITER-006`.

### 2.4. BufferSendIn

This rule group sends test input to the `bufferSendIn` port.

#### 2.4.1. OK

This rule invokes `bufferSendIn` with nominal input.

**Precondition:**
`fileOpenStatus == Os::File::OP_OK` and
`fileWriteStatus == Os::File::OP_OK`.

**Action:**
1. Clear history.
1. Update `m_NumBuffersReceived`.
1. Construct a random buffer _B_ with valid packet data and random processing bits.
1. Send _B_ to `bufferSendIn`.
1. Assert that the event history contains one element.
1. Assert that the event history for `FileWritten` contains one element.
1. Check the event arguments.
1. Check output on processing ports.
1. Check output on notification port.
1. Check output on deallocation port.
1. Verify that `Os::File::write` has been called with the expected arguments.
1. Update `m_NumBytesWritten`.
1. Update `m_NumSuccessfulWrites`.

**Test:**
1. Apply rule `BufferSendIn::OK`.

**Requirements tested:**
`SVC-DPWRITER-001`,
`SVC-DPWRITER-002`,
`SVC-DPWRITER-003`,
`SVC-DPWRITER-004`,
`SVC-DPWRITER-005`

#### 2.4.2. InvalidBuffer

This rule invokes `bufferSendIn` with an invalid buffer.

**Precondition:**
`true`

**Action:**
1. Clear history.
1. Update `m_NumBuffersReceived`.
1. Construct an invalid buffer _B_.
1. If `m_invalidBufferEventCount` < `DpWriterComponentBase::EVENTID_INVALIDBUFFER_THROTTLE`,
   then
   1. Assert that the event history contains one element.
   1. Assert that the event history for `InvalidBuffer` contains one element.
   1. Increment `m_invalidBufferEventCount`.
1. Otherwise assert that the event history is empty.
1. Verify no data product file.
1. Verify no port output.
1. Increment `m_NumErrors`.

**Test:**
1. Apply rule `BufferSendIn::InvalidBuffer`.

**Requirements tested:**
`SVC-DPWRITER-001`

#### 2.4.3. BufferTooSmallForPacket

This rule invokes `bufferSendIn` with a buffer that is too small to
hold a data product packet.

**Precondition:**
`true`

**Action:**
1. Clear history.
1. Increment `m_NumBuffersReceived`.
1. Construct a valid buffer _B_ that is too small to hold a data product packet.
1. If `m_bufferTooSmallEventCount` < `DpWriterComponentBase::EVENTID_BUFFERTOOSMALLFORPACKET_THROTTLE`,
   then
   1. Assert that the event history contains one element.
   1. Assert that the event history for `BufferTooSmallForPacket` contains one element.
   1. Check the event arguments.
   1. Increment `m_bufferTooSmallEventCount`.
1. Otherwise assert that the event history is empty.
1. Assert no DP written notification.
1. Assert buffer sent for deallocation.
1. Verify no data product file.
1. Increment `m_NumErrors`.

**Requirements tested:**
`SVC-DPWRITER-001`

#### 2.4.4. InvalidHeaderHash

This rule invokes `bufferSendIn` with a buffer that has an invalid
header hash.

**Precondition:**
`true`

**Action:**
1. Clear history.
1. Increment `m_NumBuffersReceived`.
1. Construct a valid buffer _B_ that is large enough to hold a data product
   packet and that has an invalid header hash.
1. If `m_invalidHeaderHashEventCount` < `DpWriterComponentBase::EVENTID_INVALIDHEADERHASH_THROTTLE`,
   then
   1. Assert that the event history contains one element.
   1. Assert that the event history for `InvalidHeaderHash` contains one element.
   1. Check the event arguments.
   1. Increment `m_invalidHeaderHashEventCount`.
1. Otherwise assert that the event history is empty.
1. Assert no DP written notification.
1. Assert buffer sent for deallocation.
1. Verify no data product file.
1. Increment `m_NumErrors`.

**Test:**
1. Apply rule `BufferSendIn::BufferTooSmallForPacket`.

**Requirements tested:**
`SVC-DPWRITER-001`

#### 2.4.5. InvalidHeader

This rule invokes `bufferSendIn` with an invalid packet header.

**Precondition:**
`true`

**Action:**
1. Clear history.
1. Increment `m_NumBuffersReceived`.
1. Construct a valid buffer _B_ with an invalid packet header.
1. If `m_invalidPacketHeaderEventCount` < `DpWriterComponentBase::EVENTID_INVALIDHEADER_THROTTLE`,
   then
   1. Assert that the event history contains one element.
   1. Assert that the event history for `InvalidHeader` contains one element.
   1. Check the event arguments.
   1. Increment `m_invalidPacketHeaderEventCount`.
1. Otherwise assert that the event history is empty.
1. Assert no DP written notification.
1. Assert buffer sent for deallocation.
1. Verify no data product file.
1. Increment `m_NumErrors`.

**Test:**
1. Apply rule `BufferSendIn::InvalidHeader`.

**Requirements tested:**
`SVC-DPWRITER-001`

#### 2.4.6. BufferTooSmallForData

This rule invokes `bufferSendIn` with a buffer that is too small to
hold the data size specified in the header.

**Precondition:**
`true`

**Action:**
1. Clear history.
1. Increment `m_NumBuffersReceived`.
1. Construct a valid buffer _B_ with a valid packet header, but
   a data size that will not fit in _B_.
1. If `m_bufferTooSmallForDataEventCount` < `DpWriterComponentBase::EVENTID_BUFFERTOOSMALLFORDATA_THROTTLE`,
   then
   1. Assert that the event history contains one element.
   1. Assert that the event history for `BufferTooSmallForData` contains one element.
   1. Check the event arguments.
   1. Increment `m_bufferTooSmallForDataEventCount`.
1. Otherwise assert that the event history is empty.
1. Assert no DP written notification.
1. Assert buffer sent for deallocation.
1. Verify no data product file.
1. Increment `m_NumErrors`.

**Test:**
1. Apply rule `BufferSendIn::BufferTooSmallForData`.

**Requirements tested:**
`SVC-DPWRITER-001`

#### 2.4.7. FileOpenError

This rule invokes `bufferSendIn` with a file open error.

**Precondition:**
`fileOpenStatus != Os::File::OP_OK`

**Action:**
1. Clear history.
1. Update `m_NumBuffersReceived`.
1. Construct a random buffer _B_ with valid packet data.
1. Send _B_ to `bufferSendIn`.
1. Assert that the event history contains one element.
1. Assert that the event history for `FileOpenError` contains one element.
1. Check the event arguments.
1. Assert no DP written notification.
1. Assert buffer sent for deallocation.
1. Verify no data product file.
1. Increment `m_NumFailedWrites`.
1. Increment `m_NumErrors`.

**Test:**
1. Apply rule `FileOpenStatus::Error`.
1. Apply rule `BufferSendIn::FileOpenError`.

**Requirements tested:**
`SVC-DPWRITER-004`

#### 2.4.8. FileWriteError

This rule invokes `bufferSendIn` with a file write error.

**Precondition:**
`fileOpenStatus == Os::File::OP_OK` and
`fileWriteStatus != Os::File::OP_OK`

**Action:**
1. Clear history.
1. Update `m_NumBuffersReceived`.
1. Construct a random buffer _B_ with valid packet data.
1. Send _B_ to `bufferSendIn`.
1. Assert that the event history contains one element.
1. Assert that the event history for `FileWriteError` contains one element.
1. Check the event arguments.
1. Assert no DP written notification.
1. Assert buffer sent for deallocation.
1. Verify no data product file.
1. Increment `m_NumFailedWrites`.
1. Increment `m_NumErrors`.

**Test:**
1. Apply rule `FileWriteStatus::Error`.
1. Apply rule `BufferSendIn::FileWriteError`.

**Requirements tested:**
`SVC-DPWRITER-004`

### 2.5. CLEAR_EVENT_THROTTLE

This rule group tests the `CLEAR_EVENT_THROTTLE` command.

#### 2.5.1. OK

This rule sends the `CLEAR_EVENT_THROTTLE` command.

**Precondition:** `true`

**Action:**

1. Clear the history.
1. Send command `CLEAR_EVENT_THROTTLE`.
1. Check the command response.
1. Assert `DpWriterComponentBase::m_InvalidBufferThrottle` == 0.
1. Set `m_bufferTooSmallForDataEventCount` = 0.
1. Set `m_bufferTooSmallForPacketEventCount` = 0.
1. Set `m_fileOpenErrorEventCount` = 0.
1. Set `m_fileWriteErrorEventCount` = 0.
1. Set `m_invalidBufferEventCount` = 0.
1. Set `m_invalidHeaderEventCount` = 0.
1. Set `m_invalidHeaderHashEventCount` = 0.

**Test:**

1. Apply rule `BufferSendIn::InvalidBuffer` `DpWriterComponentBase::EVENTID_INVALIDBUFFER_THROTTLE` + 1 times.
1. Apply rule `CLEAR_EVENT_THROTTLE::OK`.
1. Apply rule `BufferSendIn::InvalidBuffer`

## 3. Implementation

See [the DpWriter test README](../../../DpWriter/test/ut/README.md)
for a description of the pattern used to implement the tests.
