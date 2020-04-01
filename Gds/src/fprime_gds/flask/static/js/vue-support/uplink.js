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
    methods: {
        fileAction(event) {
            let action = event.currentTarget.innerText;
            let uplinkvue = this.$parent.$parent;
            let index = uplinkvue.selected.indexOf(this.item);
            if (action == "Remove" && index != -1) {
                uplinkvue.selected.splice(index, 1);
            } else {
                uplinkvue.uploader.command(this.item.source, action);
            }
        }
    },
    computed: {
        progressBarClass: function () {
            return "bg-success";
        },
        basename() {
            let regex = /.*[\\\/]/;
            return this.item.destination.replace(regex, "")
        }
    }
});

Vue.component("uplink", {
    props:["upfiles", "running", "uploader"],
    data: function() {
        return {"selected": [], "destination": "/"}
    },
    template: "#uplink-template",
    methods: {
        uplinkFiles() {
            if (this.selected.length == 0) {
                return;
            }
            this.uploader.upload(this.selected, this.destination);
        },
        pauseUplink() {
            this.uploader.pause();
        },
        unpauseUplink() {
            this.uploader.unpause();
        },
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
        keyify(item) {
            return "file-" + item;
        },
        columnify: function (item) {
            return [item.source, item.destination, item.state];
        },
    },
    computed: {
        elements: function () {
            return this.selected.concat(this.upfiles.reverse());
        }
    }
});

export let UplinkMixins = {
    setupUplink(uploader) {
        return {"upfiles": [], "running": false, "uploader": uploader}
    },
    updateUpfiles(files, running) {
        this.vue.upfiles = files;
        this.vue.running = running;
    }
};