var express = require('express');


function DictionaryRouter(dictionary) {

    var router = express.Router();

    router.get('/mapping', function (req, res) {
        return res.json(dictionary.getPointMapping());
    });

    router.get('/packets', function (req, res) {
        return res.json({
            packets: dictionary.getPacketIds().sort()
        });
    });

    router.get('/packets/:id', function (req, res) {
        var packet = dictionary.getPacket(req.params.id);
        return res.json({
            name: packet.name,
            pointIds: Object.keys(packet.points).sort(),
            points: packet.points
        });
    });

    router.post('/save', function (req, res) {
        dictionary.save();
        res.status(200).end();
    });

    router.post('/load', function (req, res) {
        dictionary.load();
        res.status(200).end();
    });

    router.get('/', function (req, res) {
        return res.json({
            'mapping_url': `${req.baseUrl}/mapping`,
            'packets_url': `${req.baseUrl}/packets`,
            'packet_url': `${req.baseUrl}/packets/{id}`,
            'save_url': `${req.baseUrl}/save`,
            'load_url': `${req.baseUrl}/load`,
        });
    });

    return router;
};

module.exports = DictionaryRouter;
