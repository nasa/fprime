var msgpack = require('msgpack');

var binaryString = require('./binary-string');

// step by step encoding functions
// message -> array -> buffer -> binary string

function messageToArray(message) {
    if (message.name === 'LIMIT_LOG') {
        return [
            message.mnemonic,
            message.limit,
            message.flags,
            message.timestamp,
            message.raw_value,
            message.eng_val
        ];
    }
    var array = [
        message.timestamp.getTime(),
        message.flags,
        message.raw_value
    ];
    if (message.hasOwnProperty('eng_type')) {
        array.push(message.eng_val);
    }
    return array;
}

function arrayToBuffer(array) {
    return msgpack.pack(array);
}

function bufferToBinaryString(buffer) {
    return binaryString.fromBuffer(buffer);
}

// step by step decoding functions
// binary string -> buffer -> array -> message

function binaryStringToBuffer(bString) {
    return binaryString.toBuffer(bString);
}

function bufferToArray(buffer) {
    return msgpack.unpack(buffer);
}

function arrayToMessage(array, mnemonic) {
    if (mnemonic === 'LIMIT_LOG') {
        return {
            name: mnemonic,
            mnemonic: array[0],
            limit: array[1],
            flags: array[2],
            timestamp: array[3],
            raw_value: array[4],
            eng_val: array[5]
        }
    }
    if (array.length === 3) {
        return {
            name: mnemonic,
            timestamp: new Date(array[0]),
            raw_value: array[2],
            flags: array[1]
        };
    } else if (array.length === 4) {
        return {
            name: mnemonic,
            timestamp: new Date(array[0]),
            raw_value: array[2],
            flags: array[1],
            eng_val: array[3]
        };
    } else {
        throw Error('Invalid message!');
    }
}

// functions for common encodings:
// message -> buffer
// message -> bstring

function messageToBuffer(message) {
    return arrayToBuffer(messageToArray(message));
}

function messageToBinaryString(message) {
    return bufferToBinaryString(messageToBuffer(message));
}

// functions for common decodings:
// buffer, mnemonic -> message
// bstring, mnemonic -> message

function bufferToMessage(buffer, mnemonic) {
    return arrayToMessage(bufferToArray(buffer), mnemonic);
}

function binaryStringToMessage(bstring, mnemonic) {
    return bufferToMessage(binaryStringToBuffer(bstring), mnemonic);
}

module.exports = {
    packString: messageToBinaryString,
    packBuffer: messageToBuffer,
    unpackString: binaryStringToMessage,
    unpackBuffer: bufferToMessage,
    unpackArray: arrayToMessage
};
