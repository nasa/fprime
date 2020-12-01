# Sequencing In F´

F´ supports a very basic sequence format that allows for executing F´ commands at absolute times and at times relative
to previous commands. The sequence uses the F´ dictionary to translate the human readable sequence file into a minimized
binary format for upload and running using the command sequencer.

Compiling this sequence is done with the `fprime-seqgen` utility and can be run once uploaded by issuing the `*.CS_RUN`
command of any command sequencer instance defined in the flight system.

## Writing an F´ Sequence File

F´ sequence files consist of lists of commands. These commands start with a time argument, followed by the command
mnemonic, and lastly any arguments to command.  See the example below, which are all pulled from the example sequence
file found here: [simple_sequence.seq](https://github.com/nasa/fprime/blob/devel/Gds/examples/simple_sequence.seq).

```
A2015-075T22:32:40.123 cmdDisp.CMD_NO_OP
```

Here an absolute command was chosen.  Times can be specified in in absolute or relative time format.  An absolute time
starts with an `A` and specifies a calendar time. A relative command is specified starting with an `R`, which runs
relative to the previous command or start of the sequence. A relative command with an argument is shown below.

```
R01:00:01.050 CMD_NO_OP_STRING "Awesome string!" ; And a nice comment too
```

A list of these commands can be specified in a text file typically ending with the `.seq` extension.  Comments start
with a ;. The example file above goes into greater explaination of the sample commands.

## Compiling A Sample Sequence

The `fprime-seqgen` command can compile the sequence into a binary format that F´ flight software can execute. To do
this the user should provide a path to the flight software dictionary and a path to the text file discussed above.
Below is an example on how to run the sample example sequence with the Ref dictionary. Remember to build first or the
dictionary will not be generated.

```
fprime-seqgen fprime/Gds/examples/simple_sequence.seq -d fprime/Ref/build-artifacts/*/dict/RefTopologyAppDictionary.xml 
```

Here the output file is not specified, so it will be a new file in the same directory as the sequence but ending with
the `.bin` extension.

This binary file should be uploaded to the flight software.  Given limitations on file path length, it should be copied
to `/tmp` when running locally.

## Running The Binary Sequence

Once uploaded, the sequence can be run by executing the .CS_RUN command of any command sequencer. Here we will run the
`cmdSeq` instance's .CS_RUN command using an upload location of /tmp/sample_sequence.bin. This should work in the `Ref`
application.

```
cmdSeq.CS_RUN	"/tmp/sample_sequence.bin"
```

**Note:** the sample sequence will run for multiple hours due to the specification of the relative commands. 
