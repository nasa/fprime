/**
 * loader.js:
 *
 * This file is used to load F prime data from the REST endpoint. This allows for a central access-point for these types
 * of data. In addition, it can kick of polling in order to keep aware of the latest updates in the REST layer.
 *
 * It typically has two types  of data:
 *
 * 1. static data (i.e. dictionaries) that only need to be loaded once on startup
 * 2. dynamic data that will be polled to continuously updated
 *
 * @author mstarch
 */
import {config} from "./config.js";

/**
 * Loader:
 *
 * Loader that is used to pull in data from the REST API. This loader is intended to be a singleton and thus will be
 * instantiated and exported once.
 */
class Loader {
    /**
     * Sets up the list of endpoints, and preps for the initial loading of the dictionaries.
     */
    constructor() {
        this.session = new Date().getTime().toString();
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
                "last": null,
                "shutdown": true
            },
            "events": {
                "url": "/events",
                "last": null,
                "shutdown": true
            },
            "channels": {
                "url": "/channels",
                "last": null,
                "shutdown": true
            },
            "logdata": {
                "url": "/logdata",
                "last": null
            },
            "upfiles": {
                "url": "/upload/files",
                "last": null
            },
            "downfiles": {
                "url": "/download/files",
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
            for (let endpoint in _self.endpoints) {
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
     * Load a given endpoint with a promise for when this endpoint returns its data. This wraps the basic AJAX call for
     * the user such that they only need to call load.
     * @param endpoint: url to call on the backend server. e.g. /download/files/abc
     * @param method: HTTP method to use to communicate with server. Default: "GET"
     * @param data: data to send.  Only useful if method != "GET". Default: no data
     * @param jsonify: jsonify the data. Default: true.
     */
    load(endpoint, method, data, jsonify) {
        let _self = this;
        // Default method argument to "GET"
        if (typeof(method) === "undefined") {
            method = "GET";
        }
        // JSONify data if supplied and jsonified data needed
        if (typeof(data) !== "undefined" && (typeof(jsonify) === "undefined" || jsonify)) {
            data = JSON.stringify(data);
        }
        // Kick-back a promise for this load
        return new Promise(function (resolve, reject) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                // Parse as JSON or send back raw error
                if (this.readyState == 4 && this.status == 200) {
                    let dataObj = JSON.parse(this.responseText);
                    resolve(dataObj);
                } else if(this.readyState == 4) {
                    reject(this.responseText);
                }
            };
            let random = new Date().getTime().toString();
            xhttp.open(method, endpoint + "?_no_cache=" + random + "&session=" + _self.session, method != "DELETE");
            if (typeof(data) === "undefined") {
                xhttp.send();
            } else if (typeof(jsonify) === "undefined" || jsonify) {
                xhttp.setRequestHeader("Content-Type", "application/json")
                xhttp.send(data);
            } else {
                xhttp.send(data);
            }
        });
    }

    /**
     * Register a polling function to receive updates and post updates to the callback function. This takes an endpoint
     * name from the setup list of endpoints known by this Loader, and a callback to return data to on the clock.
     * @param endpoint: endpoint to load
     * @param callback: callback to return resulting data to.
     */
    registerPoller(endpoint, callback) {
        let _self = this;
        let inProgress = false; // Used to prevent re-entrant requests
        let handler = function()
        {
            // Don't request if already requesting
            if (!inProgress) {
                inProgress = true;
                _self.load(_self.endpoints[endpoint]["url"]).then(
                    function(data) {
                        inProgress = false;
                        callback(data);
                    }
                ).catch(function(error) {
                    inProgress = false;
                    console.error("[ERROR] Polling " + _self.endpoints[endpoint]["url"] + " failed with: " + error);
                });
            }
        };
        // Clear old intervals
        if ("interval" in this.endpoints[endpoint]) {
            clearInterval(this.endpoints[endpoint]["interval"]);
        }
        this.endpoints[endpoint]["interval"] = setInterval(handler, config["dataPollIntervalMs"]);
        handler();
    }

    /**
     * Destroys the session tracking items. Best-effort shutdown attempt.
     */
    destroy() {
        for (let endpoint in this.endpoints) {
            endpoint = this.endpoints[endpoint];
            if (typeof(endpoint["shutdown"]) !== "undefined" && endpoint["shutdown"]) {
                this.load(endpoint["url"], "DELETE");
            }
        }
    }
};
export let _loader = new Loader();

