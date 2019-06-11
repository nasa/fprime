/**
 * loader.js:
 *
 * This file is used to load F prime data from the REST endpoint. This allows for a central access-point for these types
 * of data. In addition, it can kick of polling in order to keep aware of the latest updates in the REST layer.
 *
 * @author mstarch
 */
export class Loader {
    /**
     * Sets up the list of endpoints, and preps for the initial loading of the dictionaries.
     */
    constructor() {
        this.endpoints = {
            // Dictionary endpoints
            "command-dict": {
                "url": "/dictionary/commands",
                "startup": true
            },
            "event-dict":{
                "url": "/dictionary/events",
                "startup": true
            },
            "channel-dict": {
                "url": "/dictionary/channels",
                "startup": true
            },
            // Data endpoints
            "commands": {
                "url": "/commands",
                "last": null
            },
            "events": {
                "url": "/events",
                "last": null
            },
            "channels": {
                "url": "/channels",
                "last": null
            }
        };
    }
    /**
     * Sets up the loader by issuing the initial requests for the dictionary endpoints. Will "finish" when all the dicts
     * have been successfully loaded. This is based on a Promise architecture, so the user is expected to call .then()
     * and .catch() functions on it.
     */
    setup() {
        var _self = this;
        // Return a promise for when this is fully setup
        return new Promise(function(resolve, reject) {
            // Attempt to load each endpoint tracking number of pending loads
            var pending = 0;
            for (var endpoint in _self.endpoints) {
                endpoint = _self.endpoints[endpoint];
                // Send out request if not loaded, and update the pending count
                if (endpoint["startup"] == true && typeof(endpoint["data"]) === "undefined") {
                    pending = pending + 1;
                    _self.load(endpoint["url"]).then(
                        // Data successfully returned, lower pending count and set it
                        function(data) {
                            pending = pending - 1;
                            endpoint["data"] = data;
                            // When there are no pending items, then resolve the promise
                            if (pending == 0) {
                                resolve();
                            }
                        }
                    ).catch(reject);
                }
            }
        });
    }

    /**
     * Load a given endpoint with a promise for when this endpoint returns its data.
     */
    load(endpoint, method, data) {
        // Default method argument to "GET"
        if (typeof(method) === "undefined") {
            method = "GET";
        }
        // Default the data argument to null
        if (typeof(data) === "undefined") {
            data = "";
        }
        // Kick-back a promise for this load
        return new Promise(function (resolve, reject) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                // Parse as JSON or send back raw error
                if (this.readyState == 4 && this.status == 200) {
                    resolve(JSON.parse(this.responseText).content);
                } else if(this.readyState == 4) {
                    reject(this.responseText);
                }
            };
            xhttp.open(method, endpoint, true);
            xhttp.setRequestHeader("Content-Type", "application/json")
            xhttp.send(JSON.stringify(data));
        });
    }
 }