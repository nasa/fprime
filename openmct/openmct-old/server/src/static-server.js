var express = require('express');
function StaticServer(nodeSite, port) {
  var server = express();
  
  server.use('/', express.static(__dirname + '/../..'));
  
  console.log('Open MCT hosted at ' + nodeSite + ':' + port);
  
  server.listen(port);
}

module.exports = StaticServer;
