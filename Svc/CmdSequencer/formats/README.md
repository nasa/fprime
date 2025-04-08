# Svc::CmdSequencer Formats

This directory contains alternate format implementations for the command
sequencer.
To use one of these formats, instantiate the class into an object
and pass the object into the `setSequenceFormat` function of
the `CmdSequencer` object.

Note that if you call `allocateBuffer` on the `CmdSequencer` object,
you need to do that *after* setting the sequence format.
Otherwise the buffer will get allocated for the default format.
