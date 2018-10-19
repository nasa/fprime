var fs = require('fs');
var net = require('net');
var events = require('events');
const { Readable, Transform } = require('stream');

var BSON = require('bson');

var bson = new BSON();

/**
 * Transform stream, pipe data thru to deserialize BSON.
 */
class BSONStreamDecoder extends Transform {
    constructor () {
        super({objectMode: true});
    }
    _transform (buf, encoding, callback) {
        var loc = 0;
        var bsonOptions = {};
        var values = [];
        if (this.partialData) {
            buf = Buffer.concat([this.partialData, buf]);
            this.partialData = undefined;
        }
        if (buf.length < 4) {
            this.partialData = buf;
            callback();
            return;
        }
        while (loc < buf.length) {
            var size = buf[loc] | buf[loc + 1] << 8 | buf[loc + 2] << 16 | buf[loc + 3] << 24;
            if ((size + loc) > buf.length) {
                this.partialData = buf.slice(loc);
                break;
            }
            bsonOptions['index'] = loc;
            var value = bson.deserialize(buf, bsonOptions);
            values.push(value);
            loc = loc + size;
        }
        callback(null, values);
    }
}

/** Timestamp rewriter, rebases timestamps onto supplied epoch. */
class TimestampTransformer extends Transform {
    constructor (toEpoch) {
        super({objectMode: true});
        this.toEpoch = toEpoch;
    }
    _transform (values, encoding, callback) {
        if (!this.fromEpoch) {
            this.fromEpoch = values[0].timestamp.getTime();
        }

        values.forEach((value) => {
            if (value.raw_type === 76 && value.raw_value.getTime() === value.timestamp.getTime()) {
                value.raw_value = value.timestamp = new Date(this.toEpoch + value.timestamp.getTime() - this.fromEpoch);
            } else {
                value.timestamp = new Date(this.toEpoch + value.timestamp.getTime() - this.fromEpoch);
            }
        });

        callback(null, values);
    }
}

/**
 * Opens a BSON file, rewrites dates, and infinitely loops the file.
 */
function InfiniteStream(filename) {
    var emitter = new events();
    var toEpoch = Date.now();
    var currentStream;
    var lastValue;
    var fileStream;

    function onData(data) {
        lastValue = data[data.length - 1];
        emitter.emit('data', data);
    }

    function onEnd() {
        currentStream.removeListener('data', onData);
        currentStream.removeListener('end', onEnd);
        console.log('looping!');
        toEpoch = lastValue.timestamp.getTime();
        startStream();
    }

    function startStream() {
        fileStream = fs.createReadStream(filename);
        var timestampTransform = new TimestampTransformer(toEpoch);
        var bsonDecoder = new BSONStreamDecoder();
        bsonDecoder.pipe(timestampTransform);
        fileStream.pipe(bsonDecoder);
        currentStream = timestampTransform;
        currentStream.on('data', onData);
        currentStream.on('end', onEnd);
    }

    emitter.isPaused = function () {
        return fileStream.isPaused();
    }

    emitter.pause = function () {
        return fileStream.pause();
    }

    emitter.resume = function () {
        return fileStream.resume();
    }

    startStream();

    return emitter;
};

function StatsReporter(input, output) {
    var startDate = new Date(Date.now()).toISOString();
    var lastReport = Date.now();
    var lastBytes = 0;
    var valueCount = 0;
    var lastValue;
    input.on('data', function (values) {
        valueCount += values.length;
        lastValue = values[values.length - 1];
    });

    function report() {
        var now = Date.now();
        var elapsedSeconds = (now - lastReport) / 1000;
        var bytesSent = output.getBytesWritten();
        var rate = (bytesSent - lastBytes) / elapsedSeconds;
        lastBytes = bytesSent;
        lastReport = now;
        console.log(`${now} ValuesSent: ${(valueCount/1000000).toFixed(2)} million, BytesWritten: ${(bytesSent/1024/1024).toFixed(2)} MiB, BufferSize: ${(output.getBufferSize()/1024/1024).toFixed(2)} MiB, Rate: ${(rate/1024/1024).toFixed(2)} MiB/s, lastTimestamp: ${lastValue.timestamp.toISOString()}`);
    }

    setInterval(report, 0.5 * 60 * 1000);
    setTimeout(report, 5000);
}

function PersistentSocketOutput(port, host, input) {
    var backlog = [];
    var byteCount = 0;
    var socket;

    function queue(values) {
        backlog.push(values);
    }

    function send(values) {
        socket.write(Buffer.concat(values.map(bson.serialize, bson)));
    }

    function disableQueue() {
        input.removeListener('data', queue);
        input.on('data', send);
        handlerFunction = send;
        backlog.forEach(send);
        backlog = [];
    }

    function handleError(error) {
        console.log('Error on socket:', error);
    }

    function handleClose() {
        console.log('Socket closing, restarting socket');
        byteCount += socket.bytesWritten;
        input.removeListener('data', send);
        input.removeListener('data', queue);
        socket.removeListener('error', handleError);
        socket.removeListener('close', handleClose);
        setTimeout(openSocket, 500);
    }

    function openSocket() {
        socket = net.connect(port, host, disableQueue);
        input.on('data', queue);
        socket.on('error', handleError);
        socket.on('close', handleClose);
    }

    setInterval(function () {
        if (socket.bufferSize >= 64 * 1024 * 1024) {
            if (!input.isPaused()) {
                input.pause()
            }
        } else if (input.isPaused()) {
            input.resume();
        }
    }, 250);

    openSocket();

    return {
        getBytesWritten: function () {
            return byteCount + socket.bytesWritten;
        },
        getBufferSize: function () {
            return socket.bufferSize
        }
    }
}

var program = require('commander');

var input_file;

program
    .description(
`Reads a BSON file in an infinite loop, streaming it to the specified host
and port.  Dates are modified to produce a continual loop of telemetry starting
from the current system clock and proceeding based on the values in the file.`
    )
    .option('-p, --port [port]', 'Port to connect to. default: 12345', 12345)
    .option('-h, --host [host]', 'Host to connect to. default: localhost', 'localhost')
    .arguments('<input_file>')
    .action(function (infile) {
        input_file = infile;
    })
    .parse(process.argv);

if (input_file) {
    var input = InfiniteStream(input_file);
    var output = PersistentSocketOutput(program.port, program.host, input);
    var reporter = StatsReporter(input, output);
}

