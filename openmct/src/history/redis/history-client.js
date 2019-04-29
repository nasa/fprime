var redis = require('redis');

var packValue = require('../../util/pack-value');

/**
 * Redis backed history client.  Supports writing values to redis, and reading
 * values from redis.
 */
function RedisHistoryClient(options, telemetryServer) {
    if (!options.host) {
        options.host = '127.0.0.1';
    }
    if (!options.port) {
        options.port = 6379;
    }
    console.log(`Redis: History will be stored in redis server at ${options.host}:${options.port}`);
    console.log(`Redis: History expiration is not configured.`);

    this._client = redis.createClient({
        host: options.host,
        port: options.port
    });
    this._telemetryServer = telemetryServer;

    this.writeChunk = this.writeChunk.bind(this);
    this._batchComplete = this._batchComplete.bind(this);
    this._batchSize = 200;
    this._queue = [];
    this._pending = 0;
    this._overload = false;
    this._telemetryServer.on('input:chunk', this.writeChunk);
}

/**
 * Get all values for a given mnemonic between start and end dates.
 */
RedisHistoryClient.prototype.get = function (mnemonic, start, end) {
    return new Promise(function (resolve, reject) {
        this._client.zrangebyscore([mnemonic, start, end], function (err, data) {
            if (err) {
                return reject(err);
            }
            resolve(data.map(function (val) {
                return packValue.unpackString(val, mnemonic)
            }));
        });
    }.bind(this));
};

/**
 * Get latest value for a given mnemonic.
 */
RedisHistoryClient.prototype.latest = function (mnemonic) {
    return new Promise(function (resolve, reject) {
        this._client.zrevrange([mnemonic, 0, 0], function (err, data) {
            if (err) {
                return reject(err);
            }
            resolve(data.map(function (val) {
                return packValue.unpackString(val, mnemonic)
            }));
        });
    }.bind(this));
};
/**
 * Write chunk of values to history store.
 */
RedisHistoryClient.prototype.writeChunk = function (values) {
    for (var i = 0, len = values.length; i < values.length; i++) {
        this._queue.push(['zadd', values[i].name, values[i].timestamp.getTime() + '', packValue.packString(values[i])]);
    };
    if (this._queue.length >= this._batchSize) {
        this._work();
    }
};

/**
 * Ship the pending queue to redis in a batch.
 * @private
 */
RedisHistoryClient.prototype._work = function () {
    if (this._queue.length === 0) {
        return;
    }
    var commands = this._queue;
    this._queue = [];
    var batch = this._client.batch(commands);
    batch.exec(this._batchComplete);
    this._pending++;
    if (this._pending >= 100 && !this._overload) {
        this._overload = true;
        this._telemetryServer.emit('history:overload');
    }
};

/**
 * Track the completion of a batch.
 * @private
 */
RedisHistoryClient.prototype._batchComplete = function () {
    this._pending--;
    if (this._pending <= 100 && this._overload) {
        this._overload = false;
        this._telemetryServer.emit('history:okay');
    }
};

module.exports = RedisHistoryClient;
