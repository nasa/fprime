var EventEmitter = require('events');

var express = require('express');
var expressWs = require('express-ws');

var HistoryRouter = require('./routes/history');
var RealtimeRouter = require('./routes/realtime');
var DictionaryRouter = require('./routes/dictionary');
var ClientRouter = require('./routes/client');

var PersistenceRouter = require('./routes/persistence');
var Dictionary = require('./dictionary');
var BSONInput = require('./input/bson-stream');
var LimitMonitor = require('./limit-monitor');

function TelemetryServer(options) {
    this.options = options;
    this.isRunning = false;

    if (options.input) {
        if (options.input.type !== 'bson') {
            throw new Error('Unsupported input type: ' + options.input.type);
        }
    }

    if (options.history) {
        if (options.history.type === 'redis') {
            this.history = require('./history/redis');
        } else if (options.history.type === 'file') {
            this.history = require('./history/file');
        } else if (options.history.type === 'level') {
            this.history = require('./history/level');
        } else {
            throw new Error('unsupported history type: ' + options.history.type);
        }
    }
};

TelemetryServer.prototype = Object.create(EventEmitter.prototype);

TelemetryServer.prototype.run = function () {
    if (this.isRunning) {
        throw new Error('Telemetry Server is already running.');
    }
    this.isRunning = true;
    var app = express();
    expressWs(app);

    if (this.options.input) {
        var inputStream = new BSONInput(this.options.input, this);
        var realtime = new RealtimeRouter(this);
        var limitMonitor = new LimitMonitor(this);
        app.use('/realtime', realtime.router);
    }

    if (this.history) {
        this.history = new this.history(this.options.history, this);
        app.use('/history', new HistoryRouter(this.history));
    }

    if (this.options.client.persistence.type === 'embedded') {
        app.use('/persistence', new PersistenceRouter(this.options.deployment));
    };

    this.dictionary = new Dictionary(this.options.dictionary, this);
    app.use('/dictionary', new DictionaryRouter(this.dictionary));
    app.use('/', new ClientRouter(this.options.client, this));

    app.listen(this.options.client.port, this.options.client.host, function () {
        console.log(`Web server running at ${this.options.client.host}:${this.options.client.port}`)
    }.bind(this));
};


module.exports = TelemetryServer;
