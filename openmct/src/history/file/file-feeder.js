

var fs = require('fs');
var buckets = require('./buckets');

var packValue = require('../../util/pack-value');

function FileFeeder(options, telemetryServer) {
    this.options = options;
    this.telemetryServer = telemetryServer;

    this.outputPath = 'file-data';
    try {
        fs.statSync(this.outputPath);
    } catch (e) {
        fs.mkdirSync(this.outputPath);
    }

    this.writeChunk = this.writeChunk.bind(this);
    this.telemetryServer.on('input:chunk', this.writeChunk);
    this.outstanding = 0;
    this.dirExists = {};
    this.pending = [];
    setInterval(this.workQueue.bind(this), 30000);
}


FileFeeder.prototype.writeChunk = function (chunk) {
    chunk.forEach(this.writeValue, this);
};

FileFeeder.prototype.checkPath = function (path) {
    try {
        fs.statSync(this.outputPath + path);
    } catch (e) {
        fs.mkdirSync(this.outputPath + path);
    }
};

FileFeeder.prototype.writeValue = function (value) {
    if (!this.dirExists[value.name]) {
        var partial = ''
        value.name.split('.').forEach(function (p) {
            partial += '/' + p;
            this.checkPath(partial);
        }, this)
        this.dirExists[value.name] = true;
    }


    var path = this.outputPath + '/' + buckets.getBucketPath(value.name, value.timestamp);
    this.queue(path, packValue.pack(value));
};

FileFeeder.prototype.workQueue = function () {
    Object.keys(this.pending).forEach(function (key) {
        var values = this.pending[key];
        delete this.pending[key];
        this.outstanding += 1;
        fs.appendFile(key, values.join(''), function () {
            this.outstanding -= 1;
        }.bind(this));
    }, this);
};

FileFeeder.prototype.queue = function (path, value) {
    this.queued += 1;
    if (!this.pending[path]) {
        this.pending[path] = []
    }
    this.pending[path].push(value);
};


module.exports = FileFeeder;
