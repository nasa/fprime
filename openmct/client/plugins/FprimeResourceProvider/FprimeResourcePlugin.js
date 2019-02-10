/**
  * FprimeResourcePlugin.js
  *
  * Creates additional predefined resources for the Fprime OpenMCT server:
  * currently, a LAD table and an overlay plot with composition determined
  * by the arguments passed in at the command line
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  */

var FprimeResourcePlugin = function (options) {

    var packetNames = undefined,
        channelNames = undefined,
        evrNames = undefined;

    var overlayProvider = {
        get: function (identifier) {

            // Invoked when this object is loaded, and provides configuration for
            // the objects defined in the composition provider
            if (identifier.key === 'overlay-plot') {
                // configuration for overlay plot
                return http.get('channels.json').then( (result) => {
                    let channels = result.data.channels;
                    let channelIdentifiers = channels.map( (channel) => {
                        return {
                            identifier: {
                                key: channel,
                                namespace: options.bsonNamespace
                            }
                        }
                    });
                    return {
                        identifier: identifier,
                        name: 'Channel Overlay Plot',
                        type: 'telemetry.plot.overlay',
                        location: options.namespace + ':resource-root',
                        configuration: {
                            series: channelIdentifiers,
                            yAxis: {},
                            xAxis: {}
                        }
                    }
                });
            }

            if (identifier.key === 'resource-root') {
                // configuration for Resources folder
                return Promise.resolve({
                    identifier: identifier,
                    name: "Resources",
                    type: "folder",
                    location: 'ROOT'
                });
            }

            if (identifier.key === 'lad-table') {
                // configuration for Latest Value Table
                return Promise.resolve({
                    identifier: identifier,
                    name: 'Latest Value Table',
                    type: 'lad-table',
                    location: options.namespace + ':resource-root'
                });
            }

            if (identifier.key === 'event-log') {
                // configuration for event log
                return Promise.resolve({
                    identifier: identifier,
                    name: 'Event Log',
                    type: 'table',
                    location: options.namespace + ':resource-root'
                });
            }
        }
    }

    var compositionProvider = {
        appliesTo: function (domainObject) {
          // Returns true if this composition provider applies to the given domain objects
          return domainObject.identifier.namespace === options.namespace &&
                 (domainObject.type === 'folder' ||
                  domainObject.type === 'telemetry.plot.overlay' ||
                  domainObject.type === 'lad-table' ||
                  domainObject.type === 'table');
        },
        load: function (domainObject) {
            // returns the composition of the domain object passed in as argument
            if (domainObject.type === 'folder') {
                // containg folder
                return Promise.resolve([{
                    namespace: options.namespace,
                    key: 'overlay-plot'
                },{
                    namespace: options.namespace,
                    key: 'lad-table'
                },{
                    namespace: options.namespace,
                    key: 'event-log'
                }]);
            }

            if (domainObject.type === 'telemetry.plot.overlay') {
                // predefined overlay plot
                return http.get('channels.json').then( (result) => {
                    let channels = result.data.channels;
                    return channels.map( (channel) => {
                        return {
                            key: channel,
                            namespace: options.bsonNamespace
                        }
                    })
                });
            }

            if (domainObject.type === 'lad-table') {
                // latest value table populated with channels
                return getChannels().then(function (channels) {
                    tableComp = channels.map( (pointKey) => {
                        return {
                            namespace: options.bsonNamespace,
                            key: pointKey
                        }
                    });
                    return tableComp;
                });
            }

            if (domainObject.type === 'table') {
                // log populated with events
                return getEvents().then(function (events) {
                    tableComp = events.map( (pointKey) => {
                        return {
                            namespace: options.bsonNamespace,
                            key: pointKey
                        }
                    });
                    return tableComp;
                });
            };
        }
    }

    return function install (openmct) {
        // function passed to openmct to install the plugin
        openmct.objects.addRoot({
            namespace: options.namespace,
            key: 'resource-root'
        })
        openmct.objects.addProvider(options.namespace, overlayProvider);
        openmct.composition.addProvider(compositionProvider);
    }

    /**
      * Gets the list of channels defined by the BSON server. Makes HTTP requests
      * and caches the result the first time, and returns the cached result on
      * subsequent calls
      * @private
      * @return {String[]} A list of channel names
      */
    function getChannels() {
        return getPacketNames().then(function (packetNames) {
            if (!channelNames) {
                var packetPromises = []
                packetNames.forEach(function (packetKey) {
                    if (packetKey.includes("Channels")) {
                        packetPromises.push(http.get('dictionary/packets/' + packetKey))
                    }
                });
                return Promise.all(packetPromises).then(function (results) {
                    channelNames = []
                    results.forEach(function (result) {
                        channelNames = channelNames.concat(result.data.pointIds);
                    });
                    return channelNames;
                });
            } else {
                return channelNames;
            }
        });
    }

    /**
      * Gets the list of events defined by the BSON server. Makes HTTP requests
      * and caches the result the first time, and returns the cached result on
      * subsequent calls
      * @private
      * @return {String[]} A list of event names
      */
    function getEvents() {
        return getPacketNames().then(function (packetNames) {
            if (!evrNames) {
                var packetPromises = []
                packetNames.forEach(function (packetKey) {
                    if (packetKey.includes("EVR")) {
                        packetPromises.push(http.get('dictionary/packets/' + packetKey))
                    }
                });
                return Promise.all(packetPromises).then(function (results) {
                    evrNames = []
                    results.forEach(function (result) {
                        evrNames = evrNames.concat(result.data.pointIds);
                    });
                    return evrNames;
                });
            } else {
                return evrNames;
            }
        });
    }

    /**
      * Gets the list of packets defined by the BSON server. Makes HTTP requests
      * and caches the result the first time, and returns the cached result on
      * subsequent calls
      * @private
      * @return {String[]} A list of packet names
      */
    function getPacketNames () {
        if (!packetNames) {
            return http.get('/dictionary/packets').then(function (result) {
                packetNames = result.data.packets
                return packetNames
            });
        } else {
            return Promise.resolve(packetNames)
        }
    }

}
