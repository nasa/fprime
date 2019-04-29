var fs = require('fs');

function Dictionary(options, telemetryServer) {
    if (!options) {
        options = {};
    }
    if (!options.packetSeparator) {
        options.packetSeparator = '.';
    }
    if (!options.packetPrefixes) {
        options.packetPrefixes = [];
    }
    if (!options.packetsFile) {
        options.packetsFile = 'packets.json';
    }
    if (!options.pointsFile) {
        options.pointsFile = 'points.json';
    }


    this.options = options;
    this.packetPrefixes = [].slice.apply(options.packetPrefixes);
    this.packets = {};
    this.points = {};
    this.pointMapping = {};

    this.load();

    telemetryServer.on('input:chunk', this.updateFromTelemetry.bind(this));
}

Dictionary.prototype.updateFromTelemetry = function (values) {
    for (var i = 0; i < values.length; i++) {
        this.updatePoint(values[i]);
    }
};

Dictionary.prototype.load = function () {
    if (fs.existsSync(this.options.pointsFile)) {
        this.points = JSON.parse(fs.readFileSync(this.options.pointsFile));
    }
    if (fs.existsSync(this.options.packetsFile)) {
        this.packets = JSON.parse(fs.readFileSync(this.options.packetsFile, 'utf8'));
        this.pointMapping = {};

        Object.keys(this.packets).forEach(function (packetId) {
            this.packets[packetId].points.forEach(function (pointId) {
                this.pointMapping[pointId] = packetId;
            }, this);
        }, this);
    }
};

Dictionary.prototype.save = function () {
    fs.writeFileSync(this.options.packetsFile, JSON.stringify(this.packets), 'utf8');
    fs.writeFileSync(this.options.pointsFile, JSON.stringify(this.points), 'utf8');
};

Dictionary.prototype.packetBySeparator = function (pointName) {
    if (pointName.indexOf(this.options.packetSeparator) === -1) {
        return 'UNGROUPED_POINTS';
    } else {
        return pointName.split(this.options.packetSeparator)[0];
    }
};

Dictionary.prototype.packetByPrefix = function (pointName) {
    return this.packetPrefixes.filter(function (packetName) {
        return pointName.indexOf(packetName) === 0;
    })[0];
};

Dictionary.prototype.getPacketIdFromPointName = function (pointName) {
    return this.pointMapping[pointName] ||
        this.packetByPrefix(pointName) ||
        this.packetBySeparator(pointName);
};

Dictionary.prototype.addPointToPacket = function (pointName) {
    var packetId = this.getPacketIdFromPointName(pointName);
    if (!this.packets[packetId]) {
        this.packets[packetId] = {
            name: packetId,
            points: []
        };
    }
    this.packets[packetId].points.push(pointName);
    this.packets[packetId].points = this.packets[packetId].points.sort();
    this.pointMapping[pointName] = packetId;
};

Dictionary.prototype.updatePoint = function (pointData) {
    if (this.points[pointData.name] || pointData.name === 'LIMIT_LOG') {
        return;
    }
    this.addPointToPacket(pointData.name);
    var point = this.points[pointData.name] = {};
    point.name = pointData.name;
    point.key = pointData.name;
    point.id = pointData.name;
    point.values = [
        {
            key: 'utc',
            source: 'timestamp',
            name: 'Timestamp',
            format: 'utc',
            hints: {
                domain: 1
            }
        },
        {
            key: 'raw_value',
            name: 'Raw Value',
            raw_type: pointData.raw_type,
            hints: {
                range: 2
            }
        }
    ];
    if (pointData.hasOwnProperty('eng_val')) {
        point.values.push({
            key: 'eng_val',
            name: 'Engineering Value',
            eng_type: pointData.eng_type,
            hints: {
                range: 1
            }
        });
    }
};

Dictionary.prototype.getPointMapping = function () {
    return this.pointMapping;
};

Dictionary.prototype.getPacketIds = function () {
    return Object.keys(this.packets);
};

Dictionary.prototype.getPacket = function (packetId) {
    var packet = {
        name: packetId,
        points: {}
    };
    this.packets[packetId].points.forEach(function(pointId) {
        packet.points[pointId] = this.points[pointId];
    }, this);
    return packet;
};

module.exports = Dictionary;
