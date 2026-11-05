# ======================================================================
# AcConstants.fpp
# F Prime configuration constants
# ======================================================================

@ Number of rate group member output ports for ActiveRateGroup
constant ActiveRateGroupOutputPorts = 10

@ Number of rate group member output ports for PassiveRateGroup
constant PassiveRateGroupOutputPorts = 10

@ Used to drive rate groups
constant RateGroupDriverRateGroupPorts = 3

@ Used for command and registration ports
constant CmdDispatcherComponentCommandPorts = 30

@ Used for uplink/sequencer buffer/response ports
constant CmdDispatcherSequencePorts = 5

@ Used for dispatching sequences to command sequencers
constant SeqDispatcherSequencerPorts = 2

@ Used for sizing the command splitter input arrays
constant CmdSplitterPorts = CmdDispatcherSequencePorts

@ Number of static memory allocations
constant StaticMemoryAllocations = 4

@ Used to ping active components
constant HealthPingPorts = 25

@ Used for broadcasting completed file downlinks
constant FileDownCompletePorts = 1

@ Used for number of Fw::Com type ports supported by Svc::ComQueue
constant ComQueueComPorts = 2

@ Used for number of Fw::Buffer type ports supported by Svc::ComQueue
constant ComQueueBufferPorts = 1

@ Used for maximum number of connected buffer repeater consumers
constant BufferRepeaterOutputPorts = 10

@ Size of port array for DpManager
constant DpManagerNumPorts = 5

@ Size of data product routing port arrays for DpWriter
constant DpWriterNumPorts = 5

@ Size of processing port array for DpWriter
constant DpWriterNumProcPorts = 5

@ The size of a file name string
constant FileNameStringSize = 240

@ The size of an assert text string
constant FwAssertTextSize = 256

@ The size of a file name in an AssertFatalAdapter event (leading-truncation:
@ the tail of the path is kept)
@ Note: Svc::AssertFatalAdapter truncates to the smaller of this constant and
@ FW_LOG_STRING_MAX_SIZE (set in FpConstants.fpp), so raising this alone has no
@ effect once it exceeds that bound. With the defaults (240 here, 200 there)
@ FW_LOG_STRING_MAX_SIZE is what governs.
@ FwAssertTextSize (in this file) is a separate bound on the console assert
@ text, which also carries the timestamp and assert arguments.
constant AssertFatalAdapterEventFileSize = FileNameStringSize

@ The maximum size in bytes of the argument blob carried in a Svc::SeqArgs buffer
@ (CmdSeqIn / RUN / INVOKE).
@ Deliberately kept small (rather than derived from FW_CMD_ARG_BUFFER_MAX_SIZE) to
@ keep SeqArgs easy to populate from fprime-gds. It must still fit within
@ FW_CMD_ARG_BUFFER_MAX_SIZE alongside the other command arguments it travels with
@ (filename length prefix + fileName + BlockState + the SeqArgs size field).
constant SequenceArgumentsMaxSize = 12
