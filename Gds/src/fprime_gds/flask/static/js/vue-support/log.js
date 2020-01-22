/**
 *
 *
 */
// Setup component for select
import "../../third-party/js/vue-select.js"


Vue.component('v-select', VueSelect.VueSelect);

Vue.component("logging", {
    props:["logs"],
    template: "#logs-template",
    data: function() { return {"selected": ""};},
    computed:{
        options: function () {
            let kets = Object.keys(this.logs);
            return kets;
        }
    }
});

/**
 * LogMixins:
 *
 * This set of functions should be mixed in as member functions to the F´ wrappers around the above Vue.js component.
 * These provide the functions required to update logs on the fly.
 *
 * Note: to mixin these functions: Object.assign(EventMixins)
 */
export let LogMixins = {
    /**
     * Update the list of events with the supplied new list of events.
     * @param newEvents: new full list of events to render
     */
    updateLogs(data) {
        this.vue.logs = data;
        this.vue.$el.scrollTop = this.vue.$el.scrollHeight;
    },
    /**
     * Sets up the needed data items.
     * @return {} an empty list to fill with events
     */
    setupLogs() {
        return {"logs":{"":""}};
    }
};

/**
 * LogView:
 *
 *
 * @author mstarch
 */
export class LogView {
    /**
     * @param elemid: HTML ID of the element to render to
     */
    constructor(elemid) {
        Object.assign(LogView.prototype, LogMixins);
        this.vue = new Vue({
            el: elemid,
            data: this.setupLogs()
        });
    }
}