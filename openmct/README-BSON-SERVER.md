# BSON Telemetry Server

Basic Open MCT server with a built in telemetry server.  Consumes a stream of
BSON data, publishes it to clients in real time, and also stores it in a
database for historical access later.

Currently supports two different history stores-- an embedded store using LevelDB (just works out of the box) or a Redis based store.

The dictionary server will automatically track new points as it sees them in
the telemetry stream and expose them for Open MCT.  You can also have the dictionary service persist the current known dictionary to a file by POSTing `/save`.  You can have it reload the dictionary from file by POSTing to `/load`.

All configuration options for the BSON telemetry server are contained in `index.js` with documentation.

Usage:

```
› npm install
› npm start                                               

LevelDB: History will be stored in telemdb
LevelDB: History will keep latest 2 day(s) of data.
LevelDB: History will be pruned every 1 hour(s).
BSONStreamInput: Listening on: 127.0.0.1:12345
Web server running at 127.0.0.1:8000
```

Connect to `http://127.0.0.1:8000` to view Open MCT.

For testing purposes, there is a replay script which allows you to replay
bson data from a file:

```
node scripts/replay.js test-bson-file.bson
```

For a complete list of replay options, run `node scripts/replay --help`.

# Point and Packet Dictionary
In order to provide Open MCT with the necessary information about available
points, point and packet dictionaries must be defined, and every point
must be mapped to a packet for it to be exposed in the client.  By default, the
server will automatically create and populate point and packet dictionaries
based on the information it gleans from the realtime stream.

Automatic packet generation can be customized.  Additionally, point to packet
mappings can be defined manually.

You can ask the server to persist it's current dictionaries to file via a POST
to the `/dictionary/save` endpoint.  The server will reload these files at
start up.  If you make changes to these files and want to reload them without
restarting the server, POST to `/dictionary/load`.

## Automatic point generation

The server will read type information from the bson messages stream and
automatically create point defintions for every unique mnemonic name.  These
can be manually edited by editing `points.json`.

## Automatic packet generation

There are three methods for automated packet generation.

**packetSeparator**
With no changes, the adapter will automatically detect packet names by splitting
point names using the `packetSeparator`.  It will only split on the first
`packetSeparator` found-- there is no concept of a nested packet. If the
`packetSeparator` is not found in the point name, then the adapter will add the
point to the "UNGROUPED POINTS" packet.

**packetPrefix**
You can specify a list of prefixes to group points by.  If more than one prefix
matches a point, the first match will be used for grouping.  If no matching
prefix is found, the server will fall back to the `packetSeparator` grouping
method above.

**manually editing packets.json**
You can modify the `packets.json` file to specify which points each packet
contains.  If a point is not found in the packets file, then the system will
fallback to the `packetPrefix` grouping method above.

# BSON output via socket

The BSON output module generates BSON messages which are output via a user-defined socket. The BSON specification can be found on http://bsonspec.org/

## BSON message format.

field name | description
--- | ---
name | A string containing the name of the mnemonic of this sample.
flags | An unsigned integer of the flags associated with the mnemonic sample. See the mnemonic flags table for decoding the flags value.
timestamp | A date value of the timestamp of this mnemonic sample. The date value is with respect to the UNIX epoch.
raw_type | An integer identifier of the type of the raw_value. See the value type identifier table for possible values.
raw_value | The raw value of the mnemonic sample.
eng_type | An integer identifier of the type of the eng_value. This key/value pair is only present if the mnemonic has a defined conversion. See the value type identifier table for possible values.
eng_val | The engineering value of the mnemonic sample. This key/value pair is only present if the mnemonic has a defined conversion.


## Mnemonic flags

flag | meaning
--- | ---
0x80000000 | Flag indicating mnemonic value has been set.
0x40000000 | The mnemonic value is static.
0x20000000 | The mnemonic value is of questionable quality.
0x04000000 | Flag indicating a conversion is enabled for the mnemonic.
0x02000000 | Flag indicating limit checking is enabled for the mnemonic.
0x01000000 | Flag indicating delta limit checking is enabled for the mnemonic.
0x00200000 | Flag indicating the mnemonic value sample triggered a delta limit violation.
0x00100000 | Flag indicating the mnemonic is in the red-high state of limit violation.
0x00080000 | Flag indicating the mnemonic is in the red-low state of limit violation.
0x00040000 | Flag indicating the mnemonic is in the yellow-high state of limit violation.
0x00020000 | Flag indicating the mnemonic is in the yellow-low state of limit violation.
0x00010000 | Flag indicating the mnemonic is within limits.
0x00008000 | Flag indicating the most recent limit transition was a red-high violation.
0x00004000 | Flag indicating the most recent limit transition was a red-low violation.
0x00002000 | Flag indicating the most recent limit transition was a yellow-high violation.
0x00001000 | Flag indicating the most recent limit transition was a yellow-low violation.
0x00000800 | Flag indicating the most recent limit transition was back within limits.
0x00000400 | Flag indicating the mnemonic is read-only.
0x00000200 | Flag indicating the mnemonic value is from a simulated source.

## Value type identifiers.

the docs appear incorrect.  I have experimentally checked and guessed what some values actually are.

documented value | experimental value | type
--- | --- | ---
63 | 71 | signed integer
64 | 72 | unsigned integer
65 | 73 | floating-point
66 | 74 | string
67 | 75 | time
68 | 76 | date
69 | 77 | raw (byte array)

# TODO:
* [ ] client websocket should attempt to reconnect when disconnected.
* [ ] Dictionary: Support custom packet identifiers.
* [ ] Finish or remove file based history.
* [ ] switch redis to a bucket based storage system to improve data compaction rate.
* [ ] redis based history needs to prune old data
