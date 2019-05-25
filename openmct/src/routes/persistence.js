/**
  * persistence.js
  *
  * Defines HTTP endpoints for interacting with an embedded LevelDB persistence database
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  */

const express = require('express');
const level = require('level');
const bodyParser = require('body-parser');
const fs = require('fs');

/**
  * Persistence backend for OpenMCT, implemented as a simple Express
  * wrapper around a LevelDB instance.
  * @constructor
  * @param {String} key The key for the deployment which will be using this backend.
  */
function PersistenceServer(key) {

    if (!fs.existsSync('persistdb')) {
        //make sure containing folder for db config exists
        fs.mkdirSync('persistdb');
    };

    var deployment = key || 'defaultdb';
    var router = express.Router();

    // Create leveldb instance for this deployment. If this directory exists,
    // loads the configuration from the files stored on disk.
    var db = level(`./persistdb/${deployment}`);

    router.use(bodyParser.json()); //enable JSON message parsing middleware

    router.get('/', function (req, res) {
        // For the index route, just list the available routes
        return res.json({
            get_object: `${req.baseUrl}/objects/:id`,
            put_object: `${req.baseUrl}/objects/:id`,
            delete_object: `${req.baseUrl}/objects/:id`,
            list_deployments: `${req.baseUrl}/list/deployments`,
            list_objects: `${req.baseUrl}/list/objects`
        });
    });

    router.get('/objects/:id', function (req, res) {
        // get the configuration of the object with :id
        let response = {},
            status = '';

        db.get(req.params.id, (err, val) => {
            if (err) {
                status = 404;
                response.status = 'error';
                response.reason = err.toString();
            } else {
                status = 200;
                response.status = 'ok';
                response.value = val;
            }
            return res.status(status).json(response);
        });
    });

    router.get('/list/deployments', function (req, res) {
        // list all deployments for which object configuration exists, read from
        // the contents of the 'persistdb' directory
        let response = {},
            status = '';

        fs.readdir('./persistdb', (err, files) => {
            if (err) {
                status = 500;
                response.status = 'error';
                response.reason = 'Internal Server Error'
            } else {
                status = 200;
                response.status = 'ok';
                response.value = JSON.stringify(files);
            }
            return res.status(status).json(response);
        });
    });

    router.get('/list/objects', function (req, res) {
        // List the keys for all objects stored in the current leveldb instance
        let response = {},
            objectKeys = [],
            keyStream = db.createKeyStream();

        keyStream.on('data', (data) => {
            objectKeys.push(data);
        });

        keyStream.on('end', () => {
            response.status = 'ok';
            response.value = JSON.stringify(objectKeys);
            return res.status(200).json(response);
        });

        keyStream.on('error', (err) => {
            response.status = 'error';
            response.reason = 'Internal server error';
            return res.status(500).json(response);
        });
    });

    router.put('/objects/:id', function (req, res) {
        // Store the configuration for the object with :id in the database.
        // Creates the key if it doesn't exist, or replace it if it does.
        let response = {},
            status = '',
            value = JSON.stringify(req.body);

        db.put(req.params.id, value, (err, val) => {
            if (err) {
                status = 400;
                response.status = 'error';
                response.reason = err.toString();
            } else {
                status = 201;
                response.status = 'ok';
                response.value = `Succesfully updated ${req.params.id} in database ${deployment}`
            }
            return res.status(status).json(response);
        });

    });

    router.delete('/objects/:id', function (req, res) {
        // Delete the configuration for the object with :id from the database
        let response = {};
        status = '';

        db.del(req.params.id, (err, val) => {
            if (err) {
                status = 400;
                response.status = error;
                response.reason = err.toString();
            } else {
                status = 200;
                response.status = 'ok';
                response.value = `Successfully deleted ${req.params.id} in database ${deployment}`
            }
            return res.status(status).json(response);
        });
    });

    return router;
}

module.exports = PersistenceServer;
