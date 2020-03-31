import {timeToString} from "./utils.js";
Vue.component("downlink", {
    props:["downfiles"],
    template: "#downlink-template",
    methods: {
        keyify(item) {
            return "file-" + item;
        }
    },
    computed: {
        elements: function () {
            return this.downfiles;
        },
        columnify: function (item) {
            return [item.source, item.destination, item.state];
        },
    }
});

export let DownlinkMixins = {
    setupDownlink() {
        return {"downfiles": []}
    },
    updateDownfiles(files) {
        this.vue.downfiles = files;
    }
};