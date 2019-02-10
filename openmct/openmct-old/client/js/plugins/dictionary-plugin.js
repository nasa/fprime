

// Actual plugin. Must be a function with 'openmct' result operand and
// must return function of 'install (openmct)'

/*
 * Configures the structure of channels and datums so OpenMCT can display them.
 * @param {string} target - name of target (from config.js file)
 * @return {openmct object} - object that openmct uses to create internal framework
 */
function DictionaryPlugin(target) {
  let targetKey = target.toLowerCase();
  let targetName = target[0].toUpperCase() + target.substring(1);

  // Value formatters
  let value_format = {
    'hints': {
      'range': 2
    },
    'key': 'value',
    'max': 100,
    'min': 0,
    'name': 'Value',
    'units': 'units'
  };
  let time_format = {
    'key': 'utc',
    'source': 'timestamp',
    'name': 'Timestamp',
    'format': 'utc',
    'hints': {
      'domain': 1
    }
  };
  let name_format = {
    'hints': {
      'domain': 2
    },
    'key': 'name',
    'name': 'Name'
  };
  let id_format = {
    'hints': {
      'domain': 3
    },
    'key': 'identifier',
    'name': 'ID'
  };
  let severity_format = {
    'hints': {
      'range': 1
    },
    'key': 'severity',
    'name': 'Severity'
  };

  var objectProvider = {
    get: function (identifier) {
      // Return promise of function
      // Get json dictionary
      return getDictionary().then(function (dictionary) {
        // Create and describe domain object from root
        // Indentifier contians key and namespace
        if (identifier.key === targetKey) {
          return {
            // Provider if ref root
            identifier: identifier, // Domain object 'identifier' is same as root
            name: targetName,  // Name of toplevel dictionary object ('REF')
            type: 'folder',
            location: 'ROOT'
          };
        } else {
          // Provider if not ref root

          // Measurement = measurement object with same key as 'identifier.key'
          let measurement = dictionary[targetKey]['channels'][identifier.key];

          let value_formats = [name_format, id_format, time_format, value_format];
          let units = measurement['units'];
          if (units != null) {
            units.forEach(function (u, i) {
              let value_format_save = Object.assign({}, value_format);
              value_format_save['units'] = u['Label'];
              value_format_save['key'] = 'value:' + u['Label'];
              value_format_save['hints']['range'] = i + 2;
              value_format_save['name'] = u['Label'];
              value_formats.push(value_format_save);
            });
          }
          // Object provider for each object in measurments.
          // Does not populate tree

          let typeStr = targetKey + '.telemetry';
          // console.log('type' + typeStr);
          let toReturn = {
            identifier: identifier,
            name: measurement.name,
            type: typeStr,
            notes: measurement['description'],
            // type: typeStr,
            telemetry: {
              values: value_formats  // Values already in default format
            },
            location: targetKey + '.taxonomy:' + targetKey
          }
          if (measurement.name === 'Events') {
            // Object provider for events
            let eventToReturn = Object.assign({}, toReturn);
            eventToReturn.telemetry.values.push(severity_format);
            return eventToReturn;
          } else {
            // Object provider for all channels
            toReturn.limits = measurement['limits'];
            return toReturn;
          }
        }
      });
    }
  };

  var compositionProvider = {
    appliesTo: function (domainObject) {
      // Determines what object this composition provider will provide
      // In this case, the ref.taxonomy domain object with a type of folder.
      return domainObject.identifier.namespace === targetKey + '.taxonomy' &&
           domainObject.type === 'folder';
    },
    load: function (domainObject) {
      // Returns promise of an array of domain objects, in this case list of measurements.
      return getDictionary().then(function (dictionary) {
        // 'dictionary.measurements' is a list of telemetry objects
        let channels = [];
        let chanDict = dictionary[targetKey]['channels'];
        for (id in chanDict) {
          channels.push({
            namespace: targetKey + '.taxonomy',
            key: id
          });
        }
        return channels;
      });
    }
  };


  // Return function of plugin
    /*
     * Install is ued by openmct to install this functionality
     */
  return function install(openmct) {
    // Create root of dictionary
    openmct.objects.addRoot({
      // Create identifier

      // Namespace used to identify which root
      // to provide telemetry objects for
      namespace: targetKey + '.taxonomy',
      key: targetKey
    });

    // Create domain object (ref folder) under the root namespace targetKey + '.taxonomy'
    openmct.objects.addProvider(targetKey + '.taxonomy', objectProvider);

    // Composition provider will define structure of the tree and populate it.
    openmct.composition.addProvider(compositionProvider);

    openmct.types.addType(targetKey + '.telemetry', {
      name: targetName + ' Telemetry Point',
      description: targetName + ' telemetry point fromtargetName +   App.',
      cssClass: 'icon-telemetry'
    });
  };
};
