/**
  * deserialize-cosmos-binary.js
  *
  * Reads COSMOS binary logs and pushes them through as BSON packets to the
  * OpenMCT server
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  **/

const fs = require('fs');
const path = require('path');
const net = require('net');
const BSON = require('bson');

const binaryDeserializer = require('../src/util/deserialize-binary');
const config = require('../config.js');
const dictionary = require('../' + config.binaryInput.dictionaryFile)
const BSONAdapter = require('../src/bson-adapter');

const filename = process.argv[2];
const filepath = path.normalize(path.dirname(__dirname) + '/' + filename);
const outFilepath = path.normalize(__dirname + '/../res/COSMOSLog.json');
const bson = new BSON();
const deserializer = new binaryDeserializer(dictionary, config.deployment)

if (!filename) {
    console.log('Usage: node deserialize-cosmos-binary.js <COSMOS Binary log file>');
    process.exit();
} else {
    console.log(`Reading COSMOS binary log from ${filepath}`)
}

// Format of binary header. Lengths specified in bytes.
const headerFormat = [
    {
        name: 'Marker',
        key: 'marker',
        type: 'S',
        length: 8
    },
    {
        name: 'Type',
        key: 'type',
        type: 'S',
        length: 4
    },
    {
        name: 'MD5',
        key: 'md5',
        type: 'S',
        length: 32
    },
    {
        name: 'Underscore',
        key: 'underscore',
        type: 'S',
        length: 1
    },
    {
        name: 'Hostname',
        key: 'hostname',
        type: 'S',
        length: 83
    }
]

// Packet definition. Strings in length field indicate that this property is
// variable length, and the length will be read from the named property of the packet.
// Lengths are specified in bytes
const packetFormat = [
    {
        name: 'Time Seconds',
        key: 'timeSeconds',
        type: 'I',
        length: 4
    },
    {
        name :'Time Microseconds',
        key: 'timeUSeconds',
        type: 'I',
        length: 4
    },
    {
        name: 'Target Name Length',
        key: 'targetNameLength',
        type: 'U',
        length: 1
    },
    {
        name: 'Target Name',
        key: 'targetName',
        type: 'S',
        length: 'targetNameLength'
    },
    {
        name: 'Packet Name Length',
        key: 'packetNameLength',
        type: 'U',
        length: 1
    },
    {
        name: 'Packet Name',
        key: 'packetName',
        type: 'S',
        length: 'packetNameLength'
    },
    {
        name: 'Packet Length',
        key: 'packetLength',
        type: 'U',
        length: 4
    },
    {   // This is the actual F' packet, verbatim
        name: 'Packet',
        key: 'packet',
        type: 'B',
        length: 'packetLength'
    }
];

/**
  * Given a binary packet and a JSON object representing the format of the binary,
  * deserialize the packet and return it
  * @param {Buffer} data The binary buffer to be deserialized
  * @param {Object} format A JSON format object, as described above
  * @param {number} offset The point in the buffer to begin reading from
  * @return {Object} A JS object with the following format:
  *                   {
  *                     packet: The deserialized packet in JSON format
  *                     offset: The length of the packet in bytes
  *                   }
  */
function readPacket(data, format, offset) {
    let packetOffset = 0,
        packet = {},
        length = format.length; //number of fields in packet

    for (let i = 0; i < length; i += 1) {
        let item = format[i],
            bitLen,
            byteLen;

        if (typeof item.length === 'number') {
            // Use number literal as length
            byteLen = item.length;
        } else if (typeof item.length === 'string') {
            // Get length from packet value with key provided as length
            byteLen = packet[item.length];
        } else {
            throw new Error(`Invalid length field in ${item.name}`)
        }

        bitLen = byteLen * 8;

        if (item.type === 'B') {
            //Handle case where value is itself a binary packet which must be deserialized
            let packetData = data.slice(offset, offset + byteLen);
            value = deserializer.deserialize(packetData);
            if (value && value[0]) {
                value = value[0]
            }
        } else {
            value = deserializer.readBuff(data, bitLen, item.type, offset);
        }

        packet[item.key] = value;
        offset += byteLen;
        packetOffset += byteLen;
    }

    return {
        packet: packet, //deserialized data
        offset: packetOffset //total length of packet
    }
}

let data = fs.readFileSync(filepath),
    totalOffset = 0,
    len = data.length,
    packets = [];

//Read COSMOS binary header for log file and ignore
let header = readPacket(data, headerFormat, totalOffset);
totalOffset += header.offset;

//read the metadata packet and ignore, couldn't find the format for its payload
let metapacket = readPacket(data, packetFormat, totalOffset);
totalOffset += metapacket.offset;

//read the remaining packets, whose payloads are all fprime binary packets
while (totalOffset < len) {
    let packetData = readPacket(data, packetFormat, totalOffset);
    totalOffset += packetData.offset;
    packets.push(packetData.packet.packet);
}

console.log(`Writing JSON log to ${outFilepath}`);
fs.writeFileSync(outFilepath, JSON.stringify(packets));

// abuse the BSONAdapter to send packets over a websocket with nice error handling
let adapter = new BSONAdapter(config);

let openMCTTelemetryClient = {
    socket: new net.Socket(),
    name: "OpenMCT BSON Stream Socket",
    port: config.input.port,
    site: config.input.bindAddress,
    successFunction: function () {
        // this is called when the socket connects successfully
        console.log('Sending packets to BSON server...')
        packets.forEach( (packet) => {
            //serialize each packet as BSON and send it to OpenMCT server
            if (packet) {
                let packetAsBSON = bson.serialize(packet);
                this.socket.write(packetAsBSON);
            }
        });
        console.log('All packets sent, exiting')
        this.socket.end();
    }
};

adapter.connectSocket(openMCTTelemetryClient).catch( (reject) => {
    adapter.printRejectNotice(reject, openMCTTelemetryClient);
    console.log('OpenMCT Server must be running for this script to work.')
});
