/**
 * Basic historical telemetry plugin. Local storage
 */

function HistoricalTelemetryPlugin(target) {
  let targetKey = target.toLowerCase();
  return function install (openmct) {
    var provider = {
      supportsRequest: function (domainObject) {
        return domainObject.identifier.namespace === targetKey + '.taxonomy';
      },
      request: function (domainObject, options) {
        // Get log file
        return http.get('/server/logs/telem-log.json')
          .then(function (result) {
            return result.data[domainObject.identifier.key].filter(function (d) {
              return d["timestamp"] > options.start && d["timestamp"] < options.end;
            });
        });
      }
    };

    openmct.telemetry.addProvider(provider);
  }
}