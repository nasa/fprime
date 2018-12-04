/** deserialize-binary.js
 *
 * Util library for converting fprime binary packets to json
 *
 * @author Sanchit Sinha sinhasaeng@gmail.com
 * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
 */

// Utils
var vsprintf = require('sprintf-js').vsprintf;
var Long = require('long')

// BSON typecodes
const typeCodes = {
    I: 71,
    U: 72,
    F: 73,
    S: 74,
    E: 74
}

// OpenMCT binary limit state flags
const flags = {
    redHigh: 0x00100000,
    yellowHigh: 0x00040000,
    yellowLow: 0x00020000,
    redLow: 0x00080000,
    allGood: 0x00010000
}

// Packet sizes in bytes
const sizeLen = 4;
const descriptorLen = 4;
const idLen = 4;
const timeBaseLen = 2;
const timeContextLen = 1;
const secondsLen = 4;
const uSecLen = 4;
// Size of all packet descriptions except size. Used to calculate size of value
const packetDescriptionLen = 19;

/**
  * A class for decoding streams of F' binary
  * @constructor
  * @param {Object} dictionary A dictionary produced by the JSONDictonaryGen Autocoder
  * @param {String} target The name of a deployment in the dictionary to use
  */
function binaryDeserializer(dictionary, target) {
    this.dictionary = dictionary[target];
}

/**
  * This function reads a buffer using the specified parameters
  * @param {Buffer} buff - Node Buffer to read from
  * @param {number} bits - Number of bits to read
  * @param {string} type - 'U'nsigned, 'I'nteger, 'F'loat, 'S'tring, 'B'oolean,
                          or the name of any serializable specified in the dictionary
  * @param {number} offset - Byte to start reading from
  * @return Value read, number or string
*/
binaryDeserializer.prototype.readBuff = function (buff, bits, type, offset) {
    let serializableDict = this.dictionary.serializables;
    let serializable = serializableDict[type];

    if (typeof offset == "undefined") {
        offset = 0;
    }

    //recursively decode serializables
    if (serializable) {
        let members = serializable.members;
        let len = members.length;
        let decodedSerializable = [];
        let localOffset = offset;
        for (let i = 0; i < len; i++) {
            let member = members[i];
            let memberType = member.type;
            let isEnum = (typeof memberType === 'object')

            if (isEnum) {
                memberType = 'I32'
            }

            let bitLength = this.getBitNumber(memberType);
            let memberValue = this.readBuff(buff, bitLength, memberType, localOffset);

            if (isEnum) {
                decodedSerializable.push({
                    name: member.name,
                    format_string: '%s',
                    value: member.type.values[memberValue].name
                });

            } else {
                decodedSerializable.push({
                    name: member.name,
                    format_string: member.format_string,
                    value: memberValue
                });
            }

            localOffset += bitLength/8;
        }
        return this.formatSerializable(decodedSerializable);
    }

    switch(type.substring(0,1).toUpperCase()) {
        case 'U': {
            // Unsigned Int
            switch(bits) {
                case 64: {
                    let low = buff.readUInt32BE(offset),
                        high = buff.readUInt32BE(offset + 4),
                        longVal = new Long(high, low);
                    return longVal.toNumber();
                }
                case 32: {
                    return buff.readUInt32BE(offset);
                }
                case 16: {
                    return buff.readUInt16BE(offset);
                }
                case 8: {
                    return buff.readUInt8(offset);
                }
                default: {
                    // Invalid bit size
                    console.log("[ERROR] Invalid UInt size!: " + bits);
                    return null;
                }
            }
        }

        case 'I': {
            // Int
            switch(bits) {
                case 32: {
                    return buff.readInt32BE(offset);
                }
                case 16: {
                    return buff.readInt16BE(offset);
                }
                case 8: {
                    return buff.readInt8(offset);
                }
                default: {
                    // Invalid bit size
                    console.log("[ERROR] Invalid Int size!");
                    return null;
                }
            }
        }

        case 'F': {
            // Float
            switch(bits) {
                case 32: {
                    return buff.readFloatBE(offset);
                }
                default: {
                    // Invalid bit size
                    console.log("[ERROR] Invalid Float size!");
                    return null;
                }
            }
        }
        case 'S': {
            //String
            stringBuff = buff.slice(offset, offset + bits/8);
            return stringBuff.toString('utf-8');
        }

        case 'B': {
            //Boolean
            let value = buff.readIntBE(1)
            return value;
        }

        default: {
            // Invalid type
            console.log("[ERROR] Invalid type! " + type);
            return null;
        }
    }
}

