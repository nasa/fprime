/*
  Server used to translate tcp packet stream into mct format. Only works with one client atm.
*/

// System modules
var fs = require('fs');   // File system
var net = require('net'); // Net server
const WebSocket = require('ws');  // Websocket server

// User modules
var deserialize = require('./deserialize').deserialize; // Decode packets
var getIds = require('./deserialize').getIds; // Get ids for history

var ws_number = 0;
var history = {};
function RealtimeTelemServer(tcpSite, tcpPort, realMctPort, target) {
  let targetKey = target.toLowerCase();

  // Ref client
  var client = new net.Socket();
  client.connect(tcpPort, tcpSite, function() {
    console.log('Connected! Realtime server on port: ' + realMctPort);

    fs.closeSync(fs.openSync('server/logs/telem-log.json', 'w')); // Create log json

    // Register client
    client.write('Register GUI\n');

    // Populate history with ids
    getIds(targetKey).forEach(function (id) {
      history[id.toString()] = [];
    });

  });

  // Realtime webserver
  const wssr = new WebSocket.Server({port: realMctPort});

  // Get net server data and save to history
  client.on('data', function (data) {
    // Deserialize data into list of packets
    var toMCT = deserialize(data, targetKey);

    // Send to websocket
    toMCT.forEach(function (packet) {
      // Add to history dictionary
      history[(packet.id).toString()].push(packet);
      fs.writeFile('server/logs/telem-log.json', JSON.stringify(history), function (err) {
        if (err) {
          console.log(err);
        }
      });

    });
  });


  wssr.on('connection', function connection(ws) {
    ws_number += 1;
    // For every client connection:
    console.log("Realtime Client connected");

    var subscribed = {}; // Subscription dictionary unique to each connected client

    // Get target data and send realtime telem data
    client.on('data', function (data) {
      // Deserialize data into list of packets
      var toMCT = deserialize(data, targetKey);

      // Send to websocket
      toMCT.forEach(function (packet) {
        // Add to history dictionary

        // Send to realtime server
        if (subscribed[packet.id]) {

          ws.send(JSON.stringify(packet), function ack(error) {
            // console.log(JSON.stringify(packet));
            if (error) {
              // If unable to send (ie. client disconnection)
              // then subscription is reset
              console.log("Realtime Client disconnected");
              subscribed = {};
            }
          });
        }
      });
    });

    // Subscription
    ws.on('message', function incoming(message) {
      var operation = message.split(" ")[0];  // Get subscribe or unsubscribe operation
      var idReq = message.split(" ")[1];  // Get id query
        // Set id subscription
        if (operation === 'subscribe') {
          // Subscribe to id
          subscribed[idReq] = true;
        } else if (operation === 'unsubscribe') {
          // Unsubscribe to id
          subscribed[idReq] = false;
        }
    });

    ws.on('disconnect', function() {
      ws_number -= 1;
      console.log('Realtime Client disconnected: ' + ws_number.toString());
    });
  });
}

module.exports = RealtimeTelemServer;
