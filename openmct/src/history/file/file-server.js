var express = require('express');
var msgpack = require('msgpack');
var packValue = require('../../util/pack-value');
var binaryString = require('../../util/binary-string');
var fs = require('fs');

var buckets = require('./buckets');

function readBucket(fileName) {
    try {
        var data = binaryString.toBuffer(fs.readFileSync(fileName, 'utf8'));
    } catch (e) {
        // Generally from a "file not found".
        return [];
    }
    var start = 0;
    var len = 1
    var results = [];
    var res;
    while (len < data.length) {
        res = msgpack.unpack(data.slice(start, len));
        if (res) {
            results.push(packValue.unpackArray(res));
            start = len;
        }
        len++;
    }
    return results;
}

function HistoryServer(options) {
    if (!options) {
        options = {};
    }
    if (!options.port) {
        options.port = 12349;
    }
    if (!options.host) {
        options.host = '127.0.0.1'
    }

    var app = express();

    app.use(function (req, res, next) {
        res.set('Access-Control-Allow-Origin', '*');
        next();
    });

    app.get('/:mnemonic', function (req, res) {
        var mnemonic = req.params.mnemonic;
        var start = +req.query.start;
        var end = +req.query.end;

        var range = buckets.getBucketRange(mnemonic, new Date(start), new Date(end));
        var data = [].concat.apply([], range.map(function (bn) {
            return readBucket('file-data/' + bn);
        }));
        res.json(data).end();
    });

    app.listen(options.port, options.host, function () {
        console.log('FileHistoryServer: running at http://' + options.host + ':' + options.port);
    });
}

module.exports = HistoryServer;
