@ Signal Generator Settings Changed
event SettingsChanged(
                                 Frequency: U32
                                 Amplitude: F32
                                 Phase: F32
                                 SignalType: Ref.SignalType
                               ) \
  severity activity low \
  id 0 \
  format "Set Frequency(Hz) {}, Amplitude {f}, Phase {f}, Signal Type {}"

event DpStarted(
                                 records: U32
                               ) \
  severity activity low \
  id 1 \
  format "Writing {} DP records"

event DpComplete(
                                 records: U32
                                 bytes: U32
                               ) \
  severity activity low \
  id 2 \
  format "Writing {} DP records {} bytes total"

event DpRecordFull(
                                 records: U32
                                 bytes: U32
                               ) \
  severity warning low \
  id 3 \
  format "DP container full with {} records and {} bytes. Closing DP."

event DpsNotConnected \
  severity warning high \
  id 4 \
  format "DP Ports not connected!"

event DpMemoryFail \
  severity warning high \
  id 5 \
  format "Failed to acquire a DP buffer"

event InSufficientDpRecords \
  severity warning high \
  id 6 \
  format "Need to request at least one record"

event DpMemRequested(
                                 $size: U32
                               ) \
  severity activity low \
  id 7 \
  format "Requesting {} bytes for DP"

event DpMemReceived(
                                 $size: U32
                               ) \
  severity activity low \
  id 8 \
  format "Received {} bytes for DP"
