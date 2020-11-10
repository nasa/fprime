/**
 * log.js:
 *
 * Vue support for the log viewing screen. This will allow users to view the various logs exported by the underlying
 * server.
 */
// Setup component for select
import "../../third-party/js/vue-select.js"
import {_datastore} from "../datastore.js"

// Must provide v-select
Vue.component('v-select', VueSelect.VueSelect);

Vue.component("logging", {
    template: "#logs-template",
    data: function() { return {"selected": "", "logs": _datastore.registerLogHandler(this)};},
    computed:{
        /**
         * Computes the appropriate log files available.
         * @return {string[]}
         */
        options: function () {
            let kets = Object.keys(this.logs);
            return kets;
        }
    },
    methods: {
        /**
         * Updates the log data such that new logs can be displayed.
         */
        update() {
            for (let key in _datastore.logs) {
                this.$set(this.logs, key, _datastore.logs[key]);
            }
            this.$el.scrollTop = this.$el.scrollHeight;
        }
    }
});
