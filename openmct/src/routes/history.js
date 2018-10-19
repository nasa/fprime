
var express = require('express');

function HistoryServer(historyClient) {
    var router = express.Router();

    router.get('/latest/:mnemonic', function (req, res) {
        historyClient.latest(req.params.mnemonic)
            .then(function (result) {
                res.json(result).end();
            });
    });

    router.get('/:mnemonic', function (req, res) {
        var mnemonic = req.params.mnemonic;
        var start = Number(req.query.start);
        var end = Number(req.query.end);

        historyClient.get(mnemonic, start, end)
            .then(function (result) {
                res.json(result).end();
            });
    });

    router.get('/', function (req, res) {
        return res.json({
            'full_history_url': `${req.baseUrl}/{mnemonic}?start={start_timestamp}&end={end_timestamp}`,
            'latest_value_url': `${req.baseUrl}/latest/{mnemonic}`,
        });
    });

    return router;
}

module.exports = HistoryServer;
