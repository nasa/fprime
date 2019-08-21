/**
 * command-input:
 *
 *
 */
// Setup component for select
import "../third-party/vue-select.js"
import {timeToString,filter} from "./utils.js";

Vue.component('v-select', VueSelect.VueSelect);

/**
 * channel-row:
 *
 * A vue component designed to render exactly on row of the channel lising. It includes both the massage functions used
 * to transform data for display purposes (e.g. getChanTime) and also the colorization code used to colorize these lists
 * based on the channel's bounds.
 */
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


Vue.component("command-input", {
    props:["commands", "loader", "cmdhist"],
    data: function() {return {"matching": "", "selected": {"mnemonic":"NON_SELECTED", "args":[]}, "active": false}},
    template: "#command-input-template",
    methods: {
        clearArguments: function() {
            // Clear arguments
            for (let i = 0; i < this.selected.args.length; i++) {
                this.selected.args[i].value = "";
            }
        },
        sendCommand: function() {
            let _self = this;
            _self.active = true;
            let command = this.selected;
            let values = [];
            for (let i = 0; i < this.selected.args.length; i++) {
                values.push(this.selected.args[i].value);
            }
            this.loader.load("/commands/" + command.full_name, "PUT",
                {"key":0xfeedcafe, "arguments": values})
                .then(function() {_self.active = false;})
                .catch(function(err) {
                    console.error("[ERROR] Failed to send command: " + err);
                    _self.active = false;
                });
        },
        columnify: function(item) {
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
        }
    },
    computed: {
        /**
         *
         * @return {unknown[]}
         */
        commandList: function() {
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
 * Core functions of commanding, required by the above Vue components. This allows various Vue classes to use the
 * functions as part of a composition. This allows users and implementors to mix-and-match components, Vues, etc, while
 * still maintaining the required functions for the above Command Vueue component.
 *
 * This set of mixins must provide the following commands parts:
 *
 * 1. "setupCommands" for commands that takes a loader, and commands objects, and returns the needed vue data object.
 * 2. "initializeCommands" operates on the "this.vue" object, setting the $refs to have a selected command
 *
 * Usage: call "setupCommands(commands, loader)" to get the necessary data parts, then setup the vue object, the call
 *        "initializeCommands()" to setup the this.vue object.
 *
 * @author mstarch
 */
export let CommandMixins = {
    /**
     * Sets up the needed data partial-dictionary to be supplied to the Vue.js construction, should it use the above
     * Vue "command-input" component.
     *
     * Note: call this function before creating this.vue and supply to this.vue the following items as part of the data
     * input.
     * @param commands: command dictionary object
     * @param loader: f prime endpoint loader with the ability to connect to the f prime REST endpoint.
     * @return {{loader: loader object  loads endpoints, commands: command dictionary object, history: empty list}}
     */
    setupCommands(commands, loader) {
        return {"commands": commands, "loader": loader, "cmdhist":[]};
    },
    /**
     * Initialize the commanding input to have an active command. This ensures that a valid command is setup once the
     * vue object has been constructed.
     *
     * Note: call this function after creating this.vue.
     */
    initializeCommands(component) {
        // Handle initial call
        if (typeof(component) === "undefined") {
            // Sanity check function
            if (typeof(this.vue) === "undefined") {
                throw Error(typeof(this) + " does not setup a 'vue' member. Cannot initialize commands.");
            }
            component = this.vue;
        }
        // Check if it is our component for the type we are looking for
        if (component.$options.name === "command-input") {
            component.selected = component.commands["cmdDisp.CMD_NO_OP"];
            return true;
        }
        else if (component.$children.length > 0) {
            for (let i = 0; i < component.$children.length; i++) {
                if (this.initializeCommands(component.$children[i])) {
                    return true;
                }
            }
            return false;
        }
        return false;
    },
    /**
     * Updates the command history displayed by this command object. This will store the command history as an active.
     * @param cmdhist: command history to set
     */
    updateCommandHistory(cmdhist) {
        this.vue.cmdhist.push(...cmdhist);
    }
};
/**
 * CommandView:
 *
 * This is a stand-alone command view class. It will mixin the required functions to handle commands for the above
 * command-input component, and it will construct a new Vue object that wraps the supplied element.
 *
 * @author mstarch
 */
export class CommandVue {
    /**
     * Construct a stand-alone command-input given the element, commands dictionary, and loader.  See the above mixin
     * for usage of commands and loader.  "elemid" is the element that will be wrapped by the Vue object.
     * @param elemid: element to conform to the Vue.
     * @param commands: commands dictionary to use to setup commanding list
     * @param loader: loader used to send commands
     */
    constructor(elemid, commands, loader) {
        //Mixins commands
        Object.assign(CommandVue.prototype, CommandMixins);
        let data = this.setupCommands(commands, loader);
        // Setup vue component
        this.vue = new Vue({
            el: elemid,
            data: data
        });
        // Initialize commands
        this.initializeCommands();
    }
}