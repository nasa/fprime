/**
  * generateConfigJSON.js
  *
  * Given a JSON dictionary for an fprime app, generate JSON configuration
  * files for the openmct bson server. Points and packets files are saved
  * to locations specified in config.js
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  *
  */

const fs = require('fs');
const path = require('path');

const config = require('../config');

const outFilenamePointsTemplate = config.pointsFileTemplate,
      outFilenamePacketsTemplate = config.packetsFileTemplate;

// dictPath.txt is written by autocoder so that this script knows what dictionary
// it should use to build configuration
let dictName = fs.readFileSync(path.dirname(__dirname) + '/res/dictPath.txt');

let dictJSON = fs.readFileSync(path.dirname(__dirname) + '/' + dictName, {encoding: 'UTF-8'}),
    filepathConfig = path.dirname(__dirname) + '/config.js',
    configJS = fs.readFileSync(filepathConfig, {encoding: 'UTF-8'}),
    dict = JSON.parse(dictJSON),
    deployment = Object.keys(dict)[0],
    deploymentDict = dict[deployment],
    pointDict = {},
    packetDict = {};

let deploymentName = deployment,
    packetName = deploymentName + " Channels",
    evrPacketName = deploymentName + " EVRs"
    outFilenamePoints = outFilenamePointsTemplate.replace('${deployment}', deployment),
    outFilenamePackets = outFilenamePacketsTemplate.replace('${deployment}', deployment);

packetDict[packetName] = {
    name: packetName,
    points: []
}

packetDict[evrPacketName] = {
    name: evrPacketName,
    points: []
}

//OpenMCT format objects for given fields
let time_format = {
  'key': 'utc',
  'source': 'timestamp',
  'name': 'Timestamp',
  'format': 'utc',
  'hints': {
    'domain': 1
  }
};
let name_format = {
  'hints': {
    'domain': 2
  },
  'key': 'name',
  'name': 'Name'
};
let raw_value_format = {
    key: "raw_value",
    name: "Raw Value",
    hints: {"range": 2}
};

// Generate JS objects representing OpenMCT configuration files
Object.entries(deploymentDict.channels).forEach(function (channel) {
    let id = channel[0],
        props = channel[1],
        name = props.name;

    // Add this point to the packet dictionary for channels
    packetDict[packetName].points.push(name);

    // Add the definition of this point to the point dictionary for channels
    pointDict[name] = {
        name: name,
        key: name,
        id: name,
        values: [time_format, name_format, raw_value_format]
    }
});

// Generate JS objects representing OpenMCT configuration files
Object.entries(deploymentDict.events).forEach(function (evr) {
    let id = evr[0],
        props = evr[1],
        name = props.name;

    // Add this point to the packet dictionary for channels
    packetDict[evrPacketName].points.push(name);

    // Add the definition of this point to the point dictionary for events
    pointDict[name] = {
        name: name,
        key: name,
        id: name,
        values: [time_format, name_format, raw_value_format]
    }
});

//Write configuration files
let outFilepathPoints = path.dirname(__dirname) + '/' + outFilenamePoints;
let outFilepathPackets = path.dirname(__dirname) + '/' + outFilenamePackets;

console.log(`Writing points config file to ${outFilepathPoints}`);
fs.writeFileSync(outFilepathPoints, JSON.stringify(pointDict));
console.log(`Writing packets config file to ${outFilepathPackets}`);
fs.writeFileSync(outFilepathPackets, JSON.stringify(packetDict));

console.log(`\nTo start the OpenMCT server configured for this deployment, use deployment key '${deployment}'\n`)
