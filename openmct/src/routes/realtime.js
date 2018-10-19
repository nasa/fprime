var express = require('express');
var WebSocket = require('ws');

function RealtimeServer(telemetryServer) {
    this.router = express.Router();

    this.telemetryServer = telemetryServer;
    this.subscribers = {};
    this.toFlush = [];
    this.subCount = 0;
    this.status = 'noData';
    this.timerID = '';
    this.packetCount = 0;

    this.router.ws('/', this.handleConnection.bind(this));

    this.router.get('/status', (req, res) => {
        res.status(200).send({
            status: this.status,
            packetCount: this.packetCount
        });
    });

    this.notify = this.notify.bind(this);
    this.flush = this.flush.bind(this);
    this.startRecvTimer = this.startRecvTimer.bind(this);
    this.endRecvTimer = this.endRecvTimer.bind(this);

    this.telemetryServer.on('input:data', this.startRecvTimer);
};

RealtimeServer.prototype.startRecvTimer = function () {
    if (this.timerID) {
        clearTimeout(this.timerID);
    }
    this.status = 'data';
    this.packetCount++;
    this.timerID = setTimeout(this.endRecvTimer, 3000);
}

RealtimeServer.prototype.endRecvTimer = function () {
    this.status = 'noData';
};

RealtimeServer.prototype.startListening = function () {
    this.telemetryServer.on('input:data', this.notify);
    this.telemetryServer.on('input:chunk', this.flush);
};

RealtimeServer.prototype.stopListening = function () {
    this.telemetryServer.removeListener('input:data', this.notify);
    this.telemetryServer.removeListener('input:chunk', this.flush);
};

RealtimeServer.prototype.notify = function (value) {
    if (!this.subscribers[value.name]) {
        return;
    }
    var payload = JSON.stringify(value);
    this.subscribers[value.name].forEach(function (c) {
        c(payload);
    });
};

RealtimeServer.prototype.subscribe = function (id, callback) {
    if (this.subCount === 0) {
        this.startListening();
    }
    if (!this.subscribers[id]) {
        this.subscribers[id] = [];
    }
    if (this.subscribers[id]) {
        this.subscribers[id].push(callback);
    }
    this.subCount += 1;
    return function () {
        var index = this.subscribers[id].indexOf(callback);
        this.subscribers[id].splice(this.subscribers[id].indexOf(id), 1);
        if (this.subscribers[id].length === 0) {
            delete this.subscribers[id];
        }
        this.subCount -= 1;
        if (this.subCount === 0) {
            this.stopListening();
        }
    }.bind(this);
};

RealtimeServer.prototype.scheduleFlush = function (callback) {
    this.toFlush.push(callback);
};

RealtimeServer.prototype.flush = function () {
    this.toFlush.forEach(function (c) {
        c();
    });
    this.toFlush = [];
};

RealtimeServer.prototype.handleConnection = function (ws) {
    var subscribed = {};
    var chunks = [];
    var flush = function () {
        if (ws.readyState === WebSocket.OPEN && chunks.length) {
            ws.send('[' + chunks.join(',') + ']')
            chunks = [];
        }
    }

    var notify = function (payload) {
        if (chunks.length === 0) {
            this.scheduleFlush(flush);
        }
        chunks.push(payload);
    }.bind(this);

    var error = function (message) {
        console.error('Websocket: ', message);
        ws.send('ERROR: ' + message);
    };

    ws.on('message', function (message) {
        var parts = message.split(' '),
            type = parts[0],
            id = parts[1];
        if (type === 'subscribe') {
            if (!subscribed[id]) {
                subscribed[id] = this.subscribe(id, notify);
            }
        } else if (type === 'unsubscribe') {
            if (!subscribed[id]) {
                error('tried to unsubscribe to point that is not subscribed to: ' + id);
                return;
            }
            subscribed[id]();
            delete subscribed[id];
        } else {
            error('Invalid message: ' + message);
        }
    }.bind(this));

    ws.on('error', function (err) {
        console.error('Websocket Error:', err);
    });

    ws.on('close', function () {
        Object.keys(subscribed).forEach(function (k) {
            subscribed[k]();
        });
    });
};

module.exports = RealtimeServer;
