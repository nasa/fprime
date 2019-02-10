var net = require('net');
var BSON = require('bson');

/**
 * Stream Listener-- Opens a socket server and listens for a bson stream of
 * mnemonics.  Triggers events on `telemetryServer` when data is returned.
 *
 */
function BSONStreamInput(options, telemetryServer) {
    var bson = new BSON();

    var server = new net.Server();
    var connected = 0;

    server.on('connection', function (socket) {
        var pause = socket.pause.bind(socket);
        var resume = socket.resume.bind(socket);
        connected += 1;

        telemetryServer.on('history:overload', pause);
        telemetryServer.on('history:okay', resume);
        console.log(`BSONStreamInput: Received connection, total connections: ${connected}`);

        var partialData;

        socket.on('data', function (buf) {
            var loc = 0;
            var bsonOptions = {};
            var values = [];

            if (partialData) {
                buf = Buffer.concat([partialData, buf]);
                partialData = undefined;
            }
            if (buf.length < 4) {
                partialData = buf;
                return;
            }
            while (loc < buf.length) {
                var size = buf[loc] | buf[loc + 1] << 8 | buf[loc + 2] << 16 | buf[loc + 3] << 24;
                if ((size + loc) > buf.length) {
                    partialData = buf.slice(loc);
                    break;
                }
                bsonOptions['index'] = loc;
                try {
                    var value = bson.deserialize(buf, bsonOptions);
                } catch (err) {
                    console.error('BSONStreamInput: Error deserializing bson', err);
                    socket.end();
                    return;
                }

                values.push(value);
                telemetryServer.emit('input:data', value);
                loc = loc + size;
            }
            telemetryServer.emit('input:chunk', values);
        });

        socket.on('error', function (err) {
            console.error(`BSONStreamInput: Error on socket: ${err}`);
        });

        socket.on('close', function () {
            partialData = undefined;
            connected -= 1;
            console.log(`BSONStreamInput: Closed a connection, total connections: ${connected}`);
            telemetryServer.removeListener('history:overload', pause);
            telemetryServer.removeListener('history:okay', resume);
        });
    });

    server.listen(options.port, options.host, function () {
        console.log(`BSONStreamInput: Listening on: ${options.bindAddress}:${options.port}`);
    });
}

module.exports = BSONStreamInput;
