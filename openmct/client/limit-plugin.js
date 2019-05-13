(function () {
    var LIMITS = [
        {
            name: 'RED_HIGH_LIMIT_VIOLATION',
            cssClass: "s-limit-upr s-limit-red",
            flagBit: 0x00100000
        },
        {
            name: 'RED_LOW_LIMIT_VIOLATION',
            cssClass: "s-limit-lwr s-limit-red",
            flagBit: 0x00080000
        },
        {
            name: 'YELLOW_HIGH_LIMIT_VIOLATION',
            cssClass: "s-limit-upr s-limit-yellow",
            flagBit: 0x00040000
        },
        {
            name: 'YELLOW_LOW_LIMIT_VIOLATION',
            cssClass: "s-limit-lwr s-limit-yellow",
            flagBit: 0x00020000
        }
    ];

    function BSONLimitCapability(domainObject) {
        return {
            evaluate: function (datum, range) {
                for (var i = 0; i < LIMITS.length; i++) {
                    if ((LIMITS[i].flagBit & datum.flags) !== 0) {
                        return LIMITS[i];
                    }
                }
            }
        };
    };

    BSONLimitCapability.appliesTo = function (model) {
        return model.type === 'bson.telemetry';
    };

    function LimitViewerViewPolicy() {};

    LimitViewerViewPolicy.prototype.allow = function (view, domainObject) {
        if (domainObject.getId() === 'bson.server:LIMIT_LOG') {
            return view.key === 'table';
        }
        return true;
    };

    window.LimitPlugin = function (options) {
        return function install(openmct) {
            openmct.legacyExtension('capabilities', {
                key: 'limit',
                implementation: BSONLimitCapability
            });

            openmct.legacyExtension('policies', {
                category: 'view',
                implementation: LimitViewerViewPolicy
            });
        }
    };
})();

