var express = require('express');
var channels = require('../../res/channels.json');

function ClientRouter(options) {
    var router = express.Router();

    router.get('/config.json', function (req, res) {
        return res.json(options.persistence).end();
    });
    router.get('/channels.json', function (req, res) {
        return res.json(channels).end();
    });

    router.use('/openmct', express.static(__dirname + '/../../node_modules/openmct/dist'));
    router.use('/', express.static(__dirname + '/../../client'));

    return router;
}

module.exports = ClientRouter;