/**
  * Format a given sprintf string given the replacement values as a buffer,
  * and the types of the arguments
  * @param {Buffer} buff The values to be inserted into the format string
  * @param {string} strBase The format string
  * @param {string[]} argTypes The codes for the types of the arguments
  */
binaryDeserializer.prototype.formatPrintString = function (buff, strBase, argTypes) {
    let offset = 0;

    let sprintfRefExp = new RegExp('%', 'g');
    let matches = [];
    let formatString = strBase;
    let returnValue = '';
    while (sprintfRefExp.exec(formatString)) {
        matches.push(sprintfRefExp.lastIndex);
    }
    let args = argTypes.map( (type, argIndex) => {
        if (typeof type === 'string') {
            // Non-enum type
            if (type === 'string') {
                // String type has variable length defined in packet
                const stringLengthLen = 2;
                let stringLength = this.readBuff(buff, stringLengthLen * 8, 'U', offset);
                offset += stringLengthLen;
                str = this.readBuff(buff, stringLength * 8, 'S', offset);
                offset += stringLength;
                return str
            } else {
                let bits = parseInt(type.substring(1), 10);
                let num = this.readBuff(buff, bits, type, offset);
                offset += bits / 8;
                return num;
            }
        } else if (typeof type === 'object') {
            // Enum type
            let index = this.readBuff(buff, 32, 'I', offset);
            let value = type.values[index.toString()].name;

            // replace %d with %s in sprintf string when the print string
            // is supposed to be an enum label
            let char = formatString.charAt(matches[argIndex])
            if (formatString.charAt(matches[argIndex]) === 'd') {
                let startStr = formatString.slice(0, matches[argIndex]);
                let endStr = formatString.slice(matches[argIndex] + 1);
                formatString = startStr + 's' + endStr;
            }

            return value;
        } else {
            // Invalid type
            console.log('[ERROR] Invalid argument type in string formatter!: ' + type);
        }
    });

    try {
        returnValue = vsprintf(formatString, args);
    } catch (err) {
        // prevent vsprintf errors from crashing the deserializer, simply
        // log error
        let errMessage = `Failed to deserialize message: ${err.message}`;
        console.log(errMessage);
        returnValue = errMessage;
    }

    return returnValue;
}

/**
  * Convert a value given it's gain and offset
  * @param {Number} value The raw value
  * @param {Number} gain The gain to apply
  * @param {Number} offset The offset to apply
  * @return {Number} The converted value
  */
binaryDeserializer.prototype.gainOffsetConv = function (value, gain, offset) {
    return gain * value + offset;
}

/**
  * Deserialize a Fprime packet
  * @param {Buffer} data - Input buffer array of raw packet
  * @param {string} target - Target name (or deployment)
  * @return: Array of OpenMCT formatted data as JSON
 */
