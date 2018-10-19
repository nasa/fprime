
var FLAGS = [
    {bit: 0x80000000, name: 'VALUE_SET'},
    {bit: 0x40000000, name: 'VALUE_STATIC'},
    {bit: 0x20000000, name: 'VALUE_QUESTIONABLE'},
    {bit: 0x04000000, name: 'CONVERSION_ENABLED'},
    {bit: 0x02000000, name: 'LIMIT_CHECKING'},
    {bit: 0x01000000, name: 'DELTA_CHECKING'},
    {bit: 0x00200000, name: 'DELTA_LIMIT_VIOLATION'},
    {bit: 0x00100000, name: 'RED_HIGH_LIMIT_VIOLATION'},
    {bit: 0x00080000, name: 'RED_LOW_LIMIT_VIOLATION'},
    {bit: 0x00040000, name: 'YELLOW_HIGH_LIMIT_VIOLATION'},
    {bit: 0x00020000, name: 'YELLOW_LOW_LIMIT_VIOLATION'},
    {bit: 0x00010000, name: 'NO_LIMIT_VIOLATION'},
    {bit: 0x00008000, name: 'TRANSITION_TO_RED_HIGH'},
    {bit: 0x00004000, name: 'TRANSITION_TO_RED_LOW'},
    {bit: 0x00002000, name: 'TRANSITION_TO_YELLOW_HIGH'},
    {bit: 0x00001000, name: 'TRANSITION_TO_YELLOW_LOW'},
    {bit: 0x00000800, name: 'TRANSITION_TO_NOMINAL'},
    {bit: 0x00000400, name: 'READ_ONLY'},
    {bit: 0x00000200, name: 'SIMULATED'}
];

function getFlags(value) {
    return FLAGS.filter(function (flag) {
        return (flag.bit & value) !== 0
    }).map(function (flag) {
        return flag.name
    });
};

function getLimitState(value) {
    if ((value.flags & 0x00200000) !== 0) {
        return 'delta_violation'; // delta violation
    }
    if ((value.flags & 0x00100000) !== 0) {
        return 'red_high';
    }
    if ((value.flags & 0x00080000) !== 0) {
        return 'red_low';
    }
    if ((value.flags & 0x00040000) !== 0) {
        return 'yellow_high';
    }
    if ((value.flags & 0x00020000) !== 0) {
        return 'yellow_low';
    }
    return false;
}


/**
 * Simple limit monitor.  Listens for input and tracks mnemonics as they enter
 * and leave alarm state.  Emits new telemetry values whenever mnemonics enter
 * or leave alarm state.
 */
function LimitMonitor(telemetryServer) {

    var LIMIT_STATE = {};

    telemetryServer.on('input:chunk', function (values) {
        if (values.isLimit) {
            return;
        }
        var limitViolations = [];
        limitViolations.isLimit = true;
        values.forEach(function (value) {
            var limitState = getLimitState(value);
            if (limitState && LIMIT_STATE[value.name] !== limitState) {
                limitViolations.push({
                    name: 'LIMIT_LOG',
                    mnemonic: value.name,
                    limit: 'entered ' + limitState,
                    flags: value.flags,
                    timestamp: value.timestamp,
                    raw_value: value.raw_value,
                    eng_val: value.eng_val
                });
                LIMIT_STATE[value.name] = limitState;
            } else if (!limitState && LIMIT_STATE[value.name]) {
                limitViolations.push({
                    name: 'LIMIT_LOG',
                    mnemonic: value.name,
                    limit: 'left ' + LIMIT_STATE[value.name],
                    flags: value.flags,
                    timestamp: value.timestamp,
                    raw_value: value.raw_value,
                    eng_val: value.eng_val
                });
                LIMIT_STATE[value.name] = limitState;
            }
        });
        if (limitViolations.length) {
            limitViolations.forEach(function (value) {
                telemetryServer.emit('input:data', value);
            });
            telemetryServer.emit('input:chunk', limitViolations);
        }
    });
}


module.exports = LimitMonitor;
