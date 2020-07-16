/**
 * vue-support/channel.js:
 *
 * This file contains the necessary Vue definitions for displaying the channel table for F´. It also has the channel
 * mixin definitions to use channel Vue components in other compositions.
 *
 * @author mstarch
 */
import {listExistsAndItemNameNotInList, timeToString} from "./utils.js"
import "./fptable.js";
import {_datastore} from "../datastore.js";
/**
 * channel-table:
 *
 * Manages the full channel table. This includes calculating the filtered channels based on a given filtering function.
 */
Vue.component("channel-table", {
    props: {
        /**
         * fields:
         *
         * Fields to display on this object. This should be null, unless the user is specifically trying to minimize
         * this object's display.
         */
        fields: {
            type: [Array, String],
            default: ""
        },
        /**
         * The search text to initialize the table filter with (defaults to
         * nothing)
         */
        filterText: {
            type: String,
            default: ""
        },
        /**
         * A list of item ID names saying what rows in the table should be
         * shown; defaults to an empty list, meaning "show all items"
         */
        itemsShown: {
            type: [Array, String],
            default: ""
        },
        /**
         * 'compact' allows the user to hide filters/buttons/headers/etc. to
         * only show the table itself for a cleaner view
         */
        compact: {
            type: Boolean,
            default: false
        }
    },
    data: function() {
        return {"channels": _datastore.channels};
    },
    template: "#channel-table-template",
    // Defined methods
    methods: {
        /**
         * Returns a list of column values for the input channel item. These items will be rendered.
         * @param item: channel item to convert to column values
         * @return {*[]}
         */
        columnify(item) {
            if (item.time == null || item.val == null) {
                return ["", "0x" + item.id.toString(16), item.template.full_name, ""];
            }
            return [timeToString(item.time), "0x" + item.id.toString(16), item.template.full_name,
                (typeof(item.display_text) !== "undefined")? item.display_text : item.val]
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
        calculateRowStyle() {
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
            for (let i = 0; i < bounds.length; i++) {
                let bound = bounds[i];
                if ((bound.bounds[0] != null && value < bound.bounds[0]) ||
                    (bound.bounds[1] != null && value < bound.bounds[1])) {
                    return bound.class;
                } else if (bound.bounds[0] != null || bound.bounds[1] != null) {
                    return "table-success";
                }
            }
            return "";
        },
        /**
         * Converts an item to a name for use with the views functionality.
         * @param item: the channel item to convert to its name
         * @return item name
         */
        itemToName(item) {
            return item.template.full_name;
        },
        /**
         * Function that hides items with null time or value.
         * @param item: channel to hide
         * @return {boolean}
         */
        channelHider(item) {
            return item.val == null
                || item.time == null
                || listExistsAndItemNameNotInList(this.itemsShown, item);
        }
    },
    // Computed methods
    computed: {
        conform() {
            return Object.values(this.channels);
        }
    }
});
