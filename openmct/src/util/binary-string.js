/**
space-efficient buffer string encoding.  Does not inflate buffer size;
decodes consistently.

For some reason, if you pass a buffer directly to node-redis, it performs
much worse than if you pass a string.

**/

module.exports = {
    fromBuffer: function (buffer) {
        return String.fromCharCode.apply(String, buffer);
    },
    toBuffer: function (binaryString) {
        var i, length, array = [];
        for (i = 0, length = binaryString.length; i < length; ++i) {
            array.push(binaryString.charCodeAt(i));
        }
        return new Buffer(array);
    }
};

