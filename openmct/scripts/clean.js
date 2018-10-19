/**
  * clean.js
  *
  * Cleanup generated resources associated with autocoder and builds scripts
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  **/

const fs = require('fs');
const path = require('path');

const config = require('../config');

const filenames = fs.readdirSync(path.dirname(__dirname) + '/res'),
      pointsFileTemplate = config.pointsFileTemplate,
      packetsFileTemplate = config.packetsFileTemplate,
      dictionaryTemplate = config.dictionaryTemplate,
      pointsRegExp = RegExp(pointsFileTemplate.replace('${deployment}', '(\\w+)')),
      packetsRegExp = RegExp(packetsFileTemplate.replace('${deployment}', '(\\w+)')),
      dictionaryRegExp = RegExp(dictionaryTemplate.replace('${deployment}', '(\\w+)'));

let filesToRemove = [];

filesToRemove = filenames.filter( (filename) => {
      return pointsRegExp.test('/res/' + filename) ||
            packetsRegExp.test('/res/' + filename) ||
            dictionaryRegExp.test('/res/' + filename);
});

filesToRemove.forEach( (filename) => {

    fs.unlink(path.dirname(__dirname) + '/res/' + filename, (err) => {
        if (err) {
            console.log(`Remove failed: ${err.message}`)
        } else {
            console.log(`Successfully removed ${filename}`);
        }

    });
});
