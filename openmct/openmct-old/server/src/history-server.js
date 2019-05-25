// History Server

var express = require('express');
var fs = require('fs');

function HistoryServer() {
  server = express();

  server.use(function (req, res, next) {
    res.set('Access-Control-Allow-Origin', '*');
    next();
  });

  server.get('/telemetry/:pointId', function (req, res) {
    var log = fs.readFileSync('server/logs/telem-log.json');
    var start = +req.query.start;
    var end = +req.query.end;
    var ids = req.params.pointId.split(',');

    var response = [];
    ids.forEach(function (id) {
      response.push(log[id].filter(function (p) {
        return p['timestamp'] > start && p['timestamp'] < end;
      }));
    });
    res.status(200).json(response).end();
  });
}

module.exports = HistoryServer;

