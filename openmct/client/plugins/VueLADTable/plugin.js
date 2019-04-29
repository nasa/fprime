/**
  * plugin.js
  *
  * Vue.js Latest Available Data plugin - displays a table with the latest Available
  * data for any domain objects contained within
  *
  * Taken from the pull request here: https://github.com/nasa/openmct/pull/1678
  * and modified by @author Aaron Doubek-Kraft aaron.doubek-kraft@jpl.nasa.gov
  *
  * This originally used requirejs to import modules, but this seemed to be causing
  * namespace problems, so all related classes are currently placed
  * into the global namespace
  **/

var LADPlugin = function () {
    return function install (openmct) {

        openmct.types.addType('lad-table', {
            name: 'Latest Available Data Table',
            description: 'A table that shows the latest available data for all telemetry points contained within.',
            cssClass: 'icon-tabular-lad',
            createable: true,
            initialize: function (obj) {
                obj.composition = [];
            }
        });

        openmct.objectViews.addProvider({
            name: 'Latest Available Data Table',
            key: 'view.lad-table',
            cssClass: 'icon-tabular-lad',
            creatable: true,
            canView: function (d) {
                return d.type === 'lad-table';
            },
            view: function (domainObject) {

                var controller = new TableController(
                    domainObject,
                    openmct
                );
                var table = new TableView({
                    data: controller.data
                });

                return {
                    show: function (container) {
                        table.$mount(container);
                    },
                    destroy: function (container) {
                        table.$destroy();
                        controller.destroy();
                    }
                };
            }
        });
    }
}
