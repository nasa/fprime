/**
 * Basic implementation of a history and realtime server.
 */

// Dependencies
// Utils
const fs = require('fs');

// User Dependencies
const StaticServer = require('./src/static-server');
const RealtimeTelemServer = require('./src/telemetry-server');
const HistoryServer = require('./src/history-server');
const CommandServer = require('./src/command-server');
const CreateFixed = require('./src/create-fixed');
const config = require('./../config.js').values();

// Set following
const target = config.target;   // Target Name
const nodeSite = config.nodeSite;   // Host to serve the single page application
const staticPort = config.staticPort;  // Port for single page application
const tcpSite = config.tcpSite;    // Host of the TCP server
const tcpPort = config.tcpPort;  // Port for the TCP server
const realTelemPort = config.realTelemPort;   // Port streaming live telemetry datums to client
const commandPort = config.commandPort;   // Port to listen for commands from client

// Create log directory
if (!fs.existsSync('server/logs')) {
  fs.mkdirSync('server/logs');
}

// Create static server for client
const staticServer = new StaticServer(nodeSite, staticPort);
RealtimeTelemServer(tcpSite, tcpPort, realTelemPort, target);
HistoryServer();
CommandServer(tcpSite, tcpPort, commandPort, target);
