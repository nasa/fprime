/**
 * vue-support/event.js:
 *
 * Event listing support for F´ that sets up the Vue.js components used to display events. These components allow the
 * user to render events. This file also provides EventMixins, which are the core functions needed to convert events to
 * something Vue.js can display. These should be mixed with any F´ objects wrapping Vue.js component creation.
 *
 * @author mstarch
 */
import {filter, timeToString} from "./utils.js";
import {config} from "../config.js";
import {_datastore} from "../datastore.js";

let OPREG = /Opcode (0x[0-9a-fA-F]+)/;

/**
 * events-list:
 *
 * Renders lists as a colorized table. This is a thin-wrapper to pass events to the fp-table component. It supplies the
 * needed method to configure fp-table to render events.
 */
Vue.component("event-list", {
    props: {
        /**
         * fields:
         *
         * Fields to display on this object. This should be null, unless the user is specifically trying to minimize
         * this object's display.
         */
        fields: {
            type: Array,
            default: null
        }
    },
    data: function() {
        return {"events": _datastore.events, "commands": _datastore.commands};
    },
    template: "#event-list-template",
    methods: {
        /**
         * Takes in a given event item, and harvests out the column values for display in the fp-table.
         * @param item: event object to harvest
         * @return {[string, *, *, void | string, *]}
         */
        columnify(item) {
            let display_text = item.display_text;
            // Remap command EVRs to expand opcode for visualization pruposes
            let groups = null
            if (item.template.severity.value == "Severity.COMMAND" && (groups = display_text.match(OPREG)) != null) {
                let mnemonic = "UNKNOWN";
                let id = parseInt(groups[1]);
                for (let command in this.commands) {
                    command = this.commands[command];
                    if (command.id == id) {
                        mnemonic = command.mnemonic;
                    }
                }
                display_text = display_text.replace(OPREG, '<span title="' + groups[0] + '">' + mnemonic + '</span>');
            }
            return [timeToString(item.time), "0x" + item.id.toString(16), item.template.full_name,
                item.template.severity.value.replace("Severity.", ""), display_text];
        },
        /**
         * Use the row's values and bounds to colorize the row. This function will color red and yellow items using
         * the boot-strap "warning" and "danger" calls.
         * @param item: item passed in with which to calculate style
         * @return {string}: style-class to use
         */
        style(item) {
            let severity = {
                "Severity.FATAL":      "fp-color-fatal",
                "Severity.WARNING_HI": "fp-color-warn-hi",
                "Severity.WARNING_LO": "fp-color-warn-lo",
                "Severity.ACTIVITY_HI": "fp-color-act-hi",
                "Severity.ACTIVITY_LO": "fp-color-act-lo",
                "Severity.COMMAND":     "fp-color-command",
                "Severity.DIAGNOSTIC":  ""
            }
            return severity[item.template.severity.value];
        },
        /**
         * Take the given item and converting it to a unique key by merging the id and time together with a prefix
         * indicating the type of the item. Also strip spaces.
         * @param item: item to convert
         * @return {string} unique key
         */
        keyify(item) {
            return "evt-" + item.id + "-" + item.time.seconds + "-"+ item.time.microseconds;
        },
        /**
         * A function to clear the events pane to remove events that have already been seen. Note: this action is
         * irrecoverable.
         */
        clearEvents() {
            return this.events.splice(0, this.events.length);
        }
    }
});
