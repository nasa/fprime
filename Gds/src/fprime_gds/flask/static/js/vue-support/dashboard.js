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
