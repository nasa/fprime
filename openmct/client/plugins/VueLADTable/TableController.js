function TableController(domainObject, openmct) {
    this.domainObject = domainObject;
    this.openmct = openmct;
    this.rowControllers = {};
    this.composition = openmct.composition.get(domainObject);
    this.composition.on('add', this.addChild, this);
    this.composition.on('remove', this.removeChild, this);
    this.data = {
        headers: [
            'Name',
            'Timestamp',
            'Value'
        ],
        rows: [

        ]
    };
    this.composition.load();
}

TableController.prototype.addChild = function (childObject) {
    var rowController = new TableRowController(childObject, this.openmct);
    this.rowControllers[rowController.id] = rowController;
    this.data.rows.push(rowController.data);
};

TableController.prototype.removeChild = function (identifier) {
    var childId = JSON.stringify(identifier);
    var rowController = this.rowControllers[childId];
    rowController.destroy();
    this.data.rows.splice(this.data.rows.indexOf(rowController.data), 1);
};

TableController.prototype.destroy = function () {
    this.composition.off('add', this.addChild, this);
    this.composition.off('remove', this.removeChild, this);
    delete this.composition;
    Object.keys(this.rowControllers).forEach(function (childId) {
        this.rowControllers[childId].destroy();
    }, this);
};
