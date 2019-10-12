/**
 * Filter the given items by looking for a the matching string inside the list of items.  Each item is converted to a
 * string using the ifun parameter, and then the matching parameter is searched for in that converted string. Everything
 * is done in lower-case to provide for case-independent matching.
 * @param items: list of items to filter
 * @param matching: (optional) string looked for in each item.  Case independent match. Default: match everything.
 * @param ifun: (optional) object to string function. Default: JSON.stringify
 * @return {[]}
 */
export function filter(items, matching, ifun) {
    // Support multiple filters
    if (!Array.isArray(matching)) {
        matching = [matching];
    }

    // Convert object to string using given ifun function, or JSON.stringify
    let stringer = ifun;
    if (typeof(stringer) === "undefined") {
        stringer = JSON.stringify;
    }
    let output = [];
    // Loop over the items, only adding to output list if we match
    for (let i = 0; i < items.length; i++) {
        let j = 0;
        let item = items[i];
        for (j = 0; j < matching.length; j++) {
            // All filters must match (ANDed).  Throw out non-matching.
            if (typeof (matching[j]) !== "undefined" &&
                stringer(item).toLowerCase().indexOf(matching[j].toLowerCase()) == -1) {
                break;
            }
        }
        // Made it all the way through the loop, add item
        if (j == matching.length) {
            output.push(item);
        }
    }
    return output;
}

/**
 * Convert a given F´ time into a string for display purposes.
 * @param time: f´ time to convert
 * @return {string} stringified time
 */
export function timeToString(time) {
    // If we have a workstation time, convert it to calendar time
    if (time.base.value == 2) {
        let date = new Date(0);
        date.setSeconds(time.seconds);
        date.setMilliseconds(time.microseconds/1000);
        return date.toISOString();
    }
    return time.seconds + "." + time.microseconds;
}