var DictionaryPlugin = function (options) {
    if (!options) {
        options = {}
    }
    if (!options.url) {
        console.error('MUST SPECIFY DICTIONARY URL');
    }
    if (!options.namespace) {
        options.namespace = 'example.taxonomy';
    }
    if (!options.name) {
        options.name = 'BSON Telemetry Server';
    }

    var pending = {};

    var serverGet = function (path) {
        if (pending[path]) {
            return pending[path];
        }

        pending[path] = http.get(options.url + path)
            .then(function (result) {
                return result.data;
            });

        pending[path].then(function (data) {
            setTimeout(function () {
                delete pending[path];
            }, 30000);
        });

        return pending[path];
    };

    var objectProvider = {
        get: function (identifier) {
            if (identifier.key === 'root') {
                return Promise.resolve({
                    identifier: identifier,
                    name: options.name,
                    type: 'folder',
                    location: 'ROOT'
                });
            }
            if (identifier.key === 'LIMIT_LOG') {
                return Promise.resolve({
                    identifier: identifier,
                    name: 'Limit Log',
                    type: 'bson.telemetry',
                    location: options.namespace + ':root',
                    telemetry: {
                        name: 'LIMIT_LOG',
                        key: 'LIMIT_LOG',
                        id: 'LIMIT_LOG',
                        values: [
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
                                key: 'mnemonic',
                                name: 'Point',
                                hints: {
                                    range: 1
                                }
                            },
                            {
                                key: 'limit',
                                name: 'Change',
                                hints: {
                                    range: 2
                                }
                            },
                            {
                                key: 'raw_value',
                                name: 'Raw Value',
                                hints: {
                                    range: 3
                                }
                            },
                            {
                                key: 'eng_val',
                                name: 'Engineering Value',
                                hints: {
                                    range: 4
                                }
                            }
                        ]
                    }
                });
            }
            return serverGet('/packets')
                .then(function (data) {
                    if (data.packets.indexOf(identifier.key) !== -1) {
                        return {
                            identifier: identifier,
                            name: identifier.key,
                            type: 'bson.packet',
                            location: options.namespace + ':' + 'root'
                        };
                    }
                    return serverGet('/mapping')
                        .then(function (packetMapping) {
                            var packetId = packetMapping[identifier.key];
                            return serverGet('/packets/' + packetId)
                                .then(function (packet) {
                                    var measurement = packet.points[identifier.key];
                                    measurement.values.forEach(function (v) {
                                        if (v.formatString) {
                                            return;
                                        }
                                        var typeVal;
                                        if (v.key === 'raw_value') {
                                            typeVal = v.raw_type;
                                        } else if (v.key === 'eng_val') {
                                            typeVal = v.eng_type;
                                        } else {
                                            return;
                                        }
                                        if (typeVal === 71) {
                                            // signed integer.
                                            v.formatString = '%i';
                                        } else if (typeVal === 72) {
                                            // unsigned integer
                                            v.formatString = '%u';
                                        } else if (typeVal === 73) {
                                            // Float!
                                            v.formatString = '%.02f';
                                        }
                                    });
                                    return {
                                        identifier: identifier,
                                        name: measurement.name,
                                        type: 'bson.telemetry',
                                        telemetry: measurement,
                                        location: options.namespace + ':' + packetId,
                                        notes: measurement.notes
                                    };
                                });
                        });
            });
        }
    };

    var compositionProvider = {
        appliesTo: function (domainObject) {
            return domainObject.identifier.namespace === options.namespace &&
                   (domainObject.type === 'folder' ||
                    domainObject.type === 'bson.packet');
        },
        load: function (domainObject) {
            if (domainObject.type === 'folder') {
                return serverGet('/packets')
                    .then(function (data) {
                        return [{
                            namespace: options.namespace,
                            key: 'LIMIT_LOG'
                        }].concat(data.packets.map(function (k) {
                            return {
                                namespace: options.namespace,
                                key: k
                            };
                        }));
                    });
            } else if (domainObject.type === 'bson.packet') {
                return serverGet('/packets/' + domainObject.identifier.key)
                    .then(function (data) {
                        return data.pointIds.map(function (k) {
                            return {
                                namespace: options.namespace,
                                key: k
                            };
                        });
                    });
            }
        }
    };


    return function install(openmct) {
        openmct.objects.addRoot({
            namespace: options.namespace,
            key: 'root'
        });

        openmct.objects.addProvider(options.namespace, objectProvider);

        openmct.composition.addProvider(compositionProvider);

        openmct.types.addType('bson.telemetry', {
            name: 'Telemetry Point',
            description: 'Telemetry point from bson server',
            cssClass: 'icon-telemetry'
        });

        openmct.legacyExtension('types', {
            key: 'bson.packet',
            name: 'Telemetry Packet',
            description: 'Telemetry Packet from bson server',
            cssClass: 'icon-packet',
            delegates: ['telemetry']
        });

        openmct.legacyExtension('policies', {
            category: 'view',
            implementation: function () {
                return {
                    allow: function (v, d) {
                        if (d.getCapability('type').typeDef.key === 'bson.packet') {
                            return v.key === 'autoflow';
                        }
                        return true;
                    }
                }
            }
        });
    };
};
