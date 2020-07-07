/**
 * dashboard.js:
 *
 * Vue support for the dashboard screen, which will hold a user-configurable
 * menu containing other components.
 *
 * TODO: Implement the rest of this
 */

import {CommandMixins} from "./command.js";
import {EventMixins} from "./event.js";
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
            fileText: "",
            creationActions: {},
            createdComponents: []
        };
    },

    methods: {
        /**
         * Takes in the given configuration line and does the appropriate
         * action to the dashboard, e.g. adding a new component with the given
         * specifications
         *
         * @param {string} line A string representing a single line of a
         * configuration file
         */
        parseConfigLine(line) {
            // TODO: Implement this properly! Currently just looks for the literal name of the item to add, and adds it if found
            const firstWord = line.trim().split(" ")[0];
            const action = this.creationActions[firstWord];
            if (action) {
                action();
            };
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
            this.removeAllComponents();

            const fileReader = new FileReader();
            // TODO: Check if better way to bind "this"?
            const thisVueComp = this;   // Reference to Vue component for use in callback; needed due to "this" changing meaning inside function
            fileReader.onload = function(evt) {
                thisVueComp.fileText = evt.target.result;

                // Try to append text programmatically via a new component
                thisVueComp.addVueComponent("placeholder-text-todo", undefined, [`${thisVueComp.fileText}`]);

                const textLines = thisVueComp.fileText.split(/\r?\n/);
                textLines.forEach(thisVueComp.parseConfigLine);
            };
            fileReader.readAsText(configFile);
        },

        /**
         * Adds the given Vue component to this dashboard
         *
         * @param {string} compName The name of the component you want to add
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
            this.createdComponents.push(compInstance);
        },

        /**
         * Removes all the existing components on the dashboard
         */
        removeAllComponents() {
            this.createdComponents.forEach((comp) => {
                // Remove the rendered element from the DOM, then destroy it in
                // Vue
                comp.$el.remove();
                comp.$destroy();
            });
        }
    },
    created() {
        // TODO: Find a cleaner way to define this (i.e. refactor the whole mess)
        const thisVueComp = this;
        this.creationActions = {
            "command-input": function() {
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
            },

            "event-list": function() {
                thisVueComp.addVueComponent(
                    "event-list",
                    {
                        mixins: [
                            EventMixins
                        ],
                        propsData: {
                            events: [], // TODO: Not sure if this is actually used?
                            commands: loader.endpoints["command-dict"].data
                        },
                        data: EventMixins.setupEvents(),
                        mounted() {
                            // TODO: Is this acceptable, or duplication of polling on gds.js?
                            const historyCallback = function (data) {this.updateEvents(data["history"]);}
                            const boundCallback = historyCallback.bind(this);
                            loader.registerPoller("events", boundCallback);
                        }
                    }
                );
            }
        };
    }
});
