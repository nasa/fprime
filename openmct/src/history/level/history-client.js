var packValue = require('../../util/pack-value');

function HistoryClient(databaseManager) {
    this.databaseManager = databaseManager;
}

HistoryClient.prototype.get = function (mnemonic, start, end) {
    var startKey = mnemonic + ':' + start;
    var endKey = mnemonic + ':' + end;
    var startDate = new Date(start);
    var endDate = new Date(end);

    return this.databaseManager.getMultiDBWrapper(startDate, endDate)
        .read({
            gte: startKey,
            lte: endKey
        })
        .then(function (results) {
            return results.map(function (data) {
                return packValue.unpackBuffer(data.value, mnemonic);
            });
        });
};

HistoryClient.prototype.latest = function (mnemonic) {
    return this.databaseManager.getMultiDBWrapper()
        .read({
            lte: mnemonic + ':9',
            reverse: true,
            limit: 1
        })
        .then(function (results) {
            return results.filter(function (data) {
                return data.key.indexOf(mnemonic) === 0;
            }).map(function (data) {
                return packValue.unpackBuffer(data.value, mnemonic);
            }).slice(-1);
        });
}

module.exports = HistoryClient;
