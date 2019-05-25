/**
 * Helper for tracking stats.  returns an object with requested keys, and emits
 * events showing the change in values (on that object) over time.
 *
 * @param {String[]} stats a list of stats (keys) to track.
 * @param {EventEmitter} emitter an eventemitter where stats events will be reported.
 * @param {String} eventName the event name that will be emitted with stats.
 * @param {Number} [interval] interval (in milliseconds) to report stats at.
 *
 */
function trackStats(stats, emitter, eventName, interval) {
    if (!interval) {
        interval = 10000;
    }
    var lastReport = Date.now();
    var current = {};
    var last = {};

    stats.forEach(function (k) {
        current[k] = 0;
        last[k] = 0;
    });

    setInterval(function () {
        var now = Date.now();
        var changes = {};
        var report = {
            timestamp: now,
            duration: now - lastReport,
            changes: stats.reduce(function (changes, k) {
                changes[k] = current[k] - last[k];
                last[k] = current[k];
                return changes;
            }, {})
        };
        lastReport = now;
        emitter.emit(eventName, report);
    }, interval);

    return current;
}

module.exports = trackStats;
