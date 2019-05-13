module.exports = {
    deployment: 'Ref',
    pointsFileTemplate: 'res/${deployment}Points.json',
    packetsFileTemplate: 'res/${deployment}Packets.json',
    dictionaryTemplate: 'res/${deployment}Dictionary.json',
    /**
     * client
     * Configuration for web client and related http telemetry endpoints.
     */
    client: {
        /**
         * host
         * Hostname (interface) to serve Client on.
         */
        host: 'localhost',

        /**
         * port
         * Port to serve Client on.
         */
        port: 8000,


        /**
         * persistence options for OpenMCT Persistence
         */
        persistence: {
            /**
             * type - either 'localstorage', 'couchdb', 'elasticsearch', or 'embedded'
             */
            type: 'embedded',

            /**
             * url - for elasticsearch or couchdb, url to service.  Relative URLs
             * allowed.  In the case of couchdb, url should include database.
             *
             * Example couch url: http://localhost:5984/openmct
             * Example elastic url: http://localhost:9200
             */
            url: 'http://localhost:5984/openmct'
        }
    },

    /**
     * dictionary
     * Options for dictionary handling:
     */
    dictionary: {
        /**
         * pointsFile
         * Path to a file where points will be loaded from at start up.
         * POSTing "/dictionary/save" will write the current point definitions
         * to this file.
         */
        pointsFile: 'res/RefPoints.json',

        /**
         * packetsFile
         * Path to a file which defines the points included in each packet,
         * loaded at startup.  POSTing "/dictionary/save" will write the current
         * point definitions to this file.
         */
        packetsFile: 'res/RefPackets.json',

        /**
         * packetSeparator
         * a string used to split point names into packets, which are then
         * grouped in the user interface.
         */
        packetSeparator: '.',

        /**
         * packetPrefixes
         * an array of point prefixes to group points by.  If a point doesn't
         * match a defined prefix, then the system will fallback to using
         * the packetSeparator.
         */
        packetPrefixes: [
        ]
    },

    /**
     * history
     * Configuration options for the history server.
     */
    history: {

        /**
         * type
         * Type of history server to use.  Options include:
         *
         * `level`: Use LevelDB for storage of telemetry.  LevelDB is an
         *          embedded database, so no external services are necessary.
         *          LevelDB performance is tied to HDD performance, it works
         *          best with SSDs.
         *
         *
         *
         * `redis`: Use Redis for storage of telemetry.  This will require
         *          deployment and configuration of a redis database.
         *
         */
        type: 'level',

        /**
         * databasePath - LevelDB only
         * the path to a directory where telemetry data will be stored.
         */
        databasePath: 'telemdb',

        /**
         * daysToKeep - LevelDB only
         * Number of days of data to keep in history. Specify `0`to disable
         * pruning of telemetry data.  Note: if you want a minimum of 24hrs
         * in history at any time, then you must keep at least 2 days.
         */
        daysToKeep: 2,

        /**
         * pruneInterval - LevelDB only
         * Interval at which to check for and prune history, in milliseconds.
         * default is once an hour.
         */
        pruneInterval: 1 * 60 * 60 * 1000,

        /**
         * host - Redis only
         */
        host: '127.0.0.1',

        /**
         * port - Redis only
         */
        port: '6379'
    },

    /**
     * input
     * Settings for input stream.
     */
    input: {

        /**
         * type
         * Type of input stream.  Currently only supports `bson`.
         */
        type: 'bson',

        /**
         * bindAddress
         * Network address (interface) to bind to.
         */
        bindAddress: '127.0.0.1',

        /**
         * port
         * TCP port to listen on.
         */
        port: 12345
    },

      /*
       * binaryInput
       * Settings for fprime binary input stream
       */
     binaryInput: {

       /**
        * bindAddress
        * Network address (interface) to bind to.
        */
        bindAddress: '127.0.0.1',

        /**
         * port
         * TCP port to listen on.
         */
        port: 50000,

        /**
         * dictionaryFile: where the BSON deserializer will look for the dictionary
         */
        dictionaryFile: 'res/RefDictionary.json'
     },

     COSMOSInput: {
        bindAddress: '127.0.0.1',
        port: 5001
     }
}
