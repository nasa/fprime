/**
 * Basic historical telemetry plugin.
 */

var HistoricalTelemetryPlugin = function (options) {
    if (!options) {
        options = {}
    }
    if (!options.url) {
        console.error('MUST SPECIFY DICTIONARY URL');
    }
    if (!options.namespace) {
        options.namespace = 'example.taxonomy';
    }

    return function install (openmct) {
        var historyProvider = {
            supportsRequest: function (domainObject, roptions) {
                return domainObject.type === 'bson.telemetry' &&
                    domainObject.identifier.namespace === options.namespace &&
                    roptions.size !== 1;
            },
            request: function (domainObject, roptions) {
                var url = options.url + '/' +
                    domainObject.telemetry.key +
                    '?start=' + roptions.start +
                    '&end=' + roptions.end;

                return http.get(url)
                    .then(function (resp) {
                        return resp.data;
                    });
            }
        };

        openmct.telemetry.addProvider(historyProvider);

        var ladProvider = {
            supportsRequest: function (domainObject, roptions) {
                return domainObject.type === 'bson.telemetry' &&
                    domainObject.identifier.namespace === options.namespace &&
                    roptions.size === 1;
            },
            request: function (domainObject, roptions) {
                var url = options.url + '/latest/' +
                    domainObject.telemetry.key;

                    return http.get(url)
                        .then(function (resp) {
                            return resp.data;
                        });
            }
        }

        openmct.telemetry.addProvider(ladProvider);
    }
};
