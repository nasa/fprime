/**
  * BsonAdapter.js
  *
  * Connect to the fprime tcp server port specified in config.js, deserialize
  * binary and convert to JSON, then JSON to BSON, and then send them on to the
  * openmct-telemetry-server.
  *
  * @author Aaron Doubek-Kraft; aaron.doubek-kraft@jpl.nasa.gov
  */

var net = require('net');
var BSON = require('bson');

var binaryDeserializer = require('./util/deserialize-binary');

var bson = new BSON();

/**
  * Network node which converts incoming binary data into BSON data and funnels
  * it into the input socket on the OpenMCT BSON server.
  * @constructor
  * @param {Object} config The configuration object for the system containing
  *                        information about hostnames and ports for network
  *                        resources and deployment information
  */
function BSONAdapter(config) {

    this.target = config.deployment;
    this.dictionary = require("../" + config.binaryInput.dictionaryFile);
    this.deserializer = new binaryDeserializer(this.dictionary, this.target)

    // Frequency, in seconds, with which the adapter polls a socket when
    // attempting to connect.
    this.timeout = 5;

    console.log(`Using deployment key '${this.target}' \n`)

    //configuration objects that BSON adapter will attempt to connect
    this.fprimeClient = {
        socket: new net.Socket(),
        name: "Fprime Telemetry Socket",
        port: config.binaryInput.port,
        site: config.binaryInput.bindAddress,
        successFunction: function () {
            // ThreadedTCPServer will not send packets until it recieves this command
            this.socket.write('Register GUI\n');
        }
    };

    this.openMCTTelemetryClient = {
        socket: new net.Socket(),
        name: "OpenMCT BSON Stream Socket",
        port: config.input.port,
        site: config.input.bindAddress,
        successFunction: function () {}
    };

    this.COSMOSClient = {
        socket: new net.Socket(),
        name: "COSMOS Router Socket",
        port: config.COSMOSInput.port,
        site: config.COSMOSInput.bindAddress,
        successFunction: function () {}
    };

}

/**
  * Using configuration in constructor, attempt to connect to each socket and
  * setup listeners for incoming data
  */
BSONAdapter.prototype.run = function () {

    this.setupConnections()

    // event handler for recieving packets
    this.fprimeClient.socket.on('data', (data) => {
        var dataAsJSON = this.deserializer.deserialize(data);

        dataAsJSON.forEach( (datum) => {
            var datumAsBSON = bson.serialize(datum);
            this.openMCTTelemetryClient.socket.write(datumAsBSON);
        });
    });

    this.COSMOSClient.socket.on('data', (data) => {
        var dataAsJSON = this.deserializer.deserialize(data);

        dataAsJSON.forEach( (datum) => {
            var datumAsBSON = bson.serialize(datum);
            this.openMCTTelemetryClient.socket.write(datumAsBSON);
        });
    });

}

/**
  * Connect each socket, and handle connection failures
  */
BSONAdapter.prototype.setupConnections = function () {

    this.connectSocket(this.fprimeClient).catch( (reject) => {
        this.printRejectNotice(reject, this.fprimeClient);
        this.handleConnectionError(reject, this.fprimeClient);
    });

    this.connectSocket(this.openMCTTelemetryClient).catch( (reject) => {
        this.printRejectNotice(reject, this.openMCTTelemetryClient);
        this.handleConnectionError(reject, this.openMCTTelemetryClient);
    });

    this.connectSocket(this.COSMOSClient).catch( (reject) => {
       this.printRejectNotice(reject, this.COSMOSClient);
       this.handleConnectionError(reject, this.COSMOSClient);
    });

}

/**
  * Given the reject message and the client object which failed to connect,
  * print an error message
  * @param {string} reject The error message
  * @param {Object} clientObj The object representing the client which generated
  *                           the error on connection
  */
BSONAdapter.prototype.printRejectNotice = function (reject, clientObj) {
    console.log(`${clientObj.name}: Connection Error: ${reject}`);
    console.log(`${clientObj.name}: Attempting to reconnect every ${this.timeout} seconds`);
}

/**
  * Function invoked when a socket fails to connect. This will continously
  * poll the socket until it succeeds.
  * @param {string} err The error object thrown when the connection failed
  * @param {Object} clientObj The client object which failed to connect
  */
BSONAdapter.prototype.handleConnectionError = function (err, clientObj) {
    setTimeout( () => {
        this.connectSocket(clientObj).catch( (reject) => {
            this.handleConnectionError(reject, clientObj);
        });
    }, this.timeout * 1000);

}

/**
  * Given the configuration for a client object, connect it and return a Promise
  * that resolves if the connection was successful, or rejects with the error message
  * if it failed
  * @param {Object} clientObj The socket configuration object
  * @return {Promise} A promise that resolves if the socket connection succeeds
  */
BSONAdapter.prototype.connectSocket = function (clientObj) {

    var port = clientObj.port,
        site = clientObj.site,
        client = clientObj.socket;

    return new Promise( (resolve, reject) => {
        client.connect(port, site, () => {
            console.log(`BSON Adapter: Established connection to ${clientObj.name} on ${site}:${port}`);
            clientObj.successFunction()
            resolve(true);
        });

        client.on('error', (err) => {
            //clean up event listeners to prevent multiple success or failure messages
            client.removeAllListeners('error');
            client.removeAllListeners('connect')
            reject(err.message)
        });
    });

}

module.exports = BSONAdapter;
