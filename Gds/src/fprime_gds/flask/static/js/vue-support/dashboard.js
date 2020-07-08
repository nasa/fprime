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

// TODO: Delete placeholder components, move the rest into separate files
const PlaceholderTextComp = Vue.component("placeholder-text-todo", {
    template: `<pre> <slot></slot> </pre>`
});

const DashboardBox = Vue.component("dashboard-box", {
    props: {
        title: {
            type: String,
            default: ""
        },
        color: {
            type: String,
            default: "rgba(0, 0, 0, 0.0)"   // Transparent
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

Vue.component("dashboard", {
    components: {
        "dashboard-box": DashboardBox,
        "dashboard-row": DashboardRow,
        "placeholder-text-todo": PlaceholderTextComp,
        "v-runtime-template": VRuntimeTemplate
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
            // TODO: Check if better way to bind "this"?
            const thisVueComp = this;   // Reference to Vue component for use in callback; needed due to "this" changing meaning inside function
            fileReader.onload = function(event) {
                const fileText = event.target.result;
                thisVueComp.userTemplate = `${fileText}`;
            };
            fileReader.readAsText(configFile);
        }
    }
});
