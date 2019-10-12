/**
 * vue-support/channel.js:
 *
 * This file contains the necessary Vue definitions for displaying the channel table for F´. It also has the channel
 * mixin definitions to use channel Vue components in other compositions.
 *
 * @author mstarch
 */
import {filter, timeToString} from "./utils.js"
import "./fptable.js";
/**
 * channel-table:
 *
 * Manages the full channel table. This includes calculating the filtered channels based on a given filtering function.
 */
Vue.component("channel-table", {
    props:["channels"],
    template: "#channel-table-template",
    // Defined methods
    methods: {
        /**
         * Returns a list of column values for the input channel item. These items will be rendered.
         * @param item: channel item to convert to column values
         * @return {*[]}
         */
        columnify: function(item) {
            if (item.time == null || item.val == null) {
                return ["", "0x" + item.id.toString(16), item.template.full_name, ""];
            }
            return [timeToString(item.time), "0x" + item.id.toString(16), item.template.full_name, item.val]
        },
        /**
         * Converts a channel to a unique rendering key. For channels, ids are unique, and thus just use that.
         * @param item: channel item to provide
         * @return {*}
         */
        keyify(item) {
            return item.id;
        },
        /**
         * Use the row's values and bounds to colorize the row. This function will color red and yellow items using
         * the boot-strap "warning" and "danger" calls.
         * @return {string}: style-class to use
         */
        calculateRowStyle: function () {
            let value = this.channel.value;
            let channel = this.channel;
            let bounds = [
                {"class": "fp-color-fatal", "bounds": [channel.template.low_red, value <= channel.template.high_red]},
                {
                    "class": "fp-color-warn-hi",
                    "bounds": [channel.template.low_yellow, value <= channel.template.high_yellow]
                }
            ];
            // Check bounds.
            let style = "";
            for (let i = 0; i < bounds.length; i++) {
                let bound = bounds[i];
                if ((bound.bounds[0] != null && value < bound.bounds[0]) ||
                    (bound.bounds[1] != null && value < bound.bounds[1])) {
                    return bound.class;
                } else if (bound.bounds[0] != null || bound.bounds[1] != null) {
                    style = "table-success";
                }
            }
            return "";
        },
        /**
         * Converts an item to a name for use with the views functionality.
         * @param item: the channel item to convert to its name
         * @return item name
         */
        itemToName: function(item) {
            return item.template.full_name;
        },
        /**
         * Function that hides items with null time or value.
         * @param item: channel to hide
         * @return {boolean}
         */
        channelHider: function(item) {
            return item.val == null || item.time == null;
        }
    },
    // Computed methods
    computed: {
        conform: function() {
            return Object.values(this.channels);
        }
    }
});
/**
 * ChannelMixins:
 *
 * These functions mixin to allow the use of the above channel view. This means that it will collapse a list of readings
 * to a unique set of channels.
 */
export let ChannelMixins = {
    /**
     * Update the list of channels with the supplied new list of channels.
     * @param newChannels: new full list of channels to render
     */
    updateChannels(newChannels) {
        // Loop over all dictionaries
        for (let i = 0; i < newChannels.length; i++) {
            let channel = newChannels[i];
            let id = channel.id;
            this.vue.channels[id] = channel;
        }
    },
    /**
     * Sets up the needed channel data items.  Adding new keys to an object won't be detected, so we will pre-populate
     * the object with the items from the template.
     * @param templates: templates for each channel
     * @return {"channels": {}} an empty list to fill with channels
     */
    setupChannels(templates) {
        let channels = {};
        // Create a list of empty items
        for (let key in templates) {
            channels[key] = {id: key, template: templates[key], val: null, time: null};
        }

        return {"channels": channels};
    }
};

/**
 * ChannelView:
 *
 * A wrapper for the channel-list viewable. This particular instance is supported by Vue.js in order to render the
 * channels as a table.
 *
 * @author mstarch
 */
export class ChannelView {
    /**
     * Creates a ChannelView that delegates to a Vue.js view.
     * @param elemid: HTML ID of the element to render to
     */
    constructor(elemid, loader) {
        Object.assign(ChannelView.prototype, ChannelMixins);
        this.vue = new Vue({
            el: elemid,
            data: this.setupChannels(loader)
        });
    }
}
