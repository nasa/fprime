@ Signal Generator Settings Changed
event SignalGen_SettingsChanged(
                                 Frequency: U32
                                 Amplitude: F32
                                 Phase: F32
                                 SignalType: Ref.SignalType
                               ) \
  severity activity low \
  id 0 \
  format "Set Frequency(Hz) {}, Amplitude {f}, Phase {f}, Signal Type {}"

event SignalGen_DpStarted(
                                 records: U32
                               ) \
  severity activity low \
  id 1 \
  format "Writing {} DP records"

event SignalGen_DpComplete(
                                 records: U32
                                 bytes: U32
                               ) \
  severity activity low \
  id 2 \
  format "Writing {} DP records {} bytes total"

event SignalGen_DpRecordFull(
                                 records: U32
                                 bytes: U32
                               ) \
  severity warning low \
  id 3 \
  format "DP container full with {} records and {} bytes. Closing DP."

event SignalGen_DpsNotConnected \
  severity warning high \
  id 4 \
  format "DP Ports not connected!"
