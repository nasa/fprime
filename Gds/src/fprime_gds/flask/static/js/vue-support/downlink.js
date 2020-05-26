/**
 * downlink.js:
 *
 * Downlink vue support code. Provides the needed data to visualize a table list of downlinked and downlinking files.
 */
Vue.component("downlink", {
    props:["downfiles"],
    template: "#downlink-template",
    methods: {
        /**
         * Converts file into a unique key.
         * @param item: item to keyifiy
         * @return {string}
         */
        keyify(item) {
            return "file-" + item.source + item.destination;
        }
    },
    computed: {
        /**
         * The elemets to be displayed are the list of incoming files set by the REST backend.
         */
        elements() {
            return this.downfiles;
        },
        /**
         * Returns the file item as a list of clumns. This allows for filtering.
         * @param item: item to columnify
         * @return list of columns
         */
        columnify(item) {
            return [item.source, item.destination, item.state];
        },
    }
});
/**
 * Mixins for downlinking.
 * @type {number}
 */
export let DownlinkMixins = {
    /**
     * Setup downlink needed data items "downfiles"
     * @return {{downfiles: []}}
     */
    setupDownlink() {
        return {"downfiles": []}
    },
    /**
     * Function callback for REST API to fill in data files for downlinking.
     * @param files: files that are managed by the downlink server.
     */
    updateDownfiles(files) {
        this.vue.downfiles = files;
    }
};