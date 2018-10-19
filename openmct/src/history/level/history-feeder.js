var packValue = require('../../util/pack-value');

function HistoryFeeder(options, telemetryServer, databaseManager) {
    this.options = options;
    this.telemetryServer = telemetryServer;
    this.databaseManager = databaseManager;
    this.pending = 0;
    this.successes = 0;
    this.failures = 0;
    this.overload = false;

    this.callback = this.callback.bind(this);
    this.writeChunk = this.writeChunk.bind(this);
    this.telemetryServer.on('input:chunk', this.writeChunk);
}

HistoryFeeder.prototype.writeChunk = function (values) {
    var batches = [];
    var batchMap = {};
    var batchKey;
    var key;
    var value;
    for (var i = 0, len = values.length; i < values.length; i++) {
        batchKey = values[i].timestamp.toISOString().slice(0, 10);
        if (!batchMap[batchKey]) {
            batchMap[batchKey] = this.databaseManager.getDatabase(values[i].timestamp).batch();
            batches.push(batchMap[batchKey]);
        }
        key = values[i].name + ':' + values[i].timestamp.getTime()
        value = packValue.packBuffer(values[i])
        batchMap[batchKey] = batchMap[batchKey].put(key, value);
    };
    batches.forEach(function (batch) {
        this.pending++;
        batch.write(this.callback)
    }, this);
    if (this.pending >= 100 && !this.overload) {
        this.overload = true;
        this.telemetryServer.emit('history:overload');
    }
}

HistoryFeeder.prototype.callback = function (err) {
    this.pending--;
    if (this.pending <= 100 && this.overload) {
        this.overload = false;
        this.telemetryServer.emit('history:okay');
    }
    if (err) {
        console.error('LEVEL: error writing to store', err);
        this.failures++;
    } else {
        this.successes++;
    }

}

module.exports = HistoryFeeder;
