/**
 * dashboard.js:
 *
 * Vue support for the dashboard screen, which will hold a user-configurable
 * menu containing other components.
 *
 * TODO: Implement the rest of this
 */

import {CommandMixins} from "./command.js";
import {loader} from "../gds.js";


const PlaceholderTextComp = Vue.component("placeholder-text-todo", {
    template: `<pre> <slot></slot> </pre>`
});

Vue.component("dashboard", {
    components: {
        "placeholder-text-todo": PlaceholderTextComp
    },
    template: "#dashboard-template",
    data: function() {
        return {
            fileText: ""
        };
    },
    methods: {
        parseConfigText() {
            // Takes in the given configuration text and returns a component to
            // place inside this dashboard
            // TODO: Implement this!
        },

        /**
         * Takes in the given configuration file and configures the dashboard to
         * match it by adding the appropriate components to the dashboard
         *
         * TODO: Need to actually implement this; currently, just takes in a
         * file and adds its contents to the screen
         * @param configFile: A user-uploaded configuration file
         */
        configureDashboard(configFile) {
            const fileReader = new FileReader();
            // TODO: Check if better way to bind "this"?
            const thisVueComp = this;   // Reference to Vue component for use in callback; needed due to "this" changing meaning inside function
            fileReader.onload = function(evt) {
                thisVueComp.fileText = evt.target.result;

                // Try to append text programmatically via a new component
                thisVueComp.addVueComponent("placeholder-text-todo", undefined, [`${thisVueComp.fileText}`]);

                // TODO: See if there's a cleaner way to initialize this?
                thisVueComp.addVueComponent(
                    "command-input",
                    {
                        mixins: [
                            CommandMixins
                        ],
                        propsData: CommandMixins.setupCommands(loader.endpoints["command-dict"].data, loader),
                        mounted() {
                            //"this" will refer to the Vue Command-input component
                            // TODO: Avoid duplication w/ Commands constructor?
                            this.vue = this;

                            CommandMixins.initializeCommands(this);

                            // TODO: Is this acceptable, or duplication of polling on gds.js?
                            const historyCallback = function (data) {this.updateCommandHistory(data["history"]);}
                            const boundCallback = historyCallback.bind(this);
                            loader.registerPoller("commands", boundCallback);
                        }
                    }
                );
            };
            fileReader.readAsText(configFile);
        },

        /**
         * Adds the given Vue component to this dashboard
         *
         * @param {*} compName The name of the component you want to add
         * @param {*} initialOptions An object containing the initialization
         * options you want to pass to the new component
         * @param {*} slots A list of values you want to place in the Vue slots
         * on the component
         */
        addVueComponent(compName, initialOptions={}, slots=[]) {
            const ComponentConstructor = Vue.options.components[compName]; // TODO: Only gets globally registered components; is that acceptable?
            const compInstance = new ComponentConstructor(initialOptions);
            compInstance.$slots.default = slots;
            compInstance.$mount();
            this.$refs["dashboard-widgets-container"].appendChild(compInstance.$el);
        }
    }
});
