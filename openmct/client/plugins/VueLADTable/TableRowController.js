function TableRowController(domainObject, openmct) {
    this.domainObject = domainObject;
    this.openmct = openmct;
    this.id = JSON.stringify(domainObject.identifier);
    this.data = {
        'Name': domainObject.name,
        'Timestamp': '---',
        'Value': '---',
        valueClass: ''
    };

    this.metadata = openmct
        .telemetry
        .getMetadata(domainObject);
    this.formats = openmct
        .telemetry
        .getFormatMap(this.metadata);
    this.limitEvaluator = openmct
        .telemetry
        .limitEvaluator(domainObject);
    this.stopWatchingMutation = openmct
        .objects
        .observe(
            domainObject,
            '*',
            this.updateName.bind(this)
        );

    openmct.time.on('timeSystem', this.updateTimeSystem, this);

    this.timestampKey = openmct.time.timeSystem().key;

    this.valueKey = this
        .metadata
        .valuesForHints(['range'])[0].key;

    this.unsubscribe = openmct
        .telemetry
        .subscribe(domainObject, this.updateValues.bind(this), {});

    openmct
        .telemetry
        .request(domainObject, {strategy: 'latest'})
        .then((values) => values.forEach(this.updateValues, this));
};

TableRowController.prototype.updateValues = function (datum) {
    this.data.Timestamp = this.formats[this.timestampKey].format(datum);
    this.data.Value = this.formats[this.valueKey].format(datum);
    var limit = this.limitEvaluator.evaluate(datum, this.valueKey);
    if (limit) {
        this.data.valueClass = limit.cssClass;
    } else {
        this.data.valueClass = '';
    }
};

TableRowController.prototype.updateName = function (domainObject) {
    this.data.Name = domainObject.name;
};

TableRowController.prototype.updateTimeSystem = function (timeSystem) {
    this.timestampKey = timeSystem.key;
};

TableRowController.prototype.destroy = function () {
    this.stopWatchingMutation();
    this.unsubscribe();
    this.openmct.time.off('timeSystem', this.updateTimeSystem, this);
};
