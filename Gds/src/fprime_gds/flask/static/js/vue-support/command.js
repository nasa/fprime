/**
 * command.js:
 *
 * Contains the Vue components for displaying the command history, and the command sending form.
 */
// Setup component for select
import "../../third-party/js/vue-select.js"
import {listExistsAndItemNameNotInList, timeToString} from "./utils.js";
import {_datastore} from "../datastore.js";
import {_loader} from "../loader.js";

Vue.component('v-select', VueSelect.VueSelect);
/**
 *
 */
Vue.component("command-argument", {
    props:["argument"],
    template: "#command-argument-template",
    computed: {
        /**
         * Allows for validation of commands using the HTML-based validation using regex and numbers. Note: numbers here
         * are treated as text, because we can allow for hex, and octal bases.
         * @return [HTML input type, validation regex]
         */
        inputType() {
            // Unsigned integer
            if (this.argument.type[0] == 'U') {
                // Supports binary, hex, octal, and digital
                return ["text", "0[bB][01]+|0[oO][0-7]+|0[xX][0-9a-fA-F]+|[1-9]\\\\d*"];
            }
            else if (this.argument.type[0] == 'F') {
                return ["number", "\\d*.?\\d*"];
            }
            return ["text", ".*"];
        },
        /**
         * Unpack errors on arguments, for display in this GUI.
         */
        argumentError() {
            if ("error" in this.argument) {
                return this.argument.error;
            }
            return "NO ERROR!!!";
        }
    }
});


Vue.component("command-item", {
    props:["command"],
    template: "#command-item-template",
    computed: {
        /**
         * Produces the channel's time in the form of a string.
         * @return {string} seconds.microseconds
         */
        calculateCommandTime: function() {
            return timeToString(this.command.time);
        }
    }
});

/**
 * command-input:
 *
 * Input command form Vue object. This allows for sending commands from the GDS.
 */
Vue.component("command-input", {
    data: function() {
        let selected = _datastore.commands["cmdDisp.CMD_NO_OP"];
        return {
            "commands": _datastore.commands,
            "loader": _loader,
            "selected": selected,
            "active": false
        }
    },
    template: "#command-input-template",
    methods: {
        /**
         * Clear the arguments to the command.
         */
        clearArguments() {
            // Clear arguments
            for (let i = 0; i < this.selected.args.length; i++) {
                this.selected.args[i].value = "";
            }
        },
        /**
         * Send a command from this interface. This calls into the loader to send the command, and locks-out until the
         * command reaches the ground system.
         */
        sendCommand() {
            let _self = this;
            _self.active = true;
            let command = this.selected;
            let values = [];
            for (let i = 0; i < this.selected.args.length; i++) {
                values.push(this.selected.args[i].value);
            }
            this.loader.load("/commands/" + command.full_name, "PUT",
                {"key":0xfeedcafe, "arguments": values})
                .then(function() {
                    _self.active = false;
                    // Clear errors, as there is not a problem further
                    for (let i = 0; i < command.args.length; i++) {
                        command.args[i].error = "";
                    }
                })
                .catch(function(err) {
                    // Log all errors incoming
                    console.error("[ERROR] Failed to send command: " + err);
                    let response = JSON.parse(err);
                    // Argument errors are parceled out to each error
                    if ("errors" in response) {
                        for (let i = 0; i < response.errors.length; i++) {
                            command.args[i].error = response.errors[i];
                        }
                    }
                    // All other command errors
                    else {
                        command.error = response.message;
                    }
                    _self.active = false;
                });
        }
    },
    computed: {
        /**
         * List out the usable commands to be sent by the ground system.
         * @return {unknown[]}
         */
        commandList() {
            return Object.values(this.commands).sort(
                /**
                 * Compare objects by full_name
                 * @param obj1: first object
                 * @param obj2: second object
                 * @return {number} -1 or 1
                 */
                function(obj1, obj2) {
                    if (obj1.full_name <= obj2.full_name) {
                        return -1;
                    }
                    return 1;
                });
        }
    }
});

/**
 * command-history:
 *
 * Displays a list of previously-sent commands to the GDS.
 */
/**
 * command-input:
 *
 * Input command form Vue object. This allows for sending commands from the GDS.
 */
Vue.component("command-history", {
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
        return {
            "cmdhist": _datastore.command_history,
            "matching": ""
        }
    },
    template: "#command-history-template",
    methods: {
        /**
         * Converts a given item into columns.
         * @param item: item to convert to columns
         */
        columnify(item) {
            let values = [];
            for (let i = 0; i < item.args.length; i++) {
                values.push(item.args[i].value);
            }
            return [timeToString(item.time), "0x" + item.id.toString(16), item.template.full_name, values];
        },
        /**
         * Take the given item and converting it to a unique key by merging the id and time together with a prefix
         * indicating the type of the item. Also strip spaces.
         * @param item: item to convert
         * @return {string} unique key
         */
        keyify(item) {
            return "cmd-" + item.id + "-" + item.time.seconds + "-"+ item.time.microseconds;
        },
        /**
         * Returns if the given item should be hidden in the data table; by
         * default, shows all items. If the "itemsShown" property is set, only
         * show items with the given names
         *
         * @param item: The given F' data item
         * @return {boolean} Whether or not the item is shown
         */
        isItemHidden(item) {
            return listExistsAndItemNameNotInList(this.itemsShown, item);;
        }
    }
});
