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
import {_uploader} from "../uploader.js";
import {_datastore} from "../datastore.js";

Vue.component("uplink", {
    template: "#uplink-template",
     data: function() {
        return {"upfiles": _datastore.upfiles, "running": _datastore.uploading, "selected": [], "destination": "/", "error": null}
    },
    methods: {
        /**
         * Starts the uplink by uploading the curated list of files to the webserver backend. If there are no files
         * specified in the curated list, then bail.
         */
        uplinkFiles() {
            this.error = null;
            if (this.selected.length == 0) {
                return;
            }
            let _self = this;
            _uploader.upload(this.selected, this.destination).catch(
                function(error) {
                    _self.error = (error != "")? error : "Total size limited to 32MB. Please use separate uplinks";
                });
        },
        /**
         * Calls the uploader to pause the uplinker.
         */
        pauseUplink() {
            _uploader.pause();
        },
        /**
         * Calls the uploader to unpause the uplinker.
         */
        unpauseUplink() {
            _uploader.unpause();
        },
        /**
         * Handles the files event to add input files into the list being curated. This takes each file, and creates a
         * mock file object so that it displays nicely in the curateable "NOT STARTED" state.
         * @param event: event to add files
         */
        handleFiles(event) {
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
         * Returns the total list of files for display in the table. In short, this is curated + uplinked (reversed)
         * lists.
         * @return {T[] | string}
         */
        elements() {
            return this.selected.concat(this.upfiles.reverse());
        }
    }
});
