


var BASE_RESOLUTION = 30 * 60 * 1000;

function getBucketDate(date) {
    var startOfDay = Date.UTC(date.getFullYear(), date.getMonth(), date.getDate());
    var milliseconds = date - startOfDay;
    var bucketStart = Math.floor(milliseconds / BASE_RESOLUTION) * BASE_RESOLUTION;
    return +startOfDay + bucketStart;
}

function getBucketRange(pointName, start, end) {
    var parts = pointName.split('.');
    parts.push('');
    var prefix = parts.join('/');
    var count = (end - start) / BASE_RESOLUTION;
    var last = getBucketDate(start);
    var results = [prefix + last];
    while ((last + BASE_RESOLUTION) < end) {
        last = last + BASE_RESOLUTION;
        results.push(prefix + last);
    }
    return results;
}

function getBucketPath(pointName, date) {
    var parts = pointName.split('.');
    parts.push(getBucketDate(date));
    return parts.join('/');
}

module.exports = {
    getBucketPath: getBucketPath,
    getBucketDate: getBucketDate,
    getBucketRange: getBucketRange
};
