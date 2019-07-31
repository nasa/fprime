/**
 * vue-support/channel.js:
 *
 * This file contains the necessary Vue definitions for displaying the channel table for F´.
 *
 * @author mstarch
 */
import {filter, timeToString} from "./utils.js"
/**
 * channel-row:
 *
 * A vue component designed to render exactly on row of the channel lising. It includes both the massage functions used
 * to transform data for display purposes (e.g. getChanTime) and also the colorization code used to colorize these lists
 * based on the channel's bounds.
 */
Vue.component("channel-row", {
    props:["channel"],
    template: "#channel-row-template",
    computed: {
        /**
         * Use the row's values and bounds to colorize the row. This function will color red and yellow items using
         * the boot-strap "warning" and "danger" calls.
         * @return {string}: style-class to use
         */
        calculateRowStyle: function () {
            let value = this.channel.value;
            let channel = this.channel;
            let bounds = [
                {"class": "table-danger", "bounds": [channel.template.low_red, value <= channel.template.high_red]},
                {
                    "class": "table-warning",
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
         * Produces the channel's time in the form of a string.
         * @return {string} seconds.microseconds
         */
        calculateChannelTime: function() {return timeToString(this.channel.time)}
    }
});
/**
 * channel-table:
 *
 * Manages the full channel table. This includes calculating the filtered channels based on a given filtering function.
 */
Vue.component("channel-table", {
    props:["channels"],
    data: function() {return {"matching": ""}},
    template: "#channel-table-template",
    computed: {
        /**
         * Calculates the filtered list of channels.
         */
        calculateFilteredChannels: function() {
            return filter(this.channels, this.matching,
                function (channel)
                {
                    return "0x" + channel.id.toString(16) + channel.template.name + channel.val;
                });
        }
    },
    // Update function, making table sortable after it is rendered
    updated: function() {
        this.$nextTick(function() {
            // Check existance of the third party library
            if (typeof(sorttable) !== "undefined") {
                let tables = this.$el.getElementsByTagName("table");
                for (let i = 0; i < tables.length; i++) {
                    sorttable.makeSortable(tables[i]);
                }
            } else {
                console.warn("sortable.js not found, not attempting to sort tables");
            }
        })
    }
});

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
    constructor(elemid) {
        this.vue = new Vue({
            el: elemid,
            data: {
                channels: [],
            }
        });
    }

    /**
     * Make the list of channels unique for display purposes.
     * @param channels: channels to make unique
     */
    uniqueify(channels) {
        // Create unique set of data
        let chanSet = {};
        for (let i = 0; i < channels.length; i++) {
            chanSet[channels[i].id] = channels[i];
        }
        let chanList = [];
        for (let chanItem in chanSet) {
            chanList.push(chanSet[chanItem]);
        }
        return chanList;
    }

    /**
     * Update the list of channels with the supplied new list of channels.
     * @param newChannels: new full list of channels to render
     */
    updateChannels(newChannels) {
        let unique = this.uniqueify(newChannels);
        this.vue.channels = unique;
    }
}
