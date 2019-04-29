/**
 * Basic Realtime telemetry plugin using websockets.
 */
var RealtimeTelemetryPlugin = function (options) {
    if (!options) {
        options = {}
    }
    if (!options.url) {
        console.error('MUST SPECIFY DICTIONARY URL');
    }
    if (!options.namespace) {
        options.namespace = 'example.taxonomy';
    }

    return function (openmct) {
        var socket = new WebSocket(options.url);
        var listeners = {};

        socket.onmessage = function (event) {
            var points = JSON.parse(event.data);
            points.forEach(function (point) {
                if (listeners[point.name]) {
                    listeners[point.name].forEach(function (l) {
                        l(point);
                    });
                }
            });
        };

        var provider = {
            supportsSubscribe: function (domainObject) {
                return domainObject.type === 'bson.telemetry' &&
                    domainObject.identifier.namespace === options.namespace;
            },
            subscribe: function (domainObject, callback, options) {
                if (!listeners[domainObject.telemetry.key]) {
                    listeners[domainObject.telemetry.key] = [];
                }
                if (!listeners[domainObject.telemetry.key].length) {
                    socket.send('subscribe ' + domainObject.telemetry.key);
                }
                listeners[domainObject.telemetry.key].push(callback);
                return function () {
                    listeners[domainObject.telemetry.key] =
                        listeners[domainObject.telemetry.key].filter(function (c) {
                            return c !== callback;
                        });
                    if (!listeners[domainObject.telemetry.key].length) {
                        socket.send('unsubscribe ' + domainObject.telemetry.key);
                    }
                };
            }
        };

        openmct.telemetry.addProvider(provider);
    }
};
