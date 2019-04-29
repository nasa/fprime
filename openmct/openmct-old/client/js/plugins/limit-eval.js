/*
 * How to compute limits on channel and find css to display them.
 * @param {string} target - deployment name
 * @return - An openmct object
 */
function ChanLimitEval(target) {
  let targetKey = target.toLowerCase();
  // Test if there is severity warning color or not.
  function hasNull(o) {
    for (key in o) {
      if (o[key] == null) {
        return true;
      }
    }
    return false;
  }
  // This returns the function that OpenMCT wants to figure out how to highlight it.
  function LimitEvaluator(domainObject) {
    return {
      evaluate: function(datum, key) {
        // console.log(key);
        // Return based on domainObject
        let limits = domainObject.model.limits;
        let raw = datum.value;
        if (raw > limits['high_red']) {
          return {
            cssClass: 's-limit-red s-limit-upr'
          }
        } else if (raw > limits['high_orange']) {
          return {
            cssClass: 'limit-orange'
          }
        } else if (raw > limits['high_yellow']) {
          return {
            cssClass: 's-limit-yellow s-limit-upr'
          }
        } else if (raw < limits['low_red']) {
          return {
            cssClass: 's-limit-red s-limit-lwr'
          }
        } else if (raw < limits['low_orange']) {
          return {
            cssClass: 'limit-orange'
          }
        } else if (raw < limits['low_yellow']) {
          return {
            cssClass: 's-limit-yellow s-limit-lwr'
          }
        } 
      }
    }
  }
  // What does it apply to, well only to the target app, ...
  // Added the appliesTo and this gets called before the above
  LimitEvaluator.appliesTo = function(domainObject) {
    // Applies to ref telemetry iff there are no null values in the limits
    return domainObject.type === targetKey + '.telemetry' && domainObject.name !== 'Events' && !hasNull(domainObject.limits);
  }
  // Hook up into openmct here
  return function install(openmct) {
    openmct.legacyExtension('capabilities', {
      key: 'limit',
      implementation: LimitEvaluator
    });
  }
}
/*
 * How to compute severity on events and find css to display them.
 * @param {string} target - deployment name
 * @return - An openmct object
 */
function EventLimitEval(target) {
  let targetKey = target.toLowerCase();
  function LimitEvaluator(domainObject) {
    return {
      evaluate: function(datum, key) {                
        // Return based on domainObject
        
        let severity = datum.severity;
       
        // Severity color coding
        if (severity === 'WARNING_HI') { 
          return {
            cssClass: 's-limit-red'
          };
        } else if (severity === 'WARNING_LO') {
          return {
            cssClass: 's-limit-yellow'
          };

        } else if (severity === 'COMMAND') {
          return {
            cssClass: 'limit-blue'
          };

        }
      }
    }
  }

  LimitEvaluator.appliesTo = function(domainObject) {
    // Applies to ref telemetry iff there are no null values in the limits
    return domainObject.type === targetKey + '.telemetry' && domainObject.name === 'Events';
  }

  return function install(openmct) {
    openmct.legacyExtension('capabilities', {
      key: 'limit',
      implementation: LimitEvaluator
    });
  }
}