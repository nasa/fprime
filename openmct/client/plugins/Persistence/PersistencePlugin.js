/**
  * PersistencePlugin.js
  *
  * Plugin allowing OpenMCT to interface with the LevelDB-based persistence
  * plugin implemented in the telemetry server.
  *
  * @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  */

var PersistencePlugin = function (baseURL) {
    return function install (openmct) {
        openmct.legacyExtension('components', {
            provides: "persistenceService",
            type: "provider",
            implementation: () => new PersistenceProvider(baseURL)
        });
    }
}
