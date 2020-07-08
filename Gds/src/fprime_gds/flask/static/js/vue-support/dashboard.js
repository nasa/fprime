/**
 * dashboard.js:
 *
 * Vue support for the dashboard screen, which holds a user-configurable menu
 * containing other components.
 *
 * TODO: Implement the rest of this
 */

import VRuntimeTemplate from "../../third-party/js/v-runtime-template.js"

import {CommandMixins} from "./command.js";
import {EventMixins} from "./event.js";
import {loader} from "../gds.js";

// TODO: Move other components into separate files

const DashboardBox = Vue.component("dashboard-box", {
    props: {
        title: {
            type: String,
            default: ""
        },
        color: {
            type: String,
            default: "transparent"
        }
    },
    // TODO: Figure out better way to handle styles here, e.g. through classes?
    template: `<fieldset v-bind:style="{background: color}">
        <legend style="background: black; color: white;">{{ title }}</legend>
        <slot></slot>
    </fieldset>`
});

// An element that contains/marks a horizontal row of components in the dash
const DashboardRow = Vue.component("dashboard-row", {
    // TODO: Figure out better way to handle styles here, e.g. through classes?
    template: `<div style="display: flex; flex-wrap: nowrap; width: 100%;">
        <slot></slot>
    </div>`
});

//  TODO: This should be incorporated in the original component
const CommandWrapper = Vue.component("command-wrapper", {
    mixins: [CommandMixins],
    template: `<command-input v-bind:commands="commands" v-bind:loader="loader" v-bind:cmdhist="cmdhist"></command-input>`,

    data: function() {
        return CommandMixins.setupCommands(loader.endpoints["command-dict"].data, loader);
    },

    mounted() {
        //"this" will refer to the Vue Command-input component
        // TODO: Avoid duplication w/ Commands constructor?
        this.vue = this;
        CommandMixins.initializeCommands(this);

        // TODO: NOT ACCEPTABLE, destroys the previous history poller
        const historyCallback = function (data) {this.updateCommandHistory(data["history"]);}
        const boundCallback = historyCallback.bind(this);
        loader.registerPoller("commands", boundCallback);
    }
});

//  TODO: This should be incorporated in the original component
const EventWrapper = Vue.component("event-wrapper", {
    mixins: [EventMixins],
    template: `<event-list v-bind:events="events" commands="loader.endpoints['command-dict'].data"></event-list>`,

    data: function() {
        return EventMixins.setupEvents();
    },

    mounted() {
        // TODO: NOT ACCEPTABLE, destroys the previous history poller
        const historyCallback = function (data) {this.updateEvents(data["history"]);}
        const boundCallback = historyCallback.bind(this);
        loader.registerPoller("events", boundCallback);
    }
});

Vue.component("dashboard", {
    components: {
        "dashboard-box": DashboardBox,
        "dashboard-row": DashboardRow,
        "v-runtime-template": VRuntimeTemplate,
        "event-wrapper": EventWrapper
    },
    template: "#dashboard-template",

    data: function() {
        return {
            userTemplate: ``,
        };
    },

    methods: {
        /**
         * Takes in the given configuration file and configures the dashboard to
         * match it by adding the appropriate components to the dashboard
         *
         * @param configFile: A user-uploaded configuration file
         */
        configureDashboard(configFile) {
            const fileReader = new FileReader();
            const thisVueComp = this;   // Reference to Vue component for use in callback; needed due to "this" changing meaning inside function
            fileReader.onload = function(event) {
                const fileText = event.target.result;
                // Wrap user content in div so Vue can display everything with single parent (otherwise, only 1st element would be displayed)
                thisVueComp.userTemplate = `<div class="fp-flex-repeater">${fileText}</div>`;
            };
            fileReader.readAsText(configFile);
        }
    }
});
