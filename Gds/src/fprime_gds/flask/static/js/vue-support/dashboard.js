/**
 * dashboard.js:
 *
 * Vue support for the dashboard screen, which holds a user-configurable menu
 * containing other components.
 *
 */

import VRuntimeTemplate from "../../third-party/js/v-runtime-template.js"

import {DashboardBox} from "./dashboard-box.js";
import {DashboardRow} from "./dashboard-row.js";

Vue.component("dashboard", {
    components: {
        "dashboard-box": DashboardBox,
        "dashboard-row": DashboardRow,
        "v-runtime-template": VRuntimeTemplate,
    },
    template: "#dashboard-template",

    data: function() {
        return {
            userTemplate: `<h2>This is your Dashboard</h2>
            <h4>There are many like it, but this one is yours.</h4>`,
        };
    },

    methods: {
        /**
         * Takes in the given configuration file and configures the dashboard to
         * match it by adding the appropriate components to the dashboard.
         * Stores the configuration file as the default dashboard for later use.
         *
         * @param configFile: A user-uploaded configuration file
         */
        configureDashboardFromFile(configFile) {
            const fileReader = new FileReader();
            const thisVueComp = this;   // Reference to Vue component for use in callback; needed due to "this" changing meaning inside function
            fileReader.onload = function(event) {
                const fileText = event.target.result;
                thisVueComp._storeConfigurationText(fileText);
                thisVueComp.configureDashboard(fileText);
            };
            fileReader.readAsText(configFile);
        },

        /**
         * Takes in the given configuration text and configures the dashboard to
         * match it by adding the appropriate components to the dashboard
         *
         * @param configText: A user-uploaded string specifying what their
         * dashboard should look like
         */
        configureDashboard(configText) {
            // Wrap user content in div so Vue can display everything with single parent (otherwise, only 1st user element would be displayed)
            this.userTemplate = `<div class="fp-flex-repeater">${configText}</div>`;
        },

        /**
         * Stores a dashboard configuration for persistent reuse across browser
         * sessions
         *
         * @param configText: A user-uploaded string specifying what their
         * dashboard should look like
         */
        _storeConfigurationText(configText) {
            localStorage.setItem("dashboardConfigurationText", configText);
        },

        /**
         * Returns a stored dashboard configuration; returns a default string if
         * no such configuration has been set before
         */
        _getConfigurationText() {
            const storedText = localStorage.getItem("dashboardConfigurationText");
            if (storedText) { return storedText; }
            return this.userTemplate;
        }
    },

    mounted() {
        // Tries to use the previously-used dashboard if one exists
        this.configureDashboard(this._getConfigurationText());
    }
});
