// Routes commands to net server.

var net = require('net');
const WebSocket = require('ws');  // Websocket server
var fs = require('fs');

var serialize = require('./serialize').serialize;

function CommandServer(tcpSite, tcpPort, commandPort, target) {
  let targetKey = target.toLowerCase();
  var commandHistory = {'commands': []};
  fs.closeSync(fs.openSync('server/logs/command-log.json', 'w')); // Create log json
  fs.writeFileSync('server/logs/command-log.json', JSON.stringify(commandHistory), function (err) {
    if (err) {
      console.log(err);
    }
  });
  var client = new net.Socket();
  client.connect(tcpPort, tcpSite, function() {
    console.log('Connected! Command server on port: ' + commandPort);
    // Register client
    client.write('Register GUI\n');
  });

  // Command websocket listener
  const wssc = new WebSocket.Server({port: commandPort});

  wssc.on('connection', function connection(ws) {
    
    ws.on('message', function incoming(message) {
      let clientCommand = JSON.parse(message);

      // Serialize message
      let commandPacket = serialize(clientCommand, targetKey);

      // Save command to log
      commandHistory['commands'].unshift(clientCommand);
      fs.writeFileSync('server/logs/command-log.json', JSON.stringify(commandHistory), function (err) {
        if (err) {
          console.log(err);
        }
      });

      // Broadcast message to all clients
        wssc.clients.forEach(function each(client) {
           client.send(JSON.stringify(clientCommand));
        });
      client.write(commandPacket);  // Send
    });
  });
}

module.exports = CommandServer;
