/**
 * uplink.js:
 *
 * This is the Vue component used to display and handle the uplinks of the various files. This component has two basic
 * parts:
 *
 * 1. a small command form to upload N files to 1 destination allowing for a curated list and a "go" button
 * 2. a table view of all curating and uploading files
 *
 * Note: this component uses a custom row HTML for display in the table view, as it is sufficiently different from
 * the other tables. This can be found in the fptable JS document.
 *
 * These file rows are used to fill out the uplink and the downlink tables.
 *
 * @author mstarch
 */
Vue.component("uplink", {
    template: "#uplink-template",
    props: {
        /**
         * A list of files filled in from REST polling that represent the files being  managed by the file uplinker.
         */
        "upfiles": Array,
        /**
         * Filled in by REST polling, this is a boolean that describes if the uplinking is running
         * (as opposed to paused).
         */
        "running": Boolean,
        /**
         * Uploader object used to interact with the file uplinks and downlinks REST endpoints.
         */
        "uploader": Object
    },
    data: function() {
        return {"selected": [], "destination": "/"}
    },
    methods: {
        /**
         * Starts the uplink by uploading the currated list of files to the webserver backend. If there are no files
         * specified in the curated list, then bail.
         */
        uplinkFiles() {
            if (this.selected.length == 0) {
                return;
            }
            this.uploader.upload(this.selected, this.destination);
        },
        /**
         * Calls the uploader to pause the uplinker.
         */
        pauseUplink() {
            this.uploader.pause();
        },
        /**
         * Calls the uploader to unpause the uplinker.
         */
        unpauseUplink() {
            this.uploader.unpause();
        },
        /**
         * Handles the files event to add input files into the list being curated. This takes each file, and creates a
         * mock file object so that it displays nicely in the curateable "NOT STARTED" state.
         * @param event: event to add files
         */
        handelFiles(event) {
            // Bail on no files
            if (event.target.files.length == 0) {
                return;
            }
            this.selected.push(...Array.from(event.target.files).map((item) =>
                {
                    return {
                        "file": item,
                        "source": item.name,
                        "destination": this.destination,
                        "state": "NOT STARTED",
                        "percent": 0,
                        "uplink": true,
                        "start": "",
                        "end": ""
                    }
                }));
            event.target.value = "";
        },
        /**
         * Turn this file item into a key. for uniqueness
         * @param item: item to keyify
         * @return {string}
         */
        keyify(item) {
            return "file-" + item.source + item.destination + item.state;
        },
        /**
         * Change the file into a set of columns. This isn't used for display, but for filtering.
         * @param item: file item
         * @return Array of columns for filtering purposes.
         */
        columnify(item) {
            return [item.source, item.destination, item.state];
        },
    },
    computed: {
        /**
         * Returns the total list of files for display in the table. In short, this is curatec + uplinked (reversed)
         * lists.
         * @return {T[] | string}
         */
        elements() {
            return this.selected.concat(this.upfiles.reverse());
        }
    }
});
/**
 * Mixins used to handle uplink as part of this, or any Vue.
 */
export let UplinkMixins = {
    /**
     * Setup the uplink requires the "uploader" object. returns the needed top-level data to be bound down into props.
     * @param uploader: uploader object to use to upload.
     * @return initially needed uplink data items
     */
    setupUplink(uploader) {
        return {"upfiles": [], "running": false, "uploader": uploader}
    },
    /**
     * Callback used to set files and running state needed for the uplink.
     * @param files: list of files managed by the uplinker on the server side
     * @param running: boolean not-paused running status.
     */
    updateUpfiles(files, running) {
        this.vue.upfiles = files;
        this.vue.running = running;
    }
};