binaryDeserializer.prototype.deserialize = function (data) {
    let telem = this.dictionary;
    let packetArr = [];
    let packetLength = data.length;
    let offset = 0;
    // Interact deserialized packets
    while (offset < packetLength) {

        // handle case where 9-byte A5A5 header is included. Header is simply ignored if present
        let header = this.readBuff(data, 9 * 8, 'S', offset);
        if (header.startsWith('A5A5')) {
            offset += 9;
        };

        let size = this.readBuff(data, sizeLen * 8, 'U', offset);
        offset += sizeLen;

        let descriptor = this.readBuff(data, descriptorLen * 8, 'U', offset);
        offset += descriptorLen;

        let id = this.readBuff(data, idLen * 8, 'U', offset);
        offset += idLen;

        let timeBase = this.readBuff(data, timeBaseLen * 8, 'U', offset);
        offset += timeBaseLen;

        let timeContext = this.readBuff(data, timeContextLen * 8, 'U', offset);
        offset += timeContextLen;

        let seconds = this.readBuff(data, secondsLen * 8, 'U', offset);
        offset += secondsLen;

        let uSec = this.readBuff(data, uSecLen * 8, 'U', offset);
        offset += uSecLen;

        // Find telemetry format specifiers
        let telemData;
        if (descriptor == 1) {
            // If channel
            telemData = telem['channels'][id.toString()];
        } else if (descriptor == 2) {
            // If event
            telemData = telem['events'][id.toString()];
        } else {
            console.log("[ERROR] Invalid descriptor: " + descriptor);
            return null;
        }

        let valueLen = size - packetDescriptionLen;
        let valueBuff = data.slice(offset, offset += valueLen);

        // If found in dictionary
        let value;
        switch(telemData['telem_type']) {
            case 'channel': {
                // If channel type
                let type = telemData['type'];
                let bits = this.getBitNumber(type);
                if (telemData['format_string']) {
                    let modifierArg;
                    if (type === 'Enum') {
                        let index = this.readBuff(valueBuff, 32, 'I', 0);
                        modifierArg = telemData["enum_dict"][index.toString()];
                    } else {
                        modifierArg = this.readBuff(valueBuff, bits, type, 0);
                    }
                    value = vsprintf(telemData['format_string'], [modifierArg]);
                } else {
                    value = this.readBuff(valueBuff, bits, type, 0);
                }
                break;
            }
            case 'event':
                // If event type
                let strBase = telemData['format_string'];
                let argTypes = telemData['arguments'];
                value = this.formatPrintString(valueBuff, strBase, argTypes);
                break;

            default:
                // None
                break;
            }

            let timestamp = seconds * 1000 + uSec/1000.;

            let datum = {
                'timestamp': new Date(timestamp),
                'identifier': id.toString(),
                'flags': this.evaluateLimits(value, telemData.limits)
            };

            // Create datum in openMCT format
            if (telemData.telem_type === 'channel') {
                datum.name = telemData.name;
                datum.raw_type = this.getBSONTypeCode(telemData.type);
                datum.raw_value = value;
            } else if (telemData.telem_type === 'event') {
                datum.name = telemData.name;
                datum.raw_type = this.getBSONTypeCode('S');
                datum.raw_value = telemData.severity + ': ' + value;
            }

            // Create datums for eac unit type
            let units = telemData['units'];
            if (units != null) {
                units.forEach(function (u) {
                    let keyForm = 'value:' + u['Label'];
                    let valueForm = gainOffsetConv(value, parseInt(u['Gain'], 10), parseInt(u['Offset'], 10));
                    datum[keyForm] = valueForm;
                });
            }
            packetArr.push(datum);
        }
    return packetArr;
}

/**
  * Given a deployment target, get an array of the channel ids
  * @param {string} target The deployment key of the target
  * @return {Array} A list of the channel ids in the deployment
  */
binaryDeserializer.prototype.getIds = function (target, dictionary) {
    var telem = dictionary[target];  // Get format dictionary
    let ids = [];
    let channels = telem['channels'];
    for (let id in channels) {
        ids.push(id);
    }
    return ids;
}

/**
  * Given a value and a JSON object representing a set of limit values for a particular channel,
  * return the binary flag for the limit state of that channel
  * @param {number} value The channel value to evaluate
  * @param {Object} limits Limits represented in JSON format, eg. {high_red: 3 ...}
  * @return {number} The binary flag for the appropriate limit state
  */
binaryDeserializer.prototype.evaluateLimits = function (value, limits) {
    let flag = flags.allGood;
    if (limits) {
        if (value > limits.high_red && limits.high_red !== null) {
            flag = flags.redHigh;
        } else if (value > limits.high_yellow && limits.high_yellow !== null) {
            flag = flags.yellowHigh;
        } else if (value < limits.low_red && limits.low_red !== null) {
            flag = flags.redLow;
        } else if (value < limits.low_yellow && limits.low_yellow !== null) {
            flag = flags.yellowLow;
        }
    }
    return flag;
}

/**
  * Given the key for a particular data type, return it's type code in BSON
  * @param {string} data_type The character representing the data type (I, U, F, S, or E)
  * @return {number} The BSON type code
  */
binaryDeserializer.prototype.getBSONTypeCode = function (data_type) {
    var typeCode = data_type.charAt(0);
    return typeCodes[typeCode];
}

/**
  * Given the key for a particular data type, return its length in bits
  * @param {string} type The string representing this type, i.e. "I32"
  * @return {number} The length of this type in bites
  */
binaryDeserializer.prototype.getBitNumber = function (type) {
    return parseInt(type.substring(1), 10);
}

/**
  * Given a decoded serializable in JSON format, return a suitable print string
  * @param {Object} decodedSerializable The decoded serializable as JSON
  * @return {string} A string representing the serializable in the format "key: value ..."
  */
binaryDeserializer.prototype.formatSerializable = function (decodedSerializable) {
    let strings = decodedSerializable.map( (member) => {
        let formattedString = '';
        try {
            formattedString = vsprintf(member.format_string, member.value.toString());
        } catch (err) {
            console.log(err.message)
            formattedString = member.value;
        }
        return member.name + ': ' + formattedString
    });
    return strings.join(' ');
}

module.exports = binaryDeserializer;
