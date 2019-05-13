// Create fixed display of all channels

var getIds = require('./deserialize').getIds;
var fs = require('fs');

function CreateFixed(target) {

  var type = target.toLowerCase() + '.taxonomy:';

  var idSize = getIds(target.toLowerCase()).length;
  var composition = getIds(target.toLowerCase()).map(function (id) {
    return type + id.toString()
  });

  var elements = [];
  composition.forEach(function (id, i) {
    elements.push({
      "type": "fixed.telemetry",
      "x": 0,
      "y": i,
      "id": id,
      "stroke": "transparent",
      "color": "",
      "titled": true,
      "width": 8,
      "height": 1,
      "useGrid": true
    });
  });

  var fixed = {
    "mine": {
      "name": "My Items",
      "type": "folder",
      "composition": [
        "267f72c9-22c3-4d7c-ad5e-e471e76e71be"
      ],
      "location": "ROOT",
      "modified": 1500501635491,
      "persisted": 1500501635491
    },

    "267f72c9-22c3-4d7c-ad5e-e471e76e71be": {
      "layoutGrid": [
        64,
        idSize + 5
      ],
      "composition": composition,
      "name": "Fixed Display Channels",
      "type": "telemetry.fixed",
      "configuration": {
        "fixed-display": {
          "elements": elements
        }
      },
      "modified": Date.now(),
      "location": "mine",
      "persisted": Date.now()
    }
  };

  fs.writeFile('server/res/fixed.json', JSON.stringify(fixed), function (err) {
    if (err) {
      console.log(err);

    }
  });
}

module.exports = CreateFixed;
