var FileFeeder = require('./file/file-feeder');
var FileServer = require('./file/file-server');

function FileHistory(options, telemetryServer) {
    this.feeder = new FileFeeder(options, telemetryServer);
    this.server = new FileServer(options, telemetryServer);
}

module.exports = FileHistory;
