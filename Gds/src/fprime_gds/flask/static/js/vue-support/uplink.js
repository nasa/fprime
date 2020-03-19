import {timeToString} from "./utils.js";

Vue.component("uplink-row", {
    template: "#uplink-row-template",
    props: {
        /**
         * item:
         *
         * 'item' will be automatically bound to each item in the 'items' list of the consuming table. It is the loop
         * variable, and will be passed into the 'itemToColumns' function to produce columns.
         */
        item: Object,
        /**
         * itemToColumns:
         *
         * 'itemToColumns' will be bound to a function taking one item from the parent fp-table object. See fp-table.
         */
        itemToColumns: Function
    },
    computed: {
        progressBarClass: function () {
            return "bg-success";
        }
    }
});

Vue.component("uplink", {
    props:["upfiles", "uploader"],
    data: function() {
        return {"selected": [], "upload_progress": 0, "upload_elem": null, "destination": "/"}
    },
    template: "#uplink-template",
    methods: {
        progress(percent) {
            this.upload_progress = percent;
        },
        uplinkFiles() {
            this.upload_progress = 0;
            if (this.selected.length == 0) {
                return;
            }
            this.uploader.upload(this.selected, this.destination, this.progress.bind(this)).then(
                () => {
                    this.upload_elem.value = "";
                }
            );
        },
        handelFiles(event) {
            this.upload_elem = event.target;
            this.selected = event.target.files;
        },
        keyify(item) {
            return "file-" + item;
        }
    }
});

export let UplinkMixins = {
    setupUplink(uploader) {
        return {"upfiles": [], "uploader": uploader}
    }
};