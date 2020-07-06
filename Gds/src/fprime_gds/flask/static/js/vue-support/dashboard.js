/**
 * dashboard.js:
 *
 * Vue support for the dashboard screen, which will hold a user-configurable
 * menu containing other components.
 *
 * TODO: Implement the rest of this
 */

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
                const TextComponentConstructor = Vue.options.components["placeholder-text-todo"]; // TODO: Only gets globally registered components; is that acceptable?
                const textInstance = new TextComponentConstructor();
                textInstance.$slots.default = [`${thisVueComp.fileText}`];
                textInstance.$mount();
                thisVueComp.$el.appendChild(textInstance.$el);
            };
            fileReader.readAsText(configFile);
        }
    }
});
