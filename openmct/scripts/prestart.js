/**
  * prestart.js
  *
  * Do some preprocessing on the 'config.js' file so dictionaries are read
  * from the correct place for a given deployment
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  */

const path = require('path');
const fs = require('fs');
const config = require('../config')

const deployment = process.argv[2];
const channels = process.argv.slice(3, process.argv.length);
const filepath = path.normalize(path.dirname(__dirname) + '/config.js');
const channelsFilepath = path.dirname(__dirname) + '/res/channels.json'
const configJS = fs.readFileSync(filepath, {encoding: 'UTF-8'});

const pointsFileTemplate = config.pointsFileTemplate;
const packetsFileTemplate = config.packetsFileTemplate;
const dictionaryTemplate = config.dictionaryTemplate;

const resFiles = fs.readdirSync(path.dirname(__dirname) + '/res');
const dictionaryRegExp = RegExp(dictionaryTemplate.replace('${deployment}', '(\\w+)'));

let deployments = [];

resFiles.forEach( (filename) => {
    // parse available deployments from contents of 'res' directory
    let fileString = '/res/' + filename,
        match = fileString.match(dictionaryRegExp);

    if (match) {
        deployments.push(match[1]);
    }

});

if (!deployment || !deployments.includes(deployment)) {
    // Print usage information for bad input
    console.log('Usage: npm start <deployment key> [channels]\n');
    if (deployments[0]) {
        console.log('Available deployment keys are: ' + deployments.join(', ') + '\n');
    } else {
        console.log('No deployment configuration available. Please run the \'configure\' script. \n')
    }
    process.exit(-1);
}

let pointsFile = pointsFileTemplate.replace('${deployment}', deployment);
let packetsFile = packetsFileTemplate.replace('${deployment}', deployment);
let dictionaryFile = dictionaryTemplate.replace('${deployment}', deployment);

let includedChannels = []

if (channels.length > 0) {
    // if channels argument provided, create configuration for overlay plot in client
    let dict = require('../' + dictionaryFile);
    let channelArr = Object.values(dict[deployment].channels);
    let channelNames = channelArr.map( (channelObj) => channelObj.name);
    channels.forEach( (channel) => {
        if (channelNames.includes(channel)) {
            includedChannels.push(channel)
        } else {
            console.log(`ERROR: channel ${channel} not defined in ${dictionaryFile}, not including`)
        }
    });
    console.log("\nChannels " + includedChannels.join(', ') + " will be loaded into overlay plot");
}

let channelsJS = {
    channels: includedChannels
}

// replace configuration in 'config.js' with the correct names for this deployment
let newConfigJS = configJS.replace(/pointsFile: '(.*)'/, `pointsFile: '${pointsFile}'`);
newConfigJS = newConfigJS.replace(/packetsFile: '(.*)'/, `packetsFile: '${packetsFile}'`);
newConfigJS = newConfigJS.replace(/dictionaryFile: '(.*)'/, `dictionaryFile: '${dictionaryFile}'`);
newConfigJS = newConfigJS.replace(/deployment: '(.*)'/, `deployment: '${deployment}'`);

//write new config file
fs.writeFileSync(filepath, newConfigJS);
//write overlay configuration file
fs.writeFileSync(channelsFilepath, JSON.stringify(channelsJS));
