var level = require('level');

var HistoryClient = require('./level/history-client');
var HistoryFeeder = require('./level/history-feeder');
var DatabaseManager = require('./level/database-manager');

function LevelHistoryClient(options, telemetryServer) {
    console.log(`LevelDB: History will be stored in ${options.databasePath}`);
    var databaseManager = new DatabaseManager(options);
    var historyFeeder = new HistoryFeeder(options, telemetryServer, databaseManager);
    var historyClient = new HistoryClient(databaseManager);
    return historyClient;
}

module.exports = LevelHistoryClient;
