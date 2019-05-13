/**
 * Telemetry Server-- all configuration is passed in below.
 */

var TelemetryServer = require('./src/telemetry-server');
var BSONAdapter = require('./src/bson-adapter');
var config = require('./config');

var server = new TelemetryServer(config);
var adapter = new BSONAdapter(config);

server.run();
adapter.run();
