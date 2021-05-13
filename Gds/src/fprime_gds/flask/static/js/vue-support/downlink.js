/**
 * downlink.js:
 *
 * This is the Vue component used to display and handle the downlinks of the various files. This component has one basic
 * parts:
 *
 * 1. a table view of all downloading and downloaded files
 *
 * Note: this component uses a custom row HTML for display in the table view, as it is sufficiently different from
 * the other tables. This can be found in the fptable JS document.
 *
 * These file rows are used to fill out the uplink and the downlink tables.
 *
 * @author mstarch
 */
import {_datastore} from "../datastore.js";

/**
 * downlink.js:
 *
 * Downlink vue support code. Provides the needed data to visualize a table list of downlinked and downlinking files.
 */
Vue.component("downlink", {
    data: function() {
        return {"downfiles": _datastore.downfiles}
    },
    template: "#downlink-template",
    methods: {
        /**
         * Converts file into a unique key.
         * @param item: item to keyify
         * @return {string}
         */
        keyify(item) {
            return "file-" + item.source + item.destination;
        }
    },
    computed: {
        /**
         * The elements to be displayed are the list of incoming files set by the REST backend.
         */
        elements() {
            return this.downfiles;
        },
        /**
         * Returns the file item as a list of columns. This allows for filtering.
         * @param item: item to columnify
         * @return list of columns
         */
        columnify(item) {
            return [item.source, item.destination, item.state];
        },
    }
});
