@ Signal Generator Settings
async command SignalGen_Settings(
                                  Frequency: U32
                                  Amplitude: F32
                                  Phase: F32
                                  SigType: Ref.SignalType
                                ) \
  opcode 0x00

@ Toggle Signal Generator On/Off.
async command SignalGen_Toggle \
  opcode 1

@ Skip next sample
async command SignalGen_Skip \
  opcode 2

@ Signal Generator Settings
async command SignalGen_Dp(
                                  records: U32
                                ) \
  opcode 3